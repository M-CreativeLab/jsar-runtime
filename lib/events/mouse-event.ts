export default class MouseEventImpl extends Event implements MouseEvent {
  altKey: boolean;
  button: number;
  buttons: number;
  clientX: number;
  clientY: number;
  ctrlKey: boolean;
  metaKey: boolean;
  movementX: number;
  movementY: number;
  offsetX: number;
  offsetY: number;
  pageX: number;
  pageY: number;
  relatedTarget: EventTarget;
  screenX: number;
  screenY: number;
  shiftKey: boolean;
  x: number;
  y: number;
  mozMovementX: number;
  mozMovementY: number;
  webkitMovementX: number;
  webkitMovementY: number;
  msMovementX: number;
  msMovementY: number;
  detail: number;
  view: Window;
  which: number;
  inputIndex: number;

  constructor(type: string, eventInitDict: MouseEventInit) {
    super(type, eventInitDict);

    this.altKey = eventInitDict.altKey;
    this.button = eventInitDict.button;
    this.buttons = eventInitDict.buttons;
    this.clientX = eventInitDict.clientX;
    this.clientY = eventInitDict.clientY;
    this.ctrlKey = eventInitDict.ctrlKey;
    this.metaKey = eventInitDict.metaKey;
    this.movementX = eventInitDict.movementX;
    this.movementY = eventInitDict.movementY;
  }

  getModifierState(_keyArg: string): boolean {
    throw new Error("Method not implemented.");
  }
  initMouseEvent(_typeArg: string,
    _canBubbleArg: boolean, _cancelableArg: boolean, _viewArg: Window, _detailArg: number,
    _screenXArg: number, _screenYArg: number, _clientXArg: number, _clientYArg: number,
    _ctrlKeyArg: boolean, _altKeyArg: boolean, _shiftKeyArg: boolean, _metaKeyArg: boolean,
    _buttonArg: number,
    _relatedTargetArg: EventTarget): void {
    throw new Error("Method not implemented.");
  }
  initUIEvent(_typeArg: string,
    _bubblesArg?: boolean, _cancelableArg?: boolean, _viewArg?: Window, _detailArg?: number): void {
    throw new Error("Method not implemented.");
  }
}
