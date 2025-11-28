#include <command_buffers/shared.hpp>
#include <command_buffers/webgl_constants.hpp>
#include <renderer/context_webgl.hpp>
#include <renderer/content_renderer.hpp>

namespace renderer
{
  using namespace std;
  using namespace commandbuffers;


  // --- State Management ---
  void TrContextWebGL::glBlendColor(WebGLfloat red,
                                    WebGLfloat green,
                                    WebGLfloat blue,
                                    WebGLfloat alpha)
  {
    blend_color_[0] = red;
    blend_color_[1] = green;
    blend_color_[2] = blue;
    blend_color_[3] = alpha;
  }

  void TrContextWebGL::glBlendEquation(WebGLenum mode)
  {
    blend_equation_rgb_ = mode;
    blend_equation_alpha_ = mode;
  }

  void TrContextWebGL::glBlendEquationSeparate(WebGLenum mode_rgb, WebGLenum mode_alpha)
  {
    blend_equation_rgb_ = mode_rgb;
    blend_equation_alpha_ = mode_alpha;
  }

  void TrContextWebGL::glBlendFunc(WebGLenum sfactor, WebGLenum dfactor)
  {
    blend_sfactor_rgb_ = sfactor;
    blend_dfactor_rgb_ = dfactor;
    blend_sfactor_alpha_ = sfactor;
    blend_dfactor_alpha_ = dfactor;
  }

  void TrContextWebGL::glBlendFuncSeparate(WebGLenum src_rgb,
                                           WebGLenum dst_rgb,
                                           WebGLenum src_alpha,
                                           WebGLenum dst_alpha)
  {
    blend_sfactor_rgb_ = src_rgb;
    blend_dfactor_rgb_ = dst_rgb;
    blend_sfactor_alpha_ = src_alpha;
    blend_dfactor_alpha_ = dst_alpha;
  }

  void TrContextWebGL::glColorMask(WebGLboolean red,
                                   WebGLboolean green,
                                   WebGLboolean blue,
                                   WebGLboolean alpha)
  {
    color_mask_[0] = red;
    color_mask_[1] = green;
    color_mask_[2] = blue;
    color_mask_[3] = alpha;
  }

  void TrContextWebGL::glCullFace(WebGLenum mode)
  {
    cull_face_ = mode;
  }

  void TrContextWebGL::glDepthFunc(WebGLenum func)
  {
    depth_func_ = func;
  }

  void TrContextWebGL::glDepthMask(WebGLboolean flag)
  {
    depth_mask_ = flag;
  }

  void TrContextWebGL::glDepthRangef(WebGLfloat near, WebGLfloat far)
  {
    depth_range_[0] = near;
    depth_range_[1] = far;
  }

  void TrContextWebGL::glDisable(WebGLenum cap)
  {
    caps_.disable(cap);
  }

  void TrContextWebGL::glEnable(WebGLenum cap)
  {
    caps_.enable(cap);
  }

  void TrContextWebGL::glFrontFace(WebGLenum mode)
  {
    front_face_ = mode;
  }

  void TrContextWebGL::glGet(WebGLenum pname, WebGLint *params)
  {
    /* TODO(yorkie): implement */
  }

  WebGLenum TrContextWebGL::glGetError()
  {
    WebGLenum error = last_error_;
    last_error_ = WEBGL_NO_ERROR;
    return error;
  }

  void TrContextWebGL::glHint(WebGLenum target, WebGLenum mode)
  { /* TODO(yorkie): implement */
  }

  WebGLboolean TrContextWebGL::glIsEnabled(WebGLenum cap)
  {
    return caps_.isEnabled(cap);
  }

  void TrContextWebGL::glLineWidth(WebGLfloat width)
  {
    line_width_ = width;
  }

  void TrContextWebGL::glPixelStorei(WebGLenum pname, WebGLint param)
  { /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glPolygonOffset(WebGLfloat factor, WebGLfloat units)
  {
    polygon_offset_factor_ = factor;
    polygon_offset_units_ = units;
  }

  void TrContextWebGL::glSampleCoverage(WebGLfloat value, WebGLboolean invert)
  {
    sample_coverage_value_ = value;
    sample_coverage_invert_ = invert;
  }

  void TrContextWebGL::glScissor(WebGLint x, WebGLint y, WebGLsizei width, WebGLsizei height)
  {
    scissor_box_[0] = x;
    scissor_box_[1] = y;
    scissor_box_[2] = width;
    scissor_box_[3] = height;
  }

  void TrContextWebGL::glStencilFunc(WebGLenum func, WebGLint ref, WebGLuint mask)
  {
    stencil_func_ = func;
    stencil_ref_ = ref;
    stencil_mask_ = mask;
  }

  void TrContextWebGL::glStencilFuncSeparate(WebGLenum face,
                                             WebGLenum func,
                                             WebGLint ref,
                                             WebGLuint mask)
  {
    stencil_func_ = func;
    stencil_ref_ = ref;
    stencil_mask_ = mask;
  }

  void TrContextWebGL::glStencilMask(WebGLuint mask)
  {
    stencil_mask_ = mask;
  }

  void TrContextWebGL::glStencilMaskSeparate(WebGLenum face, WebGLuint mask)
  { /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glStencilOp(WebGLenum fail, WebGLenum zfail, WebGLenum zpass)
  { /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glStencilOpSeparate(WebGLenum face, WebGLenum fail, WebGLenum zfail, WebGLenum zpass)
  { /* TODO(yorkie): implement */
  }

  void TrContextWebGL::glViewport(WebGLint x, WebGLint y, WebGLsizei width, WebGLsizei height)
  {
    viewport_[0] = x;
    viewport_[1] = y;
    viewport_[2] = width;
    viewport_[3] = height;
  }
}
