# Positional Parameters

Positional parameters consume arguments that are not parsed as flags or options. They are converted with the same typed value conversion used by options.

## Single Parameters

A single parameter consumes exactly one value and is required unless a default is configured.

```cpp
auto input = app.parameter<std::string>("INPUT", "Input file");
auto format = app.parameter<std::string>("FORMAT", "Output format").default_value("text");
```

Use the returned handles to read the values. After successful parsing, every single parameter has either its parsed value or its configured default.

```cpp
auto& arguments = result.args();
auto const& input_file = arguments.get(input);
auto const& output_format = arguments.get(format);
```

## Repeated Parameters

`command::parameters<T>` defines a list that can consume multiple values. It accepts zero or more values by default. `min` and `max` constrain the count.

```cpp
auto inputs = app.parameters<std::string>("INPUT", "Input files").min(1).max(8);
```

Read the result as `std::vector<T> const&`.

```cpp
for (auto const& input_file : arguments.get(inputs)) {
    process(input_file);
}
```

Default lists are also configured through the handle.

```cpp
auto inputs = app.parameters<std::string>("INPUT", "Input files").default_values({ "stdin" });
```

## Combining Parameter Shapes

Parameters are declared in allocation order. Each repeated parameter first receives its minimum number of values, then consumes additional values while reserving the minimum required by later parameters. This supports both a repeated prefix and a repeated suffix.

```cpp
// SOURCE... DEST
auto sources = app.parameters<std::string>("SOURCE", "Source files").min(1);
auto destination = app.parameter<std::string>("DEST", "Destination");
```

```cpp
// SOURCE DEST...
auto source = app.parameter<std::string>("SOURCE", "Source file");
auto destinations = app.parameters<std::string>("DEST", "Destinations").min(1);
```

Definitions with several optional or unbounded parameter lists can be inherently ambiguous. The parser still allocates values deterministically in declaration order, but the command should use shapes that have a clear meaning for its users.

Flags and options may appear before, after, or between positional arguments. Use `--` when a positional value begins with `-` and must not be interpreted as a switch.

```sh
example --verbose source.txt destination.txt
example source.txt --verbose destination.txt
example -- --literal-file-name
```
