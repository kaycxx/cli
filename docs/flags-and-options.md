# Flags and Options

A flag does not take a value. An option takes a value and converts it to its declared C++ type.

## Flags

Register a flag with a long name and an optional one-character alias.

```cpp
auto quiet = app.flag("quiet", 'q', "Suppress normal output");
auto dry_run = app.flag("dry-run");
```

The description is optional. A flag without a description remains parseable but is omitted from the detailed help listing.

Both `--quiet` and `-q` set the first flag. Read flags with `args::get`. An absent flag returns `false`.

```cpp
auto arguments = app.parse(argc, argv);

if (arguments.get(quiet)) {
    // Suppress output.
}
```

Flags do not accept values. For example, `--quiet=true` is rejected.

## Typed Options

The option template argument defines the parsed value type. The value name is used as a placeholder in help and error output.

```cpp
auto jobs = app.option<int>("jobs", 'j', "COUNT", "Number of parallel jobs");
auto output = app.option<std::string>("output", "FILE", "Output file");
```

The parser accepts long option values as the next argument or after `=`. Short option values are read from the next argument.

```sh
example --jobs 4
example --jobs=4
example -j 4
```

Grouped short flags such as `-qv` and attached short option values such as `-j4` are not supported.

## Defaults and Optional Values

Set a default through the returned handle. Handle configuration methods return the handle again, so the configured handle can be stored directly.

```cpp
auto jobs = app.option<int>("jobs", 'j', "COUNT", "Number of parallel jobs").default_value(1);
```

An option with a default is always available through `args::get`. For an option without a default, check `args::has` before reading it.

```cpp
if (arguments.has(output)) {
    write_to(arguments.get(output));
}

auto const job_count = arguments.get(jobs);
```

An ordinary option may occur only once. Repeating it is reported as a parsing error.

## Repeatable Options

Use `repeatable_option` when the same option may occur multiple times. Its handle reads all converted values as a vector in command-line order.

```cpp
auto include = app.repeatable_option<std::string>("include", 'I', "PATH", "Additional include path");

auto arguments = app.parse(argc, argv);
auto const& include_paths = arguments.get(include);
```

For example, `--include first -I second --include=third` produces `std::vector<std::string>{ "first", "second", "third" }`. An absent repeatable option is unavailable through `args::get` unless it has a default value.

## Actions

Mark flags and options that select alternative command actions with `action()`. At most one explicitly specified action may occur on a command line. Omitting all actions is valid and lets the application perform its default action.

```cpp
auto help = app.flag("help", 'h', "Show help").action();
auto version = app.flag("version", 'V', "Show version information").action();
auto list = app.flag("list", "List entries").action();
auto show = app.option<std::string>("show", "NAME", "Show one entry").action();
```

Normal switches are not part of this restriction and may be combined with an action. Default option values do not select an action. Only an action explicitly present on the command line participates in the exclusivity check.
