// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file
 * Declares internal string formatting helpers.
 */

#include <cstddef>
#include <ostream>
#include <string>

namespace kaycxx::cli::detail {

/**
 * Returns the terminal width for an output stream.
 *
 * @param stream  Output stream whose terminal width to read.
 *
 * @returns Terminal width in character cells, or 80 when the width cannot be determined.
 */
[[nodiscard]] std::size_t terminal_width(std::ostream& stream) noexcept;

/**
 * Wraps text to fit a specific line length.
 *
 * Existing line breaks are preserved, while other whitespace between words is normalized to a single space.
 * Every non-empty line except the first is prefixed with the requested indentation. The indentation is not included in the line length.
 *
 * @param text         Text to wrap.
 * @param line_length  Maximum number of text characters per line.
 * @param indent       Number of spaces prepended to wrapped continuation lines.
 *
 * @returns Wrapped text.
 */
[[nodiscard]] std::string wrap(std::string text, std::size_t line_length, std::size_t indent = 0);

} // namespace kaycxx::cli::detail
