// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/cli/command.hpp>

#include <algorithm>
#include <cstddef>
#include <format>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include <kaycxx/cli/parse_error.hpp>

#include "detail/string.hpp"

namespace kaycxx::cli {

namespace {

/**
 * Formats a long option label.
 *
 * @param name  Long option name without the leading `--`.
 *
 * @returns Option label including the leading `--`.
 */
std::string long_option_label(std::string_view name) {
    return std::format("--{}", name);
}

/**
 * Formats a short option label.
 *
 * @param alias  Short option alias without the leading `-`.
 *
 * @returns Option label including the leading `-`.
 */
std::string short_option_label(char alias) {
    return std::format("-{}", alias);
}

/**
 * Formats an error for an option without a following value.
 *
 * @param label   Option label used on the command line.
 * @param option  Option definition whose value is missing.
 *
 * @returns Missing-value error message.
 */
std::string missing_option_value_message(std::string_view label, option_base const& option) {
    return std::format("Missing value <{}> for option {}", option.value_name(), label);
}

/**
 * Adds an option label to a value parsing error.
 *
 * @param label  Option label used on the command line.
 * @param error  Value parsing error.
 *
 * @returns Error message identifying the affected option.
 */
std::string invalid_option_value_message(std::string_view label, parse_error const& error) {
    return std::format("{} for option {}", error.what(), label);
}

/**
 * Records an explicitly used action and rejects a conflicting action.
 *
 * @param item             Parsed switch.
 * @param selected_action  Previously selected action, updated when item is an action.
 *
 * @throws parse_error  When a different action was already selected.
 */
void record_action(switch_base const& item, switch_base const*& selected_action) {
    if (!item.is_action()) {
        return;
    }
    if (selected_action != nullptr && selected_action != &item) {
        throw parse_error(std::format("Options --{} and --{} cannot be combined", selected_action->name(), item.name()));
    }
    selected_action = &item;
}

/**
 * Copies optional command metadata into owned storage.
 *
 * @param value  Optional text view to copy.
 *
 * @returns Owned optional string.
 */
std::optional<std::string> copy_option(std::optional<std::string_view> value) {
    if (value) {
        return std::string(*value);
    }

    return std::nullopt;
}

} // namespace

command::command(std::string_view name, command_options options)
    : name_(name),
      version_(copy_option(options.version)),
      author_(copy_option(options.author)),
      email_(copy_option(options.email)),
      bugs_(copy_option(options.bugs)),
      copyright_(copy_option(options.copyright)),
      license_(copy_option(options.license)),
      description_(copy_option(options.description))
{}

std::string const& command::name() const noexcept {
    return name_;
}

std::optional<std::string> const& command::version() const noexcept {
    return version_;
}

std::optional<std::string> const& command::author() const noexcept {
    return author_;
}

std::optional<std::string> const& command::email() const noexcept {
    return email_;
}

std::optional<std::string> const& command::bugs() const noexcept {
    return bugs_;
}

std::optional<std::string> const& command::license() const noexcept {
    return license_;
}

std::optional<std::string> const& command::copyright() const noexcept {
    return copyright_;
}

std::optional<std::string> const& command::description() const noexcept {
    return description_;
}

cli::flag_handle command::flag(std::string_view name, std::optional<std::string_view> description) {
    auto item = std::make_unique<cli::flag>(name, description);
    auto& definition = *item;
    add_switch(std::move(item));
    return cli::flag_handle(definition);
}

cli::flag_handle command::flag(std::string_view name, char alias, std::optional<std::string_view> description) {
    auto item = std::make_unique<cli::flag>(name, alias, description);
    auto& definition = *item;
    add_switch(std::move(item));
    return cli::flag_handle(definition);
}

void command::add_switch(std::unique_ptr<switch_base> item) {
    if (find_switch(item->name()) != nullptr) {
        throw std::invalid_argument(std::format("Duplicate switch --{}", item->name()));
    }
    if (auto alias = item->alias(); alias && find_switch(*alias) != nullptr) {
        throw std::invalid_argument(std::format("Duplicate switch -{}", *alias));
    }
    switches_.push_back(std::move(item));
}

switch_base const* command::find_switch(std::string_view name) const noexcept {
    for (auto const& item : switches_) {
        if (item->name() == name) {
            return item.get();
        }
    }
    return nullptr;
}

switch_base const* command::find_switch(char alias) const noexcept {
    for (auto const& item : switches_) {
        if (item->alias() == alias) {
            return item.get();
        }
    }
    return nullptr;
}

cli::args command::parse(int argc, char *argv[]) const {
    auto result = args();
    auto selected_action = static_cast<switch_base const*>(nullptr);
    result.parameter_definitions_.reserve(parameters_.size());
    for (auto const& parameter : parameters_) {
        result.parameter_definitions_.push_back(parameter.get());
    }

    auto record_option_value = [&](switch_base const& item, std::string_view label, std::string_view text) {
        auto const& option = static_cast<option_base const&>(item);
        if (result.values_.contains(&item) && !option.is_repeatable()) {
            throw parse_error(std::format("Option {} cannot be specified more than once", label));
        }

        try {
            if (option.is_repeatable()) {
                option.append_value(result.values_[&item], text);
            } else {
                result.values_[&item] = option.parse_value(text);
            }
        } catch (parse_error const& error) {
            throw parse_error(invalid_option_value_message(label, error));
        }
    };

    for (auto i = 1; i < argc; ++i) {
        auto arg = std::string_view(argv[i]);

        if (arg == "--") {
            // All arguments after `--` separator are positional arguments, even if they look like options or flags
            while (++i < argc) {
                result.positional_values_.emplace_back(argv[i]);
            }
            break;
        }

        // Parse long option
        if (arg.starts_with("--")) {
            auto name = arg.substr(2);
            auto value = std::optional<std::string_view>();

            // Split option name and value when concatenated with '='
            auto pos = name.find('=');
            if (pos != std::string_view::npos) {
                value = name.substr(pos + 1);
                name = name.substr(0, pos);
            }

            // Search the option by long name
            auto* item = find_switch(name);
            if (item == nullptr) {
                throw parse_error(std::format("Unknown option {}", long_option_label(name)));
            }

            if (item->expects_value()) {
                auto const label = long_option_label(name);

                // When option needs a value but it wasn't already parsed from `=` assignment then read it from next argument
                if (!value) {
                    if (i + 1 >= argc) {
                        throw parse_error(missing_option_value_message(label, static_cast<option_base const&>(*item)));
                    }
                    value = std::string_view(argv[++i]);
                }

                // Record option value
                record_option_value(*item, label, *value);
            } else {
                // Make sure no value was set when option is just a flag
                if (value) {
                    throw parse_error(std::format("Option {} does not accept a value", long_option_label(name)));
                }

                // Record flag
                result.flags_.insert(item);
            }
            record_action(*item, selected_action);
            continue;
        }

        // Parse short option (alias)
        if (arg.starts_with("-") && arg.size() > 1) {
            if (arg.size() != 2) {
                throw parse_error(std::format("Unknown option {}", arg));
            }

            // Search the option by short name
            auto item = find_switch(arg[1]);
            if (item == nullptr) {
                throw parse_error(std::format("Unknown option {}", short_option_label(arg[1])));
            }

            if (item->expects_value()) {
                auto const label = short_option_label(arg[1]);

                // When option needs a value then read it from next argument
                if (i + 1 >= argc) {
                    throw parse_error(missing_option_value_message(label, static_cast<option_base const&>(*item)));
                }

                // Record option value
                record_option_value(*item, label, argv[++i]);
            } else {
                // Record flag
                result.flags_.insert(item);
            }
            record_action(*item, selected_action);
            continue;
        }

        // Not an option or flag, so it is a positional argument
        result.positional_values_.emplace_back(arg);
    }

    apply_option_defaults(result);
    return result;
}

void command::apply_option_defaults(args& result) const {
    for (auto const& item : switches_) {
        if (!item->expects_value() || result.values_.contains(item.get())) {
            continue;
        }

        auto const& option = static_cast<option_base const&>(*item);

        if (auto value = option.default_value()) {
            result.values_[item.get()] = std::move(*value);
        }
    }
}

int command::print_help() const {
    return print_help(std::cout);
}

bool command::has_described_switches() const noexcept {
    for (auto const& item : switches_) {
        if (item->description()) {
            return true;
        }
    }
    return false;
}

bool command::has_described_parameters() const noexcept {
    for (auto const& item : parameters_) {
        if (item->description()) {
            return true;
        }
    }
    return false;
}

std::size_t command::max_switch_usage_length() const {
    auto result = std::size_t(0);
    for (auto const& item : switches_) {
        if (item->description()) {
            result = std::max(result, item->usage().size());
        }
    }
    return result;
}

std::size_t command::max_parameter_usage_length() const {
    auto result = std::size_t(0);
    for (auto const& item : parameters_) {
        if (item->description()) {
            result = std::max(result, item->usage().size());
        }
    }
    return result;
}

int command::print_help(std::ostream& out) const {
    auto const terminal_width = detail::terminal_width(out);
    auto const available_line_length = terminal_width > 1 ? terminal_width - 1 : std::size_t(1);

    out << "Usage: " << name_;
    if (!switches_.empty()) {
        out << " [OPTION]...";
    }
    for (auto const& parameter : parameters_) {
        out << " " << parameter->usage();
    }
    out << '\n';
    if (description_.has_value()) {
        out << detail::wrap(*description_, available_line_length) << '\n';
    }
    if (has_described_switches()) {
        auto const max_usage_length = max_switch_usage_length();
        auto const indent = max_usage_length + 5;
        auto const line_length = available_line_length > indent ? available_line_length - indent : std::size_t(1);
        out << '\n';
        for (auto const& item : switches_) {
            if (auto& description = item->description()) {
                auto const usage = item->usage();
                out << "  " << usage << std::string(max_usage_length - usage.size() + 3, ' ') << detail::wrap(*description, line_length, indent) << '\n';
            }
        }
    }
    if (has_described_parameters()) {
        auto const max_usage_length = max_parameter_usage_length();
        auto const indent = max_usage_length + 5;
        auto const line_length = available_line_length > indent ? available_line_length - indent : std::size_t(1);
        out << '\n';
        for (auto const& item : parameters_) {
            if (auto& description = item->description()) {
                auto const usage = item->usage();
                out << "  " << usage << std::string(max_usage_length - usage.size() + 3, ' ') << detail::wrap(*description, line_length, indent) << '\n';
            }
        }
    }
    if (bugs_.has_value()) {
        out << '\n';
        out << "Report bugs to <" << *bugs_ << ">\n";
    }
    return 0;
}

int command::print_version() const {
    return print_version(std::cout);
}

int command::print_version(std::ostream& out) const {
    out << name_;
    if (version_.has_value()) {
         out << ' ' << *version_;
    }
    out << '\n';
    auto const terminal_width = detail::terminal_width(out);
    if (copyright_.has_value() || license_.has_value()) {
        out << '\n';
        if (copyright_.has_value()) {
             out << detail::wrap(*copyright_, terminal_width) << '\n';
        }
        if (license_.has_value()) {
            out << detail::wrap(*license_, terminal_width) << '\n';
        }
    }
    if (author_.has_value() || email_.has_value()) {
        out << '\n' << "Written by";
        if (author_.has_value()) {
            out << ' ' << *author_;
        }
        if (email_.has_value()) {
            out << " <" << *email_ << '>';
        }
        out << '\n';
    }
    return 0;
}

} // namespace kaycxx::cli
