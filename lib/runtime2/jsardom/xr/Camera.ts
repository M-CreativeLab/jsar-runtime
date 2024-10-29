import { WebXRSessionManager } from './SessionManager';

/**
 * WebXR Camera which holds the views for the xrSession
 * @see https://doc.babylonjs.com/features/featuresDeepDive/webXR/webXRCamera
 */
export class WebXRCamera extends BABYLON.FreeCamera {
  private _firstFrame = false;
  private _referenceQuaternion: BABYLON.Quaternion = BABYLON.Quaternion.Identity();
  private _referencedPosition: BABYLON.Vector3 = new BABYLON.Vector3();
  private _trackingState: BABYLON.WebXRTrackingState = BABYLON.WebXRTrackingState.NOT_TRACKING;

  /**
   * This will be triggered after the first XR Frame initialized the camera,
   * including the right number of views and their rendering parameters
   */
  public onXRCameraInitializedObservable = new BABYLON.Observable<WebXRCamera>();

  /**
   * Observable raised before camera teleportation
   * @deprecated use onBeforeCameraTeleport of the teleportation feature instead
   */
  public onBeforeCameraTeleport = new BABYLON.Observable<BABYLON.Vector3>();

  /**
   *  Observable raised after camera teleportation
   * @deprecated use onAfterCameraTeleport of the teleportation feature instead
   */
  public onAfterCameraTeleport = new BABYLON.Observable<BABYLON.Vector3>();

  /**
   * Notifies when the camera's tracking state has changed.
   * Notice - will also be triggered when tracking has started (at the beginning of the session)
   */
  public onTrackingStateChanged = new BABYLON.Observable<BABYLON.WebXRTrackingState>();

  /**
   * Should position compensation execute on first frame.
   * This is used when copying the position from a native (non XR) camera
   */
  public compensateOnFirstFrame: boolean = true;

  /**
   * The last XRViewerPose from the current XRFrame
   * @internal
   */
  public _lastXRViewerPose?: XRViewerPose;

  /**
   * Creates a new webXRCamera, this should only be set at the camera after it has been updated by the xrSessionManager
   * @param name the name of the camera
   * @param scene the scene to add the camera to
   * @param _xrSessionManager a constructed xr session manager
   */
  constructor(
    name: string,
    scene: BABYLON.Scene,
    private _xrSessionManager: WebXRSessionManager
  ) {
    super(name, BABYLON.Vector3.Zero(), scene);

    // Initial camera configuration
    this.minZ = 0.01;
    this.maxZ = 1000;
    this.rotationQuaternion = new BABYLON.Quaternion();
    this.cameraRigMode = BABYLON.Camera.RIG_MODE_CUSTOM;
    this.updateUpVectorFromRotation = true;
    this._updateNumberOfRigCameras(1);
    // freeze projection matrix, which will be copied later
    this.freezeProjectionMatrix();
    this._deferOnly = true;

    this._xrSessionManager.onXRSessionInit.add(() => {
      this._referencedPosition.copyFromFloats(0, 0, 0);
      this._referenceQuaternion.copyFromFloats(0, 0, 0, 1);
      // first frame - camera's y position should be 0 for the correct offset
      this._firstFrame = this.compensateOnFirstFrame;
    });

    // Check transformation changes on each frame. Callback is added to be first so that the transformation will be
    // applied to the rest of the elements using the referenceSpace object
    this._xrSessionManager.onXRFrameObservable.add(
      () => {
        if (this._firstFrame) {
          this._updateFromXRSession();
        }
        if (this.onXRCameraInitializedObservable.hasObservers()) {
          this.onXRCameraInitializedObservable.notifyObservers(this);
          this.onXRCameraInitializedObservable.clear();
        }

        // this._updateReferenceSpace();
        this._updateFromXRSession();
      },
      undefined,
      true
    );
  }

  /**
   * Get the current XR tracking state of the camera
   */
  public get trackingState(): BABYLON.WebXRTrackingState {
    return this._trackingState;
  }

  private _setTrackingState(newState: BABYLON.WebXRTrackingState) {
    if (this._trackingState !== newState) {
      this._trackingState = newState;
      this.onTrackingStateChanged.notifyObservers(newState);
    }
  }

  /**
   * Return the user's height, unrelated to the current ground.
   * This will be the y position of this camera, when ground level is 0.
   */
  public get realWorldHeight(): number {
    const basePose = this._xrSessionManager.currentFrame && this._xrSessionManager.currentFrame.getViewerPose(this._xrSessionManager.baseReferenceSpace);
    if (basePose && basePose.transform) {
      return basePose.transform.position.y;
    } else {
      return 0;
    }
  }

  /**
   * Gets the current instance class name ("WebXRCamera").
   * @returns the class name
   */
  public getClassName(): string {
    return 'WebXRCamera';
  }

  /**
   * Set the target for the camera to look at.
   * Note that this only rotates around the Y axis, as opposed to the default behavior of other cameras
   * @param target the target to set the camera to look at
   */
  public setTarget(target: BABYLON.Vector3): void {
    // only rotate around the y axis!
    const tmpVector = BABYLON.TmpVectors.Vector3[1];
    target.subtractToRef(this.position, tmpVector);
    tmpVector.y = 0;
    tmpVector.normalize();
    const yRotation = Math.atan2(tmpVector.x, tmpVector.z);
    this.rotationQuaternion.toEulerAnglesToRef(tmpVector);
    BABYLON.Quaternion.FromEulerAnglesToRef(tmpVector.x, yRotation, tmpVector.z, this.rotationQuaternion);
  }

  public dispose() {
    super.dispose();
    this._lastXRViewerPose = undefined;
  }

  private _updateFromXRSession() {
    const xrFrame = this._xrSessionManager.currentFrame;
    if (!xrFrame) {
      this._setTrackingState(BABYLON.WebXRTrackingState.NOT_TRACKING);
      return;
    }
    const pose = xrFrame.getViewerPose(this._xrSessionManager.referenceSpace);
    this._lastXRViewerPose = pose || undefined;
    if (!pose) {
      this._setTrackingState(BABYLON.WebXRTrackingState.NOT_TRACKING);
      return;
    }

    // Set the tracking state. if it didn't change it is a no-op
    const trackingState = pose.emulatedPosition ? BABYLON.WebXRTrackingState.TRACKING_LOST : BABYLON.WebXRTrackingState.TRACKING;
    this._setTrackingState(trackingState);

    // check min/max Z and update if not the same as in cache
    if (this.minZ !== this._cache.minZ || this.maxZ !== this._cache.maxZ) {
      const xrRenderState: XRRenderStateInit = {
        // if maxZ is 0 it should be "Infinity", but it doesn't work with the WebXR API. Setting to a large number.
        depthFar: this.maxZ || 1000,
        depthNear: this.minZ,
      };

      this._xrSessionManager.updateRenderState(xrRenderState);
      this._cache.minZ = this.minZ;
      this._cache.maxZ = this.maxZ;
    }

    if (pose.transform) {
      const orientation = pose.transform.orientation;
      if (pose.transform.orientation.x === undefined) {
        // Babylon native polyfill can return an undefined orientation value
        // When not initialized
        return;
      }
      const pos = pose.transform.position;
      this._referencedPosition.set(pos.x, pos.y, pos.z);
      this._referenceQuaternion.set(orientation.x, orientation.y, orientation.z, orientation.w);

      if (!this._scene.useRightHandedSystem) {
        this._referencedPosition.z *= -1;
        this._referenceQuaternion.z *= -1;
        this._referenceQuaternion.w *= -1;
      }
      this._globalPosition.copyFrom(this._referencedPosition);

      if (this._firstFrame) {
        this._firstFrame = false;
        // we have the XR reference, now use this to find the offset to get the camera to be
        // in the right position

        // set the height to correlate to the current height
        this.position.y += this._referencedPosition.y;
        // avoid using the head rotation on the first frame.
        this._referenceQuaternion.copyFromFloats(0, 0, 0, 1);
      } else {
        // update position and rotation as reference
        this.rotationQuaternion.copyFrom(this._referenceQuaternion);
        this.position.copyFrom(this._referencedPosition);
      }
    }

    if (this.rigCameras.length !== pose.views.length) {
      this._updateNumberOfRigCameras(pose.views.length);
    }

    pose.views.forEach((view: XRView, i: number) => {
      const currentRig = <BABYLON.TargetCamera>this.rigCameras[i];
      // update right and left, where applicable
      if (!currentRig.isLeftCamera && !currentRig.isRightCamera) {
        if (view.eye === 'right') {
          currentRig._isRightCamera = true;
        } else if (view.eye === 'left') {
          currentRig._isLeftCamera = true;
        }
      }

      // Update matrices: view and projection
      this._updateMatricesToRigCamera(view, currentRig);

      // Update fov
      const fov = Math.atan2(1, view.projectionMatrix[5]) * 2;
      currentRig.fov = fov;
      if (view.eye === 'left') {
        this.fov = fov;
        this._projectionMatrix.copyFrom(currentRig._projectionMatrix);
      }

      const renderTargetTexture = this._xrSessionManager.getRenderTargetTextureForView(view);
      this._renderingMultiview = renderTargetTexture?._texture?.isMultiview || false;
      if (this._renderingMultiview) {
        // For multiview, the render target texture is the same per-view (just the slice index is different),
        // so we only need to set the output render target once for the rig parent.
        if (i == 0) {
          this._xrSessionManager.trySetViewportForView(this.viewport, view);
          this.outputRenderTarget = renderTargetTexture;
        }
      } else {
        this._xrSessionManager.trySetViewportForView(currentRig.viewport, view);
        this.outputRenderTarget = renderTargetTexture || this._xrSessionManager.getRenderTargetTextureForView(view);
      }
      currentRig.layerMask = this.layerMask;
    });
  }

  private _updateMatricesToRigCamera(view: XRView, camera: BABYLON.TargetCamera) {
    camera.parent = this.parent;
    {
      const { position, orientation } = view.transform;
      camera.position
        .set(position.x, position.y, position.z)
        .scaleInPlace(1.0);
      camera.rotationQuaternion
        .set(orientation.x, orientation.y, orientation.z, orientation.w);

      if (!this._scene.useRightHandedSystem) {
        camera.position.z *= -1;
        camera.rotationQuaternion.z *= -1;
        camera.rotationQuaternion.w *= -1;
      } else {
        camera.rotationQuaternion.multiplyInPlace(new BABYLON.Quaternion(0, 1, 0, 0));
      }

      BABYLON.Matrix.FromFloat32ArrayToRefScaled(view.projectionMatrix, 0, 1, camera._projectionMatrix);
      if (!this._scene.useRightHandedSystem) {
        camera._projectionMatrix.toggleProjectionMatrixHandInPlace();
      }
    }
  }

  private _updateNumberOfRigCameras(viewCount = 1) {
    while (this.rigCameras.length < viewCount) {
      const newCamera = new BABYLON.TargetCamera('XR-RigCamera: ' + this.rigCameras.length, BABYLON.Vector3.Zero(), this.getScene());
      newCamera.minZ = 0.1;
      newCamera.rotationQuaternion = new BABYLON.Quaternion();
      newCamera.updateUpVectorFromRotation = true;
      newCamera.isRigCamera = true;
      newCamera.rigParent = this;
      // do not compute projection matrix, provided by XR
      newCamera.freezeProjectionMatrix();
      this.rigCameras.push(newCamera);
    }
    while (this.rigCameras.length > viewCount) {
      const removedCamera = this.rigCameras.pop();
      if (removedCamera) {
        removedCamera.dispose();
      }
    }
  }
}
