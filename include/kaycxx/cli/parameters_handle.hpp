// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

/**
 * @file
 * Defines the positional parameter list handle type.
 */

#pragma once

#include <cstddef>
#include <initializer_list>
#include <utility>
#include <vector>

#include <kaycxx/cli/parameters.hpp>

namespace kaycxx::cli {

/**
 * Handle for a registered positional parameter list.
 *
 * The handle does not own the parameter definition. It references data owned by the command that created it.
 *
 * @tparam T  Parameter value type.
 */
template <parseable_value T>
class parameters_handle {
public:
    /**
     * Returns the underlying parameter list definition.
     *
     * @returns Parameter list definition owned by the command.
     */
    [[nodiscard]] parameters<T> const& definition() const noexcept {
        return *definition_;
    }

    /**
     * Sets the minimum number of values accepted by the parameter list.
     *
     * @param count  Minimum number of values.
     *
     * @returns This handle for chaining.
     */
    parameters_handle min(std::size_t count) noexcept {
        definition_->min(count);
        return *this;
    }

    /**
     * Sets the maximum number of values accepted by the parameter list.
     *
     * @param count  Maximum number of values.
     *
     * @returns This handle for chaining.
     */
    parameters_handle max(std::size_t count) noexcept {
        definition_->max(count);
        return *this;
    }

    /**
     * Sets default values for the parameter list.
     *
     * The value count is checked against the configured minimum and maximum when positional parameters are validated.
     *
     * @param values  Default values used when the parameter list is not present in parsed arguments.
     *
     * @returns This handle for chaining.
     */
    parameters_handle default_values(std::vector<T> values) {
        definition_->default_values(std::move(values));
        return *this;
    }

    /**
     * Sets default values for the parameter list.
     *
     * The value count is checked against the configured minimum and maximum when positional parameters are validated.
     *
     * @param values  Default values used when the parameter list is not present in parsed arguments.
     *
     * @returns This handle for chaining.
     */
    parameters_handle default_values(std::initializer_list<T> values) {
        definition_->default_values(std::vector<T>(values));
        return *this;
    }

private:
    friend class command;

    /**
     * Creates a handle for a command-owned parameter list definition.
     *
     * @param definition  Parameter list definition to reference.
     */
    explicit parameters_handle(parameters<T>& definition) noexcept
        : definition_(&definition)
    {}

    /** Non-owning pointer to the definition owned by the command. */
    parameters<T>* definition_;
};

} // namespace kaycxx::cli
