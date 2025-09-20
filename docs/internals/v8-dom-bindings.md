# V8 DOM Bindings Architecture

This document explains the new V8-based DOM binding implementation in `src/client/script_bindings/dom/` that replaces the N-API implementation.

## Overview

The V8 DOM bindings provide a direct interface between JavaScript and the native DOM implementation, using V8 APIs instead of N-API for better performance and integration.

## Architecture

### Base Class Pattern

All DOM classes inherit from `scripting_base::ObjectWrap<T, D, B>` where:
- `T`: The wrapper class (e.g., `Node`, `Element`)  
- `D`: The native DOM class (e.g., `dom::Node`, `dom::Element`)
- `B`: The base wrapper class for inheritance (e.g., `Element` extends `Node`)

### Inheritance Hierarchy

```
Node (wraps dom::Node)
├── Element (wraps dom::Element, extends Node)
│   ├── HTMLElement (wraps dom::HTMLElement, extends Element)
│   │   ├── HTMLDivElement (wraps dom::HTMLDivElement, extends HTMLElement)
│   │   ├── HTMLSpanElement (wraps dom::HTMLSpanElement, extends HTMLElement)
│   │   └── ... (other HTML elements)
│   └── Text (wraps dom::Text, extends Node)
├── Document (wraps dom::Document, extends Node)
└── Console (utility class, no DOM equivalent)
```

## Key Classes

### Node (`node.hpp`, `node.cpp`)
- Base class for all DOM nodes
- Implements EventTarget interface (addEventListener, removeEventListener, dispatchEvent)
- Properties: nodeName, nodeType, nodeValue, parentNode, etc.
- Methods: appendChild, removeChild, insertBefore, etc.

### Element (`element.hpp`, `element.cpp`)
- Base class for all DOM elements
- Properties: tagName, id, className, innerHTML, outerHTML
- Methods: getAttribute, setAttribute, hasAttribute, querySelector, etc.

### HTMLElement (`html_element.hpp`, `html_element.cpp`)
- Base class for all HTML elements
- Properties: innerText, hidden
- Methods: click, focus, blur

### Document (`document.hpp`, `document.cpp`)
- Document root node
- Properties: documentElement, body, title
- Methods: createElement, createTextNode, getElementById, querySelector, etc.

### Text (`text.hpp`, `text.cpp`)
- Text node implementation
- Properties: data, length, wholeText
- Methods: substringData, appendData, insertData, deleteData, etc.

### Console (`console.hpp`, `console.cpp`)
- JavaScript console implementation
- Methods: log, info, warn, error, debug, trace, assert, clear
- Creates global `console` object

## Implementation Patterns

### Property Accessors

```cpp
// Read-only property
instanceTemplate->SetAccessor(
    String::NewFromUtf8(isolate, "nodeName").ToLocalChecked(),
    NodeNameGetter,
    nullptr,  // no setter
    Local<Value>(),
    AccessControl::DEFAULT,
    PropertyAttribute::ReadOnly);

// Read-write property
instanceTemplate->SetAccessor(
    String::NewFromUtf8(isolate, "nodeValue").ToLocalChecked(),
    NodeValueGetter,
    NodeValueSetter);
```

### Method Binding

```cpp
instanceTemplate->Set(
    String::NewFromUtf8(isolate, "appendChild").ToLocalChecked(),
    FunctionTemplate::New(isolate, AppendChild));
```

### Class Initialization

```cpp
static v8::Local<v8::Function> Initialize(v8::Isolate *isolate)
{
    return scripting_base::ObjectWrap<T, D, B>::Initialize(isolate);
}
```

### Instance Creation

```cpp
static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<D> native)
{
    return scripting_base::ObjectWrap<T, D, B>::NewInstance(isolate, native);
}
```

## Binding Registration

The `binding.cpp` file initializes all DOM classes and registers them with V8:

```cpp
void Initialize(v8::Isolate *isolate, v8::Local<v8::Object> global)
{
    // Initialize constructors
    auto nodeConstructor = Node::Initialize(isolate);
    auto elementConstructor = Element::Initialize(isolate);
    // ... other classes

    // Register as globals
    global->Set(context, String::NewFromUtf8(isolate, "Node").ToLocalChecked(), 
                nodeConstructor).Check();
    // ... other registrations

    // Create global console object
    auto consoleObject = Console::CreateConsoleObject(isolate);
    global->Set(context, String::NewFromUtf8(isolate, "console").ToLocalChecked(), 
                consoleObject).Check();
}
```

## Key Differences from N-API Implementation

### No N-API Dependencies
- Uses V8 APIs directly (`v8::Object`, `v8::Function`, etc.)
- No `Napi::` namespace usage
- No NAPI environment (`Napi::Env`) dependencies

### Direct V8 Integration
- Uses `v8::FunctionTemplate` and `v8::ObjectTemplate`
- Direct V8 property accessors and method callbacks
- Better performance and integration with V8 engine

### Simplified Inheritance
- Uses template parameters for inheritance instead of complex NAPI patterns
- More type-safe with compile-time inheritance checking

## Usage

### Initialization
```cpp
#include <client/script_bindings/dom/binding.hpp>

// In V8 context setup
script_bindings::dom::Initialize(isolate, global);
```

### Creating DOM Objects
```cpp
// From native DOM objects
auto element = document->createElement("div");
auto jsElement = Element::NewInstance(isolate, element);

// Or directly use existing JavaScript constructors
// new Element() will work in JavaScript after initialization
```

## Future Extensions

To add new DOM classes:

1. Create header/implementation files following the pattern
2. Inherit from appropriate base class using ObjectWrap
3. Implement ConfigureFunctionTemplate, Initialize, and NewInstance
4. Add to binding.cpp initialization

## Testing

The bindings can be tested by:
1. Creating V8 contexts with the bindings initialized
2. Executing JavaScript that uses DOM APIs
3. Verifying proper integration with native DOM backend

Example JavaScript test:
```javascript
const div = document.createElement('div');
div.id = 'test';
div.innerHTML = '<span>Hello World</span>';
document.body.appendChild(div);
console.log('Element created:', div.tagName);
```