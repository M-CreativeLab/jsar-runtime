import { WebXRLayerWrapper } from './LayerWrapper';
import { WebXRSessionManager } from './SessionManager';
import { WebXRWebglLayerWrapper } from './WebglLayer';

/**
 * Configuration object for WebXR output canvas
 */
export class WebXRManagedOutputCanvasOptions {
  /**
   * The rendering context to be used for the xr output canvas.
   */
  public renderingContext?: WebGLRenderingContext | WebGL2RenderingContext;
  /**
   * Options for this XR Layer output
   */
  public canvasOptions?: XRWebGLLayerInit;
}
/**
* Creates a canvas that is added/removed from the webpage when entering/exiting XR
*/
export class WebXRManagedOutputCanvas implements BABYLON.WebXRRenderTarget {
  private _engine: BABYLON.Nullable<BABYLON.ThinEngine> = null;
  private _originalCanvasSize: {
    width: number;
    height: number;
  };

  /**
   * Rendering context of the canvas which can be used to display/mirror xr content
   */
  public canvasContext: WebGLRenderingContext | WebGL2RenderingContext;

  /**
   * xr layer for the canvas
   */
  public xrLayer: BABYLON.Nullable<XRWebGLLayer> = null;
  private _xrLayerWrapper: BABYLON.Nullable<WebXRLayerWrapper> = null;

  /**
   * Observers registered here will be triggered when the xr layer was initialized
   */
  public onXRLayerInitObservable: BABYLON.Observable<XRWebGLLayer> = new BABYLON.Observable();

  /**
   * Initializes the canvas to be added/removed upon entering/exiting xr
   * @param _xrSessionManager The XR Session manager
   * @param _options optional configuration for this canvas output. defaults will be used if not provided
   */
  constructor(
    xrSessionManager: WebXRSessionManager,
    private _options: WebXRManagedOutputCanvasOptions
  ) {
    this._engine = xrSessionManager.scene.getEngine();
    this._engine.onDisposeObservable.addOnce(() => {
      this._engine = null;
    });
    this._setManagedOutputCanvas(_options);
  }

  /**
   * Disposes of the object
   */
  public dispose() {
    this._setManagedOutputCanvas(null);
  }

  /**
   * Initializes a XRWebGLLayer to be used as the session's baseLayer.
   * @param xrSession xr session
   * @returns a promise that will resolve once the XR Layer has been created
   */
  public async initializeXRLayerAsync(xrSession: XRSession): Promise<XRWebGLLayer> {
    const createLayer = () => {
      this.xrLayer = new XRWebGLLayer(xrSession, this.canvasContext, this._options.canvasOptions);
      this._xrLayerWrapper = new WebXRWebglLayerWrapper(this.xrLayer);
      this.onXRLayerInitObservable.notifyObservers(this.xrLayer);
      return this.xrLayer;
    };

    return this.canvasContext
      .makeXRCompatible()
      .then(
        // catch any error and continue. When using the emulator is throws this error for no apparent reason.
        () => { },
        () => {
          // log the error, continue nonetheless!
          BABYLON.Tools.Warn('Error executing makeXRCompatible. This does not mean that the session will work incorrectly.');
        }
      )
      .then(() => {
        return createLayer();
      });
  }

  private _setManagedOutputCanvas(options: WebXRManagedOutputCanvasOptions) {
    this.canvasContext = options.renderingContext;
    this._originalCanvasSize = {
      width: this.canvasContext.drawingBufferWidth,
      height: this.canvasContext.drawingBufferHeight,
    };
  }
}
