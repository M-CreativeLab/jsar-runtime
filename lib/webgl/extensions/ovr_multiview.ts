export default class OVR_multiview2_impl implements OVR_multiview2 {
  readonly FRAMEBUFFER_ATTACHMENT_TEXTURE_NUM_VIEWS_OVR = 0x9630;
  readonly FRAMEBUFFER_ATTACHMENT_TEXTURE_BASE_VIEW_INDEX_OVR = 0x9632;
  readonly MAX_VIEWS_OVR = 0x9631;
  readonly FRAMEBUFFER_INCOMPLETE_VIEW_TARGETS_OVR = 0x9633;

  framebufferTextureMultiviewOVR(
    _target: GLenum,
    _attachment: GLenum,
    _texture: WebGLTexture | null,
    _level: GLint,
    _baseViewIndex: GLint,
    _numViews: GLsizei
  ): void {
    // FIXME: Nothing to do in mixed rendering mode, because the framebuffer is created by the host environment.
  }
}
