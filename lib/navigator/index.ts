class NavigatorImpl implements Navigator {
  clipboard: Clipboard;
  credentials: CredentialsContainer;
  doNotTrack: string;
  geolocation: Geolocation;
  maxTouchPoints: number;
  mediaCapabilities: MediaCapabilities;
  mediaDevices: MediaDevices;
  mediaSession: MediaSession;
  permissions: Permissions;
  serviceWorker: ServiceWorkerContainer;
  userActivation: UserActivation;
  wakeLock: WakeLock;
  canShare(data?: ShareData): boolean {
    throw new Error('Method not implemented.');
  }
  getGamepads(): Gamepad[] {
    throw new Error('Method not implemented.');
  }
  requestMIDIAccess(options?: MIDIOptions): Promise<MIDIAccess> {
    throw new Error('Method not implemented.');
  }
  requestMediaKeySystemAccess(keySystem: string, supportedConfigurations: MediaKeySystemConfiguration[]): Promise<MediaKeySystemAccess>;
  requestMediaKeySystemAccess(keySystem: string, supportedConfigurations: Iterable<MediaKeySystemConfiguration>): Promise<MediaKeySystemAccess>;
  requestMediaKeySystemAccess(keySystem: unknown, supportedConfigurations: unknown): Promise<MediaKeySystemAccess> {
    throw new Error('Method not implemented.');
  }
  sendBeacon(url: string | URL, data?: BodyInit): boolean {
    throw new Error('Method not implemented.');
  }
  share(data?: ShareData): Promise<void> {
    throw new Error('Method not implemented.');
  }
  vibrate(pattern: VibratePattern): boolean;
  vibrate(pattern: Iterable<number>): boolean;
  vibrate(pattern: unknown): boolean {
    throw new Error('Method not implemented.');
  }
  mozGetVRDevices: (any: any) => any;
  webkitGetUserMedia(constraints: MediaStreamConstraints, successCallback: any, errorCallback: any): void {
    throw new Error('Method not implemented.');
  }
  mozGetUserMedia(constraints: MediaStreamConstraints, successCallback: any, errorCallback: any): void {
    throw new Error('Method not implemented.');
  }
  msGetUserMedia(constraints: MediaStreamConstraints, successCallback: any, errorCallback: any): void {
    throw new Error('Method not implemented.');
  }
  webkitGetGamepads(): Gamepad[] {
    throw new Error('Method not implemented.');
  }
  msGetGamepads(): Gamepad[] {
    throw new Error('Method not implemented.');
  }
  webkitGamepads(): Gamepad[] {
    throw new Error('Method not implemented.');
  }

  /**
   * The `XRSystem` interface provides methods to:
   * 
   * 1. Get the default `XRSession`.
   * 2. Request a new `XRSession`.
   */
  xr?: XRSystem;

  /**
   * The `gl` property is the default `WebGLRenderingContext` or `WebGL2RenderingContext` for the spatial rendering 
   * context.
   */
  gl?: WebGLRenderingContext | WebGL2RenderingContext;

  /**
   * The `gpu` provides the WebGPU context.
   */
  get gpu(): GPU {
    throw new TypeError('WebGPU is not supported in this environment');
  }
  get webdriver(): boolean {
    return false;
  }
  clearAppBadge(): Promise<void> {
    throw new Error('Method not implemented.');
  }
  setAppBadge(contents?: number): Promise<void> {
    throw new Error('Method not implemented.');
  }
  registerProtocolHandler(scheme: string, url: string | URL): void {
    throw new Error('Method not implemented.');
  }
  hardwareConcurrency: number;
  cookieEnabled: boolean;
  appCodeName: string;
  appName: string;
  appVersion: string;
  platform: string;
  product: string;
  productSub: string;
  userAgent: string = 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36';
  vendor: string;
  vendorSub: string;
  language: string;
  languages: readonly string[];
  locks: LockManager;
  onLine: boolean;
  mimeTypes: MimeTypeArray;
  pdfViewerEnabled: boolean = false;
  plugins: PluginArray;
  storage: StorageManager;
  javaEnabled(): boolean {
    return false;
  }
}

export const navigator = new NavigatorImpl();

let isXRConfigured = false;
export function configureXRSystem(xr: XRSystem) {
  if (isXRConfigured) {
    throw new TypeError('XRSystem is already configured on navigator');
  }
  navigator.xr = xr;
  isXRConfigured = true;
}

let isGLConfigured = false;
export function configureGL(gl: WebGLRenderingContext | WebGL2RenderingContext) {
  if (isGLConfigured) {
    throw new TypeError('WebGLRenderingContext is already configured on navigator');
  }
  navigator.gl = gl;
  isGLConfigured = true;
}
