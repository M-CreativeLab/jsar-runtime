import Jimp from 'jimp';

import { getLinkedBinding } from '../../bindings/gom';
import { executeWithTimeProfiler } from '../common/utils';
import type * as vGomInterface from '../transmute/vgom/interface';
import { DIRTY_SYMBOL, MIME_TYPE_SYMBOL } from './common';
import { DocumentMetadata, GameObjectModelSerializer } from './serializer';
import { decode as decodeWebp } from './libwebp';
import {
  createWebGLContext,
} from '../../webgl';

/**
 * FIXME(Yorkie): this is to resolve the issue that Jimp is not a constructor in the browser env.
 * This workaround just fetches from window.Jimp when the environment is in browser.
 */
let JimpConstructor: typeof Jimp;
if (!!Jimp.prototype && Jimp.prototype.constructor === Jimp) {
  JimpConstructor = Jimp;
} else {
  JimpConstructor = (window as any).Jimp;
}

type EngineOptions = Partial<{
  /**
   * The native binding for GOM which encodes the scene data.
   */
  binding: vGomInterface.Binding;
  /**
   * Mark this environment is Unity Native.
   */
  isUnityNative: boolean;
  /**
   * Is this engine is for headless, in this mode, the serialization will be disabled.
   */
  headless: boolean;
  /**
   * The callback when GOM buffer is updated.
   * @param buffer 
   */
  onBufferUpdate: (buffer: ArrayBuffer) => void;
}>;

type OnReadyOptions = Partial<{
  title: string;
  metadata: DocumentMetadata;
}>;

type TransmuteInternalTexture = BABYLON.InternalTexture & {
  /** Used to decode texture buffer into pixels */
  [MIME_TYPE_SYMBOL]?: string;
  /** Used to control which pixels should be written */
  [DIRTY_SYMBOL]?: boolean;
};

class TransmuteEngine extends BABYLON.NullEngine {
  /**
   * The event target used by the engine.
   */
  #eventTarget: EventTarget;

  /**
   * The serializer for serializing the scene's structured Game Objects.
   */
  #serializer: GameObjectModelSerializer;

  /**
   * The native binding instance, which supports multiple environments: unity(linked) and vscode extension.
   */
  #binding: vGomInterface.Binding = null;

  /**
   * If this engine is able to run.
   */
  #runnable = false;

  /**
   * The scene to be watched for generate the changes buffer.
   */
  #watchingScene: BABYLON.Scene = null;

  /**
   * The headless mode will disable the serialization, this engine only serves as a storage for scene.
   */
  readonly #headless;

  constructor(channelId: string, options?: EngineOptions) {
    super();
    this.name = 'TransmuteEngine';

    // Enable capabilities
    this._features.needShaderCodeInlining = true;
    this._caps.supportSRGBBuffers = false;  // disable srgb buffers

    // TODO: Enable the audio engine later when it is ready.
    // BABYLON.Engine.audioEngine = BABYLON.Engine.AudioEngineFactory(this.getRenderingCanvas(), this.getAudioContext(), this.getAudioDestination());
    createWebGLContext(1, 1, null);

    // Shader related options
    this.disableUniformBuffers = true;
    this._webGLVersion = 2;
    this._shaderPlatformName = 'JSAR';
    this._shaderProcessor = new BABYLON.WebGL2ShaderProcessor();

    this.#eventTarget = new EventTarget();
    if (options?.headless === true) {
      this.#headless = true;
    } else {
      this.#headless = false;
    }

    // Set serializer.
    this.#serializer = new GameObjectModelSerializer(channelId);

    // Set `binding` option
    if (options?.binding) {
      this.#binding = options.binding;
    } else if (this.#headless === false) {
      // Only if the engine is not headless and no binding is provided, we will try to get the linked binding.
      this.#binding = getLinkedBinding();
    }
    this.#serializer.binding = this.#binding;

    // Set `onBufferUpdate` option
    if (options?.onBufferUpdate && typeof options.onBufferUpdate === 'function') {
      this.#serializer.onGomBufferUpdate = options.onBufferUpdate;
    }

    // Reset the state
    this.reset();
  }

  get isHeadless() {
    return this.#headless === true;
  }

  addEventListener(type: string, listener: EventListenerOrEventListenerObject, options?: boolean | AddEventListenerOptions) {
    return this.#eventTarget.addEventListener(type, listener, options);
  }

  async onReady(scene: BABYLON.Scene, options?: OnReadyOptions) {
    console.log('JSAR runtime is ready, trying to start bootstrap the scene.');

    // Update the metadata
    this.#serializer.addMetadataInfo(options?.title, options?.metadata);

    // Add a virtual camera for rendering the scene.
    new BABYLON.ArcRotateCamera('Camera', Math.PI / 2, Math.PI / 4, 3, BABYLON.Vector3.Zero(), scene);

    // Only if the engine is not headless, we will serialize the scene and write to the buffers.
    if (this.#headless === false) {
      await executeWithTimeProfiler('serialize scene', () => this.#serializer.serializeScene(scene));
    }
    this.#watchingScene = scene;

    // Send the `GOMContentLoaded` event to the frontend.
    this.#eventTarget.dispatchEvent(new Event('GOMContentLoaded'));
    this.#runnable = true; /** Set the runnable=true to mark this time is able to start loop */
  }

  /**
   * This watches the changes and upate the changes-buffer to the native engine.
   */
  watchChanges() {
    if (this.#runnable === false) {
      throw new TypeError('The engine is not ready yet.');
    }
    const scene = this.#watchingScene;

    /**
     * We use this variable to avoid the infinite loop when the last change buffer is not consumed.
     */
    let triesToWaitBufferEmpty = 0;
    let serializingOrWriting = false;

    /**
     * Watching changes means we need to use Babylonjs's render loop to update the change buffers.
     */
    this.runRenderLoop(async () => {
      /**
       * Check the last change buffer is consumed or not, if not we will wait for the next frame, and we add a max tries 
       * constraint to avoid the infinite loop.
       */
      if (!this.#serializer.isLastChangeBufferEmpty() &&
        triesToWaitBufferEmpty < 10) {
        triesToWaitBufferEmpty += 1;
        return;
      }
      triesToWaitBufferEmpty = 0; /** Reset the retries number when buffer is empty */

      /**
       * If the serializing or buffer writing task is not finished, we will skip this frame.
       */
      if (serializingOrWriting === true) {
        return;
      }

      // create the change buffer before animate or render.
      serializingOrWriting = true;
      try {
        const target = await this.#serializer.createChangeSerializable();
        scene.render(false, false);

        // write vgom buffer only if the target is ok and non headless mode.
        if (target != null && !this.#headless) {
          // write vgom buffer
          this.#serializer.serializeAndWrite(target);
        }
      } catch (_err) {
        // eslint-disable-next-line no-console
        // TODO: report this error?
      } finally {
        serializingOrWriting = false;
      }
    });
  }

  stopWatchingChanges() {
    this.#runnable = false;
    this.stopRenderLoop();
  }

  /**
   * Reset the engine states and timers.
   */
  reset() {
    this.stopWatchingChanges();
    this.#serializer.reset();
  }

  /**
   * Dispose the engine gracefully.
   */
  dispose() {
    super.dispose();
    this.reset();

    // dispose audio context
    // this._audioContext.close();
  }

  /**
     * Gets the list of loaded textures
     * @returns an array containing all loaded textures
     */
  getLoadedTexturesCache() {
    /**
     * We don't need texture cache and avoid texture corruptions.
     */
    return [];
  }

  createTexture(
    urlArg: string,
    noMipmap: boolean,
    invertY: boolean,
    scene: BABYLON.ISceneLike,
    samplingMode?: number,
    onLoad?: (texture: BABYLON.InternalTexture) => void,
    onError?: (message: string, exception: any) => void,
    buffer?: string | ArrayBuffer | ArrayBufferView | HTMLImageElement | Blob | ImageBitmap,
    fallback?: BABYLON.InternalTexture,
    format?: number,
    forcedExtension?: string,
    mimeType?: string
  ): BABYLON.InternalTexture {
    const texture = super.createTexture(
      urlArg,
      noMipmap,
      invertY,
      scene,
      samplingMode,
      onLoad,
      onError,
      buffer,
      fallback,
      format,
      forcedExtension,
      mimeType) as TransmuteInternalTexture;
    texture._buffer = buffer;
    texture.type = BABYLON.Engine.TEXTURETYPE_UNSIGNED_INT;
    texture.format = BABYLON.Engine.TEXTUREFORMAT_RGBA;
    texture[MIME_TYPE_SYMBOL] = mimeType;
    texture[DIRTY_SYMBOL] = true;
    return texture;
  }

  /**
   * NOTE: This method will be called when texture.update is called or every rebuilding.
   */
  updateDynamicTexture(texture: TransmuteInternalTexture, canvas: unknown, _invertY: boolean, _premulAlpha?: boolean, _format?: number): void {
    const offscreenCanvas = canvas as OffscreenCanvas;
    const context2d = offscreenCanvas.getContext('2d');
    /**
     * Check if the context 2d owns the dirty-check method, otherwise we will fallback to the default behavior, that means we will update the dynamic 
     * texture in every time, it looks okay for the development platform like VSCode or Preview in browser.
     */
    if (typeof context2d['isDirty'] === 'function') {
      const context2dImpl = context2d as any;
      const isContextDirty = context2dImpl.isDirty() as boolean;
      texture[DIRTY_SYMBOL] = isContextDirty;
    } else {
      // fallback behavior: always mark as dirty.
      texture[DIRTY_SYMBOL] = true;
    }

    /**
     * Nothing to do here, we will update the texture in Serializer.
     */
    return;
  }

  /**
   * This enables developer could update the raw texture because we mark it as dirty when this function gets called.
   */
  updateRawTexture(texture: BABYLON.InternalTexture, data: ArrayBufferView, format: number, invertY: boolean, compression?: string, type?: number, useSRGBBuffer?: boolean): void {
    super.updateRawTexture(texture, data, format, invertY, compression, type, useSRGBBuffer);
    texture[DIRTY_SYMBOL] = true;
  }

  async _readTexturePixels(
    texture: TransmuteInternalTexture,
    _width: number,
    _height: number,
    _faceIndex?: number,
    _level?: number,
    _buffer?: ArrayBufferView,
    _flushRenderer?: boolean,
    _noDataConversion?: boolean,
    _x?: number,
    _y?: number
  ): Promise<ArrayBufferView> {
    /**
     * If the texture is created in RawTexture, just returns the buffer directly.
     */
    if (texture.source === BABYLON.InternalTextureSource.Raw) {
      return texture._bufferView || null;
    }

    /**
     * Otherwise, decode the image data to pixels.
     */
    // Check if the _buffer is ArrayBufferView, otherwise we will skip the decoding.
    let mimeType = texture[MIME_TYPE_SYMBOL];
    let rawBuffer = texture._buffer;
    if (!ArrayBuffer.isView(rawBuffer) && texture.url) {
      /**
       * TODO: not blocking the reading pixels.
       */
      const textureResp = await fetch(texture.url);
      rawBuffer = new Uint8Array(await textureResp.arrayBuffer());
      mimeType = textureResp.headers.get('Content-Type');
    }
    if (!ArrayBuffer.isView(rawBuffer)) {
      return null;
    }

    const imageBuffer = new Uint8Array(rawBuffer.buffer, rawBuffer.byteOffset, rawBuffer.byteLength);
    let image: Jimp;

    switch (mimeType) {
      case 'image/jpeg':
      case 'image/png':
        image = await JimpConstructor.read(Buffer.from(imageBuffer));
        break;
      case 'image/webp':
        const webpImageData = await decodeWebp(imageBuffer);
        image = new JimpConstructor(webpImageData.width, webpImageData.height);
        for (let y = 0; y < webpImageData.height; y++) {
          for (let x = 0; x < webpImageData.width; x++) {
            const index = (y * webpImageData.width + x) * 4;
            const rgba = JimpConstructor.rgbaToInt(
              webpImageData.data[index],
              webpImageData.data[index + 1],
              webpImageData.data[index + 2],
              webpImageData.data[index + 3]
            );
            image.setPixelColor(rgba, x, y);
          }
        }
        break;
      default:
        throw new TypeError(`Unsupported texture format: ${mimeType}`);
    }
    if (image?.bitmap) {
      texture.height = image.bitmap.height;
      texture.width = image.bitmap.width;
      return image.bitmap.data;
    } else {
      return null;
    }
  }

  _readTexturePixelsSync(
    _texture: BABYLON.InternalTexture,
    _width: number,
    _height: number,
    _faceIndex?: number,
    _level?: number,
    _buffer?: ArrayBufferView,
    _flushRenderer?: boolean,
    _noDataConversion?: boolean,
    _x?: number,
    _y?: number
  ): ArrayBufferView {
    throw new TypeError('Not implemented yet.');
  }
}

export {
  vGomInterface,
  TransmuteEngine,
  TransmuteInternalTexture,
}
