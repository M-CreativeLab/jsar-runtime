# Debugging

Learn how to debug your JSAR applications using integrated development tools and browser-based debugging capabilities.

## Chrome DevTools Integration

JSAR provides full Chrome DevTools support through the Chrome DevTools Protocol (CDP), allowing you to debug your spatial applications using familiar web development tools.

### Setting Up Debugging

1. Start your JSAR application in development mode
2. Open Chrome and navigate to `chrome://inspect`
3. Your JSAR application will appear in the remote targets list
4. Click "inspect" to open the DevTools

### Features Available

- **Console**: Execute JavaScript commands and view console output
- **Elements**: Inspect DOM structure and CSS styles
- **Sources**: Set breakpoints and step through code
- **Network**: Monitor HTTP requests and responses
- **Performance**: Profile application performance
- **Memory**: Analyze memory usage and detect leaks

## Spatial Debugging

JSAR extends traditional web debugging with spatial-specific features:

### 3D Scene Inspector
- View scene graph hierarchy
- Inspect 3D object properties
- Monitor spatial transformations
- Debug collision detection

### XR Session Debugging
- Track XR session state
- Monitor input sources
- Debug hand tracking data
- Inspect WebXR pose information

## Best Practices

### Performance Debugging
- Use the Performance tab to identify frame rate issues
- Monitor WebGL draw calls and texture usage
- Profile JavaScript execution in spatial contexts

### Common Issues
- Coordinate system transformations
- Asset loading failures
- Input handling problems
- WebXR compatibility issues