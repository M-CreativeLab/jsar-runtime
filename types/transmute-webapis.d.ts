/// <reference path="transmute-private.d.ts" />

/**
 * This file declares the new APIs and properties which is introduced by JSAR in its usecases.
 */

interface OffscreenCanvas {
  /**
   * JSAR added a new context type 'jsar:htmlrenderer' for customizing the HTML content rendering, developers can use this context
   * returned by `getContext` method to render any HTML content on the offscreen canvas, and put the canvas as a texture to the 3d
   * scene.
   * 
   * @param contextId 'jsar:htmlrenderer'
   */
  getContext(contextId: 'jsar:htmlrenderer'): Transmute.HTMLRenderingContext;
}

interface WebGLUniformLocation {
  /**
   * The `name` to the uniform variable, we introduced this new property to help developers to debug the shader program better and it
   * also used internally for JSAR's defferred composition in stereo rendering.
   * 
   * For example, if you have a uniform variable in the shader program:
   * 
   * ```glsl
   * uniform mat4 uModelViewMatrix;
   * ```
   * 
   * You can get the `name` of the uniform variable in anywhere by:
   * 
   * ```javascript
   * const uModelViewMatrix = gl.getUniformLocation(program, 'uModelViewMatrix');
   * console.log(uModelViewMatrix.name); // 'uModelViewMatrix'
   * ```
   */
  readonly name?: string;
}

interface XRSession {
  /**
   * In JSAR's __Defferred Composition__, we leverage the hit testing to host to decrease the latency when user interacts with the virtual objects 
   * or the UI elements. With this method, the client(application) process does not need to do hit testing in every frame and avoid the latency
   * from the client to the host.
   * 
   * This method `updateCollisionBox` is used to tell the host the collision box of a virtual object, then the host will do the hit testing
   * to compute the hit point by itself.
   * 
   * @param min The minimum point of the collision box.
   * @param max The maximum point of the collision box.
   * 
   * @todo Support multiple collision boxes in the same application.
   * @todo Support more collision container types: sphere, capsule, etc.
   * @todo Support mesh-based collision.
   */
  updateCollisionBox(min: number[], max: number[]): void;
}

interface XRWebGLLayer {
  /**
   * At JSAR, the WebXR rendering pipeline disallows developers to create framebuffer objects for stereo rendering, because the framebuffer is
   * always created by the host such as Unity, Unreal Engine, etc and the framebuffer is shared to the client process for rendering the WebXR
   * content.
   * 
   * If the framebuffer is multiview required, namely calling `glFramebufferTextureMultiviewOVR()` in the host, the client process must use the
   * followings in their vertex shader:
   * 
   * ```glsl
   * #extension GL_OVR_multiview2 : enable
   * layout(num_views = 2) in;
   * ```
   * 
   * Otherwise, any draw calls will occur an error by the `ovr_multiview*` extension.
   * 
   * To help WebXR developers to know if the framebuffer is multiview required, this property `multiviewRequired` is introduced in every created
   * `XRWebGLLayer` object, and developers can check this property to know if the framebuffer is multiview required and change their shaders.
   * 
   * @see https://registry.khronos.org/OpenGL/extensions/OVR/OVR_multiview.txt
   * @see https://registry.khronos.org/OpenGL/extensions/OVR/OVR_multiview2.txt
   */
  readonly multiviewRequired: boolean;
}
