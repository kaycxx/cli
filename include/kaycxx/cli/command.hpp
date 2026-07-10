// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

/**
 * @file
 * Defines the command class and command metadata.
 */

#pragma once

#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <kaycxx/cli/args.hpp>
#include <kaycxx/cli/flag.hpp>
#include <kaycxx/cli/flag_handle.hpp>
#include <kaycxx/cli/option.hpp>
#include <kaycxx/cli/option_handle.hpp>
#include <kaycxx/cli/parameter.hpp>
#include <kaycxx/cli/parameter_base.hpp>
#include <kaycxx/cli/parameter_handle.hpp>
#include <kaycxx/cli/parameters.hpp>
#include <kaycxx/cli/parameters_handle.hpp>
#include <kaycxx/cli/parse_result.hpp>
#include <kaycxx/cli/switch_base.hpp>

namespace kaycxx::cli {

/**
 * Optional metadata for a command.
 */
struct command_options {
    /** Command version string used by version output. */
    std::optional<std::string_view> version;

    /** Short command description used by generated help output. */
    std::optional<std::string_view> description;

    /** Author name used by generated version output. */
    std::optional<std::string_view> author;

    /** Author contact address used by generated version output. */
    std::optional<std::string_view> email;

    /** Copyright notice used by generated version output. */
    std::optional<std::string_view> copyright;

    /** License text used by generated version output. */
    std::optional<std::string_view> license;
};

/**
 * Defines and parses a command line interface.
 *
 * A command owns all registered flags, options and positional parameters. Handles returned from registration methods are lightweight references to these owned definitions and
 * remain valid as long as the command object exists.
 */
class command {
public:
    /**
     * Creates a command definition.
     *
     * @param name     Program name used in generated output and error messages.
     * @param options  Optional command metadata.
     */
    explicit command(std::string_view name, command_options options = command_options());

    command(command const&) = delete;
    command& operator=(command const&) = delete;

    /** Moves a command definition. */
    command(command&&) noexcept = default;

    /** Moves a command definition. */
    command& operator=(command&&) noexcept = default;

    /**
     * Returns the command name.
     *
     * @returns Command name.
     */
    std::string const& name() const noexcept;

    /**
     * Returns the configured command version.
     *
     * @returns Command version or an empty optional if no version was configured.
     */
    std::optional<std::string> const& version() const noexcept;

    /**
     * Returns the configured author name.
     *
     * @returns Author name or an empty optional if no author was configured.
     */
    std::optional<std::string> const& author() const noexcept;

    /**
     * Returns the configured author email address.
     *
     * @returns Email address or an empty optional if no email address was configured.
     */
    std::optional<std::string> const& email() const noexcept;

    /**
     * Returns the configured copyright notice.
     *
     * @returns Copyright notice or an empty optional if no copyright notice was configured.
     */
    std::optional<std::string> const& copyright() const noexcept;

    /**
     * Returns the configured license text.
     *
     * @returns License text or an empty optional if no license text was configured.
     */
    std::optional<std::string> const& license() const noexcept;

    /**
     * Returns the configured command description.
     *
     * @returns Command description or an empty optional if no description was configured.
     */
    std::optional<std::string> const& description() const noexcept;

    /**
     * Registers a flag without a short alias.
     *
     * @param name         Long flag name without the leading `--`.
     * @param description  Human-readable flag description used by generated help output.
     *
     * @returns Handle used to query the parsed flag state.
     */
    cli::flag_handle flag(std::string_view name, std::string_view description);

    /**
     * Registers a flag with a short alias.
     *
     * @param name         Long flag name without the leading `--`.
     * @param alias        Short flag alias without the leading `-`.
     * @param description  Human-readable flag description used by generated help output.
     *
     * @returns Handle used to query the parsed flag state.
     */
    cli::flag_handle flag(std::string_view name, char alias, std::string_view description);

    /**
     * Registers an option without a short alias.
     *
     * @tparam T  Parsed option value type.
     *
     * @param name         Long option name without the leading `--`.
     * @param value_name   Placeholder name for the option value used by generated help output.
     * @param description  Human-readable option description used by generated help output.
     *
     * @returns Handle used to configure the option and query the parsed option value.
     */
    template <parseable_value T>
    cli::option_handle<T> option(std::string_view name, std::string_view value_name, std::string_view description) {
        auto item = std::make_unique<cli::option<T>>(name, value_name, description);
        auto& definition = *item;
        switches_.push_back(std::move(item));
        return cli::option_handle<T>(definition);
    }

    /**
     * Registers an option with a short alias.
     *
     * @tparam T  Parsed option value type.
     *
     * @param name         Long option name without the leading `--`.
     * @param alias        Short option alias without the leading `-`.
     * @param value_name   Placeholder name for the option value used by generated help output.
     * @param description  Human-readable option description used by generated help output.
     *
     * @returns Handle used to configure the option and query the parsed option value.
     */
    template <parseable_value T>
    cli::option_handle<T> option(std::string_view name, char alias, std::string_view value_name, std::string_view description) {
        auto item = std::make_unique<cli::option<T>>(name, alias, value_name, description);
        auto& definition = *item;
        switches_.push_back(std::move(item));
        return cli::option_handle<T>(definition);
    }

    /**
     * Registers a single positional parameter.
     *
     * @tparam T  Parsed parameter value type.
     *
     * @param name         Placeholder name used by generated help output and parse errors.
     * @param description  Human-readable parameter description used by generated help output.
     *
     * @returns Handle used to configure the parameter and query the parsed parameter value.
     */
    template <parseable_value T>
    cli::parameter_handle<T> parameter(std::string_view name, std::string_view description) {
        auto item = std::make_unique<cli::parameter<T>>(name, description);
        auto& definition = *item;
        parameters_.push_back(std::move(item));
        return cli::parameter_handle<T>(definition);
    }

    /**
     * Registers a positional parameter list.
     *
     * @tparam T  Parsed parameter value type.
     *
     * @param name         Placeholder name used by generated help output and parse errors.
     * @param description  Human-readable parameter description used by generated help output.
     *
     * @returns Handle used to configure the parameter list and query the parsed values.
     */
    template <parseable_value T>
    cli::parameters_handle<T> parameters(std::string_view name, std::string_view description) {
        auto item = std::make_unique<cli::parameters<T>>(name, description);
        auto& definition = *item;
        parameters_.push_back(std::move(item));
        return cli::parameters_handle<T>(definition);
    }

    /**
     * Parses command line arguments.
     *
     * @param argc  Argument count as received by `main`.
     * @param argv  Argument values as received by `main`.
     *
     * @returns Parse result containing parsed arguments or a request for generated help/version output.
     *
     * @throws parse_error  When command line arguments are invalid.
     */
    parse_result parse(int argc, char *argv[]);

    /**
     * Writes generated help output to standard output.
     *
     * @returns Exit code suitable for returning from `main`.
     */
    int print_help() const;

    /**
     * Writes generated help output.
     *
     * @param out  Output stream receiving the generated help text.
     *
     * @returns Exit code suitable for returning from `main`.
     */
    int print_help(std::ostream& out) const;

    /**
     * Writes generated version output to standard output.
     *
     * @returns Exit code suitable for returning from `main`.
     */
    int print_version() const;

    /**
     * Writes generated version output.
     *
     * @param out  Output stream receiving the generated version text.
     *
     * @returns Exit code suitable for returning from `main`.
     */
    int print_version(std::ostream& out) const;

private:
    switch_base const* find_switch(std::string_view name) const noexcept;
    switch_base const* find_switch(char alias) const noexcept;
    void apply_option_defaults(args& result) const;
    void parse_parameters(args& result, std::vector<std::string_view> const& values) const;

    std::string name_;
    std::optional<std::string> version_;
    std::optional<std::string> author_;
    std::optional<std::string> email_;
    std::optional<std::string> copyright_;
    std::optional<std::string> license_;
    std::optional<std::string> description_;
    std::vector<std::unique_ptr<switch_base>> switches_;
    std::vector<std::unique_ptr<parameter_base>> parameters_;
};

} // namespace kaycxx::cli
