// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/cli/command.hpp>

#include "detail/string.hpp"

#include <kaycxx/cli/parse_error.hpp>

#include <algorithm>
#include <cstddef>
#include <format>
#include <iostream>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace kaycxx::cli {

namespace {

std::string long_option_label(std::string_view name) {
    return std::format("--{}", name);
}

std::string short_option_label(char alias) {
    return std::format("-{}", alias);
}

std::string missing_option_value_message(std::string_view label, switch_base const& item) {
    auto const* option = dynamic_cast<option_base const*>(&item);
    if (option == nullptr) {
        return std::format("Missing value for option {}", label);
    }

    return std::format("Missing value <{}> for option {}", option->value_name(), label);
}

std::string invalid_option_value_message(std::string_view label, parse_error const& error) {
    return std::format("{} for option {}", error.what(), label);
}

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
    switches_.push_back(std::move(item));
    return cli::flag_handle(definition);
}

cli::flag_handle command::flag(std::string_view name, char alias, std::optional<std::string_view> description) {
    auto item = std::make_unique<cli::flag>(name, alias, description);
    auto& definition = *item;
    switches_.push_back(std::move(item));
    return cli::flag_handle(definition);
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

parse_result command::parse(int argc, char *argv[]) {
    auto result = args();
    auto positional = std::vector<std::string_view>();

    for (auto i = 1; i < argc; ++i) {
        auto arg = std::string_view(argv[i]);

        if (arg == "--") {
            // All arguments after `--` separator are positional arguments, even if they look like options or flags
            while (++i < argc) {
                positional.push_back(argv[i]);
            }
            break;
        }

        if (arg == "--help") {
            return parse_result::help();
        }
        if (arg == "--version") {
            return parse_result::version();
        }

        // Parse long option
        if (arg.starts_with("--")) {
            auto name = arg.substr(2);
            auto value = std::optional<std::string_view>();

            // Split open name and value when concatenated with '='
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
                        throw parse_error(missing_option_value_message(label, *item));
                    }
                    value = std::string_view(argv[++i]);
                }

                // Record option value
                try {
                    result.values_[item] = item->parse_value(*value);
                } catch (parse_error const& error) {
                    throw parse_error(invalid_option_value_message(label, error));
                }
            } else {
                // Make sure no value was set when option is just a flag
                if (value) {
                    throw parse_error(std::format("Option {} does not accept a value", long_option_label(name)));
                }

                // Record flag
                result.flags_.insert(item);
            }
            continue;
        }

        // Parse short option (alias)
        if (arg.starts_with("-") && arg.size() == 2) {
            // Search the option by short name
            auto item = find_switch(arg[1]);
            if (item == nullptr) {
                throw parse_error(std::format("Unknown option {}", short_option_label(arg[1])));
            }

            if (item->expects_value()) {
                auto const label = short_option_label(arg[1]);

                // When option needs a value then read it from next argument
                if (i + 1 >= argc) {
                    throw parse_error(missing_option_value_message(label, *item));
                }

                // Record option value
                try {
                    result.values_[item] = item->parse_value(argv[++i]);
                } catch (parse_error const& error) {
                    throw parse_error(invalid_option_value_message(label, error));
                }
            } else {
                // Record flag
                result.flags_.insert(item);
            }
            continue;
        }

        // Not an option or flag, so it is a positional argument
        positional.push_back(arg);
    }

    parse_parameters(result, positional);
    apply_option_defaults(result);
    return parse_result::parsed(std::move(result));
}

void command::apply_option_defaults(args& result) const {
    for (auto const& item : switches_) {
        if (!item->expects_value() || result.values_.contains(item.get())) {
            continue;
        }

        auto const* option = dynamic_cast<option_base const*>(item.get());
        if (option == nullptr) {
            continue;
        }

        if (auto value = option->default_value()) {
            result.values_[item.get()] = std::move(*value);
        }
    }
}

void command::parse_parameters(args& result, std::vector<std::string_view> const& values) const {
    auto position = std::size_t(0);

    for (auto i = std::size_t(0); i < parameters_.size(); ++i) {
        auto required_after = std::size_t(0);
        for (auto j = i + 1; j < parameters_.size(); ++j) {
            required_after += parameters_[j]->min_count();
        }

        auto const min_count = parameters_[i]->min_count();
        auto const max_count = std::max(parameters_[i]->max_count(), min_count);
        auto const remaining = position < values.size() ? values.size() - position : std::size_t(0);

        if (remaining < min_count) {
            throw parse_error(std::format("Missing parameter <{}>", parameters_[i]->name()));
        }

        auto const remaining_after_min = remaining - min_count;
        auto const available_extra = remaining_after_min > required_after ? remaining_after_min - required_after : std::size_t(0);
        auto const max_extra = max_count > min_count ? max_count - min_count : std::size_t(0);
        auto const take = min_count + std::min(available_extra, max_extra);

        auto const parameter_values = take == 0
            ? std::span<std::string_view const>()
            : std::span<std::string_view const>(values.data() + position, take);

        try {
            result.parameters_[parameters_[i].get()] = parameters_[i]->parse_values(parameter_values);
        } catch (parse_error const& error) {
            throw parse_error(std::format("{} for parameter <{}>", error.what(), parameters_[i]->name()));
        }
        position += take;
    }

    if (position < values.size()) {
        throw parse_error(std::format("Unexpected parameter {}", values[position]));
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
    if (version_.has_value()) {
        out << name_ << ' ' << *version_ << '\n';
    }
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
