/**
 * Renders to multiple views with a single draw call
 * Only for WebGL backends
 * @see https://www.khronos.org/registry/webgl/extensions/OVR_multiview2/
 */
export class MultiviewRenderTarget extends BABYLON.RenderTargetTexture {
  public override set samples(value: number) {
    // We override this setter because multisampling is handled by framebufferTextureMultisampleMultiviewOVR
    this._samples = value;
  }

  public override get samples(): number {
    return this._samples;
  }

  /**
   * Creates a multiview render target
   * @param scene scene used with the render target
   * @param size the size of the render target (used for each view)
   */
  constructor(scene?: BABYLON.Scene, size: number | { width: number; height: number } | { ratio: number } = 512) {
    super('multiview rtt', size, scene, false, true, BABYLON.Constants.TEXTURETYPE_UNSIGNED_INT, false, undefined, false, false, true, undefined, true);
    this._renderTarget = (this.getScene()!.getEngine() as BABYLON.Engine).createMultiviewRenderTargetTexture(this.getRenderWidth(), this.getRenderHeight());
    this._texture = this._renderTarget.texture!;
    this._texture.isMultiview = true;
    this._texture.format = BABYLON.Constants.TEXTUREFORMAT_RGBA;
    this.samples = this._getEngine()!.getCaps().maxSamples || this.samples;
    this._texture.samples = this._samples;
  }

  /**
   * @internal
   */
  public override _bindFrameBuffer() {
    if (!this._renderTarget) {
      return;
    }
    (this.getScene()!.getEngine() as BABYLON.Engine).bindMultiviewFramebuffer(this._renderTarget);
  }

  /**
   * Gets the number of views the corresponding to the texture (eg. a MultiviewRenderTarget will have > 1)
   * @returns the view count
   */
  public override getViewCount() {
    return 2;
  }
}