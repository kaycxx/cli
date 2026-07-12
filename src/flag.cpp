// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/cli/flag.hpp>

#include <format>

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

} // namespace kaycxx::cli
