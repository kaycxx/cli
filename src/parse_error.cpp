// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/cli/parse_error.hpp>

#include <string>

namespace kaycxx::cli {

parse_error::parse_error(std::string_view message)
    : std::runtime_error(std::string(message))
{}

} // namespace kaycxx::cli
