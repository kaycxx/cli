# Value Conversion

Options and positional parameters are typed. Their value types must be copy-constructible and support at least one of the conversion methods listed below.

Conversion is selected in this order:

1. `std::from_chars` for supported numeric and similar standard types.
2. An ADL-discovered `from_string` function for custom types.
3. Construction from `std::string_view`, including `std::string`.

`std::from_chars` must consume the complete argument text. Trailing invalid text is rejected.

```cpp
auto count = app.option<int>("count", 'c', "COUNT", "Number of repetitions");
auto ratio = app.option<double>("ratio", "RATIO", "Scaling ratio");
auto name = app.parameter<std::string>("NAME", "Item name");
```

## Custom Types

Define `from_string` in the same namespace as the custom type. Argument-dependent lookup finds it when the command instantiates an option or parameter for that type.

```cpp
#include <stdexcept>
#include <string_view>
#include <type_traits>

namespace graphics {

enum class color {
    red,
    green,
    blue
};

color from_string(std::string_view text, std::type_identity<color>) {
    if (text == "red") {
        return color::red;
    }
    if (text == "green") {
        return color::green;
    }
    if (text == "blue") {
        return color::blue;
    }

    throw std::invalid_argument("Expected red, green, or blue");
}

} // namespace graphics
```

The type can then be used directly.

```cpp
auto color = app.option<graphics::color>("color", "COLOR", "Output color");
```

The required customization signature is:

```cpp
T from_string(std::string_view text, std::type_identity<T>);
```

The `std::type_identity<T>` argument distinguishes conversions by result type without requiring a dummy value. A thrown `parse_error` is preserved. Other standard exceptions are converted to `parse_error` and their `what()` message is appended to the invalid-value message.

Use a flag instead of `option<bool>` for a simple enabled/disabled switch. Options are intended for values explicitly supplied by the user.
