# Parsing and Errors

`command::parse` accepts `argc` and `argv` directly from `main` and returns the parsed arguments.

```cpp
auto arguments = app.parse(argc, argv);
```

The returned `args` object is movable but not copyable. Keep it by value with `auto`.

Flags and options are parsed immediately. Unknown switches, missing or invalid option values, and conflicting actions therefore produce an error during `parse()`.

Positional arguments are retained as strings and validated lazily. Accessing any positional parameter through `args::get()` or `args::has()` validates and converts all positional parameters once.

## Explicit Validation

Call `args::validate()` to validate positional parameters without reading one of them.

```cpp
arguments.validate();
```

This is useful when a command defines positional parameters but does not otherwise access every execution path through their handles. Calling `validate()` repeatedly has no effect after the first successful validation.

## Argument Order

Flags, options, and positional arguments may be freely mixed. `--` ends switch parsing. Every following value is positional even when it begins with `-`.

```sh
example input.txt --verbose --count=2 output.txt
example -- --name-starting-with-dashes
```

## Parse Errors

Invalid command lines throw `parse_error`. Switch errors are thrown by `command::parse()`. Missing, unexpected, or invalid positional parameters are thrown when positional validation is first triggered.

Catch the exception at the application boundary to produce a concise command-line error and nonzero exit code.

```cpp
#include <iostream>

#include <kaycxx/cli.hpp>

using namespace kaycxx::cli;

int main(int argc, char* argv[]) {
    auto app = command("example");
    auto input = app.parameter<std::string>("INPUT", "Input file");

    try {
        auto arguments = app.parse(argc, argv);
        process(arguments.get(input));
        return 0;
    } catch (parse_error const& error) {
        std::cerr << app.name() << ": " << error.what() << '\n';
        return 1;
    }
}
```
