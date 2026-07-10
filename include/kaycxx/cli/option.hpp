// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

/**
 * @file
 * Defines command line options.
 */

#pragma once

#include <any>
#include <format>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include <kaycxx/cli/parse_value.hpp>
#include <kaycxx/cli/switch_base.hpp>

namespace kaycxx::cli {

/**
 * Common base class for typed options.
 */
class option_base : public switch_base {
public:
    using switch_base::switch_base;

    /**
     * Returns the value placeholder name.
     *
     * @returns Placeholder name used by generated help output and parse errors.
     */
    virtual std::string const& value_name() const noexcept = 0;

    /**
     * Returns the configured default value.
     *
     * @returns Default value stored in type-erased form or an empty optional if no default value is configured.
     */
    virtual std::optional<std::any> default_value() const = 0;
};

/**
 * Typed command line option definition.
 *
 * @tparam T  Parsed option value type.
 */
template <parseable_value T>
class option : public option_base {
public:
    /**
     * Creates an option without a short alias.
     *
     * @param name         Long option name without the leading `--`.
     * @param value_name   Placeholder name for the option value used by generated help output.
     * @param description  Human-readable option description used by generated help output.
     */
    explicit option(std::string_view name, std::string_view value_name, std::string_view description)
        : option_base(name, description),
          value_name_(value_name) {}

    /**
     * Creates an option with a short alias.
     *
     * @param name         Long option name without the leading `--`.
     * @param alias        Short option alias without the leading `-`.
     * @param value_name   Placeholder name for the option value used by generated help output.
     * @param description  Human-readable option description used by generated help output.
     */
    explicit option(std::string_view name, char alias, std::string_view value_name, std::string_view description)
        : option_base(name, alias, description),
          value_name_(value_name) {}

    /**
     * Returns the value placeholder name.
     *
     * @returns Placeholder name used by generated help output and parse errors.
     */
    std::string const& value_name() const noexcept override {
        return value_name_;
    }

    /**
     * Sets the default option value.
     *
     * @param value  Default value used when the option is not present in parsed arguments.
     */
    void default_value(T value) {
        default_value_ = std::move(value);
    }

    /**
     * Returns generated help usage for this option.
     *
     * @returns Usage text.
     */
    std::string usage() const override {
        if (auto alias = this->alias()) {
            return std::format("-{}, --{} <{}>", *alias, name(), value_name_);
        }

        return std::format("    --{} <{}>", name(), value_name_);
    }

    /**
     * Checks whether this option expects a value.
     *
     * @returns Always true.
     */
    bool expects_value() const noexcept override {
        return true;
    }

    /**
     * Returns the configured default value.
     *
     * @returns Default value stored in type-erased form or an empty optional if no default value is configured.
     */
    std::optional<std::any> default_value() const override {
        if (default_value_) {
            return std::any(*default_value_);
        }

        return std::nullopt;
    }

    /**
     * Parses an option value.
     *
     * @param text  Value text to parse.
     *
     * @returns Parsed value stored in type-erased form.
     *
     * @throws parse_error  When the value is syntactically invalid.
     */
    std::any parse_value(std::string_view text) const override {
        return detail::parse_value<T>(text);
    }

private:
    std::string value_name_;
    std::optional<T> default_value_;
};

} // namespace kaycxx::cli
