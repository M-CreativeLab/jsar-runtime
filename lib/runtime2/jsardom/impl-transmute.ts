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

import * as logger from '../../bindings/logger';
import { createBondXRSystem } from '../../webxr';
import { WebXRDefaultExperience } from './xr/DefaultExperience';

class EngineOnTransmute extends BABYLON.Engine implements JSARNativeEngine {
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
  fetch(
    url: string,
    options: FetchOptions,
    returnsAs?: FetchReturnAs
  ): Promise<FetchReturnTypes> {
    const urlObj = new URL(url);
    if (urlObj.protocol === 'file:') {
      throw new TypeError('file: protocol is not supported');
    } else {
      return fetch(url, options)
        .then((resp) => {
          if (!resp.ok) {
            throw new Error(`The request to ${url} failed with status ${resp.status}`);
          }
          if (returnsAs === 'string') {
            return resp.text();
          } else if (returnsAs === 'json') {
            return resp.json();
          } else if (returnsAs === 'arraybuffer') {
            return resp.arrayBuffer();
          }
        });
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
  vendor: string = '';
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

  constructor(init: JSARUserAgentInit) {
    this.defaultStylesheet = init.defaultStylesheet;
    this.devicePixelRatio = init.devicePixelRatio;
    this.resourceLoader = new ResourceLoaderOnTransmute();
    // this.requestManager =
    // this.deviceMemory =
  }
  domParser: DOMParser;

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
    return null;
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

  private _xrSystem: XRSystem;
  private _xrDefaultExperience: Promise<WebXRDefaultExperience>;
  private _scene: BABYLON.Scene;
  private _preloadMeshes: Map<string, Array<BABYLON.AbstractMesh | BABYLON.TransformNode>> = new Map();
  private _preloadAnimationGroups: Map<string, BABYLON.AnimationGroup[]> = new Map();
  private _defaultCamera: BABYLON.Camera;
  private _defaultDirLight: BABYLON.DirectionalLight;

  constructor(glContext: WebGLRenderingContext | WebGL2RenderingContext, xrSessionId: number) {
    super();

    this._xrSystem = createBondXRSystem(xrSessionId);
    this.engine = new EngineOnTransmute(glContext, true, {
      disableWebGL2Support: false,
      xrCompatible: true,
    });
    this.userAgent = new UserAgentBackendOnTransmute({
      defaultStylesheet: '',
      devicePixelRatio: 1,
    });

    const scene = this._scene = new BABYLON.Scene(this.engine);
    scene.useRightHandedSystem = false;
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
      const lightFront = new BABYLON.HemisphericLight('light_front', new BABYLON.Vector3(0, 2, -5), scene);
      const lightBack = new BABYLON.HemisphericLight('light_back', new BABYLON.Vector3(0, -2, 5), scene);
      lightFront.intensity = 1.2;
      lightBack.intensity = 1.2;
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
    });

    this.engine.runRenderLoop(() => {
      scene.render();
    });
  }

  async enterXrExperience(): Promise<XRSession> {
    const { baseExperience, renderTarget } = await this._xrDefaultExperience;
    await baseExperience.enterXRAsync('immersive-ar', 'local', {
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
    throw new TypeError('Method not implemented.');
  }
  decodeImage(bitmap: ImageBitmap, size?: [number, number]): Promise<ImageDataImpl> {
    throw new TypeError('Method not implemented.');
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
