# WebXR basis

The [WebXR Device API][] defines the core of the WebXR feature set, managing the selection of output devices, render the 3D scene to the chosen device at the appropriate frame rate, and manage motion vectors created using input controllers.

This reference provides the JSAR's implementation details of the basis of WebXR Device API.

## Convention

In this document, we will use the following convention to describe implementation status:

| Code | Description |
| --- | --- |
| *NI* | <b>N</b>ot <b>I</b>mplemented |
| *PI* | <b>P</b>artially <b>I</b>mplemented |
| *YI* | <b>Y</b>et to be <b>I</b>mplemented |

## Initialization

The initialization of the WebXR device is achieved by the user agent, and the developer only needs to use the initialized `XRSystem` instance `navigator.xr`.

### Class `XRSystem`

The [WebXR Device API][] interface `XRSystem` provides methods which let you get access to an `XRSession` object representing a WebXR session. With that object, you can use it to interact with the XR device.

### Instance methods

__[`isSessionSupported()`](https://developer.mozilla.org/en-US/docs/Web/API/XRSystem/isSessionSupported)__

> Returns a promise which resolves to `true` if the user agent supports the given session mode. Resolves to `false` if the specified mode isn't supported.

__[`requestSession()`](https://developer.mozilla.org/en-US/docs/Web/API/XRSystem/requestSession)__

> Returns a promise that resolves to a new `XRSession` with the specified session mode.

### Events

__[`devicechange`](https://developer.mozilla.org/en-US/docs/Web/API/XRSystem/devicechange_event)__ (*NI*)

> Sent when the set of available XR devices has changed. Also available using the `ondevicechange` event handler.

## Session

A WebXR session represents the connection between the user agent and the XR device. It is created by calling `navigator.xr.requestSession()`.

### Class `XRSession`

#### Instance properties

__[`depthDataFormat`](https://developer.mozilla.org/en-US/docs/Web/API/XRSession/depthDataFormat)__ (*NI*)

> Returns the depth-sensing data format with which the session was configured.

__[`depthUsage`](https://developer.mozilla.org/en-US/docs/Web/API/XRSession/depthUsage)__ (*NI*)

> Returns the depth-sensing usage with which the session was configured.

__[`domOverlayState`](https://developer.mozilla.org/en-US/docs/Web/API/XRSession/domOverlayState)__ (*NI*)

> Provides information about the DOM overlay, if the feature is enabled.

__[`environmentBlendMode`](https://developer.mozilla.org/en-US/docs/Web/API/XRSession/environmentBlendMode)__ (*NI*)

> Returns this session's blend mode which denotes how much of the real-world environment is visible through the XR device and how the device will blend the device imagery with it.

__[`inputSources`](https://developer.mozilla.org/en-US/docs/Web/API/XRSession/inputSources)__

> Returns a list of this session's `XRInputSource`s, each representing an input device used to control the camera and/or scene.

__[`interactionMode`](https://developer.mozilla.org/en-US/docs/Web/API/XRSession/interactionMode)__ (*NI*)

> Returns this session's interaction mode, which describes the best space (according to the user agent) for the application to draw interactive UI for the current session.

__[`preferredReflectionFormat`](https://developer.mozilla.org/en-US/docs/Web/API/XRSession/preferredReflectionFormat)__ (*NI*)

> Returns this session's preferred reflection format used for lighting estimation texture data.

__[`renderState`](https://developer.mozilla.org/en-US/docs/Web/API/XRSession/renderState)__

> An `XRRenderState` object which contains options affecting how the imagery is rendered. This includes things such as the near and far clipping planes (distances defining how close and how far away objects can be and still get rendered), as well as field of view information.

__[`visibilityState`](https://developer.mozilla.org/en-US/docs/Web/API/XRSession/visibilityState)__ (*NI*)

> A string indicating whether or not the session's imagery is visible to the user, and if so, if it's being visible but not currently the target for user events.

#### Instance methods

__[`end()`](https://developer.mozilla.org/en-US/docs/Web/API/XRSession/end)__

> Ends the WebXR session. Returns a promise which resolves when the session has been successfully ended.

__[`cancelAnimationFrame()`](https://developer.mozilla.org/en-US/docs/Web/API/XRSession/cancelAnimationFrame)__

> Removes a callback from the animation frame painting callback from `XRSession`'s set of animation frame rendering callbacks, given the identifying handle returned by a previous call to `requestAnimationFrame()`.

__[`requestAnimationFrame()`](https://developer.mozilla.org/en-US/docs/Web/API/XRSession/requestAnimationFrame)__

> Schedules the specified method to be called the next time the user agent is working on rendering an animation frame for the WebXR device. Returns an integer value which can be used to identify the request for the purposes of canceling the callback using `cancelAnimationFrame()`. This method is comparable to the `Window.requestAnimationFrame()` method.

__[`requestHitTestSource()`](https://developer.mozilla.org/en-US/docs/Web/API/XRSession/requestHitTestSource)__ (*NI*)

> Requests an `XRHitTestSource` object that handles hit test subscription.

__[`requestHitTestSourceForTransientInput()`](https://developer.mozilla.org/en-US/docs/Web/API/XRSession/requestHitTestSourceForTransientInput)__ (*NI*)

> Requests an `XRTransientInputHitTestSource` object that handles hit test subscription for a transient input source.

__[`requestLightProbe()`](https://developer.mozilla.org/en-US/docs/Web/API/XRSession/requestLightProbe)__ (*NI*)

> Requests an `XRLightProbe` object that handles light probe subscription.

__[`requestReferenceSpace()`](https://developer.mozilla.org/en-US/docs/Web/API/XRSession/requestReferenceSpace)__

> Requests that a new `XRReferenceSpace` of the specified type be created. Returns a promise which resolves with the `XRReferenceSpace` or `XRBoundedReferenceSpace` which was requested, or throws a `NotSupportedError` `DOMException` if the requested space type isn't supported by the device.

__[`updateRenderState()`](https://developer.mozilla.org/en-US/docs/Web/API/XRSession/updateRenderState)__

> Updates the properties of the session's render state.

__`updateCollisionBox(min: number[], max: number[]): void`__

Update the collision box for the session, the collision box is used to do the hit testing in the host.

In JSAR's __Defferred Composition__, we leverage the hit testing to host to decrease the latency when user interacts with the virtual objects or the UI elements. With this method, the client(application) process does not need to do hit testing in every frame and avoid the latency from the client to the host.
   
This method `updateCollisionBox` is used to tell the host the collision box of a virtual object, then the host will do the hit testing to compute the hit point by itself.

- `min` The minimum point of the collision box.
- `max` The maximum point of the collision box.

#### Events

__[`end`](https://developer.mozilla.org/en-US/docs/Web/API/XRSession/end_event)__

> Sent to the `XRSession` object after the WebXR session has ended and all hardware-related functions have completed. The event is represented by an object of type `XRSessionEvent`. Also available through the onend event handler property.

__[`inputsourceschange`](https://developer.mozilla.org/en-US/docs/Web/API/XRSession/inputsourceschange_event)__

> An event of type `XRInputSourcesChangeEvent` sent to the `XRSession` when the list of active XR input sources has changed. Also available through the `oninputsourceschange` event handler property.

__[`select`](https://developer.mozilla.org/en-US/docs/Web/API/XRSession/select_event)__

> An event of type `XRInputSourceEvent` which is sent to the session when one of the session's input sources has successfully completed a primary action. This generally corresponds to the user pressing a trigger, touchpad, or button, speaks a command, or performs a recognizable gesture. The `select` event is sent after the `selectstart` event is sent and immediately before the `selectend` event is sent. If `select` is not sent, then the select action was aborted before being completed. Also available through the `onselect` event handler property.

__[`selectend`](https://developer.mozilla.org/en-US/docs/Web/API/XRSession/selectend_event)__

> An event of type `XRInputSourceEvent` which gets sent to the session object when one of its input devices finishes its primary action or gets disconnected while in the process of handling a primary action. For example: for button or trigger actions, this means the button has been released; for spoken commands, it means the user has finished speaking. This is the last of the three `select*` events to be sent. Also available through the `onselectend` event handler property.

__[`selectstart`](https://developer.mozilla.org/en-US/docs/Web/API/XRSession/selectstart_event)__

> An event of type `XRInputSourceEvent` which is sent to the session object when one of its input devices is first engaged by the user in such a way as to cause the primary action to begin. This is the first of the `session*` event to be sent. Also available through the `onselectstart` event handler property.

__[`squeeze`](https://developer.mozilla.org/en-US/docs/Web/API/XRSession/squeeze_event)__

> An `XRInputSourceEvent` sent to indicate that a primary squeeze action has successfully completed. This indicates that the device being squeezed has been released, and may represent dropping a grabbed object, for example. It is sent immediately before the `squeezeend` event is sent to indicate that the squeeze action is over. Also available through the `onsqueeze` event handler property.

__[`squeezestart`](https://developer.mozilla.org/en-US/docs/Web/API/XRSession/squeezestart_event)__

> An event of type `XRInputSourceEvent` which is sent to the `XRSession` when the user initially squeezes a squeezable controller. This may be, for example, a trigger which is used to represent grabbing objects, or might represent actual squeezing when wearing a haptic glove. Also available through the `onsqueezestart` event handler property.

__[`squeezeend`](https://developer.mozilla.org/en-US/docs/Web/API/XRSession/squeezeend_event)__

> An `XRInputSourceEvent` sent to the `XRSession` when the primary squeeze action ends, whether or not the action was successful. Also available using the `onsqueezeend` event handler property.

__[`visibilitychange`](https://developer.mozilla.org/en-US/docs/Web/API/XRSession/visibilitychange_event)__ (*NI*)

> An `XRSessionEvent` which is sent to the session when its visibility state as indicated by the `visibilityState` changes. Also available through the `onvisibilitychange` event handler property.

### Class `XRRenderState`

#### Instance properties

__[`baseLayer`](https://developer.mozilla.org/en-US/docs/Web/API/XRRenderState/baseLayer)__

> The `XRWebGLLayer` from which the user agent's compositing system obtains the image for the XR session.

__[`depthFar`](https://developer.mozilla.org/en-US/docs/Web/API/XRRenderState/depthFar)__ (*NI*)

> The distance, in meters, of the far clip plane from the viewer. The far clip plane is the plane which is parallel to the display beyond which rendering of the scene no longer takes place. This, essentially, specifies the maximum distance the user can see.

__[`depthNear`](https://developer.mozilla.org/en-US/docs/Web/API/XRRenderState/depthNear)__ (*NI*)

> The distance, in meters, of the near clip plane from the viewer. The near clip plane is the plane, parallel to the display, at which rendering of the scene begins. Any closer to the viewer than this, and no portions of the scene are drawn.

__[`inlineVerticalFieldOfView`](https://developer.mozilla.org/en-US/docs/Web/API/XRRenderState/inlineVerticalFieldOfView)__ (*NI*)

> The default vertical field of view, defined in radians, to use when the session is in inline mode. null for all immersive sessions.

JSAR won't support the inline mode, so this property would never be implemented.

__[`layers`](https://developer.mozilla.org/en-US/docs/Web/API/XRRenderState/layers)__ (*NI*)

> An array of `XRLayer` objects which define the layers to be composited into the final image.

## Frame loop

The frame loop classes are used to manage the rendering of the scene to the XR device.

### Class `XRFrame`

#### Instance properties

__[`session`](https://developer.mozilla.org/en-US/docs/Web/API/XRFrame/session)__

> The `XRSession` that for which this `XRFrame` describes the tracking details for all objects. The information about a specific object can be obtained by calling one of the methods on the object.

__[`trackedAnchors`](https://developer.mozilla.org/en-US/docs/Web/API/XRFrame/trackedAnchors)__ (*NI*)

> An `XRAnchorSet` containing all anchors still tracked in the frame.

#### Instance methods

__[`createAnchor()`](https://developer.mozilla.org/en-US/docs/Web/API/XRFrame/createAnchor)__ (*NI*)

> Returns a promise which resolves to a free-floating `XRAnchor` object.

__[`fillJointRadii()`](https://developer.mozilla.org/en-US/docs/Web/API/XRFrame/fillJointRadii)__ (*NI*)

> Populates a `Float32Array` with radii for a list of hand joint spaces. Returns `true` if successful for all spaces.

__[`fillPoses()`](https://developer.mozilla.org/en-US/docs/Web/API/XRFrame/fillPoses)__ (*NI*)

> Populates a `Float32Array` with the matrices of the poses, relative to a given base space. Returns `true` if all spaces have a valid pose.

__[`getDepthInformation()`](https://developer.mozilla.org/en-US/docs/Web/API/XRFrame/getDepthInformation)__ (*NI*)

> Returns an `XRCPUDepthInformation` object containing CPU depth information for the frame.

__[`getHitTestResults()`](https://developer.mozilla.org/en-US/docs/Web/API/XRFrame/getHitTestResults)__ (*NI*)

> Returns an array of `XRHitTestResult` objects containing hit test results for a given `XRHitTestSource`.

__[`getHitTestResultsForTransientInput()`](https://developer.mozilla.org/en-US/docs/Web/API/XRFrame/getHitTestResultsForTransientInput)__ (*NI*)

> Returns an array of `XRTransientInputHitTestResult` objects containing hit test results for a given `XRTransientInputHitTestSource`.

__[`getJointPose()`](https://developer.mozilla.org/en-US/docs/Web/API/XRFrame/getJointPose)__

> Returns an `XRJointPose` object providing the pose of a hand joint relative to a given base space.

__[`getLightEstimate()`](https://developer.mozilla.org/en-US/docs/Web/API/XRFrame/getLightEstimate)__ (*NI*)

> Returns an `XRLightEstimate` object containing information about the lighting conditions in the environment.

__[`getPose()`](https://developer.mozilla.org/en-US/docs/Web/API/XRFrame/getPose)__

> Returns an `XRPose` object providing the pose of a space relative to a given base space.

__[`getViewerPose()`](https://developer.mozilla.org/en-US/docs/Web/API/XRFrame/getViewerPose)__

> Returns an `XRViewerPose` object providing the pose of the viewer's head relative to a given base space.

## Spaces

WebXR spaces are used to represent the pose of objects in the 3D scene, such as the viewer, hands and controllers.

Kinds of spaces are inherited from the `XRSpace` class.

### Class `XRSpace`

No defined properties or methods.

### Class `XRReferenceSpace`

> Represents a reference space which is typically expected to remain static for the duration of the `XRSession`. While objects may move within the space, the space itself remains fixed in place. There are exceptions to this static nature; most commonly, an `XRReferenceSpace` may move in order to adjust based on reconfiguration of the user's headset or other motion-sensitive device.

To create a reference space, you need to call the `XRSession.requestReferenceSpace()` method.

```js
xrSession.requestReferenceSpace('local').then((referenceSpace) => {
  // Use the reference space
});
```

#### Instance methods

__[`getOffsetReferenceSpace()`](https://developer.mozilla.org/en-US/docs/Web/API/XRReferenceSpace/getOffsetReferenceSpace)__

> Creates and returns a new reference space object as the same type as the one on which you call the method (so, either `XRReferenceSpace` or `XRBoundedReferenceSpace`). The new reference space can be used to transform a coordinate from the reference space of the object on which the method is called into a different coordinate space. This is useful for positioning objects while rendering, and to perform the needed transforms when changing the viewer's position and/or orientation in 3D space.

#### Events

__[`reset`](https://developer.mozilla.org/en-US/docs/Web/API/XRReferenceSpace/reset_event)__ (*NI*)

> Sent to the `XRReferenceSpace` object when the reference space is reset. This can happen when the user reconfigures their headset or other motion-sensitive device, causing the reference space to be redefined. Also available through the `onreset` event handler property.

#### Reference space types

__`bounded-floor`__ (*NI*)

> An `XRBoundedReferenceSpace` similar to the `local` type, except the user is not expected to move outside a predetermined boundary, given by the `boundsGeometry` in the returned object.

__`local`__

> An `XRReferenceSpace` tracking space whose native origin is located near the viewer's position at the time the session was created. The exact position depends on the underlying platform and implementation. The user isn't expected to move much if at all beyond their starting position, and tracking is optimized for this use case. For devices with six degrees of freedom (6DoF) tracking, the `local` reference space tries to keep the origin stable relative to the environment.

__`local-floor`__ (*NI*)

> An `XRReferenceSpace` similar to the `local` type, except the starting position is placed in a safe location for the viewer to stand, where the value of the y axis is 0 at floor level. If that floor level isn't known, the user agent will estimate the floor level. If the estimated floor level is non-zero, the browser is expected to round it such a way as to avoid fingerprinting (likely to the nearest centimeter).

__`unbounded`__ (*NI*)

> An `XRReferenceSpace` tracking space which allows the user total freedom of movement, possibly over extremely long distances from their origin point. The viewer isn't tracked at all; tracking is optimized for stability around the user's current position, so the native origin may drift as needed to accommodate that need.

__`viewer`__ (*NI*)

> An `XRReferenceSpace` tracking space whose native origin tracks the viewer's position and orientation. This is used for environments in which the user can physically move around, and is supported by all instances of `XRSession`, both immersive and inline, though it's most useful for inline sessions. It's particularly useful when determining the distance between the viewer and an input, or when working with offset spaces. Otherwise, typically, one of the other reference space types will be used more often.

### Class `XRBoundedReferenceSpace` (*NI*)

> Represents a reference space which may move within a region of space whose borders are defined by an array of points laid out in clockwise order along the floor to define the passable region of the space. The origin of an `XRBoundedReferenceSpace` is always at floor level, with its X and Z coordinates typically defaulting to a location near the room's center.

### Class `XRJointSpace`

> Represents the space of an `XRHand` joint.

You can use an `XRJointSpace` object and an `XRReferenceSpace` to get an `XRJointPose` by calling `XRFrame.getJointPose()`.

```ts title="joint-space-example.ts"
navigator.xr
  .requestSession({ optionalFeatures: ['hand-tracking'] })
  .then(/** */);

function renderFrame(session: XRSession, frame: XRFrame) {
  // ...

  for (const inputSource of session.inputSources) {
    if (inputSource.hand) {
      const indexFingerTip: XRJointSpace = inputSource.hand.get('index-finger-tip');
      console.info(indexFingerTip.jointName); // 'index-finger-tip'

      const jointPose: XRJointPose = frame.getJointPose(indexFingerTip, referenceSpace);
      console.info(jointPose.transform); // XRRigidTransform
    }
  }
}
```

#### Instance properties

__[`jointName`](https://developer.mozilla.org/en-US/docs/Web/API/XRJointSpace/jointName)__

> The name of the joint that is tracked.

## Views

### Class `XRView`

To draw everything the user sees, each frame requires iterating over the list of views returned by the `XRViewerPose` object's `views` list:

```ts title="views-example.ts"
for (const view of pose.views) {
  const viewport: XRViewport = glBaseLayer.getViewport(view);
  gl.viewport(viewport.x, viewport.y, viewport.width, viewport.height);

  // Draw the scene for this view
}
```

Or you can do this in multiview mode:

```ts title="multiview-example.ts"
for (const view of pose.views) {
  // Update the view and projection matrices
}

// ...
// Draw the scene
```

#### Instance properties

__[`eye`](https://developer.mozilla.org/en-US/docs/Web/API/XRView/eye)__

> Which of the two eyes (left) or (right) for which this XRView represents the perspective. This value is used to ensure that any content which is pre-rendered for presenting to a specific eye is distributed or positioned correctly. The value can also be none if the XRView is presenting monoscopic data (such as a 2D image, a fullscreen view of text, or a close-up view of something that doesn't need to appear in 3D).

__[`isFirstPersonObserver`](https://developer.mozilla.org/en-US/docs/Web/API/XRView/isFirstPersonObserver)__ (*NI*)

> Returns a boolean indicating if the `XRView` is a first-person observer view.

__[`projectionMatrix`](https://developer.mozilla.org/en-US/docs/Web/API/XRView/projectionMatrix)__

> The projection matrix that will transform the scene to appear correctly given the point-of-view indicated by `eye`. This matrix should be used directly in order to avoid presentation distortions that may lead to potentially serious user discomfort.

__[`recommendedViewportScale`](https://developer.mozilla.org/en-US/docs/Web/API/XRView/recommendedViewportScale)__ (*NI*)

> The recommended viewport scale value that you can use for `requestViewportScale()` if the user agent has such a recommendation; `null` otherwise.

__[`transform`](https://developer.mozilla.org/en-US/docs/Web/API/XRView/transform)__

> An `XRRigidTransform` which describes the current position and orientation of the viewpoint in relation to the `XRReferenceSpace` specified when `getViewerPose()` was called on the `XRFrame` being rendered.

#### Instance methods

__[`requestViewportScale()`](https://developer.mozilla.org/en-US/docs/Web/API/XRView/requestViewportScale)__ (*NI*)

> Requests that the user agent should set the requested viewport scale for this viewport to the requested value.

### Class `XRViewport`

#### Instance properties

__[`height`](https://developer.mozilla.org/en-US/docs/Web/API/XRViewport/height)__

> The height, in pixels, of the viewport.

__[`width`](https://developer.mozilla.org/en-US/docs/Web/API/XRViewport/width)__

> The width, in pixels, of the viewport.

__[`x`](https://developer.mozilla.org/en-US/docs/Web/API/XRViewport/x)__

> The offset from the origin of the destination graphics surface (typically a `XRWebGLLayer`) to the left edge of the viewport, in pixels.

__[`y`](https://developer.mozilla.org/en-US/docs/Web/API/XRViewport/y)__

> The offset from the origin of the viewport to the bottom edge of the viewport; WebGL's coordinate system places (0, 0) at the bottom left corner of the surface.

## Geometric primitives

### Class `XRRigidTransform`

#### Constructor

__[`XRRigidTransform()`](https://developer.mozilla.org/en-US/docs/Web/API/XRRigidTransform/XRRigidTransform)__

> Creates a new XRRigidTransform object which represents a transform that applies a specified position and/or orientation.

#### Instance properties

__[`position`](https://developer.mozilla.org/en-US/docs/Web/API/XRRigidTransform/position)__

> A `DOMPointReadOnly` specifying a 3-dimensional point, expressed in meters, describing the translation component of the transform. The `w` property is always 1.0.

__[`orientation`](https://developer.mozilla.org/en-US/docs/Web/API/XRRigidTransform/orientation)__

> A `DOMPointReadOnly` which contains a unit quaternion describing the rotational component of the transform. As a unit quaternion, its length is always normalized to 1.0.

__[`matrix`](https://developer.mozilla.org/en-US/docs/Web/API/XRRigidTransform/matrix)__

> Returns the transform matrix in the form of a 16-member `Float32Array`.

__[`inverse`](https://developer.mozilla.org/en-US/docs/Web/API/XRRigidTransform/inverse)__

> Returns a `XRRigidTransform` which is the inverse of this transform. That is, if applied to an object that had been previously transformed by the original transform, it will undo the transform and return the original object.

## Pose

### Class `XRPose`

#### Instance properties

__[`angularVelocity`](https://developer.mozilla.org/en-US/docs/Web/API/XRPose/angularVelocity)__ (*NI*)

> A `DOMPointReadOnly` object which specifies the angular velocity of the object being tracked, expressed in radians per second.

__[`emulatedPosition`](https://developer.mozilla.org/en-US/docs/Web/API/XRPose/emulatedPosition)__

> A Boolean value which is `false` if the position and orientation given by `transform` is obtained directly from a full six degree of freedom (6DoF) XR device (that is, a device which tracks not only the pitch, yaw, and roll of the head but also the forward, backward, and side-to-side motion of the viewer). If any component of the transform is computed or created artificially (such as by using mouse or keyboard controls to move through space), this value is instead `true`, indicating that the `transform` is in part emulated in software.

__[`linearVelocity`](https://developer.mozilla.org/en-US/docs/Web/API/XRPose/linearVelocity)__ (*NI*)

> A `DOMPointReadOnly` object which specifies the linear velocity of the object being tracked, expressed in meters per second.

__[`transform`](https://developer.mozilla.org/en-US/docs/Web/API/XRPose/transform)__

> A `XRRigidTransform` which provides the position and orientation of the pose relative to the base `XRSpace`.

### Class `XRJointPose`

The `XRJointPose` interface is an `XRPose` with additional information about the size of the skeleton joint it represents.

#### Instance properties

__[`radius`](https://developer.mozilla.org/en-US/docs/Web/API/XRJointPose/radius)__

> The radius (distance from skin) for a joint.

### Class `XRViewerPose`

The [WebXR Device API][] interface `XRViewerPose` represents the pose (the position and orientation) of a viewer's point of view on the scene. Each `XRViewerPose` can have multiple views to represent, for example, the slight separation between the left and right eye.

#### Instance properties

__[`views`](https://developer.mozilla.org/en-US/docs/Web/API/XRViewerPose/views)__

> An array of `XRView` objects, one for each viewpoint on the scene which is needed to represent the scene to the user. A typical headset provides a viewer pose with two views whose `eye` property is either `left` or `right`, indicating which eye that view represents. Taken together, these views can reproduce the 3D effect when displayed on the XR device.

## Input

### Class `XRHand`

### Class `XRInputSource`

### Class `XRInputSourceArray`

## Layers

### Class `XRLayer`

This is an interface that represents a layer of content to be presented to the user, no properties and methods are defined.

## WebGL binding

### Class `XRWebGLLayer`

#### Constructor

__[`XRWebGLLayer()`](https://developer.mozilla.org/en-US/docs/Web/API/XRWebGLLayer/XRWebGLLayer)__

> Creates and returns a new `XRWebGLLayer` object for use by the specified `XRSession`, using a particular `WebGLRenderingContext` or `WebGL2RenderingContext` as the destination context.

#### Instance properties

__[`antialias`](https://developer.mozilla.org/en-US/docs/Web/API/XRWebGLLayer/antialias)__

> A `Boolean` value indicating whether or not the WebGL context's framebuffer supports anti-aliasing. The specific type of anti-aliasing is determined by the user agent.

__[`fixedFoveation`](https://developer.mozilla.org/en-US/docs/Web/API/XRWebGLLayer/fixedFoveation)__ (*NI*)

> A number indicating the amount of foveation used by the XR compositor. Fixed Foveated Rendering (FFR) renders the edges of the eye textures at a lower resolution than the center and reduces the GPU load.

__[`framebuffer`](https://developer.mozilla.org/en-US/docs/Web/API/XRWebGLLayer/framebuffer)__ (*PI*)

> Returns a `WebGLFramebuffer` suitable for passing into the `bindFrameBuffer()` method.

__[`framebufferWidth`](https://developer.mozilla.org/en-US/docs/Web/API/XRWebGLLayer/framebufferWidth)__

> Returns the width of the `XRWebGLLayer`'s framebuffer.

__[`framebufferHeight`](https://developer.mozilla.org/en-US/docs/Web/API/XRWebGLLayer/framebufferHeight)__

> Returns the height of the `XRWebGLLayer`'s framebuffer.

__[`ignoreDepthValues`](https://developer.mozilla.org/en-US/docs/Web/API/XRWebGLLayer/ignoreDepthValues)__ (*NI*)

> A `Boolean` value indicating whether or not the depth values in the framebuffer should be ignored.

__`multiviewRequired`__

This readonly property is introduced in JSAR to help WebXR developers to know if the framebuffer is multiview required.

At JSAR, the WebXR rendering pipeline disallows developers to create framebuffer objects for stereo rendering, because the framebuffer is always created by the host such as Unity, Unreal Engine, etc and the framebuffer is shared to the client process for rendering the WebXR content.

If the framebuffer is multiview required, namely calling `glFramebufferTextureMultiviewOVR()` in the host, the client process must use the followings in their vertex shader:

```glsl
#extension GL_OVR_multiview2 : enable
layout(num_views = 2) in;
```

Otherwise, any draw calls will occur an error by the `ovr_multiview*` extension.

To help WebXR developers to know if the framebuffer is multiview required, this property `multiviewRequired` is introduced in every created `XRWebGLLayer` object, and developers can check this property to know if the framebuffer is multiview required and change their shaders.

#### Static methods

__[`getNativeFramebufferScaleFactor()`](https://developer.mozilla.org/en-US/docs/Web/API/XRWebGLLayer/getNativeFramebufferScaleFactor)__

> Returns the scaling factor that can be used to scale the resolution of the recommended WebGL framebuffer resolution to the rendering device's native resolution.

#### Instance methods

__[`getViewport()`](https://developer.mozilla.org/en-US/docs/Web/API/XRWebGLLayer/getViewport)__

> Returns a new `XRViewport` instance representing the position, width, and height to which the WebGL context's viewport must be set to contain drawing to the area of the framebuffer designated for the specified view's contents. In this way, for example, the rendering of the left eye's point of view and of the right eye's point of view are each placed into the correct parts of the framebuffer.

[WebXR Device API]: https://developer.mozilla.org/en-US/docs/Web/API/WebXR_Device_API
