# Parsing and Errors

`command::parse` accepts `argc` and `argv` directly from `main` and returns a `parse_result`.

```cpp
auto result = app.parse(argc, argv);
```

A result represents exactly one of three states:

- Parsed arguments are available through `result.args()`.
- The user requested `--help`.
- The user requested `--version`.

Check help and version before accessing arguments. Calling `args()` for either request throws `std::logic_error` because normal argument parsing intentionally did not complete.

```cpp
if (result.help_requested()) {
    return app.print_help();
}
if (result.version_requested()) {
    return app.print_version();
}

auto& arguments = result.args();
```

Parsed arguments cannot be copied. Bind the returned reference with `auto&` when it is used more than once.

## Argument Order

Flags, options, and positional arguments may be freely mixed. `--` ends switch parsing. Every following value is positional even when it begins with `-`.

```sh
example input.txt --verbose --count=2 output.txt
example -- --name-starting-with-dashes
```

## Parse Errors

Invalid command lines throw `parse_error`. This includes unknown switches, missing option values, failed value conversion, missing required positional parameters, and unexpected positional arguments.

Catch the exception at the application boundary to produce a concise command-line error and nonzero exit code.

```cpp
#include <iostream>

#include <kaycxx/cli.hpp>

using namespace kaycxx::cli;

int main(int argc, char* argv[]) {
    auto app = command("example");
    auto input = app.parameter<std::string>("INPUT", "Input file");

    try {
        auto result = app.parse(argc, argv);
        if (result.help_requested()) {
            return app.print_help();
        }
        if (result.version_requested()) {
            return app.print_version();
        }

        process(result.args().get(input));
        return 0;
    } catch (parse_error const& error) {
        std::cerr << app.name() << ": " << error.what() << '\n';
        return 1;
    }
}
```
