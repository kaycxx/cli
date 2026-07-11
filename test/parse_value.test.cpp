// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>

#include <kaycxx/assert.hpp>
#include <kaycxx/cli.hpp>
#include <kaycxx/test.hpp>

#include "support/parse_arguments.hpp"

using namespace kaycxx::assert;
using namespace kaycxx::cli;
using namespace kaycxx::cli::test_support;
using namespace kaycxx::test;

namespace test_types {

enum class color {
    red,
    green,
    blue
};

color from_string(std::string_view text, std::type_identity<color>);

color from_string(std::string_view text, std::type_identity<color>) {
    if (text == "red") {
        return color::red;
    }
    if (text == "green") {
        return color::green;
    }
    if (text == "blue") {
        return color::blue;
    }

    throw std::invalid_argument("Expected red, green, or blue");
}

} // namespace test_types

suite("parse_value") {
    it("parses numeric and string values", [] {
        auto app = command("example");
        auto count = app.option<int>("count", "COUNT", "Count");
        auto ratio = app.option<double>("ratio", "RATIO", "Ratio");
        auto name = app.option<std::string>("name", "NAME", "Name");

        auto arguments = parse_arguments(app, { "--count=42", "--ratio=1.5", "--name=example" });

        assert_equal(arguments.get(count), 42);
        assert_close(arguments.get(ratio), 1.5);
        assert_equal(arguments.get(name), "example");
    });

    it("rejects invalid and trailing numeric text", [] {
        auto app = command("example");
        [[maybe_unused]] auto count = app.option<int>("count", "COUNT", "Count");

        assert_throw<parse_error>([&] {
            parse_arguments(app, { "--count=abc" });
        }, "Invalid value \"abc\" for option --count");
        assert_throw<parse_error>([&] {
            parse_arguments(app, { "--count=42x" });
        }, "Invalid value \"42x\" for option --count");
    });

    it("reports numeric values outside the target range", [] {
        auto app = command("example");
        [[maybe_unused]] auto count = app.option<int>("count", "COUNT", "Count");

        assert_throw<parse_error>([&] {
            parse_arguments(app, { "--count=999999999999999999999999999999999999" });
        }, "Value \"999999999999999999999999999999999999\" is out of range for option --count");
    });

    it("uses from_string found through argument-dependent lookup", [] {
        auto app = command("example");
        auto color = app.option<test_types::color>("color", "COLOR", "Color");

        auto result = parse_arguments(app, { "--color=green" });

        assert_equal(result.get(color), test_types::color::green);
    });

    it("includes custom conversion errors", [] {
        auto app = command("example");
        [[maybe_unused]] auto color = app.option<test_types::color>("color", "COLOR", "Color");

        assert_throw<parse_error>([&] {
            parse_arguments(app, { "--color=purple" });
        }, "Invalid value \"purple\" (Expected red, green, or blue) for option --color");
    });
}
