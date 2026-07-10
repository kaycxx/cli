// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

#include <initializer_list>
#include <string>
#include <string_view>
#include <vector>

#include <kaycxx/cli/command.hpp>
#include <kaycxx/cli/parse_result.hpp>

namespace kaycxx::cli::test_support {

inline parse_result parse_arguments(command& app, std::initializer_list<std::string_view> arguments);

/** Parses a compact list of command-line arguments for a test. */
inline parse_result parse_arguments(command& app, std::initializer_list<std::string_view> arguments) {
    auto values = std::vector<std::string>();
    values.reserve(arguments.size() + 1);
    values.push_back(app.name());
    for (auto argument : arguments) {
        values.emplace_back(argument);
    }

    auto argv = std::vector<char*>();
    argv.reserve(values.size());
    for (auto& value : values) {
        argv.push_back(value.data());
    }

    return app.parse(static_cast<int>(argv.size()), argv.data());
}

} // namespace kaycxx::cli::test_support
