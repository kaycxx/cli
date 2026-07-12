// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

/**
 * @file
 * Defines the positional parameter handle type.
 */

#pragma once

#include <utility>

#include <kaycxx/cli/parameter.hpp>

namespace kaycxx::cli {

/**
 * Handle for a registered positional parameter.
 *
 * The handle does not own the parameter definition. It references data owned by the command that created it.
 *
 * @tparam T  Parameter value type.
 */
template <parseable_value T>
class parameter_handle {
public:
    /**
     * Returns the underlying parameter definition.
     *
     * @returns Parameter definition owned by the command.
     */
    [[nodiscard]] parameter<T> const& definition() const noexcept {
        return *definition_;
    }

    /**
     * Sets the default parameter value.
     *
     * @param value  Default value used when the positional parameter is not present in parsed arguments.
     *
     * @returns This handle for chaining.
     */
    parameter_handle default_value(T value) {
        definition_->default_value(std::move(value));
        return *this;
    }

private:
    friend class command;

    /**
     * Creates a handle for a command-owned parameter definition.
     *
     * @param definition  Parameter definition to reference.
     */
    explicit parameter_handle(parameter<T>& definition) noexcept
        : definition_(&definition)
    {}

    /** Non-owning pointer to the definition owned by the command. */
    parameter<T>* definition_;
};

} // namespace kaycxx::cli
