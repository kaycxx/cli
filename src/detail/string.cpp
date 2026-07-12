// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include "string.hpp"

#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

#if defined(_WIN32)
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

/**
 * @file
 * Implements internal string formatting helpers.
 */

namespace kaycxx::cli::detail {

std::size_t terminal_width(std::ostream& stream) noexcept {
#if defined(_WIN32)
    auto handle = INVALID_HANDLE_VALUE;
    if (&stream == &std::cout) {
        handle = GetStdHandle(STD_OUTPUT_HANDLE);
    } else if (&stream == &std::cerr || &stream == &std::clog) {
        handle = GetStdHandle(STD_ERROR_HANDLE);
    }

    auto info = CONSOLE_SCREEN_BUFFER_INFO();
    if (handle != nullptr && handle != INVALID_HANDLE_VALUE && GetConsoleScreenBufferInfo(handle, &info) != 0) {
        auto const width = info.srWindow.Right - info.srWindow.Left + 1;
        if (width > 0) {
            return static_cast<std::size_t>(width);
        }
    }
#else
    auto descriptor = -1;
    if (&stream == &std::cout) {
        descriptor = STDOUT_FILENO;
    } else if (&stream == &std::cerr || &stream == &std::clog) {
        descriptor = STDERR_FILENO;
    }

    auto size = winsize();
    if (descriptor >= 0 && ioctl(descriptor, TIOCGWINSZ, &size) == 0 && size.ws_col > 0) {
        return size.ws_col;
    }
#endif
    return 80;
}

std::string wrap(std::string text, std::size_t line_length, std::size_t indent) {
    if (line_length == 0) {
        line_length = 1;
    }

    auto const ends_with_newline = text.ends_with('\n');
    auto input = std::istringstream(std::move(text));
    auto result = std::string();
    auto source_line = std::string();
    auto word = std::string();
    auto first_source_line = true;

    while (std::getline(input, source_line)) {
        if (!first_source_line) {
            result += '\n';
        }

        auto words = std::istringstream(source_line);
        auto current_length = std::size_t(0);
        auto needs_indent = !first_source_line;

        while (words >> word) {
            if (current_length > 0 && (current_length == line_length || word.size() > line_length - current_length - 1)) {
                result += '\n';
                result.append(indent, ' ');
                current_length = 0;
            } else if (current_length > 0) {
                result += ' ';
                ++current_length;
            } else if (needs_indent) {
                result.append(indent, ' ');
                needs_indent = false;
            }

            while (word.size() > line_length) {
                result.append(word, 0, line_length);
                word.erase(0, line_length);
                result += '\n';
                result.append(indent, ' ');
            }

            result += word;
            current_length += word.size();
        }

        first_source_line = false;
    }

    if (ends_with_newline) {
        result += '\n';
    }

    return result;
}

} // namespace kaycxx::cli::detail
