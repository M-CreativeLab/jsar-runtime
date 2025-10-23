export class ErrorEvent extends Event {
  message: string | undefined;
  filename: string | undefined;
  lineno: number | undefined;
  colno: number | undefined;

  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  error: any;
  constructor(type: string, eventInitDict: {
    message?: string,
    filename?: string,
    lineno?: number,
    colno?: number,
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    error?: any
  }) {
    super(type);
    this.message = eventInitDict.message;
    this.filename = eventInitDict.filename;
    this.lineno = eventInitDict.lineno;
    this.colno = eventInitDict.colno;
    this.error = eventInitDict.error;
  }
}
