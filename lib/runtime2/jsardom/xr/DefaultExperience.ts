import { type NativeDocument as JSARNativeDocument } from '@yodaos-jsar/dom';
import { WebXRExperienceHelper } from './ExperienceHelper';
import { WebXRInput } from './Input';
import { WebXRManagedOutputCanvasOptions } from './OutputCanvas';
import {
  IWebXRControllerPointerSelectionOptions,
  IWebXRTeleportationOptions,
  IWebXRNearInteractionOptions,
  WebXRControllerPointerSelection,
  WebXRMotionControllerTeleportation,
  WebXRNearInteraction,
} from './features';

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
  public pointerSelectionOptions?: Partial<IWebXRControllerPointerSelectionOptions>;
  /**
   * optional configuration for near interaction
   */
  public nearInteractionOptions?: Partial<IWebXRNearInteractionOptions>;
  /**
   * optional configuration for teleportation
   */
  public teleportationOptions?: Partial<IWebXRTeleportationOptions>;
  /**
   * optional configuration for the output canvas
   */
  public outputCanvasOptions?: WebXRManagedOutputCanvasOptions;
  /**
   * optional UI options. This can be used among other to change session mode and reference space type
   */
  // public uiOptions?: Partial<BABYLON.WebXREnterExitUIOptions>;
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
  public pointerSelection: WebXRControllerPointerSelection;
  /**
   * Default target xr should render to
   */
  public renderTarget: BABYLON.WebXRRenderTarget;
  /**
   * Enables teleportation
   */
  public teleportation: WebXRMotionControllerTeleportation;
  /**
   * Enables near interaction for hands/controllers
   */
  public nearInteraction: WebXRNearInteraction;

  public static async CreateAsync(
    nativeDocument: JSARNativeDocument,
    options: WebXRDefaultExperienceOptions & { xrSystem: XRSystem }
  ): Promise<WebXRDefaultExperience> {
    const scene = nativeDocument.getNativeScene();
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

    if (!options.disablePointerSelection) {
      // Add default pointer selection
      const pointerSelectionOptions = {
        ...options.pointerSelectionOptions,
        xrInput: result.input,
        renderingGroupId: options.renderingGroupId,
      };

      result.pointerSelection = <WebXRControllerPointerSelection>(
        result.baseExperience.featuresManager.enableFeature(
          WebXRControllerPointerSelection.Name,
          options.useStablePlugins ? 'stable' : 'latest',
          <IWebXRControllerPointerSelectionOptions>pointerSelectionOptions
        )
      );
      result.pointerSelection.displayLaserPointer = false;

      if (!options.disableTeleportation) {
        // Add default teleportation, including rotation
        result.teleportation = <WebXRMotionControllerTeleportation>result.baseExperience.featuresManager.enableFeature(
          WebXRMotionControllerTeleportation.Name,
          options.useStablePlugins ? 'stable' : 'latest',
          <IWebXRTeleportationOptions>{
            floorMeshes: options.floorMeshes,
            xrInput: result.input,
            renderingGroupId: options.renderingGroupId,
            ...options.teleportationOptions,
          }
        );
        result.teleportation.setSelectionFeature(result.pointerSelection);
      }
    }

    if (!options.disableNearInteraction) {
      // Add default pointer selection
      result.nearInteraction = <WebXRNearInteraction>result.baseExperience.featuresManager.enableFeature(
        WebXRNearInteraction.Name,
        options.useStablePlugins ? 'stable' : 'latest',
        <IWebXRNearInteractionOptions>{
          xrInput: result.input,
          farInteractionFeature: result.pointerSelection,
          renderingGroupId: options.renderingGroupId,
          useUtilityLayer: true,
          enableNearInteractionOnAllControllers: true,
          ...options.nearInteractionOptions,
        }
      );
    }

    // Create the WebXR output target
    result.renderTarget = result.baseExperience.sessionManager.getWebXRRenderTarget(options.outputCanvasOptions);
    return result;
  }

  dispose() {
    // TODO: dispose all the things
  }
}
