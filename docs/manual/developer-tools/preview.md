# Preview

The JSAR preview system allows you to test your spatial applications in real-time during development, providing immediate feedback and iteration capabilities.

## Live Preview

### Development Server
The JSAR development server provides hot reload functionality:

```bash
npm run docs:dev
```

This starts a local server with:
- Automatic file watching
- Hot module replacement
- Live preview updates
- Error overlay display

### Preview Modes

#### Desktop Preview
Test your application in a desktop browser environment:
- WebXR emulation for basic testing
- Mouse and keyboard input simulation
- Responsive design validation

#### Device Preview
Test on actual XR devices:
- Deploy to Rokid AR Studio
- Use JSAR Canary for quick testing
- QR code deployment for instant updates

## Preview Features

### Real-time Updates
- Instant reflection of code changes
- No need to restart the application
- Preserves application state when possible

### Debug Overlay
- Performance metrics display
- Console output overlay
- Error reporting with stack traces

### Multi-device Testing
- Simultaneous preview on multiple devices
- Synchronized debugging sessions
- Cross-platform compatibility validation

## Configuration

### Preview Settings
Configure preview behavior in your JSAR project:

```json
{
  "preview": {
    "autoReload": true,
    "showDebugInfo": true,
    "device": "rokid-ar-studio"
  }
}
```

### Network Configuration
Set up network access for device preview:
- Configure firewall rules
- Set up local network access
- Enable HTTPS for secure contexts