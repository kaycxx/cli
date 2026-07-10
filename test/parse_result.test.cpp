// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <stdexcept>
#include <string>

#include <kaycxx/assert.hpp>
#include <kaycxx/cli.hpp>
#include <kaycxx/test.hpp>

#include "support/parse_arguments.hpp"

using namespace kaycxx::assert;
using namespace kaycxx::cli;
using namespace kaycxx::cli::test_support;
using namespace kaycxx::test;

suite("parse_result") {
    it("contains normally parsed arguments", [] {
        auto app = command("example");
        auto input = app.parameter<std::string>("INPUT", "Input file");

        auto result = parse_arguments(app, { "input.txt" });

        assert_false(result.help_requested());
        assert_false(result.version_requested());
        assert_equal(result.args().get(input), "input.txt");
    });

    it("reports help without validating required parameters", [] {
        auto app = command("example");
        app.parameter<std::string>("INPUT", "Input file");

        auto result = parse_arguments(app, { "--help" });

        assert_true(result.help_requested());
        assert_false(result.version_requested());
        assert_throw<std::logic_error>([&] {
            result.args();
        }, "Parsed arguments are not available");
    });

    it("reports version without validating required parameters", [] {
        auto app = command("example");
        app.parameter<std::string>("INPUT", "Input file");

        auto result = parse_arguments(app, { "--version" });

        assert_false(result.help_requested());
        assert_true(result.version_requested());
        assert_throw<std::logic_error>([&] {
            result.args();
        }, "Parsed arguments are not available");
    });
}
