const xrNative = process._linkedBinding('transmute:webxr');

export function createXRSession(options: XRSessionInit): XRSession {
  return xrNative.createSession(options);
}
