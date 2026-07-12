# Defining a Command

A `command` owns the metadata and argument definitions for one command-line interface. The command name is required. All other metadata is optional.

```cpp
#include <kaycxx/cli.hpp>

using namespace kaycxx::cli;

int main() {
    auto app = command("convert", {
        .version = "2.1.0",
        .description = "Convert input files",
        .author = "Example Author",
        .email = "author@example.com",
        .bugs = "https://example.com/issues",
        .copyright = "Copyright (c) 2026 Example Author",
        .license = "Licensed under the MIT License"
    });
}
```

The command copies its name and metadata, so the strings passed to the constructor do not need to outlive it. Metadata is available through the corresponding command getters.

## Registering Arguments

Register flags, options, and positional parameters before parsing. Names are specified without command-line punctuation: use `"verbose"`, not `"--verbose"`, and `'v'`, not `"-v"`.

```cpp
auto verbose = app.flag("verbose", 'v', "Enable verbose output");
auto level = app.option<int>("level", 'l', "LEVEL", "Set the processing level");
auto input = app.parameter<std::string>("INPUT", "Input file");
auto outputs = app.parameters<std::string>("OUTPUT", "Output files");
```

Each registration method returns a handle. The command owns the underlying definition. The handle is the typed key used to configure that definition and read its parsed value. Keep the command alive while using its handles or parsed arguments.

Descriptions are optional. `print_help()` omits arguments without descriptions from its detailed argument lists.

## Help and Version Output

Help and version switches are regular flags. The application chooses their names, aliases, descriptions, and language, then handles them like any other flag.

```cpp
auto help = app.flag("help", 'h', "Show help").action();
auto version = app.flag("version", 'V', "Show version information").action();

auto arguments = app.parse(argc, argv);

if (arguments.get(help)) {
    return app.print_help();
}
if (arguments.get(version)) {
    return app.print_version();
}
```

Marking both flags as actions makes them mutually exclusive. Passing both `--help` and `--version` produces a parse error.

`print_help()` and `print_version()` are convenience methods that generate and write formatted help and version text from the command metadata and registered arguments. Both return `0`, so their result can be returned directly from `main` as the process exit code.

Using these methods is optional. An application can instead write its own help or version text and return an appropriate exit code itself.

The overloads accepting `std::ostream&` can write the output somewhere other than standard output, which is useful for embedding and testing.

```cpp
app.print_help(output);
app.print_version(output);
```

Positional parameters are validated lazily when first accessed. Help and version flags can therefore be handled before missing required positional parameters produce an error.
