// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

/**
 * @file
 * Defines the flag handle type.
 */

#pragma once

namespace kaycxx::cli {

class command;
class switch_base;

/**
 * Handle for a registered flag.
 *
 * The handle does not own the flag definition. It references data owned by the command that created it.
 */
class flag_handle {
public:
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

    explicit flag_handle(switch_base const& definition) noexcept
        : definition_(&definition)
    {}

    switch_base const* definition_;
};

} // namespace kaycxx::cli
