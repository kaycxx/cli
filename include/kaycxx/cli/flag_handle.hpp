// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

/**
 * @file
 * Defines the flag handle type.
 */

#pragma once

#include <kaycxx/cli/switch_base.hpp>

namespace kaycxx::cli {

class command;

/**
 * Handle for a registered flag.
 *
 * The handle does not own the flag definition. It references data owned by the command that created it.
 */
class flag_handle {
public:
    /**
     * Marks the flag as a mutually exclusive command action.
     *
     * @returns This handle for chaining.
     */
    flag_handle action() noexcept {
        definition_->mark_as_action();
        return *this;
    }

    /**
     * Returns the underlying flag definition.
     *
     * @returns Flag definition owned by the command.
     */
    switch_base const& definition() const noexcept {
        return *definition_;
    }

private:
    friend class command;

    explicit flag_handle(switch_base& definition) noexcept
        : definition_(&definition)
    {}

    switch_base* definition_;
};

} // namespace kaycxx::cli
