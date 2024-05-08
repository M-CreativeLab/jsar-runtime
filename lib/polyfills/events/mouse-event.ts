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

  getModifierState(keyArg: string): boolean {
    throw new Error("Method not implemented.");
  }
  initMouseEvent(typeArg: string, canBubbleArg: boolean, cancelableArg: boolean, viewArg: Window, detailArg: number, screenXArg: number, screenYArg: number, clientXArg: number, clientYArg: number, ctrlKeyArg: boolean, altKeyArg: boolean, shiftKeyArg: boolean, metaKeyArg: boolean, buttonArg: number, relatedTargetArg: EventTarget): void {
    throw new Error("Method not implemented.");
  }
  initUIEvent(typeArg: string, bubblesArg?: boolean, cancelableArg?: boolean, viewArg?: Window, detailArg?: number): void {
    throw new Error("Method not implemented.");
  }
}
