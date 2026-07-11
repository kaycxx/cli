// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/cli/args.hpp>

#include <kaycxx/cli/parse_error.hpp>

#include <algorithm>
#include <any>
#include <cstddef>
#include <format>
#include <span>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace kaycxx::cli {

bool args::get(flag_handle const& flag) const {
    return flags_.contains(&flag.definition());
}

void args::validate() const {
    if (validated_) {
        return;
    }

    auto values = std::vector<std::string_view>();
    values.reserve(positional_values_.size());
    for (auto const& value : positional_values_) {
        values.emplace_back(value);
    }

    auto parsed_parameters = std::unordered_map<parameter_base const*, std::any>();
    auto position = std::size_t(0);

    for (auto i = std::size_t(0); i < parameter_definitions_.size(); ++i) {
        auto required_after = std::size_t(0);
        for (auto j = i + 1; j < parameter_definitions_.size(); ++j) {
            required_after += parameter_definitions_[j]->min_count();
        }

        auto const* parameter = parameter_definitions_[i];
        auto const min_count = parameter->min_count();
        auto const max_count = std::max(parameter->max_count(), min_count);
        auto const remaining = position < values.size() ? values.size() - position : std::size_t(0);

        if (remaining < min_count) {
            throw parse_error(std::format("Missing parameter <{}>", parameter->name()));
        }

        auto const remaining_after_min = remaining - min_count;
        auto const available_extra = remaining_after_min > required_after ? remaining_after_min - required_after : std::size_t(0);
        auto const max_extra = max_count > min_count ? max_count - min_count : std::size_t(0);
        auto const take = min_count + std::min(available_extra, max_extra);

        auto const parameter_values = take == 0
            ? std::span<std::string_view const>()
            : std::span<std::string_view const>(values.data() + position, take);

        try {
            parsed_parameters[parameter] = parameter->parse_values(parameter_values);
        } catch (parse_error const& error) {
            throw parse_error(std::format("{} for parameter <{}>", error.what(), parameter->name()));
        }
        position += take;
    }

    if (position < values.size()) {
        throw parse_error(std::format("Unexpected parameter {}", values[position]));
    }

    parameters_ = std::move(parsed_parameters);
    validated_ = true;
}

} // namespace kaycxx::cli
