// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <sstream>
#include <string>

#include <kaycxx/assert.hpp>
#include <kaycxx/test.hpp>

#include "../../src/detail/string.hpp"

using namespace kaycxx::assert;
using namespace kaycxx::test;

suite("detail::string") {
    it("uses the fallback width for non-terminal streams", [] {
        auto output = std::ostringstream();

        assert_equal(kaycxx::cli::detail::terminal_width(output), 80uz);
    });

    it("wraps words and indents continuation lines", [] {
        auto line = std::string(70, 'a');
        auto expected = line + " word\n    next";

        assert_equal(kaycxx::cli::detail::wrap(line + " word next", 75, 4), expected);
    });

    it("preserves existing and empty lines", [] {
        auto text = std::string("first line\nsecond line\n\nfourth line\n");
        auto expected = std::string("first line\n    second line\n\n    fourth line\n");

        assert_equal(kaycxx::cli::detail::wrap(text, 75, 4), expected);
    });

    it("splits words longer than the available width", [] {
        auto word = std::string(80, 'a');
        auto expected = std::string(75, 'a') + "\n    " + std::string(5, 'a');

        assert_equal(kaycxx::cli::detail::wrap(word, 75, 4), expected);
    });
}
