// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

/**
 * @file
 * Defines the common base class for flags and options.
 */

#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace kaycxx::cli {

/**
 * Common base class for command line switches.
 *
 * A switch is either a flag without a value or an option with a value.
 */
class switch_base {
public:
    /**
     * Creates a switch without a short alias.
     *
     * @param name         Long switch name without the leading `--`.
     * @param description  Optional human-readable description used by generated help output.
     */
    explicit switch_base(std::string_view name, std::optional<std::string_view> description = std::nullopt);

    /**
     * Creates a switch with a short alias.
     *
     * @param name         Long switch name without the leading `--`.
     * @param alias        Short switch alias without the leading `-`.
     * @param description  Optional human-readable description used by generated help output.
     */
    explicit switch_base(std::string_view name, char alias, std::optional<std::string_view> description = std::nullopt);

    /** Destroys the switch definition. */
    virtual ~switch_base() = default;

    /**
     * Returns the long switch name.
     *
     * @returns Long switch name without the leading `--`.
     */
    [[nodiscard]] std::string const& name() const noexcept;

    /**
     * Returns the short switch alias.
     *
     * @returns Short alias without the leading `-` or an empty optional if no alias is configured.
     */
    [[nodiscard]] std::optional<char> alias() const noexcept;

    /**
     * Returns the switch description.
     *
     * @returns Human-readable description or an empty optional if no description was configured.
     */
    [[nodiscard]] std::optional<std::string> const& description() const noexcept;

    /** Marks this switch as a mutually exclusive command action. */
    void mark_as_action() noexcept;

    /**
     * Checks whether this switch is a command action.
     *
     * @returns True when the switch is an action, false otherwise.
     */
    [[nodiscard]] bool is_action() const noexcept;

    /**
     * Returns generated help usage for this switch.
     *
     * @returns Usage text.
     */
    [[nodiscard]] virtual std::string usage() const = 0;

    /**
     * Checks whether this switch expects a value.
     *
     * @returns True for options, false for flags.
     */
    [[nodiscard]] virtual bool expects_value() const noexcept = 0;

private:
    /** Long switch name without the leading `--`. */
    std::string name_;

    /** Optional short switch alias without the leading `-`. */
    std::optional<char> alias_;

    /** Optional human-readable description. */
    std::optional<std::string> description_;

    /** Whether the switch is a mutually exclusive command action. */
    bool action_ = false;
};

} // namespace kaycxx::cli
