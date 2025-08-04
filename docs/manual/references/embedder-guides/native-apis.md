# Native(C++) embedder API

JSAR could be embedded in any 3d engine or application written in C++, this document describes for embedding developers how to use JSAR in their applications.

## Request for JSAR SDK

JSAR native SDK is not available in the public, if you are interested in using JSAR native SDK, please contact us at [Rokid Forum](https://forum.rokid.com/).

## `TrEmbedder` class

`TrEmbedder` is the main virtual class that should be inherited by the embedding application, it contains the main functions that should be implemented by the embedding application.

```cpp
class SimpleEmbedder : public TrEmbedder
{
public:
  DesktopEmbedder() : TrEmbedder()
  {
    auto renderer = constellation->getRenderer();
    auto api = RenderAPI::Create(kUnityGfxRendererOpenGLCore, getConstellation());
    renderer->setApi(api);
  }
};
```

### Constructor

The `TrEmbedder` constructor signature is:

```cpp
TrEmbedder(TrHostEngine hostEngine = TrHostEngine::None)
```

There is a `TrHostEngine` enum that embedding developers could use to specify the host engine that they are using, the default value is `TrHostEngine::None`, which means the embedding application is a standalone.

The possible values for `TrHostEngine` are:

```cpp
enum class TrHostEngine
{
  None,   // Standalone
  Unity,  // Embedding in Unity
  Unreal, // Embedding in Unreal
  Cocos,  // Embedding in Cocos
};
```

### Virtual Methods

The virtual methods that embedding developers are optional to implement for their applications. This section describes all the virtual methods that developers could implement.

#### `~TrEmbedder()`

If the custom embedder class has its own resources, the developers should release them in a custom destructor.

```cpp
~TrEmbedder() {
  // Release resources
}
```

#### `bool onEvent(TrEvent &event, TrContentRuntime *content)`

This virtual method will be called by the runtime when there is an event to be handled by the embedding application.

```cpp
bool onEvent(TrEvent &event, TrContentRuntime *content) {
  // Handle the event
  return true;
}
```

### Instance Methods

The instance methods are the methods inherited from the `TrEmbedder` class that developers could use to interact with the runtime.

#### `uint32_t getFps()`

This method returns the current frames per second of the runtime.

#### `uint32_t getUptime()`

This method returns the current uptime of the runtime in milliseconds.

#### `void onStart(std::string argJson)`

This method starts the runtime with the given arguments in JSON format:

```json
{
  "applicationCacheDirectory": "path/to/cache",
  "httpsProxyServer": "https://proxy.server:port",
  "enableV8Profiling": true,
  "isXRSupported": true
}
```

The above fields are used to configure the runtime:

| Field                       | Description                                  |
| --------------------------- | -------------------------------------------- |
| `applicationCacheDirectory` | The path to the application cache directory. |
| `httpsProxyServer`          | The HTTPS proxy server to use.               |
| `enableV8Profiling`         | Enable V8 profiling.                         |
| `isXRSupported`             | Enable XR support.                           |

Internally, this method will start the internal components: renderer, content manager, media service and other services.

#### `void onFrame()`

This method should be called in every frame to update the runtime, such as `Update()` in Unity or `Tick()` in Unreal.

Note: This frame in the method name doesn't mean the actual frame especially in the case of XR, it represents an update cycle by the host engine, the frame in OpenXR or WebXR should be configured by the `configureXrDevice(bool enabled, xr::TrDeviceInit &init)` method which will be explained later.

#### `bool configureXrDevice(bool enabled, xr::TrDeviceInit &init)`

This method is used to configure the XR device, the `enabled` parameter is used to enable or disable the XR device, and the `init` parameter is used to configure the XR device.

| Field                      | Description                                                                  |
| -------------------------- | ---------------------------------------------------------------------------- |
| `init.active`              | If the XR device is active.                                                  |
| `init.stereoRenderingMode` | The stereo rendering mode that defines how runtime consumes the `onFrame()`. |

The possible values for `stereoRenderingMode` are:

```cpp
enum class StereoRenderingMode
{
  MultiPass = 0,            // Multi-pass rendering
  SinglePass = 1,           // Single-pass rendering
  SinglePassInstanced = 2,  // Single-pass instanced rendering
  SinglePassMultiview = 3,  // Single-pass multiview rendering
};
```

In multi-pass rendering, the runtime will call `onFrame()` for each eye, that means one call for the left eye and another call for the right eye. However, in both single-pass rendering modes, the runtime will call `onFrame()` once, and the application should render the scene for both eyes which might be more efficient.

#### `void shutdown()`

This method should be called to shutdown the runtime, it will release all the resources and stop the internal components, it will be returned when all the resources are released.

```cpp
embedder->shutdown();
// Wait until the runtime is shutdown
```
