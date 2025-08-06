# Unity embedder API

JSAR provides a [Unity][] plugin that allows developers to embed JSAR in their Unity applications. This document describes how to use the JSAR Unity plugin in Unity applications and provides a reference for the Unity API.

[Unity]: https://unity.com/

## Prerequisites

To use the JSAR Unity plugin, you need to have Unity installed on your machine. You can download Unity from the [Unity download page](https://unity.com/).

### Supported platforms

JSAR Unity SDK supports the following platforms:

- Android arm64
- Windows x86_64
- macOS (x86_64, arm64)

On Windows and macOS, Unity is commonly used as an editor, JSAR's plugin supports developing and testing on these platforms but not rendering to the editor's scene view, developers should build and run the application to see the AR content at Android or YodaOS-Master devices.

### Supported Unity pipelines

Currently, JSAR supports the following Unity pipelines:

- Built-in Render Pipeline
- Universal Render Pipeline (URP)

### Supported stereo rendering modes

JSAR supports both multi-pass and single-pass (instanced/multiview) stereo rendering modes. The stereo rendering mode can be configured using the `XRSettings.stereoRenderingMode` property in Unity, or check out your Unity XR provider's documentation for more details.

## Download and installation

Currently JSAR Unity plugin is not available in the Unity Asset Store or any other public registries because it's in alpha, if you are interested in using JSAR Unity plugin, please contact us at [Rokid Forum](https://forum.rokid.com/).

## Usage guide

In this section, we will guide you in using this plugin both in the built-in render pipeline and the Universal Render Pipeline (URP).

> Coming soon

## C# API Reference

### Class `Transmute`

This class is the main entry point for the JSAR Unity plugin. It provides methods to start and stop the JSAR runtime, and start the applets (namely the Web applications) in your Unity scene.

#### Static properties

__`TransmuteFeature RendererFeature`__

This property is set at the Universal Render Pipeline (URP), you can access it to get the renderer feature instance.

__`Transmute Instance`__

The `Transmute` must be a singleton, this static property provides the singleton instance of the `Transmute` class.

__`string ApplicationCacheDirectory`__

The path to the application cache directory, or set it to customize the cache directory.

#### Static methods

__`MainControllerInputSource GetMainControllerInputSource()`__

Get the main controller input source to update.

__`ScreenControllerInputSource GetScreenControllerInputSource(int index)`__

Get the screen controller input source to update, the int parameter is the screen-controller index.

__`HandedInputSource GetHandInputSource(int handness)`__

Get the handed input source to update, the int parameter is the handness index, 0 for left hand, 1 for right hand.

__`void SetHandInputSourcesEnabled(bool enabled)`__

Enable or disable the handed input sources.

#### Instance properties

__`Camera CameraToUse`__

The camera to use for rendering the Web content, not working in the Universal Render Pipeline (URP).

__`LayerMask LightLayerToUse`__

> This property is not implemented yet.

Select a layer which contains the lights to use for rendering the Web content.

__`Vector3 AppletBoxPosition`__

The initial position of the applet bounding box's position when an WebXR application is started in `local` reference space.

__`GameObject AppletPrefab`__

The prefab to use for initializing the applet's GameObject.

__`string HttpsProxyServer`__

The HTTPS proxy server to use for applet networking such as `https://proxy.server:port`.

__`bool DisableExecCache`__

Disable the cache, if true, the cache will be disabled, it means any applet will be loaded from the network and will not write related files to the cache directory.

__`List<TransmuteApplet> Applets`__

The list of applets that are currently running.

#### Instance events and actions

__`UnityEvent<GameObject, TransmuteApplet> OnAppletCreated`__

This event is triggered when an applet is created.

- `GameObject` is the GameObject that contains the applet.
- `TransmuteApplet` is the `TransmuteApplet` instance that represents the applet.

__`UnityEvent<GameObject> OnXRSessionCreated`__

This event is triggered when an XR session is created.

__`UnityEvent<GameObject> OnXRSessionEnded`__

This event is triggered when an XR session is ended.

__`UnityEvent<GameObject> OnLoading`__

This event is triggered when the applet is loading.

__`UnityEvent<GameObject> OnLoaded`__

This event is triggered when the applet is loaded.

__`UnityEvent<GameObject> OnError`__

This event is triggered when an error occurs.

__`Action<string, string> OnWindowOpen`__

This action is triggered when a `window.open()` event is called in an applet, the first parameter is the URL to open, and the second parameter is the target. The implementator should handle this request if needed.

__`Action<string> OnWindowAlert`__

This action is triggered when a `window.alert()` event is called in an applet, the parameter is the message to alert. The implementator should handle this request if needed.

__`Action<string> OnWindowPrompt`__

This action is triggered when a `window.prompt()` event is called in an applet, the parameter is the message to prompt. The implementator should handle this request if needed.

#### Instance methods

__`TransmuteApplet CreateApplet(string url)`__

This creates an applet from the given URL and returns the `TransmuteApplet` instance.

__`void DestroyApplets()`__

This destroys all the applets that are currently running.

### Enum `TransmuteInputEvent.InputSourceActionState`

This enum represents the action state of an input source, it has two values: `Pressed` and `Released`.

- `Pressed` means the action button is pressed.
- `Released` means the action button is released.

### Enum `TransmuteInputEvent.HandJointIndex`

This enum represents the joint index of the hand, it has the following values:

- `WRIST`
- `THUMB_METACARPAL`
- `THUMB_PROXIMAL`
- `THUMB_DISTAL`
- `THUMB_TIP`
- `INDEX_FINGER_METACARPAL`
- `INDEX_FINGER_PHALANX_PROXIMAL`
- `INDEX_FINGER_PHALANX_INTERMEDIATE`
- `INDEX_FINGER_PHALANX_DISTAL`
- `INDEX_FINGER_TIP`
- `MIDDLE_FINGER_METACARPAL`
- `MIDDLE_FINGER_PHALANX_PROXIMAL`
- `MIDDLE_FINGER_PHALANX_INTERMEDIATE`
- `MIDDLE_FINGER_PHALANX_DISTAL`
- `MIDDLE_FINGER_TIP`
- `RING_FINGER_METACARPAL`
- `RING_FINGER_PHALANX_PROXIMAL`
- `RING_FINGER_PHALANX_INTERMEDIATE`
- `RING_FINGER_PHALANX_DISTAL`
- `RING_FINGER_TIP`
- `LITTLE_FINGER_METACARPAL`
- `LITTLE_FINGER_PHALANX_PROXIMAL`
- `LITTLE_FINGER_PHALANX_INTERMEDIATE`
- `LITTLE_FINGER_PHALANX_DISTAL`
- `LITTLE_FINGER_TIP`
- `END`

### Class `TransmuteInputEvent.XRInputSource`

This is a base class for all the input sources: `MainControllerInputSource`, `ScreenControllerInputSource`, `HandedInputSource` and new input sources in the future.

To get the corresponding input source, you still need to use the static methods in the `Transmute` class, this is because the input sources are global and shared among all the applets, when there is an input source update, all the applets are able to get the update.

#### Instance properties

__`bool Enabled`__

If the input source is enabled or not.

__`int Id`__

The input source id.

#### Instance methods

__`void SetEnabled(bool enabled)`__

Enable or disable the input source.

__`void SetTargetRayPose(Pose pose)`__

The target ray for an input source is the ray that is used to interact with the applet, this method sets the pose of the target ray.

- The `pose` parameter is the pose of the target ray to update.

__`void SetGripPose(Pose pose)`__

The grip pose for an input source is the pose when the user is holding the input device, this method sets the pose of the grip.

- The `pose` parameter is the pose of the grip to update.

__`void SetPrimaryActionState(TransmuteInputEvent.InputSourceActionState state)`__

At WebXR, the primary action is defined as an action that is triggered by the primary button, the implementor should use this method to update the primary action state (pressed or released) of the input source.

- The `state` parameter is the state of the primary action.

__`void SetSqueezeActionState(TransmuteInputEvent.InputSourceActionState state)`__

Similar to the primary action, the squeeze action is also defined by the implementor and used by the application developers.

- The `state` parameter is the state of the squeeze action.

### Class `TransmuteInputEvent.MainControllerInputSource`

This class represents the main controller input source and inherits from `TransmuteInputEvent.XRInputSource`. No new properties or methods are added to this class, please refer to the base class `TransmuteInputEvent.XRInputSource` for more details.

### Class `TransmuteInputEvent.ScreenControllerInputSource`

This class represents the screen controller input source and inherits from `TransmuteInputEvent.XRInputSource`. No new properties or methods are added to this class, please refer to the base class `TransmuteInputEvent.XRInputSource` for more details.

### Class `TransmuteInputEvent.HandedInputSource`

This class represents the handed input source and inherits from `TransmuteInputEvent.XRInputSource`.

#### Instance properties

__`int Handness`__

The handness of the input source, 0 for left hand, 1 for right hand.

#### Instance methods

__`void SetJoint(TransmuteInputEvent.HandJointIndex index, Vector3 position, Quaternion rotation)`__

Set the joint position and rotation of the hand.

- The `index` parameter is the joint index, see `TransmuteInputEvent.HandJointIndex` for more details.
- The `position` parameter is the position of the joint.
- The `rotation` parameter is the rotation of the joint.

__`void ResetJoints()`__

Reset all the joints of the hand to the default pose, the implementor should call this method when the hand is not tracked.
