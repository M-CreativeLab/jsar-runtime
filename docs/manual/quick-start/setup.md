# Setup

To get started with JSAR, you need to get the followings ready:

- Install the _Play WebXR_ app on your supported device: [Rokid AR Studio][] or [Rokid AR Lite][].
- Create a common Web project, in simplest, just create a `index.html` file with some HTML content.

[Rokid AR Studio]: https://arstudio.rokid.com/
[Rokid AR Lite]: https://arlite.rokid.com/

## Install _Play WebXR_

_Play WebXR_ is an [Unity][] application that can run on [Rokid AR Studio][] and [Rokid AR Lite][]. You can install the latest application via the app store.

When you have an installed _Play WebXR_ in your device and open it, then you will be guided to a standalone space created by _Play WebXR_, and like every Web browser does, there is an input box for you to input the URL you want to run.

## URL types supported

Just like a Web browser can open an image, a video, etc., JSAR can open kinds of URLs, including.

### Images

When a URL ends with `.jpg`, `.jpeg`, `.png`, it will be treated as an image and displayed on a virtual screen in the space.

### 3D Models

Currently only `.glb` and `.gltf` files are supported as 3D models, when user type this kind of URL, a 3D model will be rendered in the space with a scaled size.

### HTML

When a URL ends with `.html`, it will be treated as a [HTML][] file and run in the space.

## Next

In next section, we will start from [writing a sample HTML file](./write-html.mdx).

[Unity]: https://unity.com/
[HTML]: https://en.wikipedia.org/wiki/HTML
