// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/cli/switch_base.hpp>

#include <optional>

namespace kaycxx::cli {

switch_base::switch_base(std::string_view name, std::string_view description)
    : name_(name),
      alias_(std::nullopt),
      description_(description)
{}

switch_base::switch_base(std::string_view name, char alias, std::string_view description)
    : name_(name),
      alias_(alias),
      description_(description)
{}

std::string const& switch_base::name() const noexcept {
    return name_;
}

std::optional<char> switch_base::alias() const noexcept {
    return alias_;
}

std::string const& switch_base::description() const noexcept {
    return description_;
}

} // namespace kaycxx::cli
