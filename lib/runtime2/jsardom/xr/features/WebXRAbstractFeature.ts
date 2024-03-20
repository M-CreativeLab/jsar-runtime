import type { IWebXRFeature } from '../FeatureManager';
import type { WebXRSessionManager } from '../SessionManager';

/**
 * This is the base class for all WebXR features.
 * Since most features require almost the same resources and callbacks, this class can be used to simplify the development
 * Note that since the features manager is using the `IWebXRFeature` you are in no way obligated to use this class
 */
export abstract class WebXRAbstractFeature implements IWebXRFeature {
  private _attached: boolean = false;
  private _removeOnDetach: {
    observer: BABYLON.Nullable<BABYLON.Observer<any>>;
    observable: BABYLON.Observable<any>;
  }[] = [];

  /**
   * Is this feature disposed?
   */
  public isDisposed: boolean = false;

  /**
   * Should auto-attach be disabled?
   */
  public disableAutoAttach: boolean = false;

  protected _xrNativeFeatureName: string = "";

  /**
   * The name of the native xr feature name (like anchor, hit-test, or hand-tracking)
   */
  public get xrNativeFeatureName() {
    return this._xrNativeFeatureName;
  }

  public set xrNativeFeatureName(name: string) {
    // check if feature was initialized while in session but needs to be initialized before the session starts
    if (name && this._xrSessionManager.inXRSession && this._xrSessionManager.session.enabledFeatures.indexOf(name) === -1) {
      BABYLON.Logger.Warn(`The feature ${name} needs to be enabled before starting the XR session. Note - It is still possible it is not supported.`);
    }
    this._xrNativeFeatureName = name;
  }

  /**
   * Observers registered here will be executed when the feature is attached
   */
  public onFeatureAttachObservable: BABYLON.Observable<IWebXRFeature> = new BABYLON.Observable();
  /**
   * Observers registered here will be executed when the feature is detached
   */
  public onFeatureDetachObservable: BABYLON.Observable<IWebXRFeature> = new BABYLON.Observable();

  /**
   * The dependencies of this feature, if any
   */
  public dependsOn?: string[];

  /**
   * Construct a new (abstract) WebXR feature
   * @param _xrSessionManager the xr session manager for this feature
   */
  constructor(protected _xrSessionManager: WebXRSessionManager) { }

  /**
   * Is this feature attached
   */
  public get attached() {
    return this._attached;
  }

  /**
   * attach this feature
   *
   * @param force should attachment be forced (even when already attached)
   * @returns true if successful, false is failed or already attached
   */
  public attach(force?: boolean): boolean {
    // do not attach a disposed feature
    if (this.isDisposed) {
      return false;
    }
    if (!force) {
      if (this.attached) {
        return false;
      }
    } else {
      if (this.attached) {
        // detach first, to be sure
        this.detach();
      }
    }

    // if this is a native WebXR feature, check if it is enabled on the session
    // For now only check if not using babylon native
    if (this._xrSessionManager.session.enabledFeatures.indexOf(this.xrNativeFeatureName) === -1) {
      return false;
    }

    this._attached = true;
    this._addNewAttachObserver(this._xrSessionManager.onXRFrameObservable, (frame) => this._onXRFrame(<any>frame));
    this.onFeatureAttachObservable.notifyObservers(this);
    return true;
  }

  /**
   * detach this feature.
   *
   * @returns true if successful, false if failed or already detached
   */
  public detach(): boolean {
    if (!this._attached) {
      this.disableAutoAttach = true;
      return false;
    }
    this._attached = false;
    this._removeOnDetach.forEach((toRemove) => {
      toRemove.observable.remove(toRemove.observer);
    });
    this.onFeatureDetachObservable.notifyObservers(this);
    return true;
  }

  /**
   * Dispose this feature and all of the resources attached
   */
  public dispose(): void {
    this.detach();
    this.isDisposed = true;
    this.onFeatureAttachObservable.clear();
    this.onFeatureDetachObservable.clear();
  }

  /**
   * This function will be executed during before enabling the feature and can be used to not-allow enabling it.
   * Note that at this point the session has NOT started, so this is purely checking if the browser supports it
   *
   * @returns whether or not the feature is compatible in this environment
   */
  public isCompatible(): boolean {
    return true;
  }

  /**
   * This is used to register callbacks that will automatically be removed when detach is called.
   * @param observable the observable to which the observer will be attached
   * @param callback the callback to register
   */
  protected _addNewAttachObserver<T>(observable: BABYLON.Observable<T>, callback: (eventData: T, eventState: BABYLON.EventState) => void) {
    this._removeOnDetach.push({
      observable,
      observer: observable.add(callback),
    });
  }

  /**
   * Code in this function will be executed on each xrFrame received from the browser.
   * This function will not execute after the feature is detached.
   * @param _xrFrame the current frame
   */
  protected abstract _onXRFrame(_xrFrame: XRFrame): void;
}
