# kaycxx-cli

Typed command-line parsing with GNU-style flags, options, and positional parameters.

Requires C++26.

[GitHub] | [API Documentation]

## Usage

```cpp
#include <iostream>
#include <string>

#include <kaycxx/cli.hpp>

using namespace kaycxx::cli;

int main(int argc, char* argv[]) {
    auto app = command("example");

    auto verbose = app.flag("verbose", 'v', "Enable verbose output");
    auto repetitions = app.option<int>("count", 'c', "COUNT", "Number of repetitions").default_value(1);
    auto input = app.parameter<std::string>("INPUT", "Input file name");

    try {
        auto result = app.parse(argc, argv);
        if (result.help_requested()) {
            return app.print_help();
        }
        if (result.version_requested()) {
            return app.print_version();
        }

        auto& arguments = result.args();
        for (auto remaining = arguments.get(repetitions); remaining > 0; --remaining) {
            if (arguments.get(verbose)) {
                std::cout << "Input: ";
            }
            std::cout << arguments.get(input) << '\n';
        }
        return 0;
    } catch (parse_error const& error) {
        std::cerr << app.name() << ": " << error.what() << '\n';
        return 1;
    }
}
```

CMake users consume the installed package with:

```cmake
find_package(kaycxx-cli 1.0.0 CONFIG REQUIRED)
target_link_libraries(my-target PRIVATE kaycxx::cli)
```

Non-CMake users can use pkg-config:

```sh
c++ $(pkg-config --cflags kaycxx-cli) -c main.cpp
c++ main.o $(pkg-config --libs kaycxx-cli)
```

## Guides

- [Defining a Command] explains command metadata, registered arguments, handles, and help/version dispatch.
- [Flags and Options] explains boolean flags, typed options, short aliases, defaults, and value access.
- [Positional Parameters] explains single and repeated parameters, defaults, value counts, and argument allocation.
- [Value Conversion] explains built-in conversions and how custom types provide an ADL-discovered `from_string` function.
- [Parsing and Errors] explains parse results, help and version requests, the `--` separator, and `parse_error` handling.

## Build From Source

```sh
cmake -B build
cmake --build build
```

A shared library is built by default. For a static build:

```sh
cmake -B build -D BUILD_SHARED_LIBS=OFF
cmake --build build
```

## Install

```sh
cmake --install build --prefix /tmp/root
```

If no prefix is specified, CMake installs to `/usr/local` by default on Unix systems.

## Development

Run all tests:

```sh
cmake --build build --target test
```

Generate API documentation with Doxygen:

```sh
cmake --build build --target apidoc
```

The generated HTML documentation is written to `build/apidoc/html/index.html`.

[GitHub]: https://github.com/kaycxx/cli
[API Documentation]: https://kaycxx.github.io/cli/
[Defining a Command]: docs/defining-a-command.md
[Flags and Options]: docs/flags-and-options.md
[Positional Parameters]: docs/positional-parameters.md
[Value Conversion]: docs/value-conversion.md
[Parsing and Errors]: docs/parsing-and-errors.md
