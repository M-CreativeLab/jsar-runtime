# Integration with Unity

In addition to the official methods of running JSAR, we also support integration with Unity for developers who want to extend JSAR's runtime platform. 

> This chapter is intended for readers with a basic understanding of Unity. If you are unfamiliar with Unity, we recommend learning its basics first.

> We do not currently provide a public Unity integration SDK. If needed, please contact us through Discord or GitHub to request the SDK and intermediate protocols.

## Supported Platforms

JSAR's Unity SDK supports the following platforms:

- Windows x86_64
- MacOS x86_64
- MacOS arm64 (M series)
- Android arm64

## Pipeline Support

Currently, JSAR supports the following Unity pipelines:

- [x] Built-in Render Pipeline
- [ ] Universal Render Pipeline

## Installation

After obtaining the UPM package, install it locally in Unity.

## Usage

You can use JSAR's Unity SDK in various ways.

### Prefabs

You can use the "Prefabs/TransmuteBow.prefab," which represents an instance of a spatial app. Once initialized, you can use it.

### Scripts

The prefabs are essentially implemented by attaching the "Scripts/Transmute.cs" script. You can also directly attach this script to any game object and control it through code.

We provide some configurable parameters.

#### Static Parameter `string ApplicationCacheDirectory`

With this parameter, you can specify the cache directory for JSAR, used to store JSAR's resource files, such as model files, textures, scripts, audio, etc.

#### Static Parameter `bool EnableLoggingOfCoreByDefault = false`

With this parameter, you can specify whether JSAR should enable logging by default.

#### `string ScriptName`

If you are using the prefab method, you can configure this parameter to specify the path of the XSML file to run, supporting both local and network paths.

#### `bool AutoStart = true`

If you are using the prefab method, you can configure this parameter to specify whether the XSML file should start running automatically.

#### `Material DefaultMat`

Default material, initially a dynamically created Standard material. You can also specify your own material using this parameter, and it will be used to create materials for each app.

#### `Material DefaultLineMat`

Default line material, initially a dynamically created Line material. You can specify your own material using this parameter, and it will be used to create line materials for each app.

#### `bool UseTransformToReact = false`

JSAR scales objects based on their Bounding Box after loading to ensure consistent sizes. There are two scaling options: one based on the parent object's transform and the other based on the Box Collider. You can specify which one to use with this parameter.

#### Static Method `void DispatchMouseMoveEvent(Vector3 mousePosition)`

Use this method to dispatch mouse move events. If you want your app to respond to mouse movement events in your runtime environment, you can use this method to dispatch mouse move events.

#### Static Method `void DispatchMouseAction(string actionValue)`

Similar to the previous method but for dispatching mouse keyup and keydown events.

#### Static Method `void DispatchHandTracking(int handType, Vector3[] handJoints, Pose handPose, int handGesture, int handOrientation, bool isNearInteraction)`

Used to dispatch hand tracking events. If you want your app to respond to hand gestures, you need to call this method when updating hand gestures. The parameters are as follows:

- `int handType`: Hand type, 0 for left hand, 1 for right hand.
- `Vector3[] handJoints`: Hand joints, including 21 joints, each represented as a Vector3 with x, y, and z coordinates.
- `Pose handPose`: Hand position and rotation information.
- `int handGesture`: Hand gesture type, 0 for no gesture, 1 for finger, 2 for palm.
- `int handOrientation`: Hand orientation, 0 for palm down, 1 for palm up.
- `bool isNearInteraction`: Whether it's a near interaction. If true, the hand tracking coordinate system will be transformed based on the hand's position to ensure that the coordinate system always faces forward.

#### Static Method `void DispatchHandRay(int handType, Ray handRay)`

Used to dispatch ray events. Internally, JSAR uses rays to interact with objects and UI. You can use this method to dispatch your ray information to JSAR. The parameters are as follows:

- `int handType`: Hand type, 0 for left hand, 1 for right hand.
- `Ray handRay`: Ray information, including the starting point and direction of the ray.

#### Static Method `void DispatchHandPress(int handType, int offset)`

Similar to the previous method, JSAR internally uses the ray to determine selected objects and UI, and uses Press to determine whether a click occurred.

- `int handType`: Hand type, 0 for left hand, 1 for right hand.
- `int offset`: Optional.

#### Static Method `void DispatchHandRelease(int handType, int offset)`

Similar to the previous method, JSAR internally uses the ray to determine selected objects and UI, and uses Release to determine whether a click occurred.

- `int handType`: Hand type, 0 for left hand, 1 for right hand.
- `int offset`: Optional.

### Other Methods

You can also modify and extend JSAR's Unity SDK according to your needs.

## How Unity Interacts with JSAR

In JSAR, achieving simple interaction between Unity and JSAR apps, such as rotation, movement, and scaling, is straightforward. You only need to manipulate the container nodes as per your requirements, without any additional steps.

## Including Necessary Shaders

Because JSAR dynamically loads objects, when integrating into a Unity application, you need to manually include some shaders to ensure the proper rendering of JSAR apps:

- `Standard`: Used for basic material. If you use custom materials, you can exclude this shader.
- `Wireframe/Cull`: Used for rendering wireframes.
- `Wireframe/NoCull`: Used for rendering wireframes.

Please feel free to ask if you have any further questions or need additional information.
