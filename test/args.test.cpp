// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <concepts>
#include <string>
#include <vector>

#include <kaycxx/assert.hpp>
#include <kaycxx/cli.hpp>
#include <kaycxx/test.hpp>

#include "support/parse_arguments.hpp"

using namespace kaycxx::assert;
using namespace kaycxx::cli;
using namespace kaycxx::cli::test_support;
using namespace kaycxx::test;

static_assert(std::movable<args>);
static_assert(!std::copyable<args>);

suite("args") {
    it("returns parsed values through their handles", [] {
        auto app = command("example");
        auto verbose = app.flag("verbose", 'v', "Enable verbose output");
        auto quiet = app.flag("quiet", 'q', "Suppress output");
        auto count = app.option<int>("count", 'c', "COUNT", "Number of repetitions");
        auto output = app.option<std::string>("output", 'o', "FILE", "Output file");
        auto input = app.parameter<std::string>("INPUT", "Input file");
        auto values = app.parameters<int>("VALUE", "Values");

        auto arguments = parse_arguments(app, { "--verbose", "--count", "3", "input.txt", "10", "20" });
        arguments.validate();
        arguments.validate();

        assert_true(arguments.get(verbose));
        assert_false(arguments.get(quiet));
        assert_true(arguments.has(count));
        assert_equal(arguments.get(count), 3);
        assert_false(arguments.has(output));
        assert_equal(arguments.get(input), "input.txt");
        assert_equal(arguments.get(values), std::vector<int>{ 10, 20 });
    });

    it("delays positional validation until requested", [] {
        auto app = command("example");
        auto help = app.flag("help", 'h', "Show help").action();
        auto version = app.flag("version", 'V', "Show version").action();
        auto input = app.parameter<std::string>("INPUT", "Input file");

        auto help_arguments = parse_arguments(app, { "--help" });
        auto version_arguments = parse_arguments(app, { "-V" });

        assert_true(help_arguments.get(help));
        assert_false(help_arguments.get(version));
        assert_false(version_arguments.get(help));
        assert_true(version_arguments.get(version));
        assert_throw<parse_error>([&] {
            static_cast<void>(help_arguments.get(input));
        }, "Missing parameter <INPUT>");
        assert_throw<parse_error>([&] {
            version_arguments.validate();
        }, "Missing parameter <INPUT>");
    });
}
