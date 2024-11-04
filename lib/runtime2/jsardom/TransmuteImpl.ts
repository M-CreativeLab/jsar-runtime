import {
  type NativeEngine as JSARNativeEngine,
  type NativeDocument as JSARNativeDocument,
  type ResourceLoader as JSARResourceLoader,
  type RequestManager as JSARRequestManager,
  type UserAgent as JSARUserAgent,
  type UserAgentInit as JSARUserAgentInit,
  type DOMParser,
  type MediaPlayerConstructor,
  type XRSessionBackend,
  type XRSessionBackendInit,
  type SpatialDocumentImpl,
  JSARInputEvent,
  cdp as jsarCdp,
} from '@yodaos-jsar/dom';
import * as ws from 'ws';
import { getClientContext } from '@transmute/env';

import { ResourceLoaderOnTransmute } from './ResourceLoader';
import { MediaPlayerBackendOnTransmute } from './MediaPlayer';
import { WebGLMatrix } from '../../webgl/WebGLMatrix';
import { WebXRDefaultExperience } from './xr/DefaultExperience';
import type { WebXRExperienceHelper } from './xr/ExperienceHelper';

type TransmuteEngineOptions = BABYLON.EngineOptions & {
  // TODO
};
class EngineOnTransmute extends BABYLON.Engine implements JSARNativeEngine {
  private webglPlaceholdersEnabled: boolean = process.env['JSAR_WEBGL_PLACEHOLDERS'] === 'yes';

  constructor(
    glContext: WebGLRenderingContext | WebGL2RenderingContext,
    antialias: boolean,
    options?: TransmuteEngineOptions,
    adaptToDeviceRatio?: boolean
  ) {
    super(glContext, antialias, options, adaptToDeviceRatio);
    this.disableUniformBuffers = true;
  }

  get activeScene(): BABYLON.Scene {
    if (this.scenes.length <= 0) {
      throw new Error('No scene available');
    }
    return this.scenes[0];
  }

  get useRightHandedSystem(): boolean {
    return this.activeScene.useRightHandedSystem;
  }

  setMatrices(uniform: WebGLUniformLocation, matrices: Float32Array): boolean {
    /**
     * When JSAR_WEBGL_PLACEHOLDER is enabled, we will replace the projection, view and viewProjection matrices with
     * placeholders. The placeholders will be computed at the native rendering loop.
     */
    if (this.webglPlaceholdersEnabled) {
      const name = uniform?.name;
      switch (name) {
        case 'projection':
          matrices = WebGLMatrix.CreateProjectionMatrix(matrices, this.useRightHandedSystem);
          break;
        case 'view':
          matrices = WebGLMatrix.CreateViewMatrix(matrices, this.useRightHandedSystem);
          break;
        case 'viewProjection':
          matrices = WebGLMatrix.CreateViewProjectionMatrix(matrices, this.useRightHandedSystem);
          break;
        case 'viewProjectionR':
          matrices = WebGLMatrix.CreateViewProjectionMatrixForRightEye(matrices, this.useRightHandedSystem);
          break;
        default:
          break;
      }
    }
    return super.setMatrices(uniform, matrices);
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
  console: Console = console;
  attachedDocument: SpatialDocumentImpl<NativeDocumentOnTransmute>;
  closed: boolean = false;
  cdpTransport?: jsarCdp.ITransport;

  private _id: number;
  private _xrSystem: XRSystem;
  private _xrSession: XRSession;
  private _xrDefaultExperience: Promise<WebXRDefaultExperience> = null;
  private _scene: BABYLON.Scene;
  private _preloadMeshes: Map<string, Array<BABYLON.AbstractMesh | BABYLON.TransformNode>> = new Map();
  private _preloadAnimationGroups: Map<string, BABYLON.AnimationGroup[]> = new Map();
  private _defaultCamera: BABYLON.Camera;
  private _defaultLights: BABYLON.Light[] = [];

  constructor(glContext: WebGLRenderingContext | WebGL2RenderingContext) {
    super();

    this._id = getClientContext().id;
    if (navigator.xr == null) {
      throw new TypeError('WebXR is not supported.');
    }
    this._xrSystem = navigator.xr;
    this.engine = new EngineOnTransmute(glContext, true, {
      xrCompatible: true,
    });
    this.userAgent = new UserAgentBackendOnTransmute({
      defaultStylesheet: '',
      devicePixelRatio: 1,
    });

    const scene = this._scene = new BABYLON.Scene(this.engine);
    scene.useRightHandedSystem = false; /** use left-handed system */
    scene.skipFrustumClipping = false;

    // Performance optimization
    scene.autoClear = false;
    scene.autoClearDepthAndStencil = false;
    scene.blockMaterialDirtyMechanism = true;
    scene.performancePriority = BABYLON.ScenePerformancePriority.Intermediate;

    this._defaultCamera = new BABYLON.ArcRotateCamera(
      'default_camera',
      Math.PI / 2,
      Math.PI / 2,
      -1.5,
      BABYLON.Vector3.Zero(),
      scene);
    this._defaultCamera.position.y += 1.0;

    {
      // create default light
      const dir = new BABYLON.Vector3(0, 2, -5);
      const frontLight = new BABYLON.DirectionalLight('light_front', dir, scene);
      frontLight.intensity = 2;
      this._defaultLights.push(frontLight);
      const backLight = new BABYLON.DirectionalLight('light_back', dir.negate(), scene);
      backLight.intensity = 2;
      this._defaultLights.push(backLight);
    }

    this.engine.runRenderLoop(() => {
      scene.render();
    });
  }

  get id(): number {
    return this._id;
  }

  configureDefaultXrExperience(glContext: WebGLRenderingContext | WebGL2RenderingContext) {
    this._xrDefaultExperience = WebXRDefaultExperience.CreateAsync(this, {
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
        ignoreGazeController: true,
        preferredHandedness: 'right',
        enablePointerSelectionOnAllControllers: false,
        disableScenePointerVectorUpdate: true,
      },
      teleportationOptions: {
        forceHandedness: 'right',
        defaultTargetMeshOptions: {
          disableLighting: true,
        },
      },
      nearInteractionOptions: {
        preferredHandedness: 'right',
        enableNearInteractionOnAllControllers: false,
      },
      disablePointerSelection: false,
      disableTeleportation: true,
      disableNearInteraction: true,
    });
  }

  #registerListenersForXR(xrExperience: WebXRExperienceHelper) {
    xrExperience.sessionManager.onMeshPickObservable.add(({ mesh, pick }) => {
      const uvCoord = pick.getTextureCoordinates();
      this.attachedDocument.dispatchEvent(new JSARInputEvent('raycast', {
        sourceId: 'scene_default_ray',
        sourceType: 'hand',
        targetSpatialElementInternalGuid: mesh.uniqueId,
        uvCoord,
      }));
    });
    this._scene.onPointerObservable.add((pointerEvent) => {
      if (this.attachedDocument == null || !this.attachedDocument) {
        return;
      }
      if (pointerEvent.type === BABYLON.PointerEventTypes.POINTERDOWN) {
        this.attachedDocument.dispatchEvent(new JSARInputEvent('raycast_action', {
          sourceId: 'scene_default_ray',
          type: 'down',
        }));
      } else if (pointerEvent.type === BABYLON.PointerEventTypes.POINTERUP) {
        this.attachedDocument.dispatchEvent(new JSARInputEvent('raycast_action', {
          sourceId: 'scene_default_ray',
          type: 'up',
        }));
      }
    });

    const updateCollisionBox = () => {
      const xrSession = xrExperience.sessionManager.session;
      const spaceNode = this.attachedDocument.space.asNativeType<BABYLON.TransformNode>();
      try {
        const { min, max } = spaceNode.getHierarchyBoundingVectors(true, (mesh) => mesh.isPickable);
        xrSession.updateCollisionBox(
          [min.x, min.y, min.z],
          [max.x, max.y, max.z]
        );
      } catch (err) {
        console.error('Failed to update collision box:', err);
      }
    };
    this.addEventListener('documentLoaded', () => {
      updateCollisionBox();
      this._scene.onNewMeshAddedObservable.add(() => {
        updateCollisionBox();
      });
    }, { once: true });
  }

  #unregisterListenersForXR(xrExperience: WebXRExperienceHelper) {
    xrExperience.sessionManager.onMeshPickObservable.clear();
    this._scene.onPointerObservable.clear();
    this._scene.onNewMeshAddedObservable.clear();
  }

  async enterDefaultXrExperience(): Promise<XRSession> {
    const { baseExperience, renderTarget } = await this._xrDefaultExperience;
    baseExperience.sessionManager.onXRSessionInit.addOnce((session) => {
      this._xrSession = session;
      this.#registerListenersForXR(baseExperience);
    });
    await baseExperience.enterXRAsync('immersive-ar', 'unbounded', {
      optionalFeatures: [],
    }, renderTarget);
    return baseExperience.sessionManager?.session;
  }

  dispatchDocumentLoadedEvent() {
    this.dispatchEvent(new Event('documentLoaded'));
  }

  getNativeScene(): BABYLON.Scene {
    return this._scene;
  }

  getContainerPose(): XRPose {
    throw new TypeError('Method not implemented.');
  }

  getXRSession(): XRSession {
    return this._xrSession;
  }

  getRecommendedBoudingSize(): number {
    return this._xrSession?.['recommendedContentSize'] || 1.0;
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
    return createImageBitmap(new Blob([image]));
  }

  async decodeImage(bitmap: ImageBitmap, size?: [number, number]): Promise<any> {
    let expectedWidth = Math.floor(size[0]);
    let expectedHeight = Math.floor(size[1]);
    if (typeof expectedWidth !== 'number' || isNaN(expectedWidth) || expectedWidth <= 0) {
      expectedWidth = bitmap.width;
    }
    if (typeof expectedHeight !== 'number' || isNaN(expectedHeight) || expectedHeight <= 0) {
      expectedHeight = bitmap.height;
    }

    const offscreenCanvas = new OffscreenCanvas(expectedWidth, expectedHeight);
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
    return imageData as any;
  }
  stop(): void {
    // TODO
  }
  close(): void {
    this.engine.stopRenderLoop();
    this.engine.dispose();
    this._scene.dispose();

    // Check if the default XR experience is created and to be a Promise object
    if (this._xrDefaultExperience instanceof Promise) {
      this._xrDefaultExperience
        .then(async ({ baseExperience }) => {
          this.#unregisterListenersForXR(baseExperience);
          await baseExperience.exitXRAsync();
          baseExperience.dispose();
        });
    }
  }
}
