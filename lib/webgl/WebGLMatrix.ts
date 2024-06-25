const { placeholders: placeholdersNative } = process._linkedBinding('transmute:webgl');

type WebGLMatrixInit = {
  useRightHanded: boolean;
  inverseMatrix?: boolean;
  placeholderId: Transmute.WebGLPlaceholderId;
};

export class WebGLMatrix extends Float32Array {
  static CreateProjectionMatrix(from: Float32Array, useRightHanded: boolean = false) {
    return new WebGLMatrix(from, {
      useRightHanded,
      placeholderId: placeholdersNative.placeholderIds.ProjectionMatrix,
    });
  }
  static CreateViewMatrix(from: Float32Array, useRightHanded: boolean = false) {
    return new WebGLMatrix(from, {
      useRightHanded,
      placeholderId: placeholdersNative.placeholderIds.ViewMatrix,
    });
  }
  static CreateViewProjectionMatrix(from: Float32Array, useRightHanded: boolean = false) {
    return new WebGLMatrix(from, {
      useRightHanded,
      placeholderId: placeholdersNative.placeholderIds.ViewProjectionMatrix,
    });
  }
  constructor(from: Float32Array, init: WebGLMatrixInit) {
    super(from);
    this[placeholdersNative.useRightHandedKey] = init.useRightHanded;
    this[placeholdersNative.inverseMatrixKey] = typeof init.inverseMatrix === 'undefined' ? false : init.inverseMatrix;
    this[placeholdersNative.placeholderIdKey] = init.placeholderId;
  }

  get useRightHanded(): boolean {
    return this[placeholdersNative.useRightHandedKey];
  }
  set useRightHanded(value: boolean) {
    this[placeholdersNative.useRightHandedKey] = value;
  }

  get inverseMatrix(): boolean {
    return this[placeholdersNative.inverseMatrixKey];
  }
  set inverseMatrix(value: boolean) {
    this[placeholdersNative.inverseMatrixKey] = value;
  }

  inverse(): WebGLMatrix {
    this.inverseMatrix = !this.inverseMatrix;
    return this;
  }
}
