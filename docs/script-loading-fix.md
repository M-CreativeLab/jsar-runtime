# Script Loading Fix for JSAR Runtime

## Problem

In JSAR runtime version 0.9.0, external JavaScript files loaded via `<script src="...">` tags were loading asynchronously. This caused issues where subsequent inline scripts would execute before the external scripts finished loading, resulting in "undefined" errors when trying to access functions or variables defined in external scripts.

### Example of the Issue

```html
<script src="./my-library.js"></script>
<script>
  // This would fail because my-library.js hasn't loaded yet
  myLibraryFunction(); // Error: myLibraryFunction is not defined
</script>
```

## Solution

The fix preprocesses HTML content before it's passed to the DOM parser. External scripts are fetched synchronously and inlined into the HTML, ensuring proper execution order.

### How it works

1. **Detection**: The runtime scans HTML content for `<script>` tags with `src` attributes
2. **Fetching**: External scripts are fetched using the existing ResourceLoader infrastructure
3. **Inlining**: External script tags are replaced with inline script tags containing the fetched content
4. **Preservation**: Inline scripts and execution order are preserved

### Example transformation

**Before (problematic):**
```html
<script src="./my-library.js"></script>
<script>
  myLibraryFunction(); // Fails - external script not loaded yet
</script>
```

**After (fixed):**
```html
<script>
function myLibraryFunction() {
  console.log('Library function called');
}
// ... rest of my-library.js content
</script>
<script>
  myLibraryFunction(); // Works - function is now defined
</script>
```

## Configuration

The script inlining behavior can be disabled by setting the environment variable:
```bash
JSAR_DISABLE_SCRIPT_INLINING=yes
```

## Test Cases

Several test cases are provided in the `fixtures/html/` directory:

- `test-script-loading.html` - Basic external script loading
- `test-multiple-scripts.html` - Multiple external scripts
- `test-mixed-scripts.html` - Mixed inline and external scripts
- `demo-script-loading-issue.html` - Comprehensive demonstration

## Benefits

- ✅ Fixes the script loading order issue
- ✅ Maintains backward compatibility
- ✅ Uses existing ResourceLoader infrastructure (supports caching, proxies, etc.)
- ✅ Handles error cases gracefully
- ✅ Can be disabled if needed
- ✅ Works with multiple external scripts
- ✅ Preserves inline scripts and their positions

## Technical Details

The fix is implemented in `lib/runtime2/index.ts` in the `TransmuteRuntime2` class:

- `processHtmlForSynchronousScripts()` - Main processing method
- `inlineExternalScripts()` - Handles the actual script inlining logic

The implementation uses regex parsing to identify external script tags and string manipulation to replace them with inline equivalents.