import 'babylonjs';
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
    logger.info(`[WebGL] supported extensions(${exts.length}):`, exts);
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
    logger.info(`loading a JSAR document`, codeOrUrl);
    if (!this.gl) {
      throw new TypeError('The webgl is not ready or lost context state');
    }

    try {
      new URL(codeOrUrl);
    } catch (_err) {
      urlBase = 'https://example.com/'
    }
    const dom = new JSARDOM(codeOrUrl, {
      url: urlBase,
      nativeDocument,
    });
    this.appStack.push(dom);

    await dom.load();
    logger.info('loaded a jsar document');

    const spaceNode = dom.document.space.asNativeType<BABYLON.TransformNode>();
    spaceNode.setEnabled(false);
    {
      await dom.waitForSpaceReady();
      logger.info('the jsar document\'s space is ready');

      const scene = dom.nativeDocument.getNativeScene();
      const animations = scene.animationGroups
        .filter(ag => ag.name.startsWith('model.'));
      if (animations.length > 0) {
        animations[0].start(true);
        logger.info('started the first animation:', animations[0].name);
        const firstTargetedAnim = animations[0].targetedAnimations[0];
        logger.info(firstTargetedAnim.animation.getKeys());
        logger.info(firstTargetedAnim.target);
        logger.info('first targeted animation:', firstTargetedAnim.serialize());
      }
    }
    this.fitSpaceWithScene(spaceNode, 0.7);
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
