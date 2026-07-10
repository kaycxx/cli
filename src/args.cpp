// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/cli/args.hpp>

namespace kaycxx::cli {

bool args::get(flag_handle const& flag) const {
    return flags_.contains(&flag.definition());
}

} // namespace kaycxx::cli
