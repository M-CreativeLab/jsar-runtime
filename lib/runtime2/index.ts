import 'babylonjs';
import { JSARDOM } from '@yodaos-jsar/dom';
import { extname } from 'node:path';

import { getPerformanceNow, isWebXRSupported } from '@transmute/env';
import { dispatchXsmlEvent } from '@transmute/messaging';
import { NativeDocumentOnTransmute } from './jsardom/TransmuteImpl';

// viewers
import createModel3dViewer from './viewers/model3d';  // glb, gltf ...
import createImage2dViewer from './viewers/image2d';  // png, jpg, etc ...

const isDebugMode = process.env.JSAR_DEBUG_ENABLED === 'yes';

/**
 * Rewrite the `BABYLON.ThinEngine.QueueNewFrame` to use the requester's `requestAnimationFrame` function.
 * It skips the window object check.
 */
BABYLON.ThinEngine.QueueNewFrame = (func: () => void, requester?: any): number => {
  const { requestAnimationFrame } = requester || globalThis;
  if (typeof requestAnimationFrame !== 'function') {
    throw new TypeError('requestAnimationFrame is not a function, and the requester is: ' + requester);
  }

  let animationframeCallback;
  if (isDebugMode) {
    animationframeCallback = function animationframeCallback() {
      try {
        func.apply(requester, arguments);
      } catch (err) {
        console.error(`failed to execute the queued frame:`, err);
      }
    };
  } else {
    animationframeCallback = func.bind(requester);
  }
  return requestAnimationFrame(animationframeCallback);
};

BABYLON.Logger.OnNewCacheEntry = (entry) => {
  entry.split('\n').forEach((line) => {
    console.info('[Babylonjs]', line);
  });
};
BABYLON.Tools.GetAbsoluteUrl = (url: string) => {
  console.info('[Babylonjs] GetAbsoluteUrl:', url);
  return url;
};
Object.defineProperty(BABYLON.PrecisionDate, 'Now', {
  get: () => getPerformanceNow(),
});

export class TransmuteRuntime2 extends EventTarget {
  constructor(private gl: WebGLRenderingContext | WebGL2RenderingContext) {
    super();

    this.dispatchEvent(new Event('rendererReady'));
    this.prepare();
  }

  start(url: string, id: number) {
    this.onXsmlRequest(url, id);
  }

  onGpuBusy() {
    // TODO
  }

  private prepare() {
    const exts = this.gl.getSupportedExtensions();
    console.info(`[WebGL] supported extensions(${exts.length}):`);
    for (let extName of exts) {
      console.info(`  - ${extName}`);
    }
    console.info(`[JSARDOM] version=${JSARDOM.version}`);
  }

  private async onXsmlRequest(url: string, id: number) {
    console.info(`xsml request:`, url, id);

    const nativeDocument = new NativeDocumentOnTransmute(this.gl);
    if (isWebXRSupported()) {
      await nativeDocument.enterDefaultXrExperience();
      console.info(`Session#${id} has been entered XR experience.`);
    } else {
      console.info(`Skip enabling WebXR experience, reason: WebXR is not enabled.`);
    }

    try {
      await this.load(url, nativeDocument);
    } catch (err) {
      dispatchXsmlEvent(nativeDocument.id, 'error');
      console.error(`failed to load document(${url}):`, err);
    }
  }

  private async load(
    codeOrUrl: string,
    nativeDocument: NativeDocumentOnTransmute,
    urlBase?: string
  ) {
    if (!this.gl) {
      throw new TypeError('The webgl is not ready or lost context state');
    }

    // Override the `codeOrUrl` with the example url if the debug mode is enabled.
    if (process.env.JSAR_DEBUG_ENABLED === 'yes' && process.env.JSAR_EXAMPLE_URL) {
      codeOrUrl = process.env.JSAR_EXAMPLE_URL;
    }

    try {
      const urlObj = new URL(codeOrUrl);
      /**
       * Supports the formats to open directly:
       * 
       * - [x] `xsml` for mixed reality content.
       * - [ ] `html` for web page preview.
       * - [x] `glb`, `gltf`, `usdz`, etc, for 3D model preview.
       * - [x] `png`, `jpg`, etc, for image preview.
       * - [ ] `mp3`, `mp4`, `webm`, etc, for media preview.
       * - [ ] `pdf`, `epub`, etc, for document preview.
       *
       * BTW, the users could open the above formats in a html or xsml document in a new volume.
       */
      const urlExt = extname(urlObj.pathname);
      /**
       * TODO: implement this via mime type instead of the file extension?
       */
      switch (urlExt) {
        case '.glb':
        case '.gltf':
          codeOrUrl = createModel3dViewer(codeOrUrl, { playAnimation: true });
          urlBase = urlObj.href;
          console.info(`switched to the 3d model viewer.`);
          break;
        case '.png':
        case '.jpg':
        case '.jpeg':
          codeOrUrl = createImage2dViewer(codeOrUrl);
          urlBase = urlObj.href;
          console.info(`switched to the 2d image viewer.`);
          break;
        case '.html':
        case '.mp3':
        case '.mp4':
        case '.webm':
        case '.pdf':
        case '.epub':
          throw new Error(`the format is not supported yet: ${urlExt}`);
        default:
          break;
      }
    } catch (_err) {
      urlBase = 'https://example.com/'
    }

    console.info(`loading a JSAR document`, codeOrUrl);
    const dom = new JSARDOM(codeOrUrl, {
      url: urlBase,
      nativeDocument,
    });

    try {
      await dom.load();
      dispatchXsmlEvent(nativeDocument.id, 'loaded');

      const spaceNode = dom.document.space.asNativeType<BABYLON.TransformNode>();
      {
        await dom.waitForSpaceReady();
        dispatchXsmlEvent(nativeDocument.id, 'DOMContentLoaded');
      }
      spaceNode.setEnabled(true);
    } catch (err) {
      console.error(`occurs an error when loading document:`, err);
      dispatchXsmlEvent(nativeDocument.id, 'error');
      // TODO: report to the native side.
      // remove the dom from appStack
      await dom.unload();
      dom.nativeDocument.close();
    }
  }
}
