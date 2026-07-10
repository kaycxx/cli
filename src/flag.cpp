// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/cli/flag.hpp>

#include <format>
#include <stdexcept>

namespace kaycxx::cli {

std::string flag::usage() const {
    if (auto alias = this->alias()) {
        return std::format("-{}, --{}", *alias, name());
    }

    return std::format("    --{}", name());
}

bool flag::expects_value() const noexcept {
    return false;
}

std::any flag::parse_value(std::string_view) const {
    throw std::logic_error("Flag does not accept a value");
}

} // namespace kaycxx::cli
