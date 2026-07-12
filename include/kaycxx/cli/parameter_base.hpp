// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

/**
 * @file
 * Defines the common base class for positional parameters.
 */

#pragma once

#include <any>
#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <string_view>

namespace kaycxx::cli {

/**
 * Common base class for positional parameter definitions.
 */
class parameter_base {
public:
    /**
     * Creates a positional parameter definition.
     *
     * @param name         Placeholder name used by generated help output and parse errors.
     * @param description  Optional human-readable parameter description used by generated help output.
     */
    explicit parameter_base(std::string_view name, std::optional<std::string_view> description = std::nullopt);

    /** Destroys the parameter definition. */
    virtual ~parameter_base() = default;

    /**
     * Returns the parameter name.
     *
     * @returns Placeholder name used by generated help output and parse errors.
     */
    [[nodiscard]] std::string const& name() const noexcept;

    /**
     * Returns the parameter description.
     *
     * @returns Human-readable description or an empty optional if no description was configured.
     */
    [[nodiscard]] std::optional<std::string> const& description() const noexcept;

    /**
     * Returns generated help usage for this parameter.
     *
     * @returns Usage text.
     */
    [[nodiscard]] virtual std::string usage() const = 0;

    /**
     * Returns the minimum number of values consumed by this parameter.
     *
     * @returns Minimum value count.
     */
    [[nodiscard]] virtual std::size_t min_count() const noexcept = 0;

    /**
     * Returns the maximum number of values consumed by this parameter.
     *
     * @returns Maximum value count.
     */
    [[nodiscard]] virtual std::size_t max_count() const noexcept = 0;

    /**
     * Parses positional parameter values.
     *
     * @param values  Value texts to parse.
     *
     * @returns Parsed values stored in type-erased form.
     *
     * @throws parse_error  When a value is syntactically invalid or a required value is missing.
     */
    [[nodiscard]] virtual std::any parse_values(std::span<std::string_view const> values) const = 0;

private:
    /** Placeholder name used by help output and parse errors. */
    std::string name_;

    /** Optional human-readable description. */
    std::optional<std::string> description_;
};

} // namespace kaycxx::cli
