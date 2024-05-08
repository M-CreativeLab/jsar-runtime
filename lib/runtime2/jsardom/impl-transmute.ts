import {
  type NativeEngine as JSARNativeEngine,
  type NativeDocument as JSARNativeDocument,
  type ResourceLoader as JSARResourceLoader,
  type RequestManager as JSARRequestManager,
  type MediaPlayerBackend,
  type UserAgent as JSARUserAgent,
  type UserAgentInit as JSARUserAgentInit,
  type DOMParser,
  type MediaPlayerConstructor,
  type XRSessionBackend,
  type XRSessionBackendInit,
  type SpatialDocumentImpl,
  cdp as jsarCdp,
} from '@yodaos-jsar/dom';
import ImageDataImpl from '@yodaos-jsar/dom/src/living/image/ImageData';
import * as ws from 'ws';
import * as undici from 'undici';

import * as logger from '../../bindings/logger';
import { createBondXRSystem } from '../../webxr';
import { WebXRDefaultExperience } from './xr/DefaultExperience';
import {
  XRMatrixPlaceholder,
  XRMatrixPlaceholderType
} from '../../webxr/api/XRRigidTransform';
import {
  OffscreenCanvasImpl,
  createImageBitmapImpl,
  kDisposeCanvas
} from '../../polyfills/offscreencanvas';

type TransmuteEngineOptions = BABYLON.EngineOptions & {
  xrSessionId: number;
};
class EngineOnTransmute extends BABYLON.Engine implements JSARNativeEngine {
  #xrSessionId: number;
  constructor(
    glContext: WebGLRenderingContext | WebGL2RenderingContext,
    antialias: boolean,
    options?: TransmuteEngineOptions,
    adaptToDeviceRatio?: boolean
  ) {
    super(glContext, antialias, options, adaptToDeviceRatio);
    this.disableUniformBuffers = true;
    this.#xrSessionId = options?.xrSessionId;
  }

  get useRightHandedSystem(): boolean {
    if (this.scenes.length <= 0) {
      throw new Error('No scene available');
    }
    return this.scenes[0].useRightHandedSystem;
  }

  setMatrices(uniform: WebGLUniformLocation, matrices: Float32Array): boolean {
    /**
     * When JSAR_WEBGL_PLACEHOLDER is enabled, we will replace the projection, view and viewProjection matrices with
     * placeholders. The placeholders will be computed at the native rendering loop.
     */
    if (process.env['JSAR_WEBGL_PLACEHOLDER'] === 'yes') {
      const name = (uniform as any)?.name;
      // const currentEffect = this._currentEffect;
      // const isUnbound = BABYLON.Tags.MatchesQuery(currentEffect, 'UNBOUND');
      switch (name) {
        case 'projection':
          matrices = new XRMatrixPlaceholder(
            matrices,
            XRMatrixPlaceholderType.PROJECTION_MATRIX,
            this.#xrSessionId,
            this.useRightHandedSystem);
          break;
        case 'view':
          matrices = new XRMatrixPlaceholder(
            matrices,
            XRMatrixPlaceholderType.VIEW_MATRIX,
            this.#xrSessionId,
            this.useRightHandedSystem);
          break;
        case 'viewProjection':
          matrices = new XRMatrixPlaceholder(
            matrices,
            XRMatrixPlaceholderType.VIEW_PROJECTION_MATRIX,
            this.#xrSessionId,
            this.useRightHandedSystem);
          break;
        default:
          break;
      }
    }
    return super.setMatrices(uniform, matrices);
  }
}

type FetchReturnAs = 'string' | 'json' | 'arraybuffer';
type FetchReturnTypes = string | object | ArrayBuffer;
type FetchOptions = {
  accept?: string;
  cookieJar?: any;
  referrer?: string;
};

class ResourceLoaderOnTransmute implements JSARResourceLoader {
  fetch(url: string, options: FetchOptions, returnsAs: 'string'): Promise<string>;
  fetch(url: string, options: FetchOptions, returnsAs: 'json'): Promise<object>;
  fetch(url: string, options: FetchOptions, returnsAs: 'arraybuffer'): Promise<ArrayBuffer>;
  fetch<T = FetchReturnTypes>(url: string, options: FetchOptions, returnsAs?: FetchReturnAs): Promise<T>;
  async fetch(
    url: string,
    options: FetchOptions,
    returnsAs?: FetchReturnAs
  ): Promise<FetchReturnTypes> {
    const urlObj = new URL(url);
    if (urlObj.protocol === 'file:') {
      throw new TypeError('file: protocol is not supported');
    }
    const reqInit: undici.RequestInit = {
      ...(options == null ? {} : options),
    };
    const resp = await undici.request(url, <any>reqInit);
    if (resp.statusCode >= 400) {
      throw new Error(`Failed to fetch(${url}), statusCode=${resp.statusCode}`);
    }
    if (returnsAs === 'string') {
      return resp.body.text();
    } else if (returnsAs === 'json') {
      return <object>resp.body.json();
    } else if (returnsAs === 'arraybuffer') {
      return resp.body.arrayBuffer();
    } else {
      throw new TypeError('Invalid return type, must be string, json or arraybuffer');
    }
  }
}

class MediaPlayerBackendOnTransmute implements MediaPlayerBackend {
  paused: boolean;
  currentTime: number;
  duration: number;
  volume: number;
  loop: boolean;
  onended: () => void;

  constructor() {
    // TODO
  }
  load(buffer: ArrayBuffer | ArrayBufferView, onloaded: () => void): void {
    throw new TypeError('Method not implemented.');
  }
  play(when?: number): void {
    throw new TypeError('Method not implemented.');
  }
  pause(): void {
    throw new TypeError('Method not implemented.');
  }
  canPlayType(type: string): CanPlayTypeResult {
    throw new TypeError('Method not implemented.');
  }
  dispose(): void {
    throw new TypeError('Method not implemented.');
  }
}

class UserAgentBackendOnTransmute implements JSARUserAgent {
  versionString: string = '1.0';
  vendor: string = process.env.JSAR_DEVICE_VENDOR || '';
  vendorSub: string = '';
  language: string = 'zh-CN';
  languages: readonly string[] = [
    'zh-CN',
    'en-US',
  ];
  defaultStylesheet: string;
  devicePixelRatio: number;
  deviceMemory?: number;
  resourceLoader: JSARResourceLoader;
  requestManager: JSARRequestManager;
  domParser: DOMParser;

  constructor(init: JSARUserAgentInit) {
    this.defaultStylesheet = init.defaultStylesheet;
    this.devicePixelRatio = init.devicePixelRatio;
    this.resourceLoader = new ResourceLoaderOnTransmute();
  }

  alert(_message?: string) {
    throw new TypeError('Method not implemented.');
  }
  confirm(_message?: string): boolean {
    throw new TypeError('Method not implemented.');
  }
  prompt(_message?: string, _defaultValue?: string): string {
    throw new TypeError('Method not implemented.');
  }
  vibrate(_pattern: VibratePattern): boolean {
    throw new TypeError('Method not implemented.');
  }
  getWebSocketConstructor() {
    return ws.WebSocket as unknown as typeof WebSocket;
  }
  getMediaPlayerConstructor(): MediaPlayerConstructor {
    return MediaPlayerBackendOnTransmute;
  }

  /**
   * WebXR related
   * @param init 
   */
  createXRSessionBackend?(_init?: XRSessionBackendInit): XRSessionBackend {
    throw new TypeError('Method not implemented.');
  }
  isXRSessionSupported?(_mode: XRSessionMode): Promise<boolean> {
    throw new TypeError('Method not implemented.');
  }
}

export class NativeDocumentOnTransmute extends EventTarget implements JSARNativeDocument {
  engine: JSARNativeEngine;
  userAgent: JSARUserAgent;
  baseURI: string;
  console: Console = new logger.Logger();
  attachedDocument: SpatialDocumentImpl<NativeDocumentOnTransmute>;
  closed: boolean = false;
  cdpTransport?: jsarCdp.ITransport;

  private _id: number;
  private _xrSystem: XRSystem;
  private _xrDefaultExperience: Promise<WebXRDefaultExperience>;
  private _scene: BABYLON.Scene;
  private _preloadMeshes: Map<string, Array<BABYLON.AbstractMesh | BABYLON.TransformNode>> = new Map();
  private _preloadAnimationGroups: Map<string, BABYLON.AnimationGroup[]> = new Map();
  private _defaultCamera: BABYLON.Camera;
  private _defaultLights: BABYLON.Light[] = [];

  constructor(glContext: WebGLRenderingContext | WebGL2RenderingContext, xrSessionId: number) {
    super();

    const now = performance.now();
    this._id = xrSessionId;
    this._xrSystem = createBondXRSystem(xrSessionId);
    this.engine = new EngineOnTransmute(glContext, true, {
      disableWebGL2Support: false,
      xrCompatible: true,
      xrSessionId,
    });
    logger.info('Engine created in', performance.now() - now, 'ms');

    this.userAgent = new UserAgentBackendOnTransmute({
      defaultStylesheet: '',
      devicePixelRatio: 1,
    });

    const scene = this._scene = new BABYLON.Scene(this.engine);
    /**
     * Forcily use left-handed system to match the coordinate system of WebXR and WebGL. In JSAR environment, because the view matrix
     * and projection matrix are provided by the runtime which handness is keep consistent with the WebGL convention, thus we don't
     * support tweaking the handness of the coordinate system by the 3d engines.
     */
    scene.useRightHandedSystem = true;
    scene.skipFrustumClipping = true;

    this._defaultCamera = new BABYLON.ArcRotateCamera(
      'default_camera',
      Math.PI / 2,
      Math.PI / 2,
      5,
      BABYLON.Vector3.Zero(),
      scene);

    {
      // create default light
      const dir = new BABYLON.Vector3(0, 2, -5);
      const light = new BABYLON.HemisphericLight(
        'light_front',
        dir,
        scene);
      light.intensity = 1;
      this._defaultLights.push(light);
    }

    this._xrDefaultExperience = WebXRDefaultExperience.CreateAsync(scene, {
      xrSystem: this._xrSystem,
      outputCanvasOptions: {
        renderingContext: glContext,
        canvasOptions: {
          antialias: true,
          depth: true,
          stencil: true,
          alpha: true,
          framebufferScaleFactor: 1,
        },
      },
      pointerSelectionOptions: {
        preferredHandedness: 'right',
        enablePointerSelectionOnAllControllers: true,
      },
    });

    this.engine.runRenderLoop(() => {
      scene.render();
    });
  }

  get id(): number {
    return this._id;
  }

  async enterXrExperience(): Promise<XRSession> {
    const { baseExperience, renderTarget } = await this._xrDefaultExperience;
    await baseExperience.enterXRAsync('immersive-ar', 'unbounded', {
      optionalFeatures: [],
    }, renderTarget);
    return baseExperience.sessionManager?.session;
  }

  getNativeScene(): BABYLON.Scene {
    return this._scene;
  }
  getContainerPose(): XRPose {
    throw new TypeError('Method not implemented.');
  }
  getPreloadedMeshes(): Map<string, (BABYLON.AbstractMesh | BABYLON.TransformNode)[]> {
    return this._preloadMeshes;
  }
  getPreloadedAnimationGroups(): Map<string, BABYLON.AnimationGroup[]> {
    return this._preloadAnimationGroups;
  }
  observeInputEvent(name?: string): void {
    // TODO
  }
  createBoundTransformNode(nameOrId: string): BABYLON.TransformNode {
    throw new TypeError('Method not implemented.');
  }
  createImageBitmap(image: ArrayBuffer | ArrayBufferView): Promise<ImageBitmap> {
    const blob = new Blob([image]);
    return createImageBitmapImpl(blob);
  }
  async decodeImage(bitmap: ImageBitmap, size?: [number, number]): Promise<ImageDataImpl> {
    let expectedWidth = Math.floor(size[0]);
    let expectedHeight = Math.floor(size[1]);
    if (typeof expectedWidth !== 'number' || isNaN(expectedWidth) || expectedWidth <= 0) {
      expectedWidth = bitmap.width;
    }
    if (typeof expectedHeight !== 'number' || isNaN(expectedHeight) || expectedHeight <= 0) {
      expectedHeight = bitmap.height;
    }

    const offscreenCanvas = new OffscreenCanvasImpl(expectedWidth, expectedHeight);
    const ctx = offscreenCanvas.getContext('2d');
    if (ctx == null) {
      throw new TypeError('Failed to get 2d context from offscreen canvas.');
    }

    ctx.drawImage(
      bitmap,
      0, 0,
      bitmap.width, bitmap.height,
      0, 0,
      offscreenCanvas.width, offscreenCanvas.height
    );
    const imageData = ctx.getImageData(0, 0, offscreenCanvas.width, offscreenCanvas.height);
    offscreenCanvas[kDisposeCanvas]();
    return Promise.resolve(imageData as any);
  }
  stop(): void {
    // TODO
  }
  close(): void {
    this.engine.stopRenderLoop();
    this.engine.dispose();
    this._scene.dispose();
  }
}
