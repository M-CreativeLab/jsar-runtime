# Defferred Composition

Defferred composition is a new technique introduced by JSAR that allows you to defer the composition of some of virtual objects in low latency scenarios. This is useful when you want to do composition of virtual objects from different processes or threads, and you also require low latency in this composition. JSAR uses defferred composition to compose virtual objects from WebXR processes into the host scene created by Unity, Unreal Engine or any other 3D engine.

## Principles

Defferred composition must follow the principles below:

- **Low latency**: The composition must be done in a very short time to avoid delays in the rendering of the host scene.
- **Asynchronous**: The composition must be done asynchronously to avoid blocking the main thread of the host scene.

## How it works

To achieve low latency and asynchronous composition, a defferred composition system must accept the client and host are in different time frames, namely the client must be defferred in time. That's why it's called __Defferred Composition__.

When we accept that the rendering in client-side is defferred, the host can render the scene without waiting for the client to send the virtual objects, instead, the host can render the whole scene and then compose the latest virtual objects from the client. This way, the host can render the scene with low latency and the client can send the virtual objects asynchronously.

Now there is only one issue left, how should we make defferred virtual objects appear in the right place when compositing? To solve this issue, we introduced [computation graph][] and a new variable type `Placeholder` in WebXR and renderer subsystem, that means the client will be able to receive a placeholder object from WebXR's API such as `frame.getViewerPose()` or `frame.getPose()`, and then the client could use this placeholder object to set the view-relative matrices of the virtual objects, once the host renderer receives a placeholder, it will use built-in computation graph to compute the right matrices for rendering, and then the virtual objects will appear in the right place.

[computation graph]: https://www.cs.cornell.edu/courses/cs5740/2017sp/lectures/04-nn-compgraph.pdf

## Advantages

In defferred composition, the host can render the scene with low latency and the client can send the virtual objects asynchronously. It's verified that defferred composition can achieve low latency and asynchronous composition in rendering WebXR applications within an Unity scene.

## Limitations

Defferred composition has limitations as well even though it's working in JSAR.

The main limitation is that we have to introduce a new variable type `Placeholder` in WebXR Device API, and to implement the complete computation graph in current ECMAScript engines, which is not a trivial task, it requires a new Proxy way to handle the operations between placeholders and other variables.

Another limitation is that the WebXR developer must accept the fact that the rendering in client-side is defferred, that means all the poses and matrices are not precise in the client-side.
