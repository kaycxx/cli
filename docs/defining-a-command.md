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

`command::parse()` recognizes `--help` and `--version`. Its return value then reports the request through `help_requested()` or `version_requested()`, leaving the application in control of the corresponding output and exit code.

```cpp
auto result = app.parse(argc, argv);

if (result.help_requested()) {
    return app.print_help();
}
if (result.version_requested()) {
    return app.print_version();
}
```

The overloads accepting `std::ostream&` can write the output somewhere other than standard output, which is useful for embedding and testing.

```cpp
app.print_help(output);
app.print_version(output);
```

Because help and version requests return before normal argument validation, a command with required positional parameters can still handle `--help` and `--version` without those parameters.
