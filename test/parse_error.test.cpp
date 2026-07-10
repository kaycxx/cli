// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/assert.hpp>
#include <kaycxx/cli.hpp>
#include <kaycxx/test.hpp>

using namespace kaycxx::assert;
using namespace kaycxx::cli;
using namespace kaycxx::test;

suite("parse_error") {
    it("stores its message", [] {
        auto error = parse_error("Invalid command line");

        assert_equal(error.what(), "Invalid command line");
    });
}
