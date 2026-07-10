// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

/**
 * @file
 * Defines the command line parse error type.
 */

#pragma once

#include <stdexcept>
#include <string_view>

namespace kaycxx::cli {

/**
 * Error thrown when command line arguments cannot be parsed.
 */
class parse_error : public std::runtime_error {
public:
    /**
     * Creates a parse error.
     *
     * @param message  Error message.
     */
    explicit parse_error(std::string_view message);
};

} // namespace kaycxx::cli
