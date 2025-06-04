export function reportThreepioInfo(...args: any[]): void {
  console.log('threepio :', ...args);
}

export function reportThreepioError(...args: any[]): void {
  console.error('threepio error :', ...args);
}

export function reportThreepioWarning(...args: any[]): void {
  console.warn('threepio warn :', ...args);
}
