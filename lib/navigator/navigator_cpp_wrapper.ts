const { Navigator: NativeNavigator } = process._linkedBinding('transmute:browser');

/**
 * Navigator implementation that wraps the native C++ Navigator
 */
class NavigatorImpl implements Navigator {
  private nativeNavigator: any;

  constructor() {
    this.nativeNavigator = new NativeNavigator();
  }

  // Standard Navigator properties from native implementation
  get userAgent(): string {
    return this.nativeNavigator.userAgent;
  }

  get platform(): string {
    return this.nativeNavigator.platform;
  }

  get appName(): string {
    return this.nativeNavigator.appName;
  }

  get appVersion(): string {
    return this.nativeNavigator.appVersion;
  }

  get language(): string {
    return this.nativeNavigator.language;
  }

  get languages(): readonly string[] {
    return [this.language];
  }

  get cookieEnabled(): boolean {
    return this.nativeNavigator.cookieEnabled;
  }

  get onLine(): boolean {
    return this.nativeNavigator.onLine;
  }

  get hardwareConcurrency(): number {
    return this.nativeNavigator.hardwareConcurrency;
  }

  javaEnabled(): boolean {
    return this.nativeNavigator.javaEnabled;
  }

  // RPC functionality now handled in C++
  makeRpcCall(method: string, args: any[], callback?: (success: boolean, response: string) => void): number {
    return this.nativeNavigator.makeRpcCall(method, args.map(arg => String(arg)), callback);
  }

  // Document request handling now in C++
  handleDocumentRequest(url: string, documentId: number, disableCache?: boolean): void {
    this.nativeNavigator.handleDocumentRequest(url, documentId, disableCache || false);
  }

  // Placeholder implementations for other Navigator interface methods
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

  canShare(_data?: ShareData): boolean {
    throw new Error('Method not implemented.');
  }
  getGamepads(): Gamepad[] {
    throw new Error('Method not implemented.');
  }
  requestMIDIAccess(_options?: MIDIOptions): Promise<MIDIAccess> {
    throw new Error('Method not implemented.');
  }
  requestMediaKeySystemAccess(keySystem: string, supportedConfigurations: MediaKeySystemConfiguration[]): Promise<MediaKeySystemAccess>;
  requestMediaKeySystemAccess(keySystem: string, supportedConfigurations: Iterable<MediaKeySystemConfiguration>): Promise<MediaKeySystemAccess>;
  requestMediaKeySystemAccess(_keySystem: unknown, _supportedConfigurations: unknown): Promise<MediaKeySystemAccess> {
    throw new Error('Method not implemented.');
  }
  sendBeacon(_url: string | URL, _data?: BodyInit): boolean {
    throw new Error('Method not implemented.');
  }
  share(_data?: ShareData): Promise<void> {
    throw new Error('Method not implemented.');
  }
  vibrate(pattern: VibratePattern): boolean;
  vibrate(pattern: Iterable<number>): boolean;
  vibrate(_pattern: unknown): boolean {
    throw new Error('Method not implemented.');
  }
  mozGetVRDevices: (any: any) => any;
  webkitGetUserMedia(_constraints: MediaStreamConstraints, _successCallback: any, _errorCallback: any): void {
    throw new Error('Method not implemented.');
  }
  mozGetUserMedia(_constraints: MediaStreamConstraints, _successCallback: any, _errorCallback: any): void {
    throw new Error('Method not implemented.');
  }
  msGetUserMedia(_constraints: MediaStreamConstraints, _successCallback: any, _errorCallback: any): void {
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
  setAppBadge(_contents?: number): Promise<void> {
    throw new Error('Method not implemented.');
  }
  registerProtocolHandler(_scheme: string, _url: string | URL): void {
    throw new Error('Method not implemented.');
  }
  appCodeName: string;
  product: string;
  productSub: string;
  vendor: string;
  vendorSub: string;
  locks: LockManager;
  mimeTypes: MimeTypeArray;
  pdfViewerEnabled: boolean = false;
  plugins: PluginArray;
  storage: StorageManager;
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