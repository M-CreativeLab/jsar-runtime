# HTMLIframeElement Support

JSAR Runtime now supports basic HTMLIframeElement creation and DOM recognition.

## Features

- ✅ Create `<iframe>` elements using `document.createElement('iframe')`
- ✅ Add iframe elements to the DOM tree
- ✅ Query iframe elements using DOM APIs (`querySelector`, `getElementById`, etc.)
- ✅ Set and get iframe attributes (src, width, height, etc.)
- ❌ Content loading and rendering (not implemented)
- ❌ Sandboxing and security features (not implemented)

## Usage Examples

### Creating an iframe element programmatically

```javascript
// Create a new iframe element
const iframe = document.createElement('iframe');

// Set attributes
iframe.src = 'about:blank';
iframe.width = '300';
iframe.height = '200';
iframe.id = 'my-iframe';

// Add to document
document.body.appendChild(iframe);

// Query the element
const foundIframe = document.getElementById('my-iframe');
console.log(foundIframe.tagName); // "IFRAME"
```

### Using iframe in HTML

```html
<!DOCTYPE html>
<html>
<head>
  <title>Iframe Example</title>
</head>
<body>
  <!-- Static iframe element -->
  <iframe src="about:blank" width="400" height="300"></iframe>
  
  <script>
    // Query existing iframe
    const iframe = document.querySelector('iframe');
    console.log('Found iframe:', iframe.nodeName);
  </script>
</body>
</html>
```

### Querying iframe elements

```javascript
// Get all iframe elements
const iframes = document.querySelectorAll('iframe');
const iframesByTag = document.getElementsByTagName('iframe');

// Get specific iframe by ID
const myIframe = document.getElementById('my-iframe');

// Check element type
console.log(iframe instanceof Element); // true
```

## Limitations

This implementation provides basic iframe element support for DOM structure purposes only. The following features are **not implemented**:

- Content loading from src URLs
- iframe content rendering
- Sandboxing and security isolation
- Cross-origin restrictions
- Window/contentWindow access
- Document access within iframe

The iframe elements are treated as regular DOM elements that can be created, styled, and positioned, but do not load or display any content.

## Testing

A test file is available at `fixtures/html/iframe-test.html` that demonstrates all supported iframe operations.