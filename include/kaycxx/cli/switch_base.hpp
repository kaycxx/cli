// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

/**
 * @file
 * Defines the common base class for flags and options.
 */

#pragma once

#include <any>
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
    std::string const& name() const noexcept;

    /**
     * Returns the short switch alias.
     *
     * @returns Short alias without the leading `-` or an empty optional if no alias is configured.
     */
    std::optional<char> alias() const noexcept;

    /**
     * Returns the switch description.
     *
     * @returns Human-readable description or an empty optional if no description was configured.
     */
    std::optional<std::string> const& description() const noexcept;

    /** Marks this switch as a mutually exclusive command action. */
    void mark_as_action() noexcept;

    /**
     * Checks whether this switch is a command action.
     *
     * @returns True when the switch is an action, false otherwise.
     */
    bool is_action() const noexcept;

    /**
     * Returns generated help usage for this switch.
     *
     * @returns Usage text.
     */
    virtual std::string usage() const = 0;

    /**
     * Checks whether this switch expects a value.
     *
     * @returns True for options, false for flags.
     */
    virtual bool expects_value() const noexcept = 0;

    /**
     * Parses a switch value.
     *
     * @param text  Value text to parse.
     *
     * @returns Parsed value stored in type-erased form.
     *
     * @throws parse_error        When the value is syntactically invalid.
     * @throws std::logic_error   When called for a switch that does not accept values.
     */
    virtual std::any parse_value(std::string_view text) const = 0;

private:
    std::string name_;
    std::optional<char> alias_;
    std::optional<std::string> description_;
    bool action_ = false;
};

} // namespace kaycxx::cli
