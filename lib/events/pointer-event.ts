import MouseEventImpl from './mouse-event';

export class PointerEventImpl extends MouseEventImpl implements PointerEvent {
  width: number;
  height: number;
  isPrimary: boolean;
  pointerId: number;
  pointerType: string;
  pressure: number;
  tangentialPressure: number;
  tiltX: number;
  tiltY: number;
  twist: number;

  constructor(type: string, eventInitDict: PointerEventInit) {
    super(type, eventInitDict);

    this.width = eventInitDict.width;
    this.height = eventInitDict.height;
    this.isPrimary = eventInitDict.isPrimary;
    this.pointerId = eventInitDict.pointerId;
    this.pointerType = eventInitDict.pointerType;
    this.pressure = eventInitDict.pressure;
    this.tangentialPressure = eventInitDict.tangentialPressure;
    this.tiltX = eventInitDict.tiltX;
    this.tiltY = eventInitDict.tiltY;
    this.twist = eventInitDict.twist;
  }

  getCoalescedEvents(): PointerEvent[] {
    throw new Error('Method not implemented.');
  }
  getPredictedEvents(): PointerEvent[] {
    throw new Error('Method not implemented.');
  }
}
