import { WebXRFeatureName, WebXRFeaturesManager } from '../FeatureManager';
import type { WebXRSessionManager } from '../SessionManager';
import { WebXRAbstractFeature } from './WebXRAbstractFeature';
import type { WebXRInput } from '../Input';
import type { WebXRInputSource } from '../InputSource';
import { WebXRCamera } from '../Camera';
import { WebXRAbstractMotionController, WebXRControllerComponent } from '../motionController';

type Scene = BABYLON.Scene;
type Node = BABYLON.Node;
type Mesh = BABYLON.Mesh;
type AbstractMesh = BABYLON.AbstractMesh;
type Color3 = BABYLON.Color3;
type Vector3 = BABYLON.Vector3;
type Ray = BABYLON.Ray;
type PickingInfo = BABYLON.PickingInfo;
type Nullable<T> = BABYLON.Nullable<T>;
type Observer<T> = BABYLON.Observer<T>;
const {
  Matrix, Vector3, Color3, Axis, StandardMaterial, Node,
  Viewport,
  Ray, PickingInfo, Tools, UtilityLayerRenderer,
  CreateCylinder, CreateTorus
} = BABYLON;

/**
 * Options interface for the pointer selection module
 */
export interface IWebXRControllerPointerSelectionOptions {
  /**
   * if provided, this scene will be used to render meshes.
   */
  customUtilityLayerScene?: Scene;
  /**
   * Disable the pointer up event when the xr controller in screen and gaze mode is disposed (meaning - when the user removed the finger from the screen)
   * If not disabled, the last picked point will be used to execute a pointer up event
   * If disabled, pointer up event will be triggered right after the pointer down event.
   * Used in screen and gaze target ray mode only
   */
  disablePointerUpOnTouchOut: boolean;
  /**
   * For gaze mode for tracked-pointer / controllers (time to select instead of button press)
   */
  forceGazeMode: boolean;
  /**
   * Factor to be applied to the pointer-moved function in the gaze mode. How sensitive should the gaze mode be when checking if the pointer moved
   * to start a new countdown to the pointer down event.
   * Defaults to 1.
   */
  gazeModePointerMovedFactor?: number;
  /**
   * Different button type to use instead of the main component
   */
  overrideButtonId?: string;
  /**
   *  use this rendering group id for the meshes (optional)
   */
  renderingGroupId?: number;
  /**
   * The amount of time in milliseconds it takes between pick found something to a pointer down event.
   * Used in gaze modes. Tracked pointer uses the trigger, screen uses touch events
   * 3000 means 3 seconds between pointing at something and selecting it
   */
  timeToSelect?: number;
  /**
   * Should meshes created here be added to a utility layer or the main scene
   */
  useUtilityLayer?: boolean;
  /**
   * Optional WebXR camera to be used for gaze selection
   */
  gazeCamera?: WebXRCamera;
  /**
   * the xr input to use with this pointer selection
   */
  xrInput: WebXRInput;

  /**
   * Should the scene pointerX and pointerY update be disabled
   * This is required for fullscreen AR GUI, but might slow down other experiences.
   * Disable in VR, if not needed.
   * The first rig camera (left eye) will be used to calculate the projection
   */
  disableScenePointerVectorUpdate: boolean;

  /**
   * Enable pointer selection on all controllers instead of switching between them
   */
  enablePointerSelectionOnAllControllers?: boolean;

  /**
   * If set, the gaze mode will be ignored and the controller will be used for pointer selection
   */
  ignoreGazeController?: boolean;

  /**
   * The preferred hand to give the pointer selection to. This will be prioritized when the controller initialize.
   * If switch is enabled, it will still allow the user to switch between the different controllers
   */
  preferredHandedness?: XRHandedness;

  /**
   * Disable switching the pointer selection from one controller to the other.
   * If the preferred hand is set it will be fixed on this hand, and if not it will be fixed on the first controller added to the scene
   */
  disableSwitchOnClick?: boolean;

  /**
   * The maximum distance of the pointer selection feature. Defaults to 100.
   */
  maxPointerDistance?: number;

  /**
   * A function that will be called when a new selection mesh is generated.
   * This function should return a mesh that will be used as the selection mesh.
   * The default is a torus with a 0.01 diameter and 0.0075 thickness .
   */
  customSelectionMeshGenerator?: () => Mesh;

  /**
   * A function that will be called when a new laser pointer mesh is generated.
   * This function should return a mesh that will be used as the laser pointer mesh.
   * The height (y) of the mesh must be 1.
   */
  customLasterPointerMeshGenerator?: () => AbstractMesh;
}

class WebXRControllerData {
  private static _IdCounter = 200;

  xrController?: WebXRInputSource;
  webXRCamera?: WebXRCamera;
  selectionComponent?: WebXRControllerComponent;
  onButtonChangedObserver?: Nullable<Observer<WebXRControllerComponent>>;
  onFrameObserver?: Nullable<Observer<XRFrame>>;
  laserPointer: AbstractMesh;
  selectionMesh: AbstractMesh;
  meshUnderPointer: Nullable<AbstractMesh>;
  pick: Nullable<PickingInfo>;
  id: number;
  tmpRay: Ray;
  disabledByNearInteraction: boolean;

  // event support
  eventListeners?: { [event in XREventType]?: (event: XRInputSourceEvent) => void };
  screenCoordinates?: { x: number; y: number };
  pointerDownTriggered?: boolean;
  finalPointerUpTriggered?: boolean;

  constructor(props: {
    xrController?: WebXRInputSource,
    webXRCamera?: WebXRCamera;
    laserPointer: AbstractMesh,
    selectionMesh: AbstractMesh,
    meshUnderPointer: Nullable<AbstractMesh>,
    pick: Nullable<PickingInfo>,
    tmpRay: Ray,
    disabledByNearInteraction: boolean,
  }) {
    this.id = WebXRControllerData._IdCounter++;
    this.xrController = props.xrController;
    this.webXRCamera = props.webXRCamera;
    this.laserPointer = props.laserPointer;
    this.selectionMesh = props.selectionMesh;
    this.meshUnderPointer = props.meshUnderPointer;
    this.pick = props.pick;
    this.tmpRay = props.tmpRay;
    this.disabledByNearInteraction = props.disabledByNearInteraction;
  }

  select(pickedMesh: AbstractMesh, pickedPoint: Vector3, normal: Vector3) {
    this.meshUnderPointer = pickedMesh;

    // const cursorMesh = this.selectionMesh;
  }

  deselect() {
    this.meshUnderPointer = null;
    this.laserPointer.isVisible = false;
    this.selectionMesh.isVisible = false;
  }

  reset() {
    this.deselect();
    this.pick = null;
  }
}

/**
 * A module that will enable pointer selection for motion controllers of XR Input Sources
 */
export class WebXRControllerPointerSelection extends WebXRAbstractFeature {
  private _attachController = (xrController: WebXRInputSource) => {
    if (this._controllers[xrController.uniqueId]) {
      // already attached
      return;
    }
    /**
     * Skip the gaze controller if needed
     */
    if (this._options.ignoreGazeController && xrController.inputSource.targetRayMode === 'gaze') {
      return;
    }

    const { laserPointer, selectionMesh } = this._generateNewMeshPair(xrController.pointer);

    // get two new meshes
    this._controllers[xrController.uniqueId] = new WebXRControllerData({
      xrController,
      laserPointer,
      selectionMesh,
      meshUnderPointer: null,
      pick: null,
      tmpRay: new Ray(new Vector3(), new Vector3()),
      disabledByNearInteraction: false,
    });

    if (this._attachedController) {
      if (
        !this._options.enablePointerSelectionOnAllControllers &&
        this._options.preferredHandedness &&
        xrController.inputSource.handedness === this._options.preferredHandedness
      ) {
        this._attachedController = xrController.uniqueId;
      }
    } else {
      if (!this._options.enablePointerSelectionOnAllControllers) {
        this._attachedController = xrController.uniqueId;
      }
    }

    switch (xrController.inputSource.targetRayMode) {
      case 'tracked-pointer':
        return this._attachTrackedPointerRayMode(xrController);
      case 'gaze':
        return this._attachGazeMode(xrController);
      case 'screen':
        return this._attachScreenRayMode(xrController);
    }
  };

  private _controllers: {
    [controllerUniqueId: string]: WebXRControllerData;
  } = {};
  private _scene: Scene;
  private _tmpVectorForPickCompare = new Vector3();

  private _attachedController: string;

  /**
   * The module's name
   */
  public static readonly Name = WebXRFeatureName.POINTER_SELECTION;
  /**
   * The (Babylon) version of this module.
   * This is an integer representing the implementation version.
   * This number does not correspond to the WebXR specs version
   */
  public static readonly Version = 1;

  /**
   * Disable lighting on the laser pointer (so it will always be visible)
   */
  public disablePointerLighting: boolean = true;
  /**
   * Disable lighting on the selection mesh (so it will always be visible)
   */
  public disableSelectionMeshLighting: boolean = true;
  /**
   * Should the laser pointer be displayed
   */
  public displayLaserPointer: boolean = false;
  /**
   * Should the selection mesh be displayed (The ring at the end of the laser pointer)
   */
  public displaySelectionMesh: boolean = true;
  /**
   * This color will be set to the laser pointer when selection is triggered
   */
  public laserPointerPickedColor: Color3 = new Color3(0.9, 0.9, 0.9);
  /**
   * Default color of the laser pointer
   */
  public laserPointerDefaultColor: Color3 = new Color3(0.7, 0.7, 0.7);
  /**
   * default color of the selection ring
   */
  public selectionMeshDefaultColor: Color3 = new Color3(0.8, 0.8, 0.8);
  /**
   * This color will be applied to the selection ring when selection is triggered
   */
  public selectionMeshPickedColor: Color3 = new Color3(0.3, 0.3, 1.0);

  /**
   * Optional filter to be used for ray selection.  This predicate shares behavior with
   * scene.pointerMovePredicate which takes priority if it is also assigned.
   */
  public raySelectionPredicate: (mesh: AbstractMesh) => boolean;

  /**
   * constructs a new background remover module
   * @param _xrSessionManager the session manager for this module
   * @param _options read-only options to be used in this module
   */
  constructor(_xrSessionManager: WebXRSessionManager, private readonly _options: IWebXRControllerPointerSelectionOptions) {
    super(_xrSessionManager);
    this._scene = this._xrSessionManager.scene;
  }

  /**
   * attach this feature
   * Will usually be called by the features manager
   *
   * @returns true if successful.
   */
  public attach(): boolean {
    if (!super.attach()) {
      return false;
    }

    this._options.xrInput.controllers
      .forEach(this._attachController);
    this._addNewAttachObserver(this._options.xrInput.onControllerAddedObservable, this._attachController);
    this._addNewAttachObserver(this._options.xrInput.onControllerRemovedObservable, (controller) => {
      // REMOVE the controller
      this._detachController(controller.uniqueId);
    });

    this._scene.constantlyUpdateMeshUnderPointer = true;

    if (this._options.gazeCamera) {
      const webXRCamera = this._options.gazeCamera;
      const { laserPointer, selectionMesh } = this._generateNewMeshPair(webXRCamera);
      this._controllers['camera'] = new WebXRControllerData({
        webXRCamera,
        laserPointer,
        selectionMesh,
        meshUnderPointer: null,
        pick: null,
        tmpRay: new Ray(new Vector3(), new Vector3()),
        disabledByNearInteraction: false,
      });
      this._attachGazeMode();
    }
    return true;
  }

  /**
   * detach this feature.
   * Will usually be called by the features manager
   *
   * @returns true if successful.
   */
  public detach(): boolean {
    if (!super.detach()) {
      return false;
    }

    Object.keys(this._controllers).forEach((controllerId) => {
      this._detachController(controllerId);
    });

    return true;
  }

  /**
   * Will get the mesh under a specific pointer.
   * `scene.meshUnderPointer` will only return one mesh - either left or right.
   * @param controllerId the controllerId to check
   * @returns The mesh under pointer or null if no mesh is under the pointer
   */
  public getMeshUnderPointer(controllerId: string): Nullable<AbstractMesh> {
    if (this._controllers[controllerId]) {
      return this._controllers[controllerId].meshUnderPointer;
    } else {
      return null;
    }
  }

  /**
   * Get the xr controller that correlates to the pointer id in the pointer event
   *
   * @param id the pointer id to search for
   * @returns the controller that correlates to this id or null if not found
   */
  public getXRControllerByPointerId(id: number): Nullable<WebXRInputSource> {
    const keys = Object.keys(this._controllers);

    for (let i = 0; i < keys.length; ++i) {
      if (this._controllers[keys[i]].id === id) {
        return this._controllers[keys[i]].xrController || null;
      }
    }
    return null;
  }

  /**
   * @internal
   */
  public _getPointerSelectionDisabledByPointerId(id: number): boolean {
    const keys = Object.keys(this._controllers);

    for (let i = 0; i < keys.length; ++i) {
      if (this._controllers[keys[i]].id === id) {
        return this._controllers[keys[i]].disabledByNearInteraction;
      }
    }
    return true;
  }

  /**
   * @internal
   */
  public _setPointerSelectionDisabledByPointerId(id: number, state: boolean) {
    const keys = Object.keys(this._controllers);

    for (let i = 0; i < keys.length; ++i) {
      if (this._controllers[keys[i]].id === id) {
        this._controllers[keys[i]].disabledByNearInteraction = state;
        return;
      }
    }
  }

  private _identityMatrix = Matrix.Identity();
  private _screenCoordinatesRef = Vector3.Zero();
  private _viewportRef = new Viewport(0, 0, 0, 0);

  protected _onXRFrame(_xrFrame: XRFrame) {
    this._xrSessionManager.runWithXRFrameOnce(_xrFrame, () => {
      Object.keys(this._controllers).forEach((id) => {
        // only do this for the selected pointer
        const controllerData = this._controllers[id];
        if ((!this._options.enablePointerSelectionOnAllControllers && id !== this._attachedController) || controllerData.disabledByNearInteraction) {
          controllerData.reset();
          return;
        }

        controllerData.laserPointer.isVisible = this.displayLaserPointer;
        let controllerGlobalPosition: Vector3;

        // Every frame check collisions/input
        if (controllerData.xrController) {
          controllerGlobalPosition = controllerData.xrController.pointer.position;
          controllerData.xrController.getWorldPointerRayToRef(controllerData.tmpRay);
        } else if (controllerData.webXRCamera) {
          controllerGlobalPosition = controllerData.webXRCamera.position;
          controllerData.webXRCamera.getForwardRayToRef(controllerData.tmpRay);
        } else {
          console.warn('no controller or camera found in', this._attachedController);
          return;
        }

        if (this._options.maxPointerDistance) {
          controllerData.tmpRay.length = this._options.maxPointerDistance;
        }
        // update pointerX and pointerY of the scene. Only if the flag is set to true!
        if (!this._options.disableScenePointerVectorUpdate && controllerGlobalPosition) {
          const scene = this._xrSessionManager.scene;
          const camera = this._options.xrInput.xrCamera;
          if (camera) {
            camera.viewport.toGlobalToRef(scene.getEngine().getRenderWidth(), scene.getEngine().getRenderHeight(), this._viewportRef);
            Vector3.ProjectToRef(controllerGlobalPosition, this._identityMatrix, scene.getTransformMatrix(), this._viewportRef, this._screenCoordinatesRef);

            // stay safe
            if (
              typeof this._screenCoordinatesRef.x === 'number' &&
              typeof this._screenCoordinatesRef.y === 'number' &&
              !isNaN(this._screenCoordinatesRef.x) &&
              !isNaN(this._screenCoordinatesRef.y)
            ) {
              scene.pointerX = this._screenCoordinatesRef.x;
              scene.pointerY = this._screenCoordinatesRef.y;

              controllerData.screenCoordinates = {
                x: this._screenCoordinatesRef.x,
                y: this._screenCoordinatesRef.y,
              };
            }
          }
        }

        let utilityScenePick = null;
        if (this._utilityLayerScene) {
          utilityScenePick = this._utilityLayerScene.pickWithRay(controllerData.tmpRay, this._utilityLayerScene.pointerMovePredicate || this.raySelectionPredicate);
        }

        const originalScenePick = this._scene.pickWithRay(controllerData.tmpRay, this._scene.pointerMovePredicate || this.raySelectionPredicate);
        if (!utilityScenePick || !utilityScenePick.hit) {
          // No hit in utility scene
          controllerData.pick = originalScenePick;
        } else if (!originalScenePick || !originalScenePick.hit) {
          // No hit in original scene
          controllerData.pick = utilityScenePick;
        } else if (utilityScenePick.distance < originalScenePick.distance) {
          // Hit is closer in utility scene
          controllerData.pick = utilityScenePick;
        } else {
          // Hit is closer in original scene
          controllerData.pick = originalScenePick;
        }

        if (controllerData.pick && controllerData.xrController) {
          controllerData.pick.aimTransform = controllerData.xrController.pointer;
          controllerData.pick.gripTransform = controllerData.xrController.grip || null;
        }

        const pick = controllerData.pick;
        if (pick && pick.pickedPoint && pick.hit) {
          // Update laser state
          this._updatePointerDistance(controllerData.laserPointer, pick.distance);

          // Notify observers
          this._xrSessionManager.onMeshPickObservable.notifyObservers({
            mesh: pick.pickedMesh,
            pick,
          });

          // Update cursor state
          controllerData.selectionMesh.position.copyFrom(pick.pickedPoint);
          controllerData.selectionMesh.scaling.x = Math.sqrt(pick.distance);
          controllerData.selectionMesh.scaling.y = Math.sqrt(pick.distance);
          controllerData.selectionMesh.scaling.z = Math.sqrt(pick.distance);

          // To avoid z-fighting
          const pickNormal = this._convertNormalToDirectionOfRay(pick.getNormal(true), controllerData.tmpRay);
          const deltaFighting = 0.001;
          controllerData.selectionMesh.position.copyFrom(pick.pickedPoint);
          if (pickNormal) {
            const axis1 = Vector3.Cross(Axis.Y, pickNormal);
            const axis2 = Vector3.Cross(pickNormal, axis1);
            Vector3.RotationFromAxisToRef(axis2, pickNormal, axis1, controllerData.selectionMesh.rotation);
            controllerData.selectionMesh.position.addInPlace(pickNormal.scale(deltaFighting));
          }
          controllerData.select(pick.pickedMesh, pick.pickedPoint, pickNormal);
        } else {
          this._xrSessionManager.onMeshUnpickObservable.notifyObservers({
            mesh: controllerData.meshUnderPointer,
          });
          controllerData.deselect();
          this._updatePointerDistance(controllerData.laserPointer, 1);
        }
      });
    });
  }

  private get _utilityLayerScene() {
    return this._options.customUtilityLayerScene || UtilityLayerRenderer.DefaultUtilityLayer.utilityLayerScene;
  }

  private _attachGazeMode(_xrController?: WebXRInputSource) {
    // TODO
  }

  private _attachScreenRayMode(xrController: WebXRInputSource) {
    const controllerData = this._controllers[xrController.uniqueId];
    const pointerEventInit: PointerEventInit = {
      pointerId: controllerData.id,
      pointerType: 'xr',
    };
    controllerData.onFrameObserver = this._xrSessionManager.onXRFrameObservable.add(() => {
      (<BABYLON.StandardMaterial>controllerData.laserPointer.material).disableLighting = this.disablePointerLighting;
      (<BABYLON.StandardMaterial>controllerData.selectionMesh.material).disableLighting = this.disableSelectionMeshLighting;

      if (controllerData.pick) {
        this._augmentPointerInit(pointerEventInit, controllerData.id, controllerData.screenCoordinates);
        this._scene.simulatePointerMove(controllerData.pick, pointerEventInit);
      }
    });

    // use the select and squeeze events
    const selectStartListener = (event: XRInputSourceEvent) => {
      this._augmentPointerInit(pointerEventInit, controllerData.id, controllerData.screenCoordinates);
      if (controllerData.xrController && event.inputSource === controllerData.xrController.inputSource && controllerData.pick) {
        this._scene.simulatePointerDown(controllerData.pick, pointerEventInit);
        controllerData.pointerDownTriggered = true;
        (<BABYLON.StandardMaterial>controllerData.selectionMesh.material).emissiveColor = this.selectionMeshPickedColor;
        (<BABYLON.StandardMaterial>controllerData.laserPointer.material).emissiveColor = this.laserPointerPickedColor;
      }
    };

    const selectEndListener = (event: XRInputSourceEvent) => {
      this._augmentPointerInit(pointerEventInit, controllerData.id, controllerData.screenCoordinates);
      if (controllerData.xrController && event.inputSource === controllerData.xrController.inputSource && controllerData.pick) {
        this._scene.simulatePointerUp(controllerData.pick, pointerEventInit);
        (<BABYLON.StandardMaterial>controllerData.selectionMesh.material).emissiveColor = this.selectionMeshDefaultColor;
        (<BABYLON.StandardMaterial>controllerData.laserPointer.material).emissiveColor = this.laserPointerDefaultColor;
      }
    };

    controllerData.eventListeners = {
      selectend: selectEndListener,
      selectstart: selectStartListener,
    };

    this._xrSessionManager.session.addEventListener('selectstart', selectStartListener);
    this._xrSessionManager.session.addEventListener('selectend', selectEndListener);

    // let downTriggered = false;
    // const pointerEventInit: PointerEventInit = {
    //   pointerId: controllerData.id,
    //   pointerType: 'xr',
    // };
    // controllerData.onFrameObserver = this._xrSessionManager.onXRFrameObservable.add(() => {
    //   this._augmentPointerInit(pointerEventInit, controllerData.id, controllerData.screenCoordinates);
    //   if (!controllerData.pick || (this._options.disablePointerUpOnTouchOut && downTriggered)) {
    //     return;
    //   }
    //   if (!downTriggered) {
    //     this._scene.simulatePointerDown(controllerData.pick, pointerEventInit);
    //     controllerData.pointerDownTriggered = true;
    //     downTriggered = true;
    //     if (this._options.disablePointerUpOnTouchOut) {
    //       this._scene.simulatePointerUp(controllerData.pick, pointerEventInit);
    //     }
    //   } else {
    //     this._scene.simulatePointerMove(controllerData.pick, pointerEventInit);
    //   }
    // });
    // xrController.onDisposeObservable.addOnce(() => {
    //   this._augmentPointerInit(pointerEventInit, controllerData.id, controllerData.screenCoordinates);
    //   this._xrSessionManager.runInXRFrame(() => {
    //     if (controllerData.pick && !controllerData.finalPointerUpTriggered && downTriggered && !this._options.disablePointerUpOnTouchOut) {
    //       this._scene.simulatePointerUp(controllerData.pick, pointerEventInit);
    //       controllerData.finalPointerUpTriggered = true;
    //     }
    //   });
    // });
  }

  private _attachTrackedPointerRayMode(xrController: WebXRInputSource) {
    const controllerData = this._controllers[xrController.uniqueId];
    if (this._options.forceGazeMode) {
      return this._attachGazeMode(xrController);
    }
    const pointerEventInit: PointerEventInit = {
      pointerId: controllerData.id,
      pointerType: 'xr',
    };
    controllerData.onFrameObserver = this._xrSessionManager.onXRFrameObservable.add(() => {
      (<BABYLON.StandardMaterial>controllerData.laserPointer.material).disableLighting = this.disablePointerLighting;
      (<BABYLON.StandardMaterial>controllerData.selectionMesh.material).disableLighting = this.disableSelectionMeshLighting;

      if (controllerData.pick) {
        this._augmentPointerInit(pointerEventInit, controllerData.id, controllerData.screenCoordinates);
        this._scene.simulatePointerMove(controllerData.pick, pointerEventInit);
      }
    });
    if (xrController.inputSource.gamepad) {
      const init = (motionController: WebXRAbstractMotionController) => {
        if (this._options.overrideButtonId) {
          controllerData.selectionComponent = motionController.getComponent(this._options.overrideButtonId);
        }
        if (!controllerData.selectionComponent) {
          controllerData.selectionComponent = motionController.getMainComponent();
        }

        controllerData.onButtonChangedObserver = controllerData.selectionComponent.onButtonStateChangedObservable.add((component) => {
          if (component.changes.pressed) {
            const pressed = component.changes.pressed.current;
            if (controllerData.pick) {
              if (this._options.enablePointerSelectionOnAllControllers || xrController.uniqueId === this._attachedController) {
                this._augmentPointerInit(pointerEventInit, controllerData.id, controllerData.screenCoordinates);
                if (pressed) {
                  this._scene.simulatePointerDown(controllerData.pick, pointerEventInit);
                  controllerData.pointerDownTriggered = true;
                  (<BABYLON.StandardMaterial>controllerData.selectionMesh.material).emissiveColor = this.selectionMeshPickedColor;
                  (<BABYLON.StandardMaterial>controllerData.laserPointer.material).emissiveColor = this.laserPointerPickedColor;
                } else {
                  this._scene.simulatePointerUp(controllerData.pick, pointerEventInit);
                  (<BABYLON.StandardMaterial>controllerData.selectionMesh.material).emissiveColor = this.selectionMeshDefaultColor;
                  (<BABYLON.StandardMaterial>controllerData.laserPointer.material).emissiveColor = this.laserPointerDefaultColor;
                }
              }
            } else {
              if (pressed && !this._options.enablePointerSelectionOnAllControllers && !this._options.disableSwitchOnClick) {
                this._attachedController = xrController.uniqueId;
              }
            }
          }
        });
      };
      if (xrController.motionController) {
        init(xrController.motionController);
      } else {
        xrController.onMotionControllerInitObservable.add(init);
      }
    } else {
      // use the select and squeeze events
      const selectStartListener = (event: XRInputSourceEvent) => {
        this._augmentPointerInit(pointerEventInit, controllerData.id, controllerData.screenCoordinates);
        if (controllerData.xrController && event.inputSource === controllerData.xrController.inputSource && controllerData.pick) {
          this._scene.simulatePointerDown(controllerData.pick, pointerEventInit);
          controllerData.pointerDownTriggered = true;
          (<BABYLON.StandardMaterial>controllerData.selectionMesh.material).emissiveColor = this.selectionMeshPickedColor;
          (<BABYLON.StandardMaterial>controllerData.laserPointer.material).emissiveColor = this.laserPointerPickedColor;
        }
      };

      const selectEndListener = (event: XRInputSourceEvent) => {
        this._augmentPointerInit(pointerEventInit, controllerData.id, controllerData.screenCoordinates);
        if (controllerData.xrController && event.inputSource === controllerData.xrController.inputSource && controllerData.pick) {
          this._scene.simulatePointerUp(controllerData.pick, pointerEventInit);
          (<BABYLON.StandardMaterial>controllerData.selectionMesh.material).emissiveColor = this.selectionMeshDefaultColor;
          (<BABYLON.StandardMaterial>controllerData.laserPointer.material).emissiveColor = this.laserPointerDefaultColor;
        }
      };

      controllerData.eventListeners = {
        selectend: selectEndListener,
        selectstart: selectStartListener,
      };

      this._xrSessionManager.session.addEventListener('selectstart', selectStartListener);
      this._xrSessionManager.session.addEventListener('selectend', selectEndListener);
    }
  }

  private _convertNormalToDirectionOfRay(normal: Nullable<Vector3>, ray: Ray) {
    if (normal) {
      const angle = Math.acos(Vector3.Dot(normal, ray.direction));
      if (angle < Math.PI / 2) {
        normal.scaleInPlace(-1);
      }
    }
    return normal;
  }

  private _detachController(xrControllerUniqueId: string) {
    const controllerData = this._controllers[xrControllerUniqueId];
    if (!controllerData) {
      return;
    }
    if (controllerData.selectionComponent) {
      if (controllerData.onButtonChangedObserver) {
        controllerData.selectionComponent.onButtonStateChangedObservable.remove(controllerData.onButtonChangedObserver);
      }
    }
    if (controllerData.onFrameObserver) {
      this._xrSessionManager.onXRFrameObservable.remove(controllerData.onFrameObserver);
    }
    if (controllerData.eventListeners) {
      Object.keys(controllerData.eventListeners).forEach((eventName: string) => {
        const func = controllerData.eventListeners && controllerData.eventListeners[eventName as XREventType];
        if (func) {
          // For future reference - this is an issue in the WebXR typings.
          this._xrSessionManager.session.removeEventListener(eventName as XREventType, func as any);
        }
      });
    }

    if (!controllerData.finalPointerUpTriggered && controllerData.pointerDownTriggered) {
      // Stay safe and fire a pointerup, in case it wasn't already triggered
      const pointerEventInit: PointerEventInit = {
        pointerId: controllerData.id,
        pointerType: 'xr',
      };
      this._xrSessionManager.runInXRFrame(() => {
        this._augmentPointerInit(pointerEventInit, controllerData.id, controllerData.screenCoordinates);
        this._scene.simulatePointerUp(controllerData.pick || new PickingInfo(), pointerEventInit);
        controllerData.finalPointerUpTriggered = true;
      });
    }
    this._xrSessionManager.scene.onBeforeRenderObservable.addOnce(() => {
      try {
        controllerData.selectionMesh.dispose();
        controllerData.laserPointer.dispose();
        // remove from the map
        delete this._controllers[xrControllerUniqueId];
        if (this._attachedController === xrControllerUniqueId) {
          // check for other controllers
          const keys = Object.keys(this._controllers);
          if (keys.length) {
            this._attachedController = keys[0];
          } else {
            this._attachedController = '';
          }
        }
      } catch (e) {
        Tools.Warn('controller already detached.');
      }
    });
  }

  private _generateNewMeshPair(meshParent: Node) {
    const sceneToRenderTo = this._options.useUtilityLayer ?
      this._options.customUtilityLayerScene || UtilityLayerRenderer.DefaultUtilityLayer.utilityLayerScene :
      this._scene;
    const laserPointer = this._options.customLasterPointerMeshGenerator
      ? this._options.customLasterPointerMeshGenerator()
      : CreateCylinder(
        'laserPointer',
        {
          height: 1,
          diameterTop: 0.0020,
          diameterBottom: 0.010,
          tessellation: 20,
          subdivisions: 1,
        },
        sceneToRenderTo
      );
    laserPointer.parent = meshParent;
    const laserPointerMaterial = new StandardMaterial('laserPointerMat', sceneToRenderTo);
    laserPointerMaterial.emissiveColor = this.laserPointerDefaultColor;
    laserPointerMaterial.alpha = 0.7;
    laserPointer.material = laserPointerMaterial;
    laserPointer.rotation.x = Math.PI / 2;
    this._updatePointerDistance(laserPointer, 1);
    laserPointer.isPickable = false;
    laserPointer.isVisible = false;

    // Create a gaze tracker for the XR controller
    const selectionMesh = this._options.customSelectionMeshGenerator
      ? this._options.customSelectionMeshGenerator()
      : CreateTorus(
        'gazeTracker',
        {
          diameter: 0.0055 * 3,
          thickness: 0.0025 * 3,
          tessellation: 20,
        },
        sceneToRenderTo
      );

    selectionMesh.parent = null;
    selectionMesh.bakeCurrentTransformIntoVertices();
    selectionMesh.isPickable = false;
    selectionMesh.isVisible = false;
    const targetMat = new StandardMaterial('targetMat', sceneToRenderTo);
    targetMat.specularColor = Color3.Black();
    targetMat.emissiveColor = this.selectionMeshDefaultColor;
    targetMat.backFaceCulling = false;
    targetMat.alpha = 0.7;
    selectionMesh.material = targetMat;

    if (this._options.renderingGroupId !== undefined) {
      laserPointer.renderingGroupId = this._options.renderingGroupId;
      selectionMesh.renderingGroupId = this._options.renderingGroupId;
    }

    return {
      laserPointer,
      selectionMesh,
    };
  }

  private _updatePointerDistance(_laserPointer: AbstractMesh, distance: number = 100) {
    if (!_laserPointer.isVisible) {
      return;
    }
    _laserPointer.scaling.y = distance;
    // a bit of distance from the controller
    if (this._scene.useRightHandedSystem) {
      distance *= -1;
    }
    _laserPointer.position.z = distance / 2 + 0.05;
  }

  private _augmentPointerInit(pointerEventInit: PointerEventInit, id: number, screenCoordinates?: { x: number; y: number }): void {
    pointerEventInit.pointerId = id;
    pointerEventInit.pointerType = 'xr';
    if (screenCoordinates) {
      pointerEventInit.screenX = screenCoordinates.x;
      pointerEventInit.screenY = screenCoordinates.y;
    }
  }

  /** @internal */
  public get lasterPointerDefaultColor(): Color3 {
    // here due to a typo
    return this.laserPointerDefaultColor;
  }
}

//register the plugin
WebXRFeaturesManager.AddWebXRFeature(
  WebXRControllerPointerSelection.Name,
  (xrSessionManager, options) => {
    return () => new WebXRControllerPointerSelection(xrSessionManager, options);
  },
  WebXRControllerPointerSelection.Version,
  true
);
