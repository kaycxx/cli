// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

/**
 * @file
 * Defines the repeatable option handle type.
 */

#pragma once

#include <utility>

#include <kaycxx/cli/option.hpp>

namespace kaycxx::cli {

/**
 * Handle for a registered repeatable option.
 *
 * The handle does not own the option definition. It references data owned by the command that created it.
 *
 * @tparam T  Option value type.
 */
template <parseable_value T>
class repeatable_option_handle {
public:
    /**
     * Marks the option as a mutually exclusive command action.
     *
     * @returns This handle for chaining.
     */
    repeatable_option_handle action() noexcept {
        definition_->mark_as_action();
        return *this;
    }

    /**
     * Returns the underlying option definition.
     *
     * @returns Option definition owned by the command.
     */
    option<T> const& definition() const noexcept {
        return *definition_;
    }

    /**
     * Sets a default option value.
     *
     * When the option is absent, the value is returned as the only element of its result vector.
     *
     * @param value  Default value used when the option is not present in parsed arguments.
     * @returns This handle for chaining.
     */
    repeatable_option_handle default_value(T value) {
        definition_->default_value(std::move(value));
        return *this;
    }

private:
    friend class command;

    explicit repeatable_option_handle(option<T>& definition) noexcept
        : definition_(&definition)
    {}

    option<T>* definition_;
};

} // namespace kaycxx::cli
