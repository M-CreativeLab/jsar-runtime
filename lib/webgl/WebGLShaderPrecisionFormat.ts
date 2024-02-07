export class WebGLShaderPrecisionFormatImpl implements WebGLShaderPrecisionFormat {
  constructor(readonly rangeMin: number, readonly rangeMax: number, readonly precision: number) {}
}
