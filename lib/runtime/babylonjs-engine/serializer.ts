import * as logger from '../../bindings/logger';
import { toIndicesArray, executeWithTimeProfiler } from '../common/utils';
import { TransmuteInternalTexture, vGomInterface } from './engine';
import { DIRTY_SYMBOL, VGO_GUID_SYMBOL } from './common';

const nextTick = typeof setImmediate === 'function' ? setImmediate : (fn) => setTimeout(fn, 0);

export type DocumentMetadata = Partial<{
  specVersion: string;
  description: string;
  author: string;
  keywords: string;
  license: string;
  licenseUrl: string;
  viewportInitialScale: number;
}>;

export class GameObjectModelSerializer {
  #channelId: string;
  /**
   * The native binding instance, which supports multiple environments: unity(linked) and vscode extension.
   */
  #binding: vGomInterface.Binding = null;
  #gameObjectModel: vGomInterface.VirtualGameObjectModel;
  #lastChangeSerializable: vGomInterface.VirtualGameObjectModel;
  #watchingNodes: Array<(BABYLON.TransformNode | BABYLON.Mesh) & { __vgoGuid: string }> = [];
  #onGomBufferUpdate: (buffer: ArrayBuffer) => void;

  constructor(channelId: string) {
    this.#channelId = channelId;
  }

  get binding() {
    return this.#binding;
  }

  set binding(value: vGomInterface.Binding) {
    this.#binding = value;
  }

  get channelId() {
    return this.#channelId;
  }

  get onGomBufferUpdate() {
    return this.#onGomBufferUpdate;
  }

  set onGomBufferUpdate(value: (buffer: ArrayBuffer) => void) {
    this.#onGomBufferUpdate = value;
  }

  #doSerializeAndWrite(gomTarget?: vGomInterface.VirtualGameObjectModel) {
    if (!gomTarget) {
      gomTarget = this.#gameObjectModel;
    }
    const size = gomTarget.serializeAndWrite(this.#channelId);
    if (size > 0) {
      if (typeof this.#onGomBufferUpdate === 'function') {
        const buffer = this.#gameObjectModel.fetchBufferFromWritter(this.#channelId);
        // FIXME: use nextTick to avoid the huge performance impact from this function.
        nextTick(() => this.#onGomBufferUpdate(buffer));
      }
    }
  }

  reset() {
    this.#watchingNodes = [];
    this.#gameObjectModel = new this.#binding.VirtualGameObjectModel();
  }

  addMetadataInfo(title: string, metadata: DocumentMetadata) {
    if (title) {
      this.#gameObjectModel.setTitle(title);
    }
    if (metadata) {
      const docMetadata = new this.#binding.DocumentMetadata();
      docMetadata.specVersion = metadata.specVersion;
      docMetadata.description = metadata.description;
      docMetadata.author = metadata.author;
      docMetadata.keywords = metadata.keywords;
      docMetadata.viewportInitialScale = metadata.viewportInitialScale;
      this.#gameObjectModel.setMetadata(docMetadata);
    }
  }

  serializeAndWrite(gomTarget?: vGomInterface.VirtualGameObjectModel) {
    this.#doSerializeAndWrite(gomTarget);
  }

  /**
   * Serialize a scene.
   * @param {BABYLON.Scene} scene 
   */
  async serializeScene(scene: BABYLON.Scene) {
    await executeWithTimeProfiler('prepare guids', () => this.#generateGuids(scene));
    await executeWithTimeProfiler(`serialize ${scene.transformNodes.length} transform nodes`, () => this.serializeTransformNodes(scene));
    await executeWithTimeProfiler(`serialize ${scene.meshes.length} meshes`, () => this.serializeAllMeshes(scene));
    await executeWithTimeProfiler('serializeToBuffer', () => this.#doSerializeAndWrite());
    // FIXME: dispose the scene will cause the scene to be empty.
    // scene.dispose();
  }

  /**
   * Check if the last change buffer is empty, namely the buffer is consumed by the another side.
   * @returns a boolean if the buffer is empty.
   */
  isLastChangeBufferEmpty(): boolean {
    if (this.#lastChangeSerializable == null) {
      return true;
    }
    return this.#lastChangeSerializable.isBufferEmpty(this.#channelId);
  }

  #createPropertyChange(
    gom: vGomInterface.VirtualGameObjectModel,
    nodeId: string,
    name: string,
    property: BABYLON.Vector3 | BABYLON.Quaternion
  ) {
    if (property._isDirty === false) {
      return;
    }
    if (property instanceof BABYLON.Quaternion) {
      property = property.toEulerAngles();
    }
    gom.createPropertyChange(nodeId, {
      name,
      type: 'vector3',
      value: property,
    });
    property._isDirty = false;
  }

  async createChangeSerializable() {
    const gom = new this.#binding.VirtualGameObjectModel();
    for (const node of this.#watchingNodes) {
      this.#createPropertyChange(gom, node.__vgoGuid, 'position', node.position);
      if (node.rotationQuaternion != null) {
        this.#createPropertyChange(gom, node.__vgoGuid, 'rotation', node.rotationQuaternion);
      } else {
        this.#createPropertyChange(gom, node.__vgoGuid, 'rotation', node.rotation);
      }
      this.#createPropertyChange(gom, node.__vgoGuid, 'scale', node.scaling);

      if (node instanceof BABYLON.AbstractMesh) {
        /**
         * The material details are synced by other place, so we only need to sync the material reference guid here.
         */
        if (node.material) {
          gom.createPropertyChange(node.__vgoGuid, {
            name: 'materialReferenceGuid',
            type: 'string',
            value: `${node.material.uniqueId}`,
          });
        }

        /**
         * Vertex Syncing:
         * 
         * - When the mesh is marked as updateable, we should mark it as dirty.
         * - When the mesh has skeleton, we should mark it as dirty, then it will sync the vertex data to Unity side.
         * - When the mesh has morph target, we should mark it as dirty, then it will sync the vertex data to Unity side.
         */
        let isMeshDirty = false;
        if (node.geometry) {
          /**
           * When the mesh is marked as updateable, we should mark it as dirty.
           */
          const isPositionUpdatable = node.geometry.isVertexBufferUpdatable(BABYLON.VertexBuffer.PositionKind);
          const isNormalUpdatable = node.geometry.isVertexBufferUpdatable(BABYLON.VertexBuffer.NormalKind);
          if (isPositionUpdatable || isNormalUpdatable) {
            isMeshDirty = true;
          }
        }
        /**
         * When the mesh has skeleton, we should mark it as dirty, then it will sync the vertex data to Unity side.
         */
        if (node.skeleton) {
          isMeshDirty = true;
        }
        /**
         * When the mesh has morph target, we should mark it as dirty, then it will sync the vertex data to Unity side.
         */
        if (node.morphTargetManager && node.morphTargetManager.numTargets > 0) {
          isMeshDirty = true;
        }

        if (isMeshDirty === true) {
          const triangles = toIndicesArray(node.getIndices());
          let positionVertexData = node.getPositionData(true, true);
          let normalsVertexData = node.getNormalsData(true, true);
          if (Array.isArray(positionVertexData)) {
            positionVertexData = new Float32Array(positionVertexData);
          }
          if (Array.isArray(normalsVertexData)) {
            normalsVertexData = new Float32Array(normalsVertexData);
          }
          gom.createVerticesSyncChange(
            node.__vgoGuid,
            triangles,
            {
              'position': positionVertexData,
              'normals': normalsVertexData,
              // TODO: support other vertex data such as colors, uvs, etc.
            }
          );
        }
      }

      // update outline properties
      if (node instanceof BABYLON.AbstractMesh) {
        let value: string;
        if (!node.renderOutline) {
          value = '{"enabled":false}';
        } else {
          const color = node.outlineColor;
          value = JSON.stringify({
            enabled: true,
            width: node.outlineWidth,
            color: [color.r, color.g, color.b],
          });
        }
        gom.createPropertyChange(node.__vgoGuid, {
          name: 'outline',
          type: 'string',
          value,
        });
      }

      // update material properties
      if (node instanceof BABYLON.AbstractMesh && node.material) {
        await this.serializeMaterial(node.material, gom);
      }
    }

    // Update the last change serializable and return.
    this.#lastChangeSerializable = gom;
    return gom;
  }

  /**
   * Generate the mesh unique id for the given scene, because the Babylonjs mesh id is not unique.
   * @param {BABYLON.Scene} scene 
   */
  #generateGuids(scene: BABYLON.Scene) {
    for (const node of scene.getNodes()) {
      if (!node[VGO_GUID_SYMBOL]) {
        // Check if "jsardom.guid" in node's metadata, if not, generate a new one.
        if (node.metadata?.['jsardom.guid']) {
          node[VGO_GUID_SYMBOL] = node.metadata['jsardom.guid'];
        } else {
          node[VGO_GUID_SYMBOL] = BABYLON.Tools.RandomId();
        }
      }
    }
  }

  /**
   * Serialize all transform nodes in the given scene.
   * @param {BABYLON.Scene} scene
   */
  serializeTransformNodes(scene) {
    for (const node of scene.transformNodes) {
      this.serializeTransformNode(node);
    }
  }

  /**
   * Serialize all meshes in the given scene.
   * @param {BABYLON.Scene} scene
   */
  serializeAllMeshes(scene) {
    for (const mesh of scene.meshes) {
      this.serializeMesh(mesh);
    }
  }

  /**
   * Serialize a transform node.
   * @param {BABYLON.TransformNode} node 
   */
  serializeTransformNode(node: BABYLON.TransformNode) {
    if (node.isEnabled(false) === false) {
      return;
    }
    const vGo = this.#gameObjectModel.createGameObjectAsChild(node['__vgoGuid'], node);

    // Handle with extension parts.
    const extendNode = node as any;
    if (extendNode.isBound) {
      vGo.data.asBounds();
    }

    // append this node to the watching list
    this.#watchingNodes.push(node as any);
  }

  /**
   * Serialize a mesh.
   * @param {BABYLON.Mesh} mesh 
   */
  serializeMesh(mesh: BABYLON.Mesh) {
    if (mesh.isEnabled(false) === false) {
      return;
    }

    const vGo = this.#gameObjectModel.createGameObjectAsChild((mesh as any).__vgoGuid, mesh);
    if (vGo == null) {
      return;
    }

    /**
     * Babylon.js doesn't convert the vertex data to left-handed system, so we need to check the "right-handed-system" tag
     * here to determine whether we need to convert the triangle winding order.
     * 
     * The "right-handed-system" tag is added when we load the gltf/glb file, which are in right-handed system.
     */
    const useRightHandedSystem = BABYLON.Tags.MatchesQuery(mesh, 'right-handed-system');
    const triangles = mesh.getIndices();
    let positionVertexData = mesh.getVerticesData(BABYLON.VertexBuffer.PositionKind);
    let normalsVertexData = mesh.getVerticesData(BABYLON.VertexBuffer.NormalKind);
    let colorsVertexData = mesh.getVerticesData(BABYLON.VertexBuffer.ColorKind);
    let uvVertexData = mesh.getVerticesData(BABYLON.VertexBuffer.UVKind);

    if (triangles && positionVertexData != null) {
      /**
       * FIXME(): The right-handed system should keep the triangle winding order, otherwise the mesh will be flipped.
       */
      const trianglesArray = toIndicesArray(triangles, !useRightHandedSystem);
      if (Array.isArray(positionVertexData)) {
        positionVertexData = new Float32Array(positionVertexData);
      }

      vGo.data.setMeshTrianglesData(trianglesArray);
      vGo.data.setMeshVertexBuffer('position', positionVertexData);

      if (normalsVertexData) {
        if (Array.isArray(normalsVertexData)) {
          normalsVertexData = new Float32Array(normalsVertexData);
        }
        vGo.data.setMeshVertexBuffer('normals', normalsVertexData);
      }

      if (colorsVertexData) {
        if (Array.isArray(colorsVertexData)) {
          colorsVertexData = new Float32Array(colorsVertexData);
        }
        vGo.data.setMeshVertexBuffer('colors', colorsVertexData);
      }

      if (uvVertexData) {
        if (Array.isArray(uvVertexData)) {
          uvVertexData = new Float32Array(uvVertexData);
        }
        vGo.data.setMeshVertexBuffer('uv', uvVertexData);
      }
      vGo.data.computeAndSetMeshBuffers();
    }

    // Set line rendering properties if the type is line mesh.
    const meshClass = mesh.getClassName();
    if (meshClass === 'LinesMesh') {
      const linesMesh = mesh as BABYLON.LinesMesh;
      vGo.data.setLineRenderingColors(linesMesh.color);
    } else if (meshClass === 'GreasedLineMesh') {
      const greasedLineMesh = mesh as BABYLON.GreasedLineMesh;
      vGo.data.setLineRenderingColors(greasedLineMesh.greasedLineMaterial.color);
    }

    if (mesh.hasBoundingInfo) {
      vGo.data.setMeshBounds(mesh.getRawBoundingInfo());
    }
    if (mesh.isBlocker) {
      vGo.data.asBlocker();
    }

    if (mesh.renderOutline) {
      vGo.data.setMeshOutline(true, {
        color: mesh.outlineColor,
        width: mesh.outlineWidth,
      });
    }

    if (mesh.skeleton) {
      this.serializeMeshSkeletonAndBones(mesh, vGo);
    }

    // append this node to the watching list
    this.#watchingNodes.push(mesh as any);
  }

  async serializeMaterial(material: BABYLON.Material, targetGom?: vGomInterface.VirtualGameObjectModel) {
    const customType = material.getClassName();
    const vMaterial = await this.createVirtualMaterial(material);
    if (targetGom) {
      targetGom.createMaterialSyncChange(`${material.uniqueId}`, customType, vMaterial);
    } else {
      this.#gameObjectModel.createMaterialSyncChange(`${material.uniqueId}`, customType, vMaterial);
    }
  }

  /**
   * Serialize the skeleton and bones into the virtual game object.
   */
  serializeMeshSkeletonAndBones(mesh: BABYLON.Mesh, vGo: vGomInterface.VirtualGameObject) {
    const boneWeights = mesh.getVerticesData(BABYLON.VertexBuffer.MatricesWeightsKind);
    const boneIndexes = mesh.getVerticesData(BABYLON.VertexBuffer.MatricesIndicesKind);

    if (boneWeights.length !== boneIndexes.length) {
      throw new Error('The bone weights and bone indexes are not matched.');
    }
    for (let i = 0; i < boneWeights.length; i += 4) {
      vGo.data.addBoneWeights(boneWeights[i], boneWeights[i + 1], boneWeights[i + 2], boneWeights[i + 3]);
      vGo.data.addBoneIndices(boneIndexes[i], boneIndexes[i + 1], boneIndexes[i + 2], boneIndexes[i + 3]);
    }
    for (const bone of mesh.skeleton.bones) {
      /**
       * Skip if the bone index is -1 because it's invalid.
       */
      if (bone.getIndex() < 0) {
        /**
         * TODO: warn the user that the bone index has an invalid value?
         */
        continue;
      }
      const matrix = new this.#binding.VirtualMatrix(4, 4);
      bone.getBindMatrix().toArray().forEach((v, i) => {
        matrix.values[i] = v;
      });
      matrix.update();  /** update the underlaying matrix values */
      vGo.data.addBindPose(matrix);
      vGo.data.addBoneReference(bone.getTransformNode()['__vgoGuid']);
    }
  }

  #markObjectAsDirty(obj: any, isDirty: boolean) {
    obj[DIRTY_SYMBOL] = isDirty;
  }

  #isObjectDirty(obj: any) {
    const dirtyValue = obj[DIRTY_SYMBOL];
    if (dirtyValue === undefined) {
      /** If this value is not set, we treat it as dirty */
      return true;
    } else {
      return dirtyValue;
    }
  }

  /**
   * Create the virtual texture(to be serialized and pass to Unity side) from the Babylonjs's Texture object. This function distincts the dynamic texture 
   * and static texture:
   * 
   * - When creating from the static texture, it will serialize the texture data into base64 string.
   * - When creating from the dynamic texture, it will serialize the canvas context.
   * 
   * @param texture 
   * @returns 
   */
  async createVirtualTexture(baseTexture: BABYLON.BaseTexture) {
    const vTexture = new this.#binding.VirtualTexture(baseTexture.name);
    vTexture.guid = `${baseTexture.uniqueId}`;

    const textureType = baseTexture.getClassName();
    const internalTexture = baseTexture.getInternalTexture() as TransmuteInternalTexture;
    if (!this.#isObjectDirty(baseTexture) && !this.#isObjectDirty(internalTexture)) {
      // Only if the texture and internal texture are not dirty, we could skip.
      return vTexture;
    }

    try {
      switch (textureType) {
        case 'Texture':
          {
            const texture = baseTexture as BABYLON.Texture;
            const pixels = await texture.readPixels();  /** read pixels */
            if (pixels != null) {
              vTexture.setPixels(new Uint8Array(pixels.buffer, pixels.byteOffset, pixels.byteLength));
            } else if (texture.url) {
              vTexture.setSourceUrl(texture.url);
            } else {
              logger.info(`No pixels and url found.`, texture);
            }
            vTexture.invertY = texture.invertY;

            // Update height & width after calling `readPixels()` which update texture size by its type.
            const textureSize = baseTexture.getSize();
            vTexture.height = textureSize.height;
            vTexture.width = textureSize.width;
          }
          break;
        case 'DynamicTexture':
        case 'InteractiveDynamicTexture':
          {
            const { width, height } = baseTexture.getSize();
            const texture = baseTexture as BABYLON.DynamicTexture;
            const canvasContext = texture.getContext();   /** read pixels by context2d */
            const imageData = canvasContext.getImageData(0, 0, width, height);
            vTexture.height = imageData.height;
            vTexture.width = imageData.width;

            const pixels = new Uint8Array(imageData.data.buffer, imageData.data.byteOffset, imageData.data.byteLength);
            vTexture.setPixels(pixels);
            vTexture.invertY = texture.invertY;

            /** 
             * Update canvas dirty state.
             * 
             * NOTE: This is not working at browser, because the standard `OffscreenCanvas` doesn't implement the dirty-check feature.
             */
            if (typeof canvasContext['markAsNotDirty'] === 'function') {
              (canvasContext as any).markAsNotDirty();
            }
          }
          break;
        default:
          logger.warn(`The texture(${baseTexture.name}) is not supported to serialize, type=${baseTexture.getClassName()}`, baseTexture);
          break;
      }
    } catch (err) {
      logger.error(`failed to update texture properties:`, err);
    } finally {
      // When we finished a texture passing, we should mark the texture and its internal to be not dirty.
      this.#markObjectAsDirty(baseTexture, false);
      this.#markObjectAsDirty(internalTexture, false);
    }
    return vTexture;
  }

  /**
   * Create an instance of VirtualMaterial, which could be used for bootstrapping and changes sync.
   * @param material 
   * @param disableTexture
   * @returns 
   */
  async createVirtualMaterial(material: BABYLON.Material) {
    const customType = material.getClassName();
    const vMaterial = new this.#binding.VirtualMaterial(material);
    vMaterial.setAlpha(material.alpha);
    vMaterial.setAlphaMode(material.alphaMode);
    vMaterial.setWireframe(material.wireframe);

    /**
     * Setup for transparency mode for surface type.
     */
    if (typeof material.transparencyMode === 'number') {
      if (material.transparencyMode === BABYLON.Material.MATERIAL_OPAQUE) {
        vMaterial.setSurfaceType(0);
      } else {
        vMaterial.setSurfaceType(3);
      }
    }

    switch (customType) {
      case 'StandardMaterial':
        const standardMatProps = material as BABYLON.StandardMaterial;
        vMaterial.setStandardDiffuseColor(standardMatProps.diffuseColor);
        vMaterial.setStandardSpecularColor(standardMatProps.specularColor);
        vMaterial.setStandardEmissiveColor(standardMatProps.emissiveColor);
        vMaterial.setStandardAmbientColor(standardMatProps.ambientColor);
        if (standardMatProps.diffuseTexture) {
          vMaterial.setStandardDiffuseTexture(await this.createVirtualTexture(standardMatProps.diffuseTexture));
        }
        if (standardMatProps.specularTexture) {
          vMaterial.setStandardSpecularTexture(await this.createVirtualTexture(standardMatProps.specularTexture));
        }
        if (standardMatProps.emissiveTexture) {
          vMaterial.setStandardEmissiveTexture(await this.createVirtualTexture(standardMatProps.emissiveTexture));
        }
        if (standardMatProps.ambientTexture) {
          vMaterial.setStandardAmbientTexture(await this.createVirtualTexture(standardMatProps.ambientTexture));
        }
        break;
      case 'PBRMaterial':
        const pbrMatProps = material as BABYLON.PBRMaterial;
        vMaterial.setAlbedoColor(pbrMatProps.albedoColor);
        vMaterial.setAmbientColor(pbrMatProps.ambientColor);
        vMaterial.setEmissiveColor(pbrMatProps.emissiveColor);
        if (pbrMatProps.albedoTexture) {
          vMaterial.setAlbedoTexture(await this.createVirtualTexture(pbrMatProps.albedoTexture));
        }
        if (pbrMatProps.ambientTexture) {
          vMaterial.setAmbientTexture(await this.createVirtualTexture(pbrMatProps.ambientTexture));
        }
        if (pbrMatProps.emissiveTexture) {
          const vTexture = await this.createVirtualTexture(pbrMatProps.emissiveTexture);
          vMaterial.setEmissiveTexture(vTexture);
        }
        if (typeof pbrMatProps.metallic === 'number') {
          vMaterial.setMetallic(pbrMatProps.metallic);
        }
        if (typeof pbrMatProps.roughness === 'number') {
          vMaterial.setRoughness(pbrMatProps.roughness);
        }
        break;
      case 'ShaderMaterial':
        /**
         * We don't support shader material for now.
         */
        break;
      /**
       * TODO(Yorkie): other types of material system for babylonjs.
       */
      default:
        console.info(`Skip the specific material setup, material=${customType}(#${material.name})`);
        console.info('The material details is', material);
    }
    return vMaterial;
  }
}
