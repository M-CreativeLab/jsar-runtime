import 'babylonjs';
import { JSARDOM } from '@yodaos-jsar/dom';
import { extname } from 'node:path';

import * as logger from '@transmute/logger';
import { isWebXRSupported } from '@transmute/env';
import { dispatchXsmlEvent } from '@transmute/messaging';
import { NativeDocumentOnTransmute } from './jsardom/impl-transmute';

// viewers
import createModel3dViewer from './viewers/model3d';  // glb, gltf ...
import createImage2dViewer from './viewers/image2d';  // png, jpg, etc ...

/**
 * Rewrite the `BABYLON.ThinEngine.QueueNewFrame` to use the requester's `requestAnimationFrame` function.
 * It skips the window object check.
 */
BABYLON.ThinEngine.QueueNewFrame = (func: () => void, requester?: any): number => {
  const { requestAnimationFrame } = requester || globalThis;
  if (typeof requestAnimationFrame !== 'function') {
    throw new TypeError('requestAnimationFrame is not a function, and the requester is: ' + requester);
  }
  return requestAnimationFrame(function () {
    try {
      func.apply(requester, arguments);
    } catch (err) {
      logger.error(`failed to execute the queued frame:`, err);
    }
  });
};
BABYLON.Logger.OnNewCacheEntry = (entry) => {
  entry.split('\n').forEach((line) => {
    logger.info('[Babylonjs]', line);
  });
};
BABYLON.Tools.GetAbsoluteUrl = (url: string) => {
  logger.info('[Babylonjs] GetAbsoluteUrl:', url);
  return url;
};
Object.defineProperty(BABYLON.PrecisionDate, 'Now', {
  get: () => performance.now()
});

export class TransmuteRuntime2 extends EventTarget {
  private scene: BABYLON.Scene;
  private appStack: Array<{
    id: number;
    dom: JSARDOM<NativeDocumentOnTransmute>;
  }> = [];

  constructor(private gl: WebGLRenderingContext | WebGL2RenderingContext) {
    super();

    this.dispatchEvent(new Event('rendererReady'));
    this.prepare();
  }

  start(url: string, id: number) {
    this.onXsmlRequest(url, id);
  }

  onGpuBusy() {
    const { dom } = this.appStack.pop();
    if (dom != null) {
      dom.unload();
    }
  }

  private prepare() {
    const exts = this.gl.getSupportedExtensions();
    logger.info(`[WebGL] supported extensions(${exts.length}):`);
    for (let extName of exts) {
      logger.info(`  - ${extName}`);
    }
    logger.info(`[JSARDOM] version=${JSARDOM.version}`);
  }

  private async onXsmlRequest(url: string, id: number) {
    /**
     * Unload all the documents before loading a new one.
     */
    if (this.appStack.length > 0) {
      await this.unloadAll();
    }
    logger.info(`xsml request:`, url, id);

    const nativeDocument = new NativeDocumentOnTransmute(this.gl, id);
    if (isWebXRSupported()) {
      await nativeDocument.enterDefaultXrExperience();
      logger.info(`Session#${id} has been entered XR experience.`);
    } else {
      logger.info(`Skip enabling WebXR experience, reason: WebXR is not enabled.`);
    }

    try {
      await this.load(url, nativeDocument);
    } catch (err) {
      dispatchXsmlEvent(nativeDocument.id, 'error');
      logger.error(`failed to load document(${url}):`, err);
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
          logger.info(`switched to the 3d model viewer.`);
          break;
        case '.png':
        case '.jpg':
        case '.jpeg':
          codeOrUrl = createImage2dViewer(codeOrUrl);
          urlBase = urlObj.href;
          logger.info(`switched to the 2d image viewer.`);
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

    logger.info(`loading a JSAR document`, codeOrUrl);
    const dom = new JSARDOM(codeOrUrl, {
      url: urlBase,
      nativeDocument,
    });
    this.appStack.push({ id: nativeDocument.id, dom });

    try {
      await dom.load();
      dispatchXsmlEvent(nativeDocument.id, 'loaded');

      const spaceNode = dom.document.space.asNativeType<BABYLON.TransformNode>();
      spaceNode.setEnabled(false);
      {
        await dom.waitForSpaceReady();
        dispatchXsmlEvent(nativeDocument.id, 'fcp');
      }
      this.fitSpaceWithScene(spaceNode);
    } catch (err) {
      logger.error(`occurs an error when loading document:`, err);
      // TODO: report to the native side.
      // remove the dom from appStack
      for (let i = 0; i < this.appStack.length; ++i) {
        if (this.appStack[i].dom.id === dom.id) {
          this.appStack.splice(i, 1);
          break;
        }
      }
      await dom.unload();
      dom.nativeDocument.close();
    }
  }

  private async unloadTop() {
    const { dom } = this.appStack.pop();
    if (dom != null) {
      await dom.unload();
      dom.nativeDocument.close();
    }
  }

  private async unloadAll() {
    await Promise.all(this.appStack.map(({ dom }) => {
      return dom.unload().then(() => dom.nativeDocument.close());
    }));
    logger.info('all the documents have been unloaded.');
  }

  private fitSpaceWithScene(spaceNode: BABYLON.TransformNode, ratio = 1.0) {
    /**
     * Scale the space to fit the scene.
     */
    const boundingVectors = spaceNode.getHierarchyBoundingVectors(true);
    const sceneSize = boundingVectors.max.subtract(boundingVectors.min);
    const scalingRatio = Math.min(ratio / sceneSize.x, ratio / sceneSize.y, ratio / sceneSize.z);
    spaceNode.scaling = new BABYLON.Vector3(scalingRatio, scalingRatio, scalingRatio);
    spaceNode.setEnabled(true);
    logger.info('space has been scaled to fit the scene, and the scaling ratio is:', scalingRatio);
  }
}
