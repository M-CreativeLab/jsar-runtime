export function threepioLog(...args: any[]): void {
  console.log('threepio :', ...args);
}

export function threepioError(...args: any[]): void {
  console.error('threepio error :', ...args);
}

export function threepioWarn(...args: any[]): void {
  console.warn('threepio warn :', ...args);
}