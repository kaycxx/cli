// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

/**
 * @file
 * Defines the result of command line parsing.
 */

#pragma once

#include <kaycxx/cli/args.hpp>

namespace kaycxx::cli {

/**
 * Result of command line parsing.
 *
 * A parse result either contains parsed arguments or indicates that the user requested generated help or version output.
 */
class parse_result {
public:
    /**
     * Creates a result containing parsed arguments.
     *
     * @param arguments  Parsed arguments.
     *
     * @returns Parse result containing parsed arguments.
     */
    static parse_result parsed(cli::args arguments);

    /**
     * Creates a result indicating a help request.
     *
     * @returns Help-request parse result.
     */
    static parse_result help();

    /**
     * Creates a result indicating a version request.
     *
     * @returns Version-request parse result.
     */
    static parse_result version();

    /**
     * Checks whether generated help output was requested.
     *
     * @returns True if help was requested, false otherwise.
     */
    bool help_requested() const noexcept;

    /**
     * Checks whether generated version output was requested.
     *
     * @returns True if version output was requested, false otherwise.
     */
    bool version_requested() const noexcept;

    /**
     * Returns parsed arguments.
     *
     * @returns Parsed arguments.
     *
     * @throws std::logic_error  When the result does not contain parsed arguments.
     */
    cli::args const& args() const;

private:
    enum class type {
        parsed,
        help,
        version
    };

    explicit parse_result(type result_type, cli::args arguments = cli::args());

    type type_;
    cli::args args_;
};

} // namespace kaycxx::cli
