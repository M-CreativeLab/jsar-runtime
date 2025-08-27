# Script Execution Order

This document describes the script execution order implementation in JSAR Runtime.

## Overview

JSAR Runtime implements blocking script execution for `HTMLScriptElement` to ensure document-order script execution, matching classic browser behavior.

## Behavior

- Scripts are executed in the order they appear in the document
- Only classic scripts (not `async`, `defer`, or `type="module"`) participate in the execution queue
- If multiple scripts initiate loading simultaneously, execution waits for all previous scripts to finish before proceeding
- Asynchronous loading is still permitted, but execution is blocked until it's the script's turn

## Implementation

The feature is implemented through:

1. **Script Execution Queue** in `BrowsingContext` - tracks pending scripts in document order
2. **Registration** in `HTMLScriptElement::connectedCallback()` - classic scripts register with the queue
3. **Execution Control** in `HTMLScriptElement::compileScript()` - scripts wait for their turn instead of executing immediately
4. **Queue Management** - scripts notify the queue when execution completes

## Example

```html
<!-- These scripts will execute in order a, b, c regardless of download speed -->
<script src="script-a.js"></script>  <!-- Will execute first -->
<script src="script-b.js"></script>  <!-- Will wait for script-a -->
<script src="script-c.js"></script>  <!-- Will wait for script-b -->

<!-- These scripts are excluded from blocking order -->
<script src="async-script.js" async></script>        <!-- Executes when ready -->
<script src="defer-script.js" defer></script>        <!-- Executes after parsing -->
<script type="module" src="module-script.js"></script> <!-- Module loading order -->
```

## Files Modified

- `src/client/dom/browsing_context.hpp` - Added execution queue declaration
- `src/client/dom/browsing_context.cpp` - Implemented queue management
- `src/client/html/html_script_element.hpp` - Added queue integration methods  
- `src/client/html/html_script_element.cpp` - Modified execution flow to use queue