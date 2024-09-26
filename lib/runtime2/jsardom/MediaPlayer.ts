import { type MediaPlayerBackend } from '@yodaos-jsar/dom';

export class MediaPlayerBackendOnTransmute implements MediaPlayerBackend {
  private _audioInstance: HTMLAudioElement;

  constructor() {
    this._audioInstance = new Audio();
  }
  load(buffer: ArrayBuffer | ArrayBufferView, onloaded: () => void): void {
    this._audioInstance.srcObject = new Blob([buffer]);
    this._audioInstance.onloadeddata = onloaded;
    this._audioInstance.load();
  }
  play(when?: number | undefined): void {
    this._audioInstance.currentTime = when || 0;
    this._audioInstance.play();
  }
  pause(): void {
    this._audioInstance.pause();
  }
  canPlayType(type: string): CanPlayTypeResult {
    return this._audioInstance.canPlayType(type);
  }
  dispose(): void {
    return;
  }
  get paused(): boolean {
    return this._audioInstance.paused;
  }
  get currentTime(): number {
    return this._audioInstance.currentTime;
  }
  get duration(): number {
    return this._audioInstance.duration;
  }
  get volume(): number {
    return this._audioInstance.volume;
  }
  set volume(value: number) {
    this._audioInstance.volume = value;
  }
  get loop(): boolean {
    return this._audioInstance.loop;
  }
  set loop(value: boolean) {
    this._audioInstance.loop = value;
  }
  get onended(): () => void {
    return this._audioInstance.onended as any;
  }
  set onended(value: () => void) {
    this._audioInstance.onended = value;
  }
}
