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

suite("parameters") {
    it("exposes its configured value range", [] {
        auto app = command("example");
        auto values = app.parameters<int>("VALUE", "Values").min(1).max(3);
        auto& definition = values.definition();

        assert_equal(definition.name(), "VALUE");
        assert_equal(definition.description().value(), "Values");
        assert_equal(definition.min_count(), 1uz);
        assert_equal(definition.max_count(), 3uz);
    });

    it("parses zero or more values", [] {
        auto app = command("example");
        auto values = app.parameters<int>("VALUE", "Values");

        auto empty_result = parse_arguments(app, {});
        auto values_result = parse_arguments(app, { "1", "2", "3" });

        assert_equal(empty_result.get(values), std::vector<int>());
        assert_equal(values_result.get(values), std::vector<int>{ 1, 2, 3 });
    });

    it("reserves a required suffix after repeated values", [] {
        auto app = command("example");
        auto sources = app.parameters<std::string>("SOURCE", "Source files").min(1);
        auto destination = app.parameter<std::string>("DEST", "Destination");

        auto arguments = parse_arguments(app, { "one", "two", "output" });

        assert_equal(arguments.get(sources), std::vector<std::string>{ "one", "two" });
        assert_equal(arguments.get(destination), "output");
    });

    it("supports repeated values after a required prefix", [] {
        auto app = command("example");
        auto source = app.parameter<std::string>("SOURCE", "Source file");
        auto destinations = app.parameters<std::string>("DEST", "Destinations").min(1);

        auto arguments = parse_arguments(app, { "input", "one", "two" });

        assert_equal(arguments.get(source), "input");
        assert_equal(arguments.get(destinations), std::vector<std::string>{ "one", "two" });
    });

    it("supports default values", [] {
        auto app = command("example");
        auto values = app.parameters<std::string>("VALUE", "Values").default_values({ "default" });

        auto result = parse_arguments(app, {});

        assert_equal(result.get(values), std::vector<std::string>{ "default" });
    });

    it("reports missing minimum values", [] {
        auto app = command("example");
        auto values = app.parameters<int>("VALUE", "Values").min(2);

        assert_throw<parse_error>([&] {
            auto arguments = parse_arguments(app, { "1" });
            arguments.get(values);
        }, "Missing parameter <VALUE>");
    });

    it("reports a missing suffix after consuming the minimum prefix", [] {
        auto app = command("example");
        auto sources = app.parameters<std::string>("SOURCE", "Source files").min(1);
        auto destination = app.parameter<std::string>("DEST", "Destination");

        assert_throw<parse_error>([&] {
            auto arguments = parse_arguments(app, { "input" });
            arguments.get(sources);
            arguments.get(destination);
        }, "Missing parameter <DEST>");
    });

    it("rejects values beyond the maximum", [] {
        auto app = command("example");
        auto values = app.parameters<int>("VALUE", "Values").max(2);

        assert_throw<parse_error>([&] {
            auto arguments = parse_arguments(app, { "1", "2", "3" });
            arguments.get(values);
        }, "Unexpected parameter 3");
    });
}
