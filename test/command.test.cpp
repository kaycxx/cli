// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <sstream>
#include <string>
#include <string_view>

#include <kaycxx/assert.hpp>
#include <kaycxx/cli.hpp>
#include <kaycxx/test.hpp>

#include "support/parse_arguments.hpp"

using namespace kaycxx::assert;
using namespace kaycxx::cli;
using namespace kaycxx::cli::test_support;
using namespace kaycxx::test;

suite("command") {
    it("copies its name and metadata", [] {
        auto name = std::string("example");
        auto version = std::string("1.2.3");
        auto description = std::string("Example command");
        auto author = std::string("Example Author");
        auto email = std::string("author@example.com");
        auto bugs = std::string("https://example.com/issues");
        auto copyright = std::string("Copyright (c) 2026 Example Author");
        auto license = std::string("Licensed under the MIT License");

        auto app = command(name, {
            .version = std::string_view(version),
            .description = std::string_view(description),
            .author = std::string_view(author),
            .email = std::string_view(email),
            .bugs = std::string_view(bugs),
            .copyright = std::string_view(copyright),
            .license = std::string_view(license)
        });

        name = "changed";
        version = "changed";
        description = "changed";
        author = "changed";
        email = "changed";
        bugs = "changed";
        copyright = "changed";
        license = "changed";

        assert_equal(app.name(), "example");
        assert_equal(app.version().value(), "1.2.3");
        assert_equal(app.description().value(), "Example command");
        assert_equal(app.author().value(), "Example Author");
        assert_equal(app.email().value(), "author@example.com");
        assert_equal(app.bugs().value(), "https://example.com/issues");
        assert_equal(app.copyright().value(), "Copyright (c) 2026 Example Author");
        assert_equal(app.license().value(), "Licensed under the MIT License");
    });

    it("supports omitted metadata", [] {
        auto app = command("example");

        assert_false(app.version().has_value());
        assert_false(app.description().has_value());
        assert_false(app.author().has_value());
        assert_false(app.email().has_value());
        assert_false(app.bugs().has_value());
        assert_false(app.copyright().has_value());
        assert_false(app.license().has_value());
    });

    it("allows switches between positional arguments", [] {
        auto app = command("example");
        auto verbose = app.flag("verbose", 'v', "Enable verbose output");
        auto values = app.parameters<std::string>("VALUE", "Values");

        auto arguments = parse_arguments(app, { "first", "--verbose", "second" });

        assert_true(arguments.get(verbose));
        assert_equal(arguments.get(values), std::vector<std::string>{ "first", "second" });
    });

    it("treats arguments after the separator as positional", [] {
        auto app = command("example");
        auto values = app.parameters<std::string>("VALUE", "Values");

        auto result = parse_arguments(app, { "--", "--verbose", "-v" });

        assert_equal(result.get(values), std::vector<std::string>{ "--verbose", "-v" });
    });

    it("rejects unknown long and short options", [] {
        auto app = command("example");

        assert_throw<parse_error>([&] {
            parse_arguments(app, { "--missing" });
        }, "Unknown option --missing");
        assert_throw<parse_error>([&] {
            parse_arguments(app, { "-x" });
        }, "Unknown option -x");
    });

    it("rejects multiple actions", [] {
        auto app = command("example");
        [[maybe_unused]] auto help = app.flag("help", 'h', "Show help").action();
        [[maybe_unused]] auto run = app.option<std::string>("run", 'r', "NAME", "Run an action").action();

        assert_throw<parse_error>([&] {
            parse_arguments(app, { "--help", "--run", "test" });
        }, "Options --help and --run cannot be combined");
        assert_throw<parse_error>([&] {
            parse_arguments(app, { "-r", "test", "-h" });
        }, "Options --run and --help cannot be combined");
    });

    it("allows one action with normal switches", [] {
        auto app = command("example");
        auto verbose = app.flag("verbose", 'v', "Enable verbose output");
        auto run = app.option<std::string>("run", 'r', "NAME", "Run an action").action();

        auto arguments = parse_arguments(app, { "--verbose", "--run", "test" });

        assert_true(arguments.get(verbose));
        assert_equal(arguments.get(run), "test");
    });

    it("allows repeated use of the same action", [] {
        auto app = command("example");
        auto help = app.flag("help", 'h', "Show help").action();

        auto arguments = parse_arguments(app, { "--help", "-h" });

        assert_true(arguments.get(help));
    });

    it("does not select actions through default values", [] {
        auto app = command("example");
        auto mode = app.option<std::string>("mode", "MODE", "Select mode").default_value("default").action();
        auto help = app.flag("help", "Show help").action();

        auto arguments = parse_arguments(app, { "--help" });

        assert_equal(arguments.get(mode), "default");
        assert_true(arguments.get(help));
    });

    it("includes command metadata in help output", [] {
        auto app = command("example", {
            .description = "Example command",
            .bugs = "https://example.com/issues"
        });
        auto output = std::ostringstream();

        auto const exit_code = app.print_help(output);

        assert_equal(exit_code, 0);
        assert_equal(
            output.str(),
            "Usage: example\n"
            "Example command\n"
            "\n"
            "Report bugs to <https://example.com/issues>\n"
        );
    });

    it("includes command metadata in version output", [] {
        auto app = command("example", {
            .version = "1.2.3",
            .author = "Example Author",
            .email = "author@example.com",
            .copyright = "Copyright (c) 2026 Example Author",
            .license = "Licensed under the MIT License"
        });
        auto output = std::ostringstream();

        auto const exit_code = app.print_version(output);

        assert_equal(exit_code, 0);
        assert_equal(
            output.str(),
            "example 1.2.3\n"
            "\n"
            "Copyright (c) 2026 Example Author\n"
            "Licensed under the MIT License\n"
            "\n"
            "Written by Example Author <author@example.com>\n"
        );
    });

    it("includes registered arguments in help output", [] {
        auto app = command("example");
        [[maybe_unused]] auto quiet = app.flag("quiet", 'q', "Suppress output");
        [[maybe_unused]] auto count = app.option<int>("count", 'c', "COUNT", "Number of repetitions");
        [[maybe_unused]] auto input = app.parameter<std::string>("INPUT", "Input file");
        [[maybe_unused]] auto output_files = app.parameters<std::string>("OUTPUT", "Output files");
        [[maybe_unused]] auto undocumented_switch = app.flag("undocumented-switch-with-long-usage");
        [[maybe_unused]] auto undocumented_parameter = app.parameter<std::string>("UNDOCUMENTED_PARAMETER_WITH_LONG_USAGE");
        auto output = std::ostringstream();

        auto const exit_code = app.print_help(output);
        auto const text = output.str();

        assert_equal(exit_code, 0);
        assert_contain(text, "-q, --quiet           Suppress output");
        assert_contain(text, "-c, --count <COUNT>   Number of repetitions");
        assert_contain(text, "<INPUT>       Input file");
        assert_contain(text, "<OUTPUT>...   Output files");
    });

    it("omits arguments without descriptions from help details", [] {
        auto app = command("example");
        [[maybe_unused]] auto quiet = app.flag("quiet", 'q');
        [[maybe_unused]] auto count = app.option<int>("count", 'c', "COUNT");
        [[maybe_unused]] auto input = app.parameter<std::string>("INPUT");
        [[maybe_unused]] auto output_files = app.parameters<std::string>("OUTPUT");
        auto output = std::ostringstream();

        app.print_help(output);
        auto const text = output.str();

        assert_contain(text, "Usage: example [OPTION]... <INPUT> <OUTPUT>...");
        assert_not_contain(text, "--quiet");
        assert_not_contain(text, "--count");
        assert_not_contain(text, "\n  <INPUT>");
        assert_not_contain(text, "\n  <OUTPUT>");
    });
}
