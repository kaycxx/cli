// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

/**
 * @file
 * Defines access to parsed command line arguments.
 */

#pragma once

#include <any>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <kaycxx/cli/flag_handle.hpp>
#include <kaycxx/cli/option_handle.hpp>
#include <kaycxx/cli/parameter_base.hpp>
#include <kaycxx/cli/parameter_handle.hpp>
#include <kaycxx/cli/parameters_handle.hpp>
#include <kaycxx/cli/repeatable_option_handle.hpp>
#include <kaycxx/cli/switch_base.hpp>

namespace kaycxx::cli {

/**
 * Parsed command line arguments.
 *
 * Instances are returned directly by command::parse(). Values are queried with the handles returned by command registration methods.
 *
 * Parsed arguments have unique ownership and can be moved but not copied.
 *
 * Positional parameters are validated and converted lazily when first accessed or when validate() is called explicitly.
 *
 * The command that created an instance must outlive it and all handles used to access it.
 */
class args {
public:
    /** Creates an empty argument collection. */
    args() = default;

    args(args const&) = delete;
    args& operator=(args const&) = delete;

    /** Moves parsed arguments. */
    args(args&&) noexcept = default;

    /** Moves parsed arguments. */
    args& operator=(args&&) noexcept = default;

    /**
     * Validates and converts all positional parameters.
     *
     * Calling this method repeatedly has no effect after the first successful validation.
     *
     * @throws parse_error  When a required positional parameter is missing, an unexpected positional argument is present, or conversion fails.
     */
    void validate() const;

    /**
     * Checks whether a flag was set.
     *
     * @param flag  Flag handle returned by command::flag().
     *
     * @returns True if the flag was present in the parsed arguments, false otherwise.
     */
    bool get(flag_handle const& flag) const;

    /**
     * Returns a parsed option value.
     *
     * @tparam T  Option value type.
     *
     * @param option  Option handle returned by command::option().
     *
     * @returns Parsed option value.
     */
    template <parseable_value T>
    T const& get(option_handle<T> const& option) const {
        return std::any_cast<T const&>(values_.at(&option.definition()));
    }

    /**
     * Checks whether an option value is available.
     *
     * @tparam T  Option value type.
     *
     * @param option  Option handle returned by command::option().
     *
     * @returns True if the option was provided or has a default value, false otherwise.
     */
    template <parseable_value T>
    bool has(option_handle<T> const& option) const {
        return values_.contains(&option.definition());
    }

    /**
     * Returns all parsed values of a repeatable option.
     *
     * @tparam T  Option value type.
     *
     * @param option  Repeatable option handle returned by command::repeatable_option().
     * @returns Parsed option values in command-line order.
     */
    template <parseable_value T>
    std::vector<T> const& get(repeatable_option_handle<T> const& option) const {
        return std::any_cast<std::vector<T> const&>(values_.at(&option.definition()));
    }

    /**
     * Checks whether values for a repeatable option are available.
     *
     * @tparam T  Option value type.
     *
     * @param option  Repeatable option handle returned by command::repeatable_option().
     * @returns True if the option was provided or has a default value, false otherwise.
     */
    template <parseable_value T>
    bool has(repeatable_option_handle<T> const& option) const {
        return values_.contains(&option.definition());
    }

    /**
     * Returns a parsed positional parameter value.
     *
     * @tparam T  Parameter value type.
     *
     * @param parameter  Parameter handle returned by command::parameter().
     *
     * @returns Parsed parameter value.
     *
     * @throws parse_error  When positional parameter validation fails.
     */
    template <parseable_value T>
    T const& get(parameter_handle<T> const& parameter) const {
        validate();
        return std::any_cast<T const&>(parameters_.at(&parameter.definition()));
    }

    /**
     * Checks whether a positional parameter value is available.
     *
     * @tparam T  Parameter value type.
     *
     * @param parameter  Parameter handle returned by command::parameter().
     *
     * @returns True if the parameter was provided or has a default value, false otherwise.
     *
     * @throws parse_error  When positional parameter validation fails.
     */
    template <parseable_value T>
    bool has(parameter_handle<T> const& parameter) const {
        validate();
        return parameters_.contains(&parameter.definition());
    }

    /**
     * Returns parsed positional parameter list values.
     *
     * @tparam T  Parameter value type.
     *
     * @param parameters  Parameter list handle returned by command::parameters().
     *
     * @returns Parsed parameter values.
     *
     * @throws parse_error  When positional parameter validation fails.
     */
    template <parseable_value T>
    std::vector<T> const& get(parameters_handle<T> const& parameters) const {
        validate();
        return std::any_cast<std::vector<T> const&>(parameters_.at(&parameters.definition()));
    }

    /**
     * Checks whether positional parameter list values are available.
     *
     * @tparam T  Parameter value type.
     *
     * @param parameters  Parameter list handle returned by command::parameters().
     *
     * @returns True if the parameter list was provided or has default values, false otherwise.
     *
     * @throws parse_error  When positional parameter validation fails.
     */
    template <parseable_value T>
    bool has(parameters_handle<T> const& parameters) const {
        validate();
        return parameters_.contains(&parameters.definition());
    }

private:
    friend class command;

    std::unordered_set<switch_base const*> flags_;
    std::unordered_map<switch_base const*, std::any> values_;
    std::vector<parameter_base const*> parameter_definitions_;
    std::vector<std::string> positional_values_;
    mutable std::unordered_map<parameter_base const*, std::any> parameters_;
    mutable bool validated_ = false;
};

} // namespace kaycxx::cli
