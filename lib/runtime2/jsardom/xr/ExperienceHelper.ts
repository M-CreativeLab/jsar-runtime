import { WebXRCamera } from './Camera';
import { WebXRFeatureName, WebXRFeaturesManager } from './FeatureManager';
import { WebXRSessionManager } from './SessionManager';

export class WebXRExperienceHelper implements BABYLON.IDisposable {
  private _nonVRCamera: BABYLON.Nullable<BABYLON.Camera> = null;
  private _attachedToElement: boolean = false;
  private _spectatorCamera: BABYLON.Nullable<BABYLON.UniversalCamera> = null;
  private _originalSceneAutoClear = true;
  private _supported = false;
  private _spectatorMode = false;
  private _lastTimestamp = 0;

  /**
   * Camera used to render xr content
   */
  public camera: WebXRCamera;
  /** A features manager for this xr session */
  public featuresManager: WebXRFeaturesManager;
  /**
   * Observers registered here will be triggered after the camera's initial transformation is set
   * This can be used to set a different ground level or an extra rotation.
   *
   * Note that ground level is considered to be at 0. The height defined by the XR camera will be added
   * to the position set after this observable is done executing.
   */
  public onInitialXRPoseSetObservable = new BABYLON.Observable<WebXRCamera>();
  /**
   * Fires when the state of the experience helper has changed
   */
  public onStateChangedObservable = new BABYLON.Observable<BABYLON.WebXRState>();
  /** Session manager used to keep track of xr session */
  public sessionManager: WebXRSessionManager;
  /**
   * The current state of the XR experience (eg. transitioning, in XR or not in XR)
   */
  public state: BABYLON.WebXRState = BABYLON.WebXRState.NOT_IN_XR;

  /**
   * Creates the experience helper
   * @param scene the scene to attach the experience helper to
   * @returns a promise for the experience helper
   */
  public static CreateAsync(scene: BABYLON.Scene, xrSystem: XRSystem): Promise<WebXRExperienceHelper> {
    const helper = new WebXRExperienceHelper(scene);
    return helper.sessionManager
      .initializeAsync(xrSystem)
      .then(() => {
        helper._supported = true;
        return helper;
      })
      .catch((e) => {
        helper._setState(BABYLON.WebXRState.NOT_IN_XR);
        helper.dispose();
        throw e;
      });
  }

  /**
   * Creates a WebXRExperienceHelper
   * @param _scene The scene the helper should be created in
   */
  private constructor(private _scene: BABYLON.Scene) {
    this.sessionManager = new WebXRSessionManager(_scene);
    this.camera = new WebXRCamera('xrcamera', _scene, this.sessionManager);
    this.featuresManager = new WebXRFeaturesManager(this.sessionManager);

    _scene.onDisposeObservable.addOnce(() => {
      this.dispose();
    });
  }

  private _nonXRToXRCamera() {
    // this.camera.setTransformationFromNonVRCamera(this._nonVRCamera!);
    this.onInitialXRPoseSetObservable.notifyObservers(this.camera);
  }

  private _setState(val: BABYLON.WebXRState) {
    if (this.state === val) {
      return;
    }
    this.state = val;
    this.onStateChangedObservable.notifyObservers(this.state);
  }

  /**
     * Enters XR mode (This must be done within a user interaction in most browsers eg. button click)
     * @param sessionMode options for the XR session
     * @param referenceSpaceType frame of reference of the XR session
     * @param renderTarget the output canvas that will be used to enter XR mode
     * @param sessionCreationOptions optional XRSessionInit object to init the session with
     * @returns promise that resolves after xr mode has entered
     */
  public async enterXRAsync(
    sessionMode: XRSessionMode,
    referenceSpaceType: XRReferenceSpaceType,
    sessionCreationOptions: XRSessionInit = {},
    renderTarget: BABYLON.WebXRRenderTarget = this.sessionManager.getWebXRRenderTarget(),
  ): Promise<WebXRSessionManager> {
    if (!this._supported) {
      throw new Error('WebXR not supported in this browser or environment');
    }
    this._setState(BABYLON.WebXRState.ENTERING_XR);
    if (referenceSpaceType !== 'viewer' && referenceSpaceType !== 'local') {
      sessionCreationOptions.optionalFeatures = sessionCreationOptions.optionalFeatures || [];
      sessionCreationOptions.optionalFeatures.push(referenceSpaceType);
    }
    sessionCreationOptions = await this.featuresManager._extendXRSessionInitObject(sessionCreationOptions);
    // we currently recommend "unbounded" space in AR (#7959)
    if (sessionMode === 'immersive-ar' && referenceSpaceType !== 'unbounded') {
      BABYLON.Logger.Warn('We recommend using \'unbounded\' reference space type when using \'immersive-ar\' session mode');
    }
    // make sure that the session mode is supported
    try {
      await this.sessionManager.initializeSessionAsync(sessionMode, sessionCreationOptions);
      await this.sessionManager.setReferenceSpaceTypeAsync(referenceSpaceType);
      const baseLayer = await renderTarget.initializeXRLayerAsync(this.sessionManager.session);

      const xrRenderState: XRRenderStateInit = {
        // if maxZ is 0 it should be "Infinity", but it doesn't work with the WebXR API. Setting to a large number.
        depthFar: this.camera.maxZ || 10000,
        depthNear: this.camera.minZ,
      };

      // The layers feature will have already initialized the xr session's layers on session init.
      if (!this.featuresManager.getEnabledFeature(WebXRFeatureName.LAYERS)) {
        xrRenderState.baseLayer = baseLayer;
      }

      this.sessionManager.updateRenderState(xrRenderState);
      // run the render loop
      this.sessionManager.runXRRenderLoop();
      // Cache pre xr scene settings
      this._originalSceneAutoClear = this._scene.autoClear;
      this._nonVRCamera = this._scene.activeCamera;
      this._attachedToElement = !!this._nonVRCamera?.inputs?.attachedToElement;
      this._nonVRCamera?.detachControl();

      this._scene.activeCamera = this.camera;
      // do not compensate when AR session is used
      if (sessionMode !== 'immersive-ar') {
        this._nonXRToXRCamera();
      } else {
        // Kept here, TODO - check if needed
        this._scene.clearColor.set(0, 0, 0, 0);
        this._scene.autoClear = true;
        this._scene.autoClearDepthAndStencil = false;
        this.camera.compensateOnFirstFrame = false;
        // reset the camera's position to the origin
        this.camera.position.set(0, 0, -2);
        this.camera.rotationQuaternion.set(0, 0, 0, 1);
        this.onInitialXRPoseSetObservable.notifyObservers(this.camera);
      }

      this.sessionManager.onXRSessionEnded.addOnce(() => {
        // when using the back button and not the exit button (default on mobile), the session is ending but the EXITING state was not set
        if (this.state !== BABYLON.WebXRState.EXITING_XR) {
          this._setState(BABYLON.WebXRState.EXITING_XR);
        }
        // Reset camera rigs output render target to ensure sessions render target is not drawn after it ends
        this.camera.rigCameras.forEach((c) => {
          c.outputRenderTarget = null;
        });

        // Restore scene settings
        this._scene.autoClear = this._originalSceneAutoClear;
        this._scene.activeCamera = this._nonVRCamera;
        if (this._attachedToElement && this._nonVRCamera) {
          this._nonVRCamera.attachControl(!!this._nonVRCamera.inputs.noPreventDefault);
        }
        if (sessionMode !== 'immersive-ar' && this.camera.compensateOnFirstFrame) {
          if ((<any>this._nonVRCamera).setPosition) {
            (<any>this._nonVRCamera).setPosition(this.camera.position);
          } else {
            this._nonVRCamera!.position.copyFrom(this.camera.position);
          }
        }
        this._setState(BABYLON.WebXRState.NOT_IN_XR);
      });

      // Wait until the first frame arrives before setting state to in xr
      this.sessionManager.onXRFrameObservable.addOnce(() => {
        this._setState(BABYLON.WebXRState.IN_XR);
      });
      return this.sessionManager;
    } catch (e) {
      BABYLON.Logger.Log(e.message);
      this._setState(BABYLON.WebXRState.NOT_IN_XR);
      throw e;
    }
  }

  /**
   * Exits XR mode and returns the scene to its original state
   * @returns promise that resolves after xr mode has exited
   */
  public exitXRAsync() {
    // only exit if state is IN_XR
    if (this.state !== BABYLON.WebXRState.IN_XR) {
      return Promise.resolve();
    }
    this._setState(BABYLON.WebXRState.EXITING_XR);
    return this.sessionManager.exitXRAsync();
  }

  dispose(): void {
    this.exitXRAsync();
    this.camera.dispose();
    this.onStateChangedObservable.clear();
    this.onInitialXRPoseSetObservable.clear();
    this.sessionManager.dispose();
    this._spectatorCamera?.dispose();
    if (this._nonVRCamera) {
      this._scene.activeCamera = this._nonVRCamera;
    }
  }
}
