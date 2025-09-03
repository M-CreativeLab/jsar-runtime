# v8_objectwrap Inheritance Guide

The enhanced `v8_objectwrap` now supports inheritance, allowing you to create class hierarchies similar to the DOM (Node → Element → HTMLElement).

## Overview

The `ObjectWrap` template now accepts an optional third parameter `BaseT` for specifying the base class:

```cpp
template <typename T, typename D = void, typename BaseT = void>
class ObjectWrap
```

- `T`: The class to wrap (same as before)
- `D`: Optional inner instance type (same as before)  
- `BaseT`: Base class type for inheritance (new parameter, defaults to `void` for no inheritance)

## Setting Up Inheritance

### 1. Define Your Class Hierarchy

```cpp
// Base class - no inheritance
class DOMNode : public ObjectWrap<DOMNode>
{
public:
    DOMNode(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args)
        : ObjectWrap<DOMNode>(isolate, args) {}
    
    static std::string Name() { return "Node"; }
    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
};

// Derived class - inherits from DOMNode
class DOMElement : public ObjectWrap<DOMElement, void, DOMNode>
{
public:
    DOMElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args)
        : ObjectWrap<DOMElement, void, DOMNode>(isolate, args) {}
    
    static std::string Name() { return "Element"; }
    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
};

// Further derived class - inherits from DOMElement
class HTMLElement : public ObjectWrap<HTMLElement, void, DOMElement>
{
public:
    HTMLElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args)
        : ObjectWrap<HTMLElement, void, DOMElement>(isolate, args) {}
    
    static std::string Name() { return "HTMLElement"; }
    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
};
```

### 2. Initialize Classes in Inheritance Order

**Important**: Always initialize base classes before derived classes:

```cpp
void InitializeClasses(v8::Isolate *isolate)
{
    // Initialize in inheritance order: base classes first
    DOMNode::Initialize(isolate);
    DOMElement::Initialize(isolate);
    HTMLElement::Initialize(isolate);
}
```

### 3. Configure Function Templates

In your `ConfigureFunctionTemplate` methods, add class-specific properties and methods:

```cpp
void DOMNode::ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl)
{
    // Add Node-specific properties
    tpl->PrototypeTemplate()->Set(
        v8::String::NewFromUtf8(isolate, "nodeType").ToLocalChecked(),
        v8::Number::New(isolate, 1),
        v8::PropertyAttribute::ReadOnly);
}

void DOMElement::ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl)
{
    // Base class configuration is automatically called
    // Add Element-specific properties
    tpl->PrototypeTemplate()->Set(
        v8::String::NewFromUtf8(isolate, "tagName").ToLocalChecked(),
        v8::String::NewFromUtf8(isolate, "DIV").ToLocalChecked(),
        v8::PropertyAttribute::ReadOnly);
}
```

## Using Inheritance

### Type Checking

JavaScript `instanceof` works correctly with the inheritance chain:

```javascript
let htmlElement = new HTMLElement();
console.log(htmlElement instanceof HTMLElement); // true
console.log(htmlElement instanceof Element);     // true
console.log(htmlElement instanceof Node);        // true
```

### Safe Unwrapping

Use `UnwrapAs<BaseType>()` to safely cast to base types:

```cpp
v8::Local<v8::Object> jsObject = /* get JS object */;

// Unwrap to specific type
HTMLElement* htmlElement = HTMLElement::Unwrap(jsObject);

// Unwrap to base types
DOMElement* element = HTMLElement::UnwrapAs<DOMElement>(jsObject);
DOMNode* node = HTMLElement::UnwrapAs<DOMNode>(jsObject);
```

## Key Features

1. **Automatic Prototype Chain**: V8's `FunctionTemplate::Inherit()` sets up the correct prototype chain
2. **Safe Unwrapping**: `UnwrapAs<BaseType>()` method for casting to base types
3. **Initialization Order**: Base classes are automatically initialized when derived classes initialize
4. **Template Storage**: Function templates are stored for inheritance purposes

## Best Practices

1. Always initialize classes in inheritance order (base classes first)
2. Use meaningful class names that reflect the inheritance hierarchy
3. Keep `ConfigureFunctionTemplate` focused on class-specific functionality
4. Use `UnwrapAs<BaseType>()` when you need to access base class functionality
5. Consider virtual functions in your C++ class hierarchy for polymorphic behavior

## Example Usage

See `v8_object_wrap_example.hpp` for a complete working example of Node → Element → HTMLElement inheritance.