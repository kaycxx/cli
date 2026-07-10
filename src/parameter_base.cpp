// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/cli/parameter_base.hpp>

namespace kaycxx::cli {

parameter_base::parameter_base(std::string_view name, std::optional<std::string_view> description)
    : name_(name)
{
    if (description) {
        description_ = std::string(*description);
    }
}

std::string const& parameter_base::name() const noexcept {
    return name_;
}

std::optional<std::string> const& parameter_base::description() const noexcept {
    return description_;
}

} // namespace kaycxx::cli
