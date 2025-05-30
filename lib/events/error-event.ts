export class ErrorEventImpl extends Event implements ErrorEvent {
  colno: number;
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  error: any;
  filename: string;
  lineno: number;
  message: string;

  constructor(type: string, eventInitDict: ErrorEventInit) {
    super(type, eventInitDict);

    this.colno = eventInitDict.colno;
    this.error = eventInitDict.error;
    this.filename = eventInitDict.filename;
    this.lineno = eventInitDict.lineno;
    this.message = eventInitDict.message;
  }
}
