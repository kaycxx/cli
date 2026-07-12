// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

/**
 * @file
 * Defines positional parameter lists.
 */

#pragma once

#include <any>
#include <cstddef>
#include <format>
#include <limits>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <kaycxx/cli/parameter_base.hpp>
#include <kaycxx/cli/parse_error.hpp>
#include <kaycxx/cli/parse_value.hpp>

namespace kaycxx::cli {

/**
 * Typed positional parameter list definition.
 *
 * A parameter list can consume zero or more positional values. Minimum and maximum counts define how many values may be consumed
 * before following positional parameters are considered.
 *
 * @tparam T  Parsed parameter value type.
 */
template <parseable_value T>
class parameters : public parameter_base {
public:
    /**
     * Creates a positional parameter list definition.
     *
     * @param name         Placeholder name used by generated help output and parse errors.
     * @param description  Optional human-readable parameter description used by generated help output.
     */
    explicit parameters(std::string_view name, std::optional<std::string_view> description = std::nullopt)
        : parameter_base(name, description)
    {}

    /**
     * Sets the minimum number of values consumed by this parameter list.
     *
     * @param count  Minimum value count.
     */
    void min(std::size_t count) noexcept {
        min_count_ = count;
    }

    /**
     * Sets the maximum number of values consumed by this parameter list.
     *
     * @param count  Maximum value count.
     */
    void max(std::size_t count) noexcept {
        max_count_ = count;
    }

    /**
     * Sets default values for this parameter list.
     *
     * @param values  Default values used when the parameter list is not present in parsed arguments.
     */
    void default_values(std::vector<T> values) {
        default_values_ = std::move(values);
    }

    /**
     * Returns generated help usage for this parameter list.
     *
     * @returns Usage text.
     */
    [[nodiscard]] std::string usage() const override {
        return std::format("<{}>...", name());
    }

    /**
     * Returns the minimum number of values consumed by this parameter list.
     *
     * @returns Zero when default values are configured, otherwise the configured minimum count.
     */
    [[nodiscard]] std::size_t min_count() const noexcept override {
        return default_values_ ? 0 : min_count_;
    }

    /**
     * Returns the maximum number of values consumed by this parameter list.
     *
     * @returns Configured maximum value count.
     */
    [[nodiscard]] std::size_t max_count() const noexcept override {
        return max_count_;
    }

    /**
     * Parses this positional parameter list.
     *
     * @param values  Value texts to parse.
     *
     * @returns Parsed parameter values stored in type-erased form.
     *
     * @throws parse_error  When one of the values is syntactically invalid or the parsed or default value count is outside the configured range.
     */
    [[nodiscard]] std::any parse_values(std::span<std::string_view const> values) const override {
        if (values.empty() && default_values_) {
            validate_count(default_values_->size());
            return *default_values_;
        }

        validate_count(values.size());

        auto result = std::vector<T>();
        result.reserve(values.size());

        for (auto value : values) {
            try {
                result.push_back(detail::parse_value<T>(value));
            } catch (parse_error const& error) {
                throw parse_error(std::format("{} for parameter <{}>", error.what(), name()));
            }
        }

        return result;
    }

private:
    /**
     * Validates a parsed or default value count.
     *
     * @param count  Number of values to validate.
     *
     * @throws parse_error  When the count is outside the configured range.
     */
    void validate_count(std::size_t count) const {
        if (count < min_count_) {
            throw parse_error(std::format("Missing parameter <{}>", name()));
        }
        if (count > max_count_) {
            throw parse_error(std::format("Too many values for parameter <{}>", name()));
        }
    }

    /** Configured minimum number of values. */
    std::size_t min_count_ = 0;

    /** Configured maximum number of values. */
    std::size_t max_count_ = std::numeric_limits<std::size_t>::max();

    /** Optional values used when no command-line values are present. */
    std::optional<std::vector<T>> default_values_;
};

} // namespace kaycxx::cli
