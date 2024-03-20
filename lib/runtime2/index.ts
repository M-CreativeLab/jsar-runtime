import 'babylonjs';
import { GLTFFileLoader } from '@babylonjs/loaders/glTF';
import { JSARDOM } from '@yodaos-jsar/dom';

import * as logger from '../bindings/logger';
import { requestRendererReady } from '../bindings/renderer';
import { markRuntimeAvailable } from '../bindings/env';
import { addXsmlRequestListener, type XsmlRequestEvent } from '../bindings/messaging';
import { NativeDocumentOnTransmute } from './jsardom/impl-transmute';

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
  logger.info('[Babylonjs]', entry);
};
BABYLON.SceneLoader.RegisterPlugin(new GLTFFileLoader() as any);

export class TransmuteRuntime2 extends EventTarget {
  private gl: WebGLRenderingContext;
  private scene: BABYLON.Scene;

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

  private onRendererReady(gl: WebGLRenderingContext) {
    this.gl = gl;
    this.dispatchEvent(new Event('rendererReady'));
    this.prepare(gl);
  }

  private prepare(gl: WebGLRenderingContext) {
    const exts = gl.getSupportedExtensions();
    logger.info(`[WebGL] supported extensions(${exts.length}):`, exts);
  }

  private async onXsmlRequest(event: XsmlRequestEvent) {
    logger.info(`xsml request:`, event.url, event.sessionId);

    const nativeDocument = new NativeDocumentOnTransmute(this.gl, event.sessionId);
    await nativeDocument.enterXrExperience();
    logger.info(`Session#${event.sessionId} has been entered XR experience.`);

    const defaultCode = `
<xsml>
<head>
  <style>
    @keyframes rotate {
      from {
        rotation: 0 0 30;
      }
      to {
        rotation: 0 360 30;
      }
    }
    cube {
      animation: rotate 50s linear infinite;
      position: 0.25 0 -1;
    }
  </style>
</head>
<space>
  <cube size="1" />
</space>
</xsml>
    `;

    try {
      await this.load(defaultCode, nativeDocument);
    } catch (err) {
      logger.error('failed to load the default document:', err);
    }
  }

  private async load(
    code: string,
    nativeDocument: NativeDocumentOnTransmute,
    urlBase: string = 'https://example.com/'
  ) {
    logger.info(`loading a JSAR document`, code);
    if (!this.gl) {
      throw new TypeError('The webgl is not ready or lost context state');
    }
    const dom = new JSARDOM(code, {
      url: urlBase,
      nativeDocument,
    });
    await dom.load();
    logger.info('loaded a jsar document');

    const spaceNode = dom.document.space.asNativeType<BABYLON.TransformNode>();
    spaceNode.setEnabled(false);
    {
      await dom.waitForSpaceReady();
      logger.info('the jsar document\'s space is ready');
    }
    this.fitSpaceWithScene(spaceNode, 2.5);
  }

  private fitSpaceWithScene(spaceNode: BABYLON.TransformNode, ratio = 1) {
    /**
     * Scale the space to fit the scene.
     */
    const boundingVectors = spaceNode.getHierarchyBoundingVectors(true);
    const sceneSize = boundingVectors.max.subtract(boundingVectors.min);
    const scalingRatio = Math.min(ratio / sceneSize.x, ratio / sceneSize.y, ratio / sceneSize.z);
    spaceNode.scaling = new BABYLON.Vector3(scalingRatio, scalingRatio, scalingRatio);
    spaceNode.setEnabled(true);
  }
}
