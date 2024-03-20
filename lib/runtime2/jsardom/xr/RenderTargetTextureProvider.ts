import { WebXRLayerWrapper } from './LayerWrapper';

/**
 * An interface for objects that provide render target textures for XR rendering.
 */
export interface IWebXRRenderTargetTextureProvider extends BABYLON.IDisposable {
  /**
   * Attempts to set the framebuffer-size-normalized viewport to be rendered this frame for this view.
   * In the event of a failure, the supplied viewport is not updated.
   * @param viewport the viewport to which the view will be rendered
   * @param view the view for which to set the viewport
   * @returns whether the operation was successful
   */
  trySetViewportForView(viewport: BABYLON.Viewport, view: XRView): boolean;
  /**
   * Gets the correct render target texture to be rendered this frame for this eye
   * @param eye the eye for which to get the render target
   * @returns the render target for the specified eye or null if not available
   */
  getRenderTargetTextureForEye(eye: XREye): BABYLON.Nullable<BABYLON.RenderTargetTexture>;
  /**
   * Gets the correct render target texture to be rendered this frame for this view
   * @param view the view for which to get the render target
   * @returns the render target for the specified view or null if not available
   */
  getRenderTargetTextureForView(view: XRView): BABYLON.Nullable<BABYLON.RenderTargetTexture>;
}

/**
* Provides render target textures and other important rendering information for a given XRLayer.
* @internal
*/
export abstract class WebXRLayerRenderTargetTextureProvider implements IWebXRRenderTargetTextureProvider {
  public abstract trySetViewportForView(viewport: BABYLON.Viewport, view: XRView): boolean;
  public abstract getRenderTargetTextureForEye(eye: XREye): BABYLON.Nullable<BABYLON.RenderTargetTexture>;
  public abstract getRenderTargetTextureForView(view: XRView): BABYLON.Nullable<BABYLON.RenderTargetTexture>;

  protected _renderTargetTextures = new Array<BABYLON.RenderTargetTexture>();
  protected _framebufferDimensions: BABYLON.Nullable<{ framebufferWidth: number; framebufferHeight: number }>;

  private _engine: BABYLON.Engine;

  constructor(
    private readonly _scene: BABYLON.Scene,
    public readonly layerWrapper: WebXRLayerWrapper
  ) {
    this._engine = _scene.getEngine();
  }

  private _createInternalTexture(
    textureSize: { width: number; height: number },
    texture: WebGLTexture
  ): BABYLON.InternalTexture {
    const internalTexture = new BABYLON.InternalTexture(this._engine, BABYLON.InternalTextureSource.Unknown, true);
    internalTexture.width = textureSize.width;
    internalTexture.height = textureSize.height;
    internalTexture._hardwareTexture = new BABYLON.WebGLHardwareTexture(texture, this._engine._gl);
    internalTexture.isReady = true;
    return internalTexture;
  }

  protected _createRenderTargetTexture(
    width: number,
    height: number,
    framebuffer: BABYLON.Nullable<WebGLFramebuffer>,
    colorTexture?: WebGLTexture,
    depthStencilTexture?: WebGLTexture,
    multiview?: boolean
  ): BABYLON.RenderTargetTexture {
    if (!this._engine) {
      throw new Error('Engine is disposed');
    }

    const textureSize = { width, height };

    // Create render target texture from the internal texture
    const renderTargetTexture = multiview ?
      new BABYLON.MultiviewRenderTarget(this._scene, textureSize) :
      new BABYLON.RenderTargetTexture('XR renderTargetTexture', textureSize, this._scene);
    const renderTargetWrapper = renderTargetTexture.renderTarget as BABYLON.WebGLRenderTargetWrapper;
    renderTargetWrapper._samples = renderTargetTexture.samples;
    // Set the framebuffer, make sure it works in all scenarios - emulator, no layers and layers
    if (framebuffer || !colorTexture) {
      renderTargetWrapper._framebuffer = framebuffer;
    }

    // Create internal texture
    if (colorTexture) {
      if (multiview) {
        renderTargetWrapper._colorTextureArray = colorTexture;
      } else {
        const internalTexture = this._createInternalTexture(textureSize, colorTexture);
        renderTargetWrapper.setTexture(internalTexture, 0);
        renderTargetTexture._texture = internalTexture;
      }
    }

    if (depthStencilTexture) {
      if (multiview) {
        renderTargetWrapper._depthStencilTextureArray = depthStencilTexture;
      } else {
        renderTargetWrapper._depthStencilTexture = this._createInternalTexture(textureSize, depthStencilTexture);
      }
    }

    renderTargetTexture.disableRescaling();
    // Firefox reality fails if skipInitialClear is set to true, so make sure only modern XR implementations set it.
    if (typeof XRWebGLBinding !== 'undefined') {
      renderTargetTexture.skipInitialClear = true;
    }

    this._renderTargetTextures.push(renderTargetTexture);

    return renderTargetTexture;
  }

  protected _destroyRenderTargetTexture(renderTargetTexture: BABYLON.RenderTargetTexture) {
    this._renderTargetTextures.splice(this._renderTargetTextures.indexOf(renderTargetTexture), 1);
    renderTargetTexture.dispose();
  }

  public getFramebufferDimensions(): BABYLON.Nullable<{ framebufferWidth: number; framebufferHeight: number }> {
    return this._framebufferDimensions;
  }

  public dispose() {
    this._renderTargetTextures.forEach((rtt) => rtt.dispose());
    this._renderTargetTextures.length = 0;
  }
}
