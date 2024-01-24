/**
 * This is the interface for the `PropertyChange` class.
 */
export interface PropertyChange {
  /**
   * The name of the property.
   */
  name: string;
  /**
   * The type of the property.
   */
  type: string;
  /**
   * The value of the property.
   */
  value: any;
}

/**
 * This is the interface for the `PropertyChangeOnGameObject` class.
 */
export declare class PropertyChangeOnGameObject {
  /**
   * The node GUID.
   */
  guid: number;
  /**
   * The name of the property.
   */
  name: string;
  /**
   * The type of the property.
   */
  type: string;

  constructor(guid: number, descriptor: PropertyChange);
}

/**
 * This is the interface for the `MaterialSyncChangeOnGameObject` class.
 */
export declare class MaterialSyncChangeOnGameObject {
  /**
   * The node GUID.
   */
  guid: number;
  /**
   * The material object to sync, which is a `VirtualMaterial` instance.
   */
  material: VirtualMaterial;
}

/**
 * This is the interface for the `VirtualVector3` class.
 */
export declare class VirtualVector3 {
  /**
   * Create a new VirtualVector3 from a BABYLON.Vector3 instance.
   */
  constructor(data: BABYLON.Vector3);

  /**
   * The x component of the vector.
   */
  x: number;
  /**
   * The y component of the vector.
   */
  y: number;
  /**
   * The z component of the vector.
   */
  z: number;
}

/**
 * This is the interface for the `VirtualQuaternion` class.
 */
export declare class VirtualQuaternion {
  /**
   * Create a new VirtualQuaternion from a BABYLON.Quaternion instance.
   */
  constructor(data: BABYLON.Quaternion);

  /**
   * The x component of the quaternion.
   */
  x: number;
  /**
   * The y component of the quaternion.
   */
  y: number;
  /**
   * The z component of the quaternion.
   */
  z: number;
  /**
   * The w component of the quaternion.
   */
  w: number;
}

/**
 * This is the interface for `VirtualMatrix` class.
 */
export declare class VirtualMatrix {
  /**
   * Create a matrix representation, and define the row and column counts.
   * @param rowsCount the row number
   * @param columnsCount the column number
   */
  constructor(rowsCount: number, columnsCount: number);

  /**
   * The row count of the matrix.
   */
  rowsCount: number;

  /**
   * The column count of the matrix.
   */
  columnsCount: number;

  /**
   * Get the value of the matrix at the specified row and column.
   */
  values: number[];

  /**
   * Update the underlaying matrix data.
   */
  update(): void;
}

/**
 * This is the interface for the `VirtualTransform` class.
 * 
 * A transform is a node which controls the position and rotation of its children.
 */
export declare class VirtualTransform {
  /**
   * Create a new VirtualTransform from a BABYLON.TransformNode instance.
   */
  constructor(transformNode: BABYLON.TransformNode);
}

/**
 * The options to be used for initialize a virtual texture.
 */
export type TextureInitOptions = Partial<{
  /**
   * If this texture is to be a dynamical texture.
   */
  isDynamicTexture: boolean;
  /**
   * The texture canvas context.
   */
  canvasContext: BABYLON.ICanvasRenderingContext;
  /**
   * The texture canvas width.
   */
  canvasWidth: number;
  /**
   * The texture canvas height.
   */
  canvasHeight: number;
}>;

/**
 * This is the interface for the `VirtualTexture` class.
 */
export declare class VirtualTexture {
  /**
   * The texture guid.
   */
  guid: number;
  /**
   * The texture name.
   */
  name: string;

  /**
   * The texture height.
   */
  height: number;

  /**
   * The texture width.
   */
  width: number;

  /**
   * If this texture need to invert by y-axis.
   */
  invertY: boolean;

  /**
   * Create a new VirtualTexture from a BABYLON.Texture instance.
   * 
   * @param data the texture data, commonly `tex.serialize()`.
   * @param initOptions the options to be used for initialize a virtual texture.
   */
  constructor(name: string, initOptions?: TextureInitOptions);

  /**
   * Set the texture's pixels.
   * @param pixels 
   */
  setPixels(pixels: Uint8Array): void;

  /**
   * Set the image url of this texture.
   * @param url 
   */
  setSourceUrl(url: string): void;
}

/**
 * This is the interface for the `VirtualMaterial` class.
 */
export declare class VirtualMaterial {
  /**
   * Create a new VirtualMaterial.
   * @param material the material to create from.
   */
  constructor(material: BABYLON.Material);

  /**
   * Set if this material is to be rendered as wireframe.
   * @param v the wireframe value, true for wireframe, false for not.
   */
  setWireframe(v: boolean): void;

  /**
   * Set the material's alpha value.
   * @param v the alpha value.
   */
  setAlpha(v: number): void;

  /**
   * Set the material's alpha mode.
   * @param v the alpha mode.
   */
  setAlphaMode(v: number): void;

  /**
   * Set the surface type
   * @param v the surface type.
   */
  setSurfaceType(v: number): void;

  /**
   * Set the material's standard diffuse color.
   * @param v the diffuse color.
   */
  setStandardDiffuseColor(v: BABYLON.Color3): void;

  /**
   * Set the material's standard specular color.
   * @param v the specular color.
   */
  setStandardSpecularColor(v: BABYLON.Color3): void;

  /**
   * Set the material's standard emissive color.
   * @param v the emissive color.
   */
  setStandardEmissiveColor(v: BABYLON.Color3): void;

  /**
   * Set the material's standard ambient color.
   * @param v the ambient color.
   */
  setStandardAmbientColor(v: BABYLON.Color3): void;

  /**
   * Set the material's standard diffuse texture.
   * @param tex the diffuse texture.
   */
  setStandardDiffuseTexture(tex: VirtualTexture): void;

  /**
   * Set the material's standard specular texture.
   * @param tex the specular texture.
   */
  setStandardSpecularTexture(tex: VirtualTexture): void;

  /**
   * Set the material's standard emissive texture.
   * @param tex the emissive texture.
   */
  setStandardEmissiveTexture(tex: VirtualTexture): void;

  /**
   * Set the material's standard emissive texture.
   * @param tex the emissive texture.
   */
  setStandardAmbientTexture(tex: VirtualTexture): void;

  /**
   * Set the PBR material's albedo color.
   * @param v the albedo color.
   */
  setAlbedoColor(v: BABYLON.Color3): void;

  /**
   * Set the PBR material's ambient color.
   * @param v the ambient color.
   */
  setAmbientColor(v: BABYLON.Color3): void;

  /**
   * Set the PBR material's emissive color.
   * @param v the emissive color.
   */
  setEmissiveColor(v: BABYLON.Color3): void;

  /**
   * Set the PBR material's albedo texture.
   * @param tex the emissive texture.
   */
  setAlbedoTexture(tex: VirtualTexture): void;

  /**
   * Set the PBR material's ambient texture.
   * @param tex the ambient texture.
   */
  setAmbientTexture(tex: VirtualTexture): void;

  /**
   * Set the PBR material's emissive texture.
   * @param tex the emissive texture.
   */
  setEmissiveTexture(tex: VirtualTexture): void;

  /**
   * Set the PBR material's metallic value.
   * @param v the metallic value.
   */
  setMetallic(v: number): void;

  /**
   * Set the PBR material's roughness value.
   * @param v the roughness value.
   */
  setRoughness(v: number): void;
}

/**
 * The type of the vertex buffer, which is used to identify the vertex buffer, value can be:
 * 
 * - `position`: the vertex buffer is for position.
 * - `normals`: the vertex buffer is for normals.
 * - `uv`: the vertex buffer is for uv.
 * - `uv2`: the vertex buffer is for uv2.
 * - `uv3`: the vertex buffer is for uv3.
 * - `uv4`: the vertex buffer is for uv4.
 */
export type VertexType = 'position' | 'normals' | 'colors' | 'uv' | 'uv2' | 'uv3' | 'uv4';

/**
 * The metadata of the vertex buffer. It is used to identify the vertex buffer, value can be:
 * 
 * - `stride`: the stride of the vertex buffer.
 * - `offset`: the offset of the vertex buffer.
 * - `byteStride`: the byte stride of the vertex buffer.
 * - `byteOffset`: the byte offset of the vertex buffer.
 */
export type VertexBufferMetadata = {
  stride: number;
  offset: number;
  byteStride: number;
  byteOffset: number;
};

/**
 * This is the interface for the `VirtualGameObjectData` class, which is used to set the data of a game object. The data 
 * of a game object is the data of its mesh, material, ui, etc.
 */
export declare class VirtualGameObjectData {
  /**
   * Create a new VirtualGameObjectData.
   * @param gameObject the game object to set data to.
   */
  constructor(gameObject: BABYLON.Node);

  /**
   * Set this mesh's triangles data.
   * @param data the triangles data.
   */
  setMeshTrianglesData(data: Uint32Array): void;

  /**
   * Set this mesh's vertex buffer data.
   * @param type specify the type of the vertex buffer.
   * @param data the vertex buffer data.
   * @param vertexBufferMetadata the metadata of the vertex buffer.
   */
  setMeshVertexBuffer(type: VertexType, data: Float32Array, vertexBufferMetadata?: VertexBufferMetadata): void;

  /**
   * Compute the mesh buffers internally.
   */
  computeAndSetMeshBuffers(): void;

  /**
   * Set the mesh's bounding info.
   */
  setMeshBounds(bounding: BABYLON.BoundingInfo): void;

  /**
   * Set the mesh's material.
   * @param type the type of the material.
   * @param material the material object.
   */
  setMaterial(type: string, material: VirtualMaterial): void;

  /**
   * Write the bone weight values.
   * @param weight0 the first bone weight
   * @param weight1 the second bone weight
   * @param weight2 the third bone weight
   * @param weight3 the fourth bone weight
   */
  addBoneWeights(weight0: number, weight1: number, weight2: number, weight3: number): void;

  /**
   * Write the bone index values.
   * @param index0 the first bone index
   * @param index1 the second bone index
   * @param index2 the third bone index
   * @param index3 the fourth bone index
   */
  addBoneIndices(index0: number, index1: number, index2: number, index3: number): void;

  /**
   * Add a bind pose matrix to the skinned mesh.
   * @param matrix the bind pose matrix(4x4).
   */
  addBindPose(matrix: VirtualMatrix): void;

  /**
   * Add a reference to the given guid transform.
   * @param guid the guid to find the bone transform.
   */
  addBoneReference(guid: number): void;

  /**
   * Set the mesh's outline
   * @param visible if the outline is visible.
   * @param props the properties of this outline.
   */
  setMeshOutline(visible: boolean, props: { color: BABYLON.Color3, width: number }): void;

  /**
   * Mark the game object as a bounds, a bounds is a group of its children.
   */
  asBounds(): void;

  /**
   * Mark the game object as a blocker, a block is a game object which is used to block the ray.
   */
  asBlocker(): void;

  /**
   * Set the line rendering colors.
   */
  setLineRenderingColors(start: BABYLON.Color3, end?: BABYLON.Color3): void;

  /**
   * Set the line rendering widths.
   */
  setLineRenderingWidths(start: number, end?: number): void;
}

/**
 * This is the interface for the `VirtualGameObject` class.
 */
export declare class VirtualGameObject {
  /**
   * The object guid.
   */
  guid: number;

  /**
   * The object name.
   */
  name: string;

  /**
   * The object type.
   */
  type: string;

  /**
   * The object's transform.
   */
  transform: VirtualTransform;

  /**
   * If this object is visible.
   */
  visibility: boolean;

  /**
   * The extended data of this object.
   */
  data: VirtualGameObjectData;

  /**
   * Create a new VirtualGameObject from a BABYLON.Node instance.
   * 
   * @param guid the node guid.
   * @param gameObject the game object.
   */
  constructor(guid: number, gameObject: BABYLON.Node);
}

/**
 * This is the interface for the `DocumentMetadata` class.
 */
export declare class DocumentMetadata {
  /**
   * Create a new document metadata.
   */
  constructor();

  /**
   * The specification version.
   */
  specVersion: string;
  /**
   * The document description.
   */
  description: string;
  /**
   * The document author.
   */
  author: string;
  /**
   * The document keywords.
   */
  keywords: string;
  /**
   * The document license.
   */
  license: string;
  /**
   * The document license url.
   */
  licenseUrl: string;
  /**
   * The document viewport initial scale value.
   */
  viewportInitialScale: number;
}

/**
 * This is the interface for the `VirtualGameObjectModel` class.
 */
export declare class VirtualGameObjectModel {
  /**
   * Create a new game object model.
   */
  constructor();

  /**
   * Set the title of the model.
   * @param title the title of the model.
   */
  setTitle(title: string): void;

  /**
   * Set the metadata of the model.
   * @param metadata the metadata of the model.
   */
  setMetadata(metadata: DocumentMetadata): void;

  /**
   * Create a new game object as this model's child.
   * @param guid the node guide.
   * @param gameObject the game object to create from.
   */
  createGameObjectAsChild(guid: number, gameObject: BABYLON.Node): VirtualGameObject;

  /**
   * Create a new property change on the game object.
   * @param guid the node guide.
   * @param descriptor the property change descriptor.
   */
  createPropertyChange(guid: number, descriptor: PropertyChange): PropertyChangeOnGameObject;

  /**
   * Create a new vertices sync change on the specific game object.
   * @param guid 
   * @param triangles
   * @param vertices 
   */
  createVerticesSyncChange(
    guid: number,
    triangles: Uint32Array,
    vertices: Partial<{ [key in VertexType]: Float32Array }>
  );

  /**
   * Create a material sync change of a game object.
   * @param guid the material's target node guid.
   * @param material the material to sync.
   */
  createMaterialSyncChange(guid: number, type: string, material: VirtualMaterial): MaterialSyncChangeOnGameObject;

  /**
   * Serialize the game object model and write it to the Unity side.
   */
  serializeAndWrite(channelId: string): number;

  /**
   * If the underlying buffer is empty, it means this vgom model is consumed by the Unity side.
   */
  isBufferEmpty(channelId: string): boolean;

  /**
   * Fetch the buffer from the writter.
   */
  fetchBufferFromWritter(channelId: string): ArrayBuffer;

  /**
   * Fetch the buffer from the writter and clear it.
   */
  fetchAndClearBufferFromWritter(channelId: string): ArrayBuffer;
}

/**
 * The interface is to define the types of a `vgom` binding should implement.
 */
export interface Binding {
  ///
  /// Base types
  ///
  DocumentMetadata: typeof DocumentMetadata;
  VirtualMatrix: typeof VirtualMatrix;

  ///
  /// Methods
  ///
  /**
   * The method is to create the vgom model.
   */
  VirtualGameObjectModel: typeof VirtualGameObjectModel;
  /**
   * The method is to create a texture.
   */
  VirtualTexture: typeof VirtualTexture;
  /**
   * The method is to create a material.
   */
  VirtualMaterial: typeof VirtualMaterial;
};
