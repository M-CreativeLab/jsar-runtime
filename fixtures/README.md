# Fixtures Directory

The `fixtures` directory contains various HTML files and other resources used for testing and examples.

## Purpose

The purpose of the files in the `fixtures` directory is to provide test cases and examples for development and testing purposes.

## Contents

The directory contains the following types of files:

- HTML files for testing and examples, including:
  - **Flexbox Layout Tests:**
    - `fixtures/html/layout-flexbox-alignment.html` - Flexbox alignment properties
    - `fixtures/html/layout-flexbox-column.html` - Column layout tests
    - `fixtures/html/layout-flexbox-grow-shrink.html` - Flex grow/shrink behavior
    - `fixtures/html/layout-flexbox-nested.html` - Nested flexbox layouts
    - `fixtures/html/layout-flexbox-wrap.html` - Flex wrap functionality
    - `fixtures/html/text-in-flexbox.html` - Text content in flexbox (enhanced with comment tests)
  - **Flexbox Comment Node Tests (New):**
    - `fixtures/html/layout-flexbox-comment-nodes.html` - Basic comment node scenarios in flexbox
    - `fixtures/html/layout-flexbox-comment-nodes-advanced.html` - Advanced flexbox properties with comments
    - `fixtures/html/layout-flexbox-comment-nodes-edge-cases.html` - Edge cases and stress tests with comments
  - **Other Layout Tests:**
    - `fixtures/html/layout-grid-*.html` - CSS Grid layout tests
  - **Canvas and WebGL Tests:**
    - `fixtures/html/babylon-bounding-box.html`
    - `fixtures/html/canvas.html`
  - **And many others for various web technologies**
- Other resources like `.gitignore` and `.gitkeep` files

### Flexbox Comment Node Tests

The flexbox comment node tests specifically verify that HTML comment nodes are properly ignored during flexbox layout calculations. These tests cover:

- Comments between flex items
- Comments at container boundaries (leading/trailing)
- Comments in nested flexbox layouts
- Comments with all flex-direction values (row, column, row-reverse, column-reverse)
- Comments with various justify-content and align-items properties
- Comments in flex-wrap scenarios
- Comments with flex-grow and flex-shrink properties
- Edge cases: empty containers, malformed comments, mixed content
- Performance testing with high comment density

## Usage

To use the files in the `fixtures` directory for testing and development purposes, you can open the HTML files in a web browser or use them in your development environment to test various features and functionalities.
