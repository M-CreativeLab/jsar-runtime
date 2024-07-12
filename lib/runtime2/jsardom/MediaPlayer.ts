import { type MediaPlayerBackend } from '@yodaos-jsar/dom';

export class MediaPlayerBackendOnTransmute implements MediaPlayerBackend {
  paused: boolean;
  currentTime: number;
  duration: number;
  volume: number;
  loop: boolean;
  onended: () => void;

  constructor() {
    // TODO
  }
  load(buffer: ArrayBuffer | ArrayBufferView, onloaded: () => void): void {
    throw new TypeError('Method not implemented.');
  }
  play(when?: number): void {
    throw new TypeError('Method not implemented.');
  }
  pause(): void {
    throw new TypeError('Method not implemented.');
  }
  canPlayType(type: string): CanPlayTypeResult {
    throw new TypeError('Method not implemented.');
  }
  dispose(): void {
    throw new TypeError('Method not implemented.');
  }
}
