// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

/**
 * @file
 * Defines command line flags.
 */

#pragma once

#include <string>

#include <kaycxx/cli/switch_base.hpp>

namespace kaycxx::cli {

/**
 * Command line flag definition.
 *
 * A flag is a switch without a value. It is either present or absent in parsed arguments.
 */
class flag : public switch_base {
public:
    using switch_base::switch_base;

    /**
     * Returns generated help usage for this flag.
     *
     * @returns Usage text.
     */
    [[nodiscard]] std::string usage() const override;

    /**
     * Checks whether this flag expects a value.
     *
     * @returns Always false.
     */
    [[nodiscard]] bool expects_value() const noexcept override;
};

} // namespace kaycxx::cli
