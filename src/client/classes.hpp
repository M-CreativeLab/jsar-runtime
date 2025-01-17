#pragma once

/**
 * @namespace browser
 * The `browser` namespace contains classes related to the browser window,
 */
namespace browser
{
  class Location; // Represents the location (URL) of a browser window
  class Window;   // Represents a browser window
}

/**
 * @namespace dom
 * The `dom` namespace contains classes related to the Document Object Model (DOM),
 * such as `Document` and `Element`.
 */
namespace dom
{
  class Document; // Represents an HTML or XML document
  class Element;  // Represents an element in the DOM tree
}

/**
 * @namespace builtin_scene
 * The `builtin_scene` namespace contains classes related to scene management,
 * such as `Scene` and `Hierarchy`.
 */
namespace builtin_scene
{
  class Scene;     // Represents a 3D scene
  class Hierarchy; // Represents the hierarchy of objects in a scene
}

/**
 * @namespace client_graphics
 * The `client_graphics` namespace contains classes related to graphics rendering,
 * such as `WebGLContext` and `WebGL2Context`.
 */
namespace client_graphics
{
  class WebGLContext;  // Represents a WebGL 1.0 rendering context
  class WebGL2Context; // Represents a WebGL 2.0 rendering context
}

/**
 * @namespace client_xr
 * The `client_xr` namespace contains classes related to extended reality (XR) devices,
 * such as `XRDeviceClient`.
 */
namespace client_xr
{
  class XRDeviceClient; // Represents a client for interacting with XR devices
}

/**
 * @namespace media_client
 * The `media_client` namespace contains classes related to media playback,
 * such as `MediaPlayer` and `AudioPlayer`.
 */
namespace media_client
{
  class MediaPlayer; // Represents a media player for video and audio playback
  class AudioPlayer; // Represents an audio player for audio playback
}
