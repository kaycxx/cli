// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/cli/parse_result.hpp>

#include <stdexcept>
#include <utility>

namespace kaycxx::cli {

parse_result parse_result::parsed(cli::args arguments) {
    return parse_result(type::parsed, std::move(arguments));
}

parse_result parse_result::help() {
    return parse_result(type::help);
}

parse_result parse_result::version() {
    return parse_result(type::version);
}

parse_result::parse_result(type result_type, cli::args arguments)
    : type_(result_type),
      args_(std::move(arguments))
{}

bool parse_result::help_requested() const noexcept {
    return type_ == type::help;
}

bool parse_result::version_requested() const noexcept {
    return type_ == type::version;
}

cli::args const& parse_result::args() const {
    if (type_ != type::parsed) {
        throw std::logic_error("Parsed arguments are not available");
    }

    return args_;
}

} // namespace kaycxx::cli
