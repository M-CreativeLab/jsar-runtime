import './features/index';
import { WebXRExperienceHelper } from './ExperienceHelper';
import { WebXRInput } from './Input';
import { WebXRManagedOutputCanvasOptions } from './OutputCanvas';

/**
 * Options for the default xr helper
 */
export class WebXRDefaultExperienceOptions {
  /**
   * Enable or disable default UI to enter XR
   */
  public disableDefaultUI?: boolean;
  /**
   * Should pointer selection not initialize.
   * Note that disabling pointer selection also disables teleportation.
   * Defaults to false.
   */
  public disablePointerSelection?: boolean;
  /**
   * Should teleportation not initialize. Defaults to false.
   */
  public disableTeleportation?: boolean;
  /**
   * Should nearInteraction not initialize. Defaults to false.
   */
  public disableNearInteraction?: boolean;
  /**
   * Floor meshes that will be used for teleport
   */
  public floorMeshes?: Array<BABYLON.AbstractMesh>;
  /**
   * If set to true, the first frame will not be used to reset position
   * The first frame is mainly used when copying transformation from the old camera
   * Mainly used in AR
   */
  public ignoreNativeCameraTransformation?: boolean;
  /**
   * Optional configuration for the XR input object
   */
  public inputOptions?: Partial<BABYLON.IWebXRInputOptions>;
  /**
   * optional configuration for pointer selection
   */
  public pointerSelectionOptions?: Partial<BABYLON.IWebXRControllerPointerSelectionOptions>;
  /**
   * optional configuration for near interaction
   */
  public nearInteractionOptions?: Partial<BABYLON.IWebXRNearInteractionOptions>;
  /**
   * optional configuration for teleportation
   */
  public teleportationOptions?: Partial<BABYLON.IWebXRTeleportationOptions>;
  /**
   * optional configuration for the output canvas
   */
  // public outputCanvasOptions?: WebXRManagedOutputCanvasOptions;
  /**
   * optional UI options. This can be used among other to change session mode and reference space type
   */
  public uiOptions?: Partial<BABYLON.WebXREnterExitUIOptions>;
  /**
   * When loading teleportation and pointer select, use stable versions instead of latest.
   */
  public useStablePlugins?: boolean;

  /**
   * An optional rendering group id that will be set globally for teleportation, pointer selection and default controller meshes
   */
  public renderingGroupId?: number;

  /**
   * A list of optional features to init the session with
   * If set to true, all features we support will be added
   */
  public optionalFeatures?: boolean | string[];
}

export class WebXRDefaultExperience {
  /**
   * Base experience
   */
  public baseExperience: WebXRExperienceHelper;
  /**
     * Enables ui for entering/exiting xr
     */
  // public enterExitUI: WebXREnterExitUI;
  /**
   * Input experience extension
   */
  public input: WebXRInput;
  /**
   * Enables laser pointer and selection
   */
  // public pointerSelection: WebXRControllerPointerSelection;
  /**
   * Default target xr should render to
   */
  public renderTarget: BABYLON.WebXRRenderTarget;
  /**
   * Enables teleportation
   */
  // public teleportation: WebXRMotionControllerTeleportation;
  /**
   * Enables near interaction for hands/controllers
   */
  // public nearInteraction: WebXRNearInteraction;

  public static async CreateAsync(
    scene: BABYLON.Scene,
    options: {
      xrSystem: XRSystem;
      ignoreNativeCameraTransformation?: boolean;
      renderingGroupId?: number;
      inputOptions?: any;
      outputCanvasOptions: WebXRManagedOutputCanvasOptions;
    }
  ): Promise<WebXRDefaultExperience> {
    const result = new WebXRDefaultExperience();
    scene.onDisposeObservable.addOnce(() => {
      result.dispose();
    });

    const xrHelper = await WebXRExperienceHelper.CreateAsync(scene, options.xrSystem);
    result.baseExperience = xrHelper;

    if (options.ignoreNativeCameraTransformation) {
      result.baseExperience.camera.compensateOnFirstFrame = false;
    }
    // Add controller support
    result.input = new WebXRInput(xrHelper.sessionManager, xrHelper.camera, {
      controllerOptions: {
        doNotLoadControllerMesh: true,
        renderingGroupId: options.renderingGroupId,
      },
      ...(options.inputOptions || {}),
    });
    // Create the WebXR output target
    result.renderTarget = result.baseExperience.sessionManager.getWebXRRenderTarget(options.outputCanvasOptions);
    return result;
  }

  dispose() {
  }
}
