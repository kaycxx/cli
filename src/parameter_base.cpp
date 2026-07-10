// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/cli/parameter_base.hpp>

namespace kaycxx::cli {

parameter_base::parameter_base(std::string_view name, std::string_view description)
    : name_(name),
      description_(description)
{}

std::string const& parameter_base::name() const noexcept {
    return name_;
}

std::string const& parameter_base::description() const noexcept {
    return description_;
}

} // namespace kaycxx::cli
