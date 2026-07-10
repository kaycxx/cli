# Flags and Options

A flag does not take a value. An option takes a value and converts it to its declared C++ type.

## Flags

Register a flag with a long name and an optional one-character alias.

```cpp
auto quiet = app.flag("quiet", 'q', "Suppress normal output");
auto dry_run = app.flag("dry-run", "Do not write changes");
```

Both `--quiet` and `-q` set the first flag. Read flags with `args::get`. An absent flag returns `false`.

```cpp
auto& arguments = result.args();

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

When an option occurs more than once, the last parsed value is retained.
