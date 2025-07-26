# HTMLButtonElement Implementation

This document describes the HTMLButtonElement implementation added to JSAR Runtime.

## Overview

The HTMLButtonElement represents an HTML `<button>` element. It provides the standard button functionality including:

- Button types (submit, reset, button)
- Disabled state management
- Form association
- Name and value attributes
- Default styling that matches typical button appearance

## Files Added/Modified

### Core Implementation
- `src/client/html/html_button_element.hpp` - C++ header file
- `src/client/html/html_button_element.cpp` - C++ implementation
- `src/client/html/all_html_elements.hpp` - Added include

### JavaScript Bindings
- `src/bindings/dom/html_button_element.hpp` - Bindings header
- `src/bindings/dom/html_button_element.cpp` - Bindings implementation
- `src/bindings/dom/all_html_elements.hpp` - Added include

### Element Registration
- `src/client/dom/element.hpp` - Added "button" to TYPED_ELEMENT_MAP

### Documentation/Compatibility
- `docs/api/browser-compat-data/html/elements/button.json` - Updated support status

### Test File
- `fixtures/html/button-test.html` - Test HTML file

## Properties Supported

| Property | Type | Description |
|----------|------|-------------|
| disabled | boolean | Whether the button is disabled |
| type | string | Button type: "submit", "reset", or "button" |
| form | string | ID of associated form element |
| name | string | Name attribute for form submission |
| value | string | Value attribute for form submission |

## Default Styling

The button element includes default CSS styling:
- `display: inline-block`
- `text-align: center`
- `cursor: pointer`
- Standard padding and borders
- Disabled state styling (opacity and cursor changes)

## Usage Example

```html
<button id="myButton" type="button" name="action" value="save">Save</button>

<script>
const button = document.getElementById('myButton');
console.log(button.type); // "button"
button.disabled = true;   // Disables the button
</script>
```

## Browser Compatibility

JSAR Runtime now supports HTMLButtonElement as of version 0.8.3.