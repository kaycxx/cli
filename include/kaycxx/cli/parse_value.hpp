// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

/**
 * @file
 * Defines value parsing support used by typed options and positional parameters.
 */

#pragma once

#include <charconv>
#include <concepts>
#include <exception>
#include <format>
#include <string_view>
#include <system_error>
#include <type_traits>

#include <kaycxx/cli/parse_error.hpp>

namespace kaycxx::cli {

/**
 * Checks whether a type can be parsed with `std::from_chars`.
 *
 * @tparam T  Value type to check.
 */
template <typename T>
concept from_chars_parseable = requires(std::string_view text, T& value) {
    requires std::default_initializable<T>;
    { std::from_chars(text.data(), text.data() + text.size(), value) } -> std::same_as<std::from_chars_result>;
};

/**
 * Checks whether a type can be constructed from `std::string_view`.
 *
 * @tparam T  Value type to check.
 */
template <typename T>
concept string_view_constructible =
    std::constructible_from<T, std::string_view>;

/**
 * Checks whether a type can be parsed with an ADL-discovered `from_string` function.
 *
 * Supported functions must live in the same namespace as the parsed type and use the signature `T from_string(std::string_view, std::type_identity<T>)`.
 *
 * @tparam T  Value type to check.
 */
template <typename T>
concept from_string_parseable = requires(std::string_view text) {
    { from_string(text, std::type_identity<T>()) } -> std::same_as<T>;
};

/**
 * Checks whether a type can be used as a command line value.
 *
 * Values must be copy-constructible because parsed values are stored in `std::any`.
 *
 * @tparam T  Value type to check.
 */
template <typename T>
concept parseable_value =
    std::copy_constructible<T> && (
        from_chars_parseable<T> ||
        from_string_parseable<T> ||
        string_view_constructible<T>
    );

namespace detail {

/**
 * Parses a command line value.
 *
 * Parsing tries `std::from_chars` first, then ADL `from_string`, and finally construction from `std::string_view`.
 *
 * @tparam T  Value type to parse.
 *
 * @param text  Value text to parse.
 *
 * @returns Parsed value.
 *
 * @throws parse_error  When the value is syntactically invalid.
 */
template <parseable_value T>
[[nodiscard]] T parse_value(std::string_view text) {
    if constexpr (from_chars_parseable<T>) {
        auto value = T();
        auto const begin = text.data();
        auto const end = text.data() + text.size();
        auto const result = std::from_chars(begin, end, value);

        if (result.ec == std::errc::result_out_of_range) {
            throw parse_error(std::format("Value \"{}\" is out of range", text));
        }
        if (result.ec != std::errc() || result.ptr != end) {
            throw parse_error(std::format("Invalid value \"{}\"", text));
        }

        return value;
    } else {
        try {
            if constexpr (from_string_parseable<T>) {
                return from_string(text, std::type_identity<T>());
            } else {
                return T(text);
            }
        } catch (parse_error const&) {
            throw;
        } catch (std::exception const& error) {
            throw parse_error(std::format("Invalid value \"{}\" ({})", text, error.what()));
        } catch (...) {
            throw parse_error(std::format("Invalid value \"{}\"", text));
        }
    }
}

} // namespace detail

} // namespace kaycxx::cli
