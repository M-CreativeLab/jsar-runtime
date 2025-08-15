// Export the C++ Navigator implementation directly
const { Navigator: NativeNavigator } = process._linkedBinding('transmute:browser');

// Create a single navigator instance
export const navigator = new NativeNavigator();

// XR System configuration
let isXRConfigured = false;
export function configureXRSystem(xr: XRSystem) {
  if (isXRConfigured) {
    throw new TypeError('XRSystem is already configured on navigator');
  }
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  (navigator as any).xr = xr;
  isXRConfigured = true;
}

// WebGL configuration
let isGLConfigured = false;
export function configureGL(gl: WebGLRenderingContext | WebGL2RenderingContext) {
  if (isGLConfigured) {
    throw new TypeError('WebGLRenderingContext is already configured on navigator');
  }
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  (navigator as any).gl = gl;
  isGLConfigured = true;
}
