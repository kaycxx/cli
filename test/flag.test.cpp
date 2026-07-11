// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/assert.hpp>
#include <kaycxx/cli.hpp>
#include <kaycxx/test.hpp>

#include "support/parse_arguments.hpp"

using namespace kaycxx::assert;
using namespace kaycxx::cli;
using namespace kaycxx::cli::test_support;
using namespace kaycxx::test;

suite("flag") {
    it("exposes its definition", [] {
        auto app = command("example");
        auto quiet = app.flag("quiet", 'q', "Suppress output");
        auto& definition = quiet.definition();

        assert_equal(definition.name(), "quiet");
        assert_equal(definition.alias().value(), 'q');
        assert_equal(definition.description().value(), "Suppress output");
        assert_false(definition.expects_value());
    });

    it("supports flags without aliases or descriptions", [] {
        auto app = command("example");
        auto quiet = app.flag("quiet");

        assert_false(quiet.definition().alias().has_value());
        assert_false(quiet.definition().description().has_value());
    });

    it("parses long and short forms", [] {
        auto app = command("example");
        auto quiet = app.flag("quiet", 'q', "Suppress output");

        auto long_result = parse_arguments(app, { "--quiet" });
        auto short_result = parse_arguments(app, { "-q" });
        auto absent_result = parse_arguments(app, {});

        assert_true(long_result.get(quiet));
        assert_true(short_result.get(quiet));
        assert_false(absent_result.get(quiet));
    });

    it("rejects assigned values", [] {
        auto app = command("example");
        [[maybe_unused]] auto quiet = app.flag("quiet", 'q', "Suppress output");

        assert_throw<parse_error>([&] {
            parse_arguments(app, { "--quiet=true" });
        }, "Option --quiet does not accept a value");
    });
}
