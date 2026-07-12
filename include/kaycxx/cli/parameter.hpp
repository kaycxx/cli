// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

/**
 * @file
 * Defines single positional parameters.
 */

#pragma once

#include <any>
#include <cstddef>
#include <format>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>

#include <kaycxx/cli/parameter_base.hpp>
#include <kaycxx/cli/parse_error.hpp>
#include <kaycxx/cli/parse_value.hpp>

namespace kaycxx::cli {

/**
 * Typed single positional parameter definition.
 *
 * @tparam T  Parsed parameter value type.
 */
template <parseable_value T>
class parameter : public parameter_base {
public:
    /**
     * Creates a positional parameter definition.
     *
     * @param name         Placeholder name used by generated help output and parse errors.
     * @param description  Optional human-readable parameter description used by generated help output.
     */
    explicit parameter(std::string_view name, std::optional<std::string_view> description = std::nullopt)
        : parameter_base(name, description)
    {}

    /**
     * Sets the default parameter value.
     *
     * @param value  Default value used when the parameter is not present in parsed arguments.
     */
    void default_value(T value) {
        default_value_ = std::move(value);
    }

    /**
     * Returns generated help usage for this parameter.
     *
     * @returns Usage text.
     */
    [[nodiscard]] std::string usage() const override {
        return std::format("<{}>", name());
    }

    /**
     * Returns the minimum number of values consumed by this parameter.
     *
     * @returns Zero when a default value is configured, otherwise one.
     */
    [[nodiscard]] std::size_t min_count() const noexcept override {
        return default_value_ ? 0 : 1;
    }

    /**
     * Returns the maximum number of values consumed by this parameter.
     *
     * @returns Always one.
     */
    [[nodiscard]] std::size_t max_count() const noexcept override {
        return 1;
    }

    /**
     * Parses this positional parameter.
     *
     * @param values  Value texts to parse.
     *
     * @returns Parsed parameter value stored in type-erased form.
     *
     * @throws parse_error  When the value is syntactically invalid or no value is present and no default value is configured.
     */
    [[nodiscard]] std::any parse_values(std::span<std::string_view const> values) const override {
        if (values.empty()) {
            if (default_value_) {
                return *default_value_;
            }

            throw parse_error(std::format("Missing parameter <{}>", name()));
        }

        try {
            return detail::parse_value<T>(values.front());
        } catch (parse_error const& error) {
            throw parse_error(std::format("{} for parameter <{}>", error.what(), name()));
        }
    }

private:
    /** Optional value used when the parameter is absent. */
    std::optional<T> default_value_;
};

} // namespace kaycxx::cli
