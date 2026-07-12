// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

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

suite("option") {
    it("exposes its definition", [] {
        auto app = command("example");
        auto count = app.option<int>("count", 'c', "COUNT", "Number of repetitions");
        auto& definition = count.definition();

        assert_equal(definition.name(), "count");
        assert_equal(definition.alias().value(), 'c');
        assert_equal(definition.value_name(), "COUNT");
        assert_equal(definition.description().value(), "Number of repetitions");
        assert_true(definition.expects_value());
        assert_false(definition.is_action());
        assert_false(definition.is_repeatable());
    });

    it("exposes repeatable option definitions", [] {
        auto app = command("example");
        auto include = app.repeatable_option<std::string>("include", 'I', "PATH", "Include path");

        assert_true(include.definition().is_repeatable());
    });

    it("marks options as actions", [] {
        auto app = command("example");
        auto output = app.option<std::string>("output", "FILE", "Output file").action();

        assert_true(output.definition().is_action());
    });

    it("parses long, assigned, and short forms", [] {
        auto app = command("example");
        auto count = app.option<int>("count", 'c', "COUNT", "Number of repetitions");

        auto long_result = parse_arguments(app, { "--count", "1" });
        auto assigned_result = parse_arguments(app, { "--count=2" });
        auto short_result = parse_arguments(app, { "-c", "3" });

        assert_equal(long_result.get(count), 1);
        assert_equal(assigned_result.get(count), 2);
        assert_equal(short_result.get(count), 3);
    });

    it("supports default and absent values", [] {
        auto app = command("example");
        auto count = app.option<int>("count", "COUNT", "Number of repetitions").default_value(1);
        auto output = app.option<std::string>("output", "FILE", "Output file");

        auto arguments = parse_arguments(app, {});

        assert_true(arguments.has(count));
        assert_equal(arguments.get(count), 1);
        assert_false(arguments.has(output));
    });

    it("rejects repeated single-value options", [] {
        auto app = command("example");
        [[maybe_unused]] auto count = app.option<int>("count", 'c', "COUNT", "Number of repetitions");

        assert_throw<parse_error>([&] {
            parse_arguments(app, { "--count", "1", "-c", "2" });
        }, "Option -c cannot be specified more than once");
    });

    it("collects repeatable option values in command-line order", [] {
        auto app = command("example");
        auto include = app.repeatable_option<std::string>("include", 'I', "PATH", "Include path");

        auto empty_arguments = parse_arguments(app, {});
        auto arguments = parse_arguments(app, { "--include", "first", "--include=second", "-I", "third" });

        assert_false(empty_arguments.has(include));
        assert_true(arguments.has(include));
        assert_equal(arguments.get(include), std::vector<std::string>{ "first", "second", "third" });
    });

    it("supports repeatable option defaults", [] {
        auto app = command("example");
        auto include = app.repeatable_option<std::string>("include", "PATH", "Include path").default_value("default");

        auto arguments = parse_arguments(app, {});

        assert_true(arguments.has(include));
        assert_equal(arguments.get(include), std::vector<std::string>{ "default" });
    });

    it("reports missing values", [] {
        auto app = command("example");
        [[maybe_unused]] auto count = app.option<int>("count", 'c', "COUNT", "Number of repetitions");

        assert_throw<parse_error>([&] {
            parse_arguments(app, { "--count" });
        }, "Missing value <COUNT> for option --count");
        assert_throw<parse_error>([&] {
            parse_arguments(app, { "-c" });
        }, "Missing value <COUNT> for option -c");
    });
}
