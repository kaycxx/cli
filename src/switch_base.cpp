// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/cli/switch_base.hpp>

#include <optional>

namespace kaycxx::cli {

switch_base::switch_base(std::string_view name, std::optional<std::string_view> description)
    : name_(name),
      alias_(std::nullopt)
{
    if (description) {
        description_ = std::string(*description);
    }
}

switch_base::switch_base(std::string_view name, char alias, std::optional<std::string_view> description)
    : name_(name),
      alias_(alias)
{
    if (description) {
        description_ = std::string(*description);
    }
}

std::string const& switch_base::name() const noexcept {
    return name_;
}

std::optional<char> switch_base::alias() const noexcept {
    return alias_;
}

std::optional<std::string> const& switch_base::description() const noexcept {
    return description_;
}

void switch_base::mark_as_action() noexcept {
    action_ = true;
}

bool switch_base::is_action() const noexcept {
    return action_;
}

} // namespace kaycxx::cli
