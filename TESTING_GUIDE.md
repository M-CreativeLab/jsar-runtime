# Testing and Validation Guide for TransmuteBrowser

This document outlines how to test the new TransmuteBrowser architecture and screen-space GUI renderer.

## Build and Run (macOS only)

```bash
# Build the project
make darwin

# Run the desktop example
./build/examples/desktop_opengl --help
```

## New Features to Test

### 1. Screen-Space GUI Components

- **StatPanel**: Should appear in the top-left corner showing FPS and uptime
- **InputBox**: Should appear at the bottom center of the window
  - Click to focus the input box (green border when focused)
  - Type URLs and press Enter to open new content
  - Placeholder text: "Enter URL..."

### 2. Multiple Content Management

- Start with default content at startup (as specified by `-n` parameter)
- Use the input box to open additional content instances
- Each content should have its own spatial position
- Content should be positioned in a grid layout (3 per row)

### 3. Content Dragging

- Each content instance should have a bar component beneath it
- Bar shows "Content X ⋮⋮" where X is the content ID
- Click and drag the bar to move content spatially
- Bar should change color when hovered/dragged:
  - Default: Dark gray (#3a3a3a)
  - Hovered: Light gray (#505050)  
  - Dragging: Blue (#0080ff)

### 4. API Testing

Test the new Content management API:
- `openContent(url)` - Opens new content at calculated position
- `closeContent(id)` - Closes content by ID
- `getContentById(id)` - Retrieves content by ID
- `getContentByPosition(pos, tolerance)` - Finds content near position
- `getAllContents()` - Returns all active content instances

## Expected Behavior

### GUI Interactions
1. **Mouse Input**:
   - Left click on input box should focus it
   - Left click on content bars should start dragging
   - Mouse movement while dragging should move content

2. **Keyboard Input**:
   - Text input should work in focused input box
   - Arrow keys should move cursor in input box
   - Enter should submit URL and open new content
   - Backspace/Delete should work for text editing

### Content Management  
1. **Spatial Positioning**:
   - Content should be positioned in 3D space
   - Default position: (0, 0, 0.35)
   - Grid layout: 3 content instances per row
   - Spacing: 2.0 units between instances

2. **Lifecycle**:
   - Content should be created via constellation->open()
   - Content should be properly cleaned up when closed
   - Bar components should be created automatically

### Error Handling
- Invalid URLs should not crash the application
- Failed content creation should be logged
- Dragging should stop when mouse is released

## Visual Validation

### GUI Components
- Input box should be 400x40 pixels, centered at bottom
- Input box should have dark background with border
- Text should be white on dark background
- Cursor should blink green when focused

### Content Bars
- Bars should be 200x30 pixels
- Positioned beneath each content instance
- Should show content ID and drag handle (⋮⋮)
- Should follow content position updates

### Overall Layout
- StatPanel in top-left corner
- InputBox at bottom center
- Content bars beneath each content instance
- All screen-space elements should render on top

## Performance Considerations

- Screen renderer should not impact main rendering performance
- GUI components should only update when needed
- Content dragging should be smooth (no stuttering)
- Multiple content instances should not degrade performance significantly

## Integration Points

The new architecture integrates with existing systems:
- **Constellation**: For content lifecycle management
- **TrContentRuntime**: Wrapped by Content class
- **XR Renderer**: For spatial positioning
- **Window Context**: For input handling and display
- **Environment Renderer**: Should continue working normally