import { EmitData } from '../capabilities/generate-domcument/interfaces';
import { EventEmitter } from 'events';

export function emitDataFun(emiter: EventEmitter, event: string, data: EmitData) {
  emiter.emit(event, data);
}