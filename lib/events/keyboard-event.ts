export default class KeyboardEventImpl extends Event implements KeyboardEvent {
  altKey: boolean;
  ctrlKey: boolean;
  metaKey: boolean;
  shiftKey: boolean;
  key: string;
  code: string;
  location: number;
  repeat: boolean;
  isComposing: boolean;
  charCode: number;
  keyCode: number;
  which: number;
  detail: number;
  view: Window;

  constructor(type: string, eventInitDict: KeyboardEventInit) {
    super(type, eventInitDict);

    this.altKey = eventInitDict.altKey || false;
    this.ctrlKey = eventInitDict.ctrlKey || false;
    this.metaKey = eventInitDict.metaKey || false;
    this.shiftKey = eventInitDict.shiftKey || false;
    this.key = eventInitDict.key || "";
    this.code = eventInitDict.code || "";
    this.location = eventInitDict.location || 0;
    this.repeat = eventInitDict.repeat || false;
    this.isComposing = eventInitDict.isComposing || false;
    this.charCode = eventInitDict.charCode || 0;
    this.keyCode = eventInitDict.keyCode || 0;
    this.which = eventInitDict.which || 0;
    this.detail = eventInitDict.detail || 0;
    this.view = eventInitDict.view;
  }

  getModifierState(keyArg: string): boolean {
    switch (keyArg) {
      case "Alt":
        return this.altKey;
      case "Control":
        return this.ctrlKey;
      case "Meta":
        return this.metaKey;
      case "Shift":
        return this.shiftKey;
      default:
        return false;
    }
  }

  initKeyboardEvent(
    typeArg: string,
    bubblesArg?: boolean,
    cancelableArg?: boolean,
    viewArg?: Window,
    keyArg?: string,
    locationArg?: number,
    ctrlKeyArg?: boolean,
    altKeyArg?: boolean,
    shiftKeyArg?: boolean,
    metaKeyArg?: boolean
  ): void {
    // Legacy method - implement for compatibility but prefer constructor
    this.type = typeArg;
    this.bubbles = bubblesArg || false;
    this.cancelable = cancelableArg || false;
    this.view = viewArg;
    this.key = keyArg || "";
    this.location = locationArg || 0;
    this.ctrlKey = ctrlKeyArg || false;
    this.altKey = altKeyArg || false;
    this.shiftKey = shiftKeyArg || false;
    this.metaKey = metaKeyArg || false;
  }

  // DOM Level 3 constants for key locations
  static readonly DOM_KEY_LOCATION_STANDARD = 0;
  static readonly DOM_KEY_LOCATION_LEFT = 1;
  static readonly DOM_KEY_LOCATION_RIGHT = 2;
  static readonly DOM_KEY_LOCATION_NUMPAD = 3;

  readonly DOM_KEY_LOCATION_STANDARD = 0;
  readonly DOM_KEY_LOCATION_LEFT = 1;
  readonly DOM_KEY_LOCATION_RIGHT = 2;
  readonly DOM_KEY_LOCATION_NUMPAD = 3;
}