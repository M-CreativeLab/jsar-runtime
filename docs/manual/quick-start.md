# Quick Start

If you are a Web developer, the way to try out a browser is to use this browser to visit Web pages. If you are a system developer, you also use this browser to visit Web pages to experience and test it.

To get started with JSAR, the spatial Web browser engine, you'll need either a desktop browser or a supported XR device with our pre-built browser application.

## JSAR on Desktop

<div style="text-align: left; width: 80%;">
  <img src="https://raw.githubusercontent.com/M-CreativeLab/jsar-runtime/refs/heads/main/docs/macos-screenshot-00.png" alt="Transmute Browser" style="width: 100%;" />
</div>

[Transmute Browser][] is our desktop browser built with JSAR technology, designed to deliver [spatial web browsing](https://maggieappleton.com/spatial-web/) experiences on desktop platforms.

| Platform | Status |
| -------- | ------ |
| macOS    | Ok |
| Windows  | Not Supported |
| Linux    | Not Supported |

With [Transmute Browser][], you can experience spatial web browsing on your desktop, where web content is presented in a 3D environment that you can naturally interact with. Additionally, developers can use it to test and debug their JSAR-powered web applications without requiring physical XR devices.

If you'd like to try this browser, you can download it from our [download page][Transmute Browser].

After downloading the archive file, extract the archive and you'll find a `transmute_browser` executable file. You can run it from the command line:

```bash
./transmute_browser /path/to/your/webpage.html
./transmute_browser https://path/to/your/page
```

Currently JSAR is not fully aligned with Web standards, so directly accessing some Web pages may cause issues (though this situation will continuously improve). We recommend experiencing JSAR through the HTML pages in this directory first: https://github.com/M-CreativeLab/jsar-runtime/tree/main/fixtures/html, such as:

```bash
./transmute_browser https://raw.githubusercontent.com/M-CreativeLab/jsar-runtime/refs/heads/main/fixtures/html/three-loader-pdb.html
```

Transmute Browser supports rendering Web content in stereo 3D mode via `--stereo` flag. You can pass this flag to enable stereo 3D rendering:

```bash
./transmute_browser --stereo /path/to/your/webpage.html
```

For more help, you can run `./transmute_browser --help` to see the available options.

```bash
./transmute_browser --help
Usage: transmute_browser [options] [url]
Options:
  -w <width>              Window width (default: 1600)
  -h <height>             Window height (default: 900)
  -n <count>              Number of apps (default: 1)
  --samples             MSAA samples (default: 4)
  --mono                  Monoscopic XR rendering (default)
  --stereo [mode]         Stereo XR rendering mode (default: singlepass):
                            multipass - Multiple rendering passes
                            singlepass - Single rendering pass
  --env-map <path>        Specify environment map directory path
  --no-env-map            Disable environment map rendering
  --help                  Show this help

Examples:
  transmute_browser --mono
  transmute_browser --stereo                 # Uses singlepass by default
  transmute_browser --stereo multipass
  transmute_browser --stereo singlepass
  transmute_browser --env-map /path/to/cubemap  # Use custom environment map
  transmute_browser --no-env-map             # Disable environment map
```

[Transmute Browser]: https://m-creativelab.github.io/jsar-runtime/download.html

## JSAR on XR Devices

| Device | Status |
| ------ | ------ |
| [Rokid AR Studio][] | Ok |
| [Rokid AR Lite][] | Ok |
| Meta Quest 4  | Not started |
| Meta Quest 3s | Not started |
| Pico Neo 3    | Not started |

For XR devices, we provide an immersive application called **Play WebXR** that integrates JSAR browser engine to spatially present Web contents, images, and [glTF][] 3d models. You can install it from your device's application store.

After installation, you can type URL to open a web content just like in a traditional web browser.

### For Meta Quest and Pico Users

While we haven't yet released official JSAR browser applications for Meta Quest and Pico devices, we're excited about the potential to bring spatial web browsing to these platforms. Our focus is currently on perfecting core functionality and web standards support, but we recognize the importance of these platforms in the XR ecosystem.

If you're a developer interested in Meta Quest or Pico development, we'd love to have your contribution in bringing JSAR to these devices. The JSAR engine is designed to be platform-agnostic and should be compatible with these devices' capabilities.

From a technical perspective, JSAR is built on [OpenGL ES 3.0][GLES3] rendering and provides a [Unity][] SDK, making it compatible with any [AOSP][] devices that support Unity and OpenGL ES 3.0. This means JSAR can potentially run on a wide range of AOSP-based XR devices that meet these technical requirements.

### For Rokid Users

If you are a Rokid developer and own a Rokid device, you can find the **Play WebXR** app in the device's app store. Once installed, simply launch the app and enter a URL to start browsing web content in spatial AR.

Additionally, on Rokid AR Lite devices, JSAR is built into the system, allowing you to open web content in spatial mode through the system browser (which uses JSAR for rendering under the hood).

[Chromium]: https://www.chromium.org/Home
[Rokid AR Studio]: https://arstudio.rokid.com/
[Rokid AR Lite]: https://arlite.rokid.com/
[glTF]: https://www.khronos.org/gltf/
[GLES3]: https://www.khronos.org/opengles/
[Unity]: https://unity.com/
[AOSP]: https://source.android.com/
[Meta Quest]: https://www.meta.com/quest/
[Pico]: https://www.picoxr.com/
