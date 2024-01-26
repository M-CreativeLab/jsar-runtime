import {
  type SpatialDocumentImpl,
  type NativeDocument,
  type UserAgent,
  type UserAgentInit,
  type ResourceLoader,
  type RequestManager,
  type DOMParser,
  type JSARDOM,
  type MediaPlayerConstructor,
  type MediaPlayerBackend,
  type XRSessionBackend,
  type XRSessionBackendInit,
  type XRFeature
} from '@yodaos-jsar/dom';
import * as undici from 'undici';
import * as ws from 'ws';

import { Logger } from '../bindings/logger';
import type { TransmuteRuntime } from './index';
import { TransmuteEngine } from './babylonjs-engine/engine';
import {
  OffscreenCanvasImpl,
  createImageBitmapImpl,
  kDisposeCanvas,
} from '../polyfills/offscreencanvas';

function canParseURL(url: string): boolean {
  try {
    new URL(url);
    return true;
  } catch (_e) {
    return false;
  }
}

type ResourceAs = 'string' | 'json' | 'arraybuffer';
type ResourceResult = string | object | ArrayBuffer;
type ResourceFetchOptions = {
  accept?: string;
  cookieJar?: any;
  referrer?: string;
};
type ResourceLoaderInit = {
  disableCache: boolean;
  httpsProxyServer: string | undefined;
};

export class TransmuteResourceLoader implements ResourceLoader {
  private _disableCache: boolean = false;
  private _httpsProxyServer: string | undefined;
  private _httpsProxyAgent: undici.ProxyAgent | undefined;

  constructor(
    init: ResourceLoaderInit,
    private _runtime: TransmuteRuntime,
    private _nativeDocument: TransmuteNativeDocument
  ) {
    this._disableCache = init?.disableCache || false;
    const httpsProxyServer = this._httpsProxyServer = init?.httpsProxyServer;
    if (httpsProxyServer &&
      (httpsProxyServer.startsWith('http://') || httpsProxyServer.startsWith('https://'))
    ) {
      this._httpsProxyAgent = new undici.ProxyAgent({
        uri: this._httpsProxyServer,
      });
    }
  }

  private _readFile(pathname: string, returnsAs: ResourceAs): Promise<ResourceResult> {
    /**
     * FIXME: This is a workaround for the issue that the path of the file URL on Windows starts with a slash.
     */
    let isWin: boolean;
    if (typeof navigator !== 'undefined') {
      isWin = navigator.platform?.indexOf('Win') > -1;
    } else {
      isWin = process.platform === 'win32';
    }
    if (isWin && pathname.startsWith('/')) {
      pathname = pathname.slice(1);
    }

    if (returnsAs === 'string') {
      return this._runtime.readTextFile(pathname);
    } else if (returnsAs === 'json') {
      return this._runtime.readTextFile(pathname)
        .then((text) => JSON.parse(text));
    } else if (returnsAs === 'arraybuffer') {
      return this._runtime.readBinaryFile(pathname);
    } else {
      throw new TypeError(`Unknown return type: "${returnsAs}"`);
    }
  }

  fetch(url: string, options: ResourceFetchOptions, returnsAs: 'string'): Promise<string>;
  fetch(url: string, options: ResourceFetchOptions, returnsAs: 'json'): Promise<object>;
  fetch(url: string, options: ResourceFetchOptions, returnsAs: 'arraybuffer'): Promise<ArrayBuffer>;
  fetch<T = ResourceResult>(url: string, options: ResourceFetchOptions, returnsAs?: ResourceAs): Promise<T>;
  async fetch(
    url: string,
    options: ResourceFetchOptions,
    returnsAs?: ResourceAs
  ): Promise<ResourceResult> {
    if (!canParseURL(url)) {
      throw new TypeError(`Failed to fetch: Invalid URL ${url}`);
    }
    const urlObj = new URL(url);
    if (urlObj.protocol === 'file:') {
      return this._readFile(urlObj.pathname, returnsAs);
    } else if (this._disableCache === true) {
      return this._request(url, options, returnsAs, false);
    } else {
      const [isCached, cachedUrl] = await this._runtime.isResourceCached(url);
      if (isCached && await this._runtime.shouldUseResourceCache(url, cachedUrl)) {
        return this._readFile(cachedUrl, returnsAs);
      } else {
        return this._request(url, options, returnsAs, true);
      }
    }
  }

  private async _request(
    url: string,
    options: ResourceFetchOptions,
    returnsAs: ResourceAs,
    writeCache: boolean = true
  ): Promise<ResourceResult> {
    const reqInit: undici.RequestInit = {
      ...options,
    };
    if (this._httpsProxyAgent) {
      reqInit.dispatcher = this._httpsProxyAgent;
    }
    const resp = await undici.fetch(url, reqInit);
    if (resp.status >= 400) {
      throw new Error(`Failed to fetch(${url}), statusCode=${resp.status}, text=${resp.statusText}`);
    }
    if (returnsAs === 'string') {
      const str = await resp.text();
      if (writeCache) {
        this._runtime.cacheResource(url, str);
      }
      return str;
    } else if (returnsAs === 'json') {
      const obj = await resp.json() as any;
      if (writeCache) {
        this._runtime.cacheResource(url, obj);
      }
      return obj;
    } else if (returnsAs === 'arraybuffer') {
      const buf = await resp.arrayBuffer();
      if (writeCache) {
        this._runtime.cacheResource(url, new Uint8Array(buf));
      }
      return buf
    }
  }
}

/**
 * @internal
 * 
 * This class is used to implement the MediaPlayerBackend interface for the transmute browser usecase.
 */
class AudioPlayerOnBrowser implements MediaPlayerBackend {
  private _audioInstance: HTMLAudioElement;

  constructor() {
    this._audioInstance = new Audio();
  }
  load(buffer: ArrayBuffer | ArrayBufferView, onloaded: () => void): void {
    this._audioInstance.src = URL.createObjectURL(new Blob([buffer]));
    this._audioInstance.onloadeddata = onloaded;
  }
  play(when?: number | undefined): void {
    this._audioInstance.play();
    this._audioInstance.currentTime = when || 0;
  }
  pause(): void {
    this._audioInstance.pause();
  }
  canPlayType(type: string): CanPlayTypeResult {
    return this._audioInstance.canPlayType(type);
  }
  dispose(): void {
    // TODO
  }
  get paused(): boolean {
    return this._audioInstance.paused;
  }
  get currentTime(): number {
    return this._audioInstance.currentTime;
  }
  get duration(): number {
    return this._audioInstance.duration;
  }
  get volume(): number {
    return this._audioInstance.volume;
  }
  set volume(value: number) {
    this._audioInstance.volume = value;
  }
  get loop(): boolean {
    return this._audioInstance.loop;
  }
  set loop(value: boolean) {
    this._audioInstance.loop = value;
  }
  get onended(): () => void {
    return this._audioInstance.onended as any;
  }
  set onended(value: () => void) {
    this._audioInstance.onended = value;
  }
}

class TransmuteXRSessionBackend implements XRSessionBackend {
  get enabledFeatures(): XRFeature[] {
    // Enable these features by default.
    return [
      'local',
      'hit-test',
      'hand-tracking',
    ];
  }
  constructor(_init: XRSessionBackendInit | undefined, private _nativeDocument: TransmuteNativeDocument) {
  }
  async request(): Promise<void> {
    this._nativeDocument.observeInputEvent();
  }
  async requestReferenceSpace(type: XRReferenceSpaceType): Promise<XRReferenceSpace | XRBoundedReferenceSpace> {
    return null;
  }
  async end(): Promise<void> {
    // Do nothing
  }
}

export class TransmuteUserAgent implements UserAgent {
  get versionString(): string {
    return process.env.JSAR_VERSION;
  }
  vendor: string;
  vendorSub: string;
  language: string;
  languages: readonly string[];
  defaultStylesheet: string;
  devicePixelRatio: number = 1.0;
  domParser: DOMParser;
  resourceLoader: ResourceLoader;
  requestManager: RequestManager;

  constructor(
    init: UserAgentInit & ResourceLoaderInit,
    private _runtime: TransmuteRuntime,
    private _nativeDocument: TransmuteNativeDocument
  ) {
    this.defaultStylesheet = init.defaultStylesheet;
    this.devicePixelRatio = init.devicePixelRatio;
    this.resourceLoader = new TransmuteResourceLoader({
      disableCache: init.disableCache,
      httpsProxyServer: init.httpsProxyServer,
    }, this._runtime, this._nativeDocument);
  }

  alert(_message?: string): void {
    throw new TypeError('Method(alert) not implemented.');
  }

  confirm(_message?: string): boolean {
    throw new TypeError('Method(confirm) not implemented.');
  }

  prompt(_message?: string, _defaultValue?: string): string {
    throw new TypeError('Method(prompt) not implemented.');
  }

  getMediaPlayerConstructor(): MediaPlayerConstructor {
    const linkedBindingFn = (process as any)._linkedBinding;
    if (typeof linkedBindingFn !== 'function') {
      return AudioPlayerOnBrowser;
    } else {
      return (process as any)._linkedBinding('webaudio').AudioPlayer;
    }
  }

  getWebSocketConstructor(): typeof WebSocket {
    return ws.WebSocket as unknown as typeof WebSocket;
  }

  createXRSessionBackend(init?: XRSessionBackendInit): TransmuteXRSessionBackend {
    return new TransmuteXRSessionBackend(init, this._nativeDocument);
  }
}

export class TransmuteNativeDocument extends EventTarget implements NativeDocument {
  engine: TransmuteEngine;
  userAgent: UserAgent;
  baseURI: string;
  console: Console;
  attachedDocument: SpatialDocumentImpl;
  closed: boolean;

  private _scene: BABYLON.Scene;
  private _containerPose: XRPose;
  private _preloadMeshes: Map<string, Array<BABYLON.AbstractMesh | BABYLON.TransformNode>> = new Map();
  private _preloadAnimationGroups: Map<string, BABYLON.AnimationGroup[]> = new Map();

  constructor(
    private _channelId: string,
    private _options: {
      engineInit: ConstructorParameters<typeof TransmuteEngine>[1];
      runtime: TransmuteRuntime;
      disableCache: boolean;
      httpsProxyServer: string | undefined;
    }
  ) {
    super();

    this.engine = new TransmuteEngine(_channelId, _options.engineInit);
    this.userAgent = new TransmuteUserAgent(
      {
        defaultStylesheet: '',
        devicePixelRatio: 1,
        disableCache: this._options.disableCache,
        httpsProxyServer: this._options.httpsProxyServer,
      },
      _options.runtime,
      this
    );
    this.console = new Logger();
    this._scene = new BABYLON.Scene(this.engine);
  }

  getNativeScene(): BABYLON.Scene {
    return this._scene;
  }

  getContainerPose(): XRPose {
    return this._containerPose;
  }

  getViewerPose(): XRPose {
    return null;
  }

  /**
   * @internal
   */
  _setContainerPose(
    dom: JSARDOM<TransmuteNativeDocument>,
    position: DOMPointInit,
    rotation: DOMPointInit
  ): void {
    const { DOMPointReadOnly, XRRigidTransform, XRPose } = dom.window;
    const transform = new XRRigidTransform(
      DOMPointReadOnly.fromPoint(position),
      DOMPointReadOnly.fromPoint(rotation)
    );
    this._containerPose = XRPose.createForImpl(this, [], {
      transform,
      emulatedPosition: true,
    });
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
    // TODO
    return null;
  }

  stop(): void {
    // TODO
  }

  close(): void {
    this.engine.stopRenderLoop();
    this.engine.dispose();
    this._scene.dispose();
  }

  createImageBitmap(image: ArrayBuffer | ArrayBufferView): Promise<ImageBitmap> {
    return createImageBitmapImpl(new Blob([image]));
  }

  decodeImage(bitmap: ImageBitmap, size?: [number, number]) {
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

  async ready(dom: JSARDOM<TransmuteNativeDocument>) {
    const metadata = await dom.createDocumentManifest();
    await this.engine.onReady(this._scene, {
      title: metadata.title,
      metadata: {
        specVersion: metadata.specVersion,
        description: metadata.description,
        author: metadata.author,
        keywords: metadata.keywords,
        license: metadata.license,
        licenseUrl: metadata.licenseUrl,
        viewportInitialScale: metadata.viewport?.initialScale,
      },
    });
  }
}
