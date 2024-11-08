import OVR_multiview2_impl from './ovr_multiview';

export default class Oculus_multiview_impl extends OVR_multiview2_impl implements OCULUS_multiview {
  framebufferTextureMultisampleMultiviewOVR(
    _target: GLenum,
    _attachment: GLenum,
    _texture: WebGLTexture | null,
    _level: GLint,
    _samples: GLsizei,
    _baseViewIndex: GLint,
    _numViews: GLsizei
  ): void {
    // Nothing to do in mixed rendering mode.
  }
}
