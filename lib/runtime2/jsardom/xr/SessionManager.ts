import * as logger from '../../../bindings/logger';
import { WebXRLayerWrapper } from './LayerWrapper';
import { WebXRWebglLayerWrapper } from './WebglLayer';
import { WebXRLayerRenderTargetTextureProvider } from './RenderTargetTextureProvider';
import { WebXRManagedOutputCanvas, WebXRManagedOutputCanvasOptions } from './OutputCanvas';

export class WebXRSessionManager implements BABYLON.IDisposable, BABYLON.IWebXRRenderTargetTextureProvider {
  private _engine: BABYLON.Nullable<BABYLON.Engine>;
  private _referenceSpace: XRReferenceSpace;
  private _baseLayerWrapper: BABYLON.Nullable<WebXRLayerWrapper>;
  private _baseLayerRTTProvider: BABYLON.Nullable<WebXRLayerRenderTargetTextureProvider>;
  private _xrSystem: XRSystem;
  private _sessionMode: XRSessionMode;
  private _onEngineDisposedObserver: BABYLON.Nullable<BABYLON.Observer<BABYLON.ThinEngine>>;

  /**
   * The base reference space from which the session started. good if you want to reset your
   * reference space
   */
  public baseReferenceSpace: XRReferenceSpace;
  /**
   * Current XR frame
   */
  public currentFrame: BABYLON.Nullable<XRFrame>;
  /** 
   * WebXR timestamp updated every frame.
   */
  public currentTimestamp: number = -1;
  /**
   * Used just in case of a failure to initialize an immersive session.
   * The viewer reference space is compensated using this height, creating a kind of "viewer-floor" reference space
   */
  public defaultHeightCompensation = 1.7;
  /**
   * Fires every time a new xrFrame arrives which can be used to update the camera
   */
  public onXRFrameObservable: BABYLON.Observable<XRFrame> = new BABYLON.Observable<XRFrame>();
  /**
   * Fires when the reference space changed
   */
  public onXRReferenceSpaceChanged: BABYLON.Observable<XRReferenceSpace> = new BABYLON.Observable();
  /**
     * Fires when the xr session is ended either by the device or manually done
     */
  public onXRSessionEnded: BABYLON.Observable<any> = new BABYLON.Observable<any>();
  /**
   * Fires when the xr session is initialized: right after requestSession was called and returned with a successful result
   */
  public onXRSessionInit: BABYLON.Observable<XRSession> = new BABYLON.Observable<XRSession>();
  /**
   * Underlying xr session
   */
  public session: XRSession;
  /**
   * The viewer (head position) reference space. This can be used to get the XR world coordinates
   * or get the offset the player is currently at.
   */
  public viewerReferenceSpace: XRReferenceSpace;
  /**
   * Are we currently in the XR loop?
   */
  public inXRFrameLoop: boolean = false;
  /**
   * Are we in an XR session?
   */
  public inXRSession: boolean = false;

  private _worldScalingFactor: number = 1;
  /**
   * Observable raised when the world scale has changed
   */
  public onWorldScaleFactorChangedObservable: BABYLON.Observable<{
    previousScaleFactor: number;
    newScaleFactor: number;
  }> = new BABYLON.Observable(undefined, true);

  /**
     * Scale factor to apply to all XR-related elements (camera, controllers)
     */
  public get worldScalingFactor(): number {
    return this._worldScalingFactor;
  }

  public set worldScalingFactor(value: number) {
    const oldValue = this._worldScalingFactor;
    this._worldScalingFactor = value;
    this.onWorldScaleFactorChangedObservable.notifyObservers({
      previousScaleFactor: oldValue,
      newScaleFactor: value,
    });
  }

  /**
   * Returns a promise that resolves with a boolean indicating if the provided session mode is supported by this browser
   * @param sessionMode defines the session to test
   * @returns a promise with boolean as final value
   */
  public static async IsSessionSupportedAsync(xrSystem: XRSystem, sessionMode: XRSessionMode): Promise<boolean> {
    if (!xrSystem) {
      return false;
    }

    // When the specs are final, remove supportsSession!
    const functionToUse = xrSystem.isSessionSupported;
    if (!functionToUse) {
      return false;
    } else {
      return functionToUse
        .call(xrSystem, sessionMode)
        .then((result: boolean) => {
          return typeof result === 'undefined' ? true : result;
        })
        .catch((e: any) => {
          BABYLON.Logger.Warn(e);
          return false;
        });
    }
  }

  /**
   * Constructs a WebXRSessionManager, this must be initialized within a user action before usage
   * @param scene The scene which the session should be created for
   */
  constructor(
    /** The scene which the session should be created for */
    public scene: BABYLON.Scene
  ) {
    this._engine = scene.getEngine();
    this._onEngineDisposedObserver = this._engine.onDisposeObservable.addOnce(() => {
      this._engine = null;
    });
    scene.onDisposeObservable.addOnce(() => {
      this.dispose();
    });
  }

  /**
   * The current reference space used in this session. This reference space can constantly change!
   * It is mainly used to offset the camera's position.
   */
  get referenceSpace(): XRReferenceSpace {
    return this._referenceSpace;
  }

  /**
   * Set a new reference space and triggers the observable
   */
  set referenceSpace(newReferenceSpace: XRReferenceSpace) {
    this._referenceSpace = newReferenceSpace;
    this.onXRReferenceSpaceChanged.notifyObservers(this._referenceSpace);
  }

  trySetViewportForView(viewport: BABYLON.Viewport, view: XRView): boolean {
    return this._baseLayerRTTProvider?.trySetViewportForView(viewport, view) || false;
  }
  getRenderTargetTextureForEye(eye: XREye): BABYLON.Nullable<BABYLON.RenderTargetTexture> {
    return this._baseLayerRTTProvider?.getRenderTargetTextureForEye(eye) || null;
  }
  getRenderTargetTextureForView(view: XRView): BABYLON.Nullable<BABYLON.RenderTargetTexture> {
    return this._baseLayerRTTProvider?.getRenderTargetTextureForView(view) || null;
  }
  /**
   * Creates a WebXRRenderTarget object for the XR session
   * @param options optional options to provide when creating a new render target
   * @returns a WebXR render target to which the session can render
   */
  public getWebXRRenderTarget(options?: WebXRManagedOutputCanvasOptions): BABYLON.WebXRRenderTarget {
    return new WebXRManagedOutputCanvas(this, options);
  }

  /**
   * Initializes the manager
   * After initialization enterXR can be called to start an XR session
   * @returns Promise which resolves after it is initialized
   */
  public async initializeAsync(xrSystem: XRSystem): Promise<void> {
    if (!xrSystem) {
      throw new TypeError('WebXR not available');
    }
    this._xrSystem = xrSystem;
    return;
  }

  /**
     * Initializes an xr session
     * @param xrSessionMode mode to initialize
     * @param xrSessionInit defines optional and required values to pass to the session builder
     * @returns a promise which will resolve once the session has been initialized
     */
  public async initializeSessionAsync(
    xrSessionMode: XRSessionMode = 'immersive-ar',
    xrSessionInit: XRSessionInit = {}
  ): Promise<XRSession> {
    const xr = this._xrSystem;
    if (!xr) {
      throw new TypeError('WebXR not available when trying to initialize session.');
    }
    return xr.requestSession(xrSessionMode, xrSessionInit)
      .then((session: XRSession) => {
        logger.info(`a session request(mode=${xrSessionMode} was successful`, session);
        this.session = session;
        this._sessionMode = xrSessionMode;
        this.onXRSessionInit.notifyObservers(session);
        this.inXRSession = true;
        this.session.addEventListener(
          'end',
          () => {
            this.inXRSession = false;
            this.onXRSessionEnded.notifyObservers(null);
            if (this._engine) {
              this._engine.framebufferDimensionsObject = null;
              this._engine.restoreDefaultFramebuffer();
              this._engine.customAnimationFrameRequester = null;
              this._engine._renderLoop();
            }
            this._baseLayerRTTProvider = null;
            this._baseLayerWrapper = null;
          },
          { once: true }
        );
        return this.session;
      });
  }

  /**
     * Checks if a session would be supported for the creation options specified
     * @param sessionMode session mode to check if supported eg. immersive-vr
     * @returns A Promise that resolves to true if supported and false if not
     */
  public isSessionSupportedAsync(sessionMode: XRSessionMode): Promise<boolean> {
    return WebXRSessionManager.IsSessionSupportedAsync(this._xrSystem, sessionMode);
  }

  /**
   * Resets the reference space to the one started the session
   */
  public resetReferenceSpace() {
    this.referenceSpace = this.baseReferenceSpace;
  }

  /**
   * @internal
   */
  public _setBaseLayerWrapper(baseLayerWrapper: BABYLON.Nullable<WebXRLayerWrapper>): void {
    this._baseLayerWrapper = baseLayerWrapper;
    this._baseLayerRTTProvider = this._baseLayerWrapper?.createRenderTargetTextureProvider(this) || null;
  }

  /**
   * Sets the reference space on the xr session
   * @param referenceSpaceType space to set
   * @returns a promise that will resolve once the reference space has been set
   */
  public setReferenceSpaceTypeAsync(referenceSpaceType: XRReferenceSpaceType = 'local'): Promise<XRReferenceSpace> {
    return this.session
      .requestReferenceSpace(referenceSpaceType)
      .then(
        (referenceSpace) => {
          return referenceSpace as XRReferenceSpace;
        },
        (rejectionReason) => {
          BABYLON.Logger.Error('XR.requestReferenceSpace failed for the following reason: ');
          BABYLON.Logger.Error(rejectionReason);
          BABYLON.Logger.Log('Defaulting to universally-supported "viewer" reference space type.');

          return this.session.requestReferenceSpace('viewer').then(
            (referenceSpace) => {
              const heightCompensation = new XRRigidTransform({ x: 0, y: -this.defaultHeightCompensation, z: 0 });
              return (referenceSpace as XRReferenceSpace).getOffsetReferenceSpace(heightCompensation);
            },
            (rejectionReason) => {
              BABYLON.Logger.Error(rejectionReason);
              throw 'XR initialization failed: required "viewer" reference space type not supported.';
            }
          );
        }
      )
      .then((referenceSpace) => {
        // create viewer reference space before setting the first reference space
        return this.session.requestReferenceSpace('viewer').then((viewerReferenceSpace) => {
          this.viewerReferenceSpace = viewerReferenceSpace as XRReferenceSpace;
          return referenceSpace;
        });
      })
      .then((referenceSpace) => {
        // initialize the base and offset (currently the same)
        this.referenceSpace = this.baseReferenceSpace = referenceSpace;
        return this.referenceSpace;
      });
  }

  /**
     * Starts rendering to the xr layer
     */
  public runXRRenderLoop() {
    if (!this.inXRSession || !this._engine) {
      return;
    }

    // Tell the engine's render loop to be driven by the xr session's refresh rate and provide xr pose information
    this._engine.customAnimationFrameRequester = {
      requestAnimationFrame: (callback: FrameRequestCallback) => this.session.requestAnimationFrame(callback),
      renderFunction: (timestamp: number, xrFrame: BABYLON.Nullable<XRFrame>) => {
        if (!this.inXRSession || !this._engine) {
          return;
        }
        // Store the XR frame and timestamp in the session manager
        this.currentFrame = xrFrame;
        this.currentTimestamp = timestamp;

        if (xrFrame) {
          this.inXRFrameLoop = true;
          this._engine.framebufferDimensionsObject = this._baseLayerRTTProvider?.getFramebufferDimensions() || null;
          this.onXRFrameObservable.notifyObservers(xrFrame);
          this._engine._renderLoop();
          this._engine.framebufferDimensionsObject = null;
          this.inXRFrameLoop = false;
        }
      },
    };

    this._engine.framebufferDimensionsObject = this._baseLayerRTTProvider?.getFramebufferDimensions() || null;

    // Stop window's animation frame and trigger sessions animation frame
    if (typeof globalThis.cancelAnimationFrame === 'function') {
      globalThis.cancelAnimationFrame(this._engine._frameHandler);
    }
    this._engine._renderLoop();
  }

  /**
   * Updates the render state of the session
   * @param state state to set
   */
  public updateRenderState(state: XRRenderStateInit): void {
    if (state.baseLayer) {
      this._setBaseLayerWrapper(new WebXRWebglLayerWrapper(state.baseLayer));
    }
    this.session.updateRenderState(state);
  }

  /**
   * Updates the render state of the session.
   * Note that this is deprecated in favor of WebXRSessionManager.updateRenderState().
   * @param state state to set
   * @returns a promise that resolves once the render state has been updated
   * @deprecated
   */
  public updateRenderStateAsync(state: XRRenderState): Promise<void> {
    return Promise.resolve(this.session.updateRenderState(state));
  }

  /**
     * Run a callback in the xr render loop
     * @param callback the callback to call when in XR Frame
     * @param ignoreIfNotInSession if no session is currently running, run it first thing on the next session
     */
  public runInXRFrame(callback: () => void, ignoreIfNotInSession = true): void {
    if (this.inXRFrameLoop) {
      callback();
    } else if (this.inXRSession || !ignoreIfNotInSession) {
      this.onXRFrameObservable.addOnce(callback);
    }
  }

  dispose(): void {
    // disposing without leaving XR? Exit XR first
    if (this.inXRSession) {
      this.exitXRAsync();
    }
    this.onXRFrameObservable.clear();
    this.onXRSessionEnded.clear();
    this.onXRReferenceSpaceChanged.clear();
    this.onXRSessionInit.clear();
    this._engine?.onDisposeObservable.remove(this._onEngineDisposedObserver);
    this._engine = null;
  }

  /**
   * Stops the xrSession and restores the render loop
   * @returns Promise which resolves after it exits XR
   */
  public exitXRAsync() {
    if (this.session && this.inXRSession) {
      this.inXRSession = false;
      return this.session.end().catch(() => {
        BABYLON.Logger.Warn('Could not end XR session.');
      });
    }
    return Promise.resolve();
  }
}
