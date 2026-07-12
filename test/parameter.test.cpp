// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <string>

#include <kaycxx/assert.hpp>
#include <kaycxx/cli.hpp>
#include <kaycxx/test.hpp>

#include "support/parse_arguments.hpp"

using namespace kaycxx::assert;
using namespace kaycxx::cli;
using namespace kaycxx::cli::test_support;
using namespace kaycxx::test;

suite("parameter") {
    it("exposes its definition", [] {
        auto app = command("example");
        auto input = app.parameter<std::string>("INPUT", "Input file");
        auto& definition = input.definition();

        assert_equal(definition.name(), "INPUT");
        assert_equal(definition.description().value(), "Input file");
        assert_equal(definition.min_count(), 1uz);
        assert_equal(definition.max_count(), 1uz);
    });

    it("parses required values", [] {
        auto app = command("example");
        auto input = app.parameter<std::string>("INPUT", "Input file");

        auto result = parse_arguments(app, { "input.txt" });

        assert_equal(result.get(input), "input.txt");
    });

    it("supports default values", [] {
        auto app = command("example");
        auto format = app.parameter<std::string>("FORMAT", "Output format").default_value("text");

        auto result = parse_arguments(app, {});

        assert_equal(result.get(format), "text");
    });

    it("allows a defaulted parameter before a required parameter", [] {
        auto app = command("example");
        auto format = app.parameter<std::string>("FORMAT", "Output format").default_value("text");
        auto input = app.parameter<std::string>("INPUT", "Input file");

        auto arguments = parse_arguments(app, { "input.txt" });

        assert_equal(arguments.get(format), "text");
        assert_equal(arguments.get(input), "input.txt");
    });

    it("reports missing required values", [] {
        auto app = command("example");
        auto input = app.parameter<std::string>("INPUT", "Input file");

        assert_throw<parse_error>([&] {
            auto arguments = parse_arguments(app, {});
            static_cast<void>(arguments.get(input));
        }, "Missing parameter <INPUT>");
    });

    it("adds the parameter name to conversion errors", [] {
        auto app = command("example");
        auto count = app.parameter<int>("COUNT", "Count");

        assert_throw<parse_error>([&] {
            auto arguments = parse_arguments(app, { "invalid" });
            static_cast<void>(arguments.get(count));
        }, "Invalid value \"invalid\" for parameter <COUNT>");
    });
}
