/// <reference path="transmute-private.d.ts" />

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
   * also used internally for ATW(Asynchronous Time Warp) in JSAR's asynchronous stereo rendering.
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
   * In JSAR's asynchronous stereo rendering, we leverage the hit testing to host to decrease the latency when user interacts with the
   * virtual objects or the UI elements. With this method, the client(application) process does not need to do hit testing in every frame
   * and avoid the latency from the client to the host.
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

interface XRInputSource {
  /**
   * Set the target ray's hit test result, this is used for updating the host ray rendering.
   *
   * @param hit If the target ray hits an object.
   * @param endTransform The ray's end transform if hit or null.
   * @returns null
   */
  setTargetRayHitTestResult(hit: boolean, endTransform?: XRRigidTransform | null): void;
}

interface XRWebGLLayer {
  /**
   * Indicates whether the `XRWebGLLayer` is multiview required, if true, the application should use OVR_multiview2 extension to render.
   */
  multiviewRequired: boolean;
}
