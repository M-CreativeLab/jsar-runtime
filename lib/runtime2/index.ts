import 'babylonjs';
import { JSARDOM } from '@yodaos-jsar/dom';
import { extname } from 'path';

import * as logger from '../bindings/logger';
import { requestRendererReady } from '../bindings/renderer';
import { markRuntimeAvailable } from '../bindings/env';
import { addXsmlRequestListener, type XsmlRequestEvent } from '../bindings/messaging';
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
  private gl: WebGLRenderingContext;
  private scene: BABYLON.Scene;
  private appStack: Array<JSARDOM<NativeDocumentOnTransmute>> = [];

  start() {
    requestRendererReady(this.onRendererReady.bind(this));
    markRuntimeAvailable([
      `version=${process.env['JSAR_VERSION']}`,
      `babylonjs=${BABYLON.Engine.Version}`,
      `nodejs=${process.versions.node}`,
      `v8=${process.versions.v8}`,
    ].join(','));
    addXsmlRequestListener(this.onXsmlRequest.bind(this));
  }

  onGpuBusy() {
    const dom = this.appStack.pop();
    if (dom != null) {
      dom.unload();
    }
  }

  private onRendererReady(gl: WebGLRenderingContext) {
    this.gl = gl;
    this.dispatchEvent(new Event('rendererReady'));
    this.prepare(gl);
  }

  private prepare(gl: WebGLRenderingContext) {
    const exts = gl.getSupportedExtensions();
    logger.info(`[WebGL] supported extensions(${exts.length}):`);
    for (let extName of exts) {
      logger.info(`  - ${extName}`);
    }
    logger.info(`[JSARDOM] version=${JSARDOM.version}`);
  }

  private async onXsmlRequest(event: XsmlRequestEvent) {
    logger.info(`xsml request:`, event.url, event.sessionId);

    const nativeDocument = new NativeDocumentOnTransmute(this.gl, event.sessionId);
    await nativeDocument.enterXrExperience();
    logger.info(`Session#${event.sessionId} has been entered XR experience.`);

    // const modelUrl = 'https://ar.rokidcdn.com/web-assets/pages/models/diamante.glb'; // demo
    // const modelUrl = 'https://ar.rokidcdn.com/web-assets/pages/models/floating_fox.glb';
    // const modelUrl = 'https://ar.rokidcdn.com/web-assets/pages/models/pirateFort.glb';
    // const modelUrl = 'https://ar.rokidcdn.com/web-assets/pages/models/blackhole.glb';
    // const modelUrl = 'https://ar.rokidcdn.com/web-assets/pages/models/bird.glb';
    try {
      await this.load(event.url, nativeDocument);
    } catch (err) {
      logger.error('failed to load the default document:', err);
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
    this.appStack.push(dom);

    try {
      await dom.load();
      logger.info('loaded a jsar document');

      const spaceNode = dom.document.space.asNativeType<BABYLON.TransformNode>();
      spaceNode.setEnabled(false);
      {
        await dom.waitForSpaceReady();
        logger.info('the jsar document\'s space is ready');
      }
      this.fitSpaceWithScene(spaceNode);
    } catch (err) {
      // remove the dom from appStack
      for (let i = 0; i < this.appStack.length; ++i) {
        if (this.appStack[i].id === dom.id) {
          this.appStack.splice(i, 1);
          break;
        }
      }
      await dom.unload();
      logger.error(`failed to load the jsar document: ${codeOrUrl} width the error:`, err);
      // TODO: report to the native side.
    }
  }

  private fitSpaceWithScene(spaceNode: BABYLON.TransformNode, ratio = 0.5) {
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
