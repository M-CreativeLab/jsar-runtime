import { decode } from 'windows-1252';

const OriginalTextDecoder = globalThis.TextDecoder;
const labelsOfWin1252 = [
  'ansi_x3.4-1968',
  'ascii',
  'cp1252',
  'cp819',
  'csisolatin1',
  'ibm819',
  'iso-8859-1',
  'iso-ir-100',
  'iso8859-1',
  'iso88591',
  'iso_8859-1',
  'iso_8859-1:1987',
  'l1',
  'latin1',
  'us-ascii',
  'windows-1252',
  'x-cp1252'
];

class TextDecoderForWin1252 implements TextDecoder {
  encoding: string = 'windows-1252';
  fatal: boolean = false;
  ignoreBOM: boolean = false;

  constructor(options?: TextDecoderOptions) {
    if (options) {
      this.fatal = options.fatal || false;
      this.ignoreBOM = options.ignoreBOM || false;
    }
  }

  decode(input?: ArrayBuffer | ArrayBufferView, options?: TextDecodeOptions): string {
    if (options?.stream) {
      throw new TypeError('stream option is not supported');
    }
    let inputBuffer: Buffer;
    if (ArrayBuffer.isView(input)) {
      inputBuffer = Buffer.from(new Uint8Array(input.buffer, input.byteOffset, input.byteLength));
    } else {
      inputBuffer = Buffer.from(input);
    }
    return decode(inputBuffer, { mode: 'fatal' });
  }
}

globalThis.TextDecoder = function _TextDecoder(label?: string, options?: TextDecoderOptions) {
  if (labelsOfWin1252.includes(label)) {
    return new TextDecoderForWin1252(options);
  } else {
    return new OriginalTextDecoder(label, {
      ...options,
      fatal: false,
    });
  }
} as any;
