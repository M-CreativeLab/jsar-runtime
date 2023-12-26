import os from 'os';
import { type SpatialDocumentImpl, JSARDOM, JSARInputEvent } from '@yodaos-jsar/dom';

import { TransmuteEngine } from './babylonjs-engine/engine';
import * as logger from '../bindings/logger';
import * as env from '../bindings/env';
import * as messaging from '../bindings/messaging';
import { TransmuteNativeDocument } from './impl-transmute';

// Patch to Babylon.js
if (BABYLON.Scene.prototype) {
  const sceneProto = BABYLON.Scene.prototype as any;
  if (!sceneProto.sortActiveAnimatables) {
    sceneProto.sortActiveAnimatables = function (): void {
      this._activeAnimatables.sort((a: any, b: any) => {
        return a.playOrder - b.playOrder;
      });
    };
  }
}

class ReadyContext {
  screenHeight: number;
  screenWidth: number;
  applicationCacheDirectory: string;

  static FromString(s: string) {
    const ctx = new ReadyContext();
    try {
      const envObject = JSON.parse(s);
      ctx.screenHeight = envObject.ScreenHeight || 800;
      ctx.screenWidth = envObject.ScreenWidth || 800;
      ctx.applicationCacheDirectory = envObject.ApplicationCacheDirectory || os?.tmpdir();
      logger.info(`created a runtime context`, {
        screenHeight: ctx.screenHeight,
        screenWidth: ctx.screenWidth,
        applicationCacheDirectory: ctx.applicationCacheDirectory,
      });
    } catch (err) {
      console.warn(`failed to parse the env string "${s}", the parsing error is: ${err}`);
    }
    return ctx;
  }
}

type RuntimeInitOptions = {
  onReadTextFile: (pathname: string) => Promise<string>;
  onReadBinaryFile: (pathname: string) => Promise<ArrayBuffer>;
  onWriteConsoleLine?: (line: string) => void;
  isResourceCached?: (runtime: TransmuteRuntime, uri: string) => Promise<[boolean, string?]>;
  cacheResource?: (runtime: TransmuteRuntime, uri: string, content: string | NodeJS.ArrayBufferView) => Promise<void>;
}

type XRPoseInit = {
  position: DOMPointInit;
  rotation: DOMPointInit;
};

type ScriptRunOptions = {
  channelId: string;
  containerPose: XRPoseInit;
  url?: string;
  cwd?: string;
  filename?: string;
  onError?: (err: Error) => void;
};

export class DocumentContentLoadedEvent extends CustomEvent<SpatialDocumentImpl> {
  constructor(document: SpatialDocumentImpl) {
    super('documentcontentloaded', { detail: document });
  }
}

/**
 * The JSAR(Transmute) runtime.
 */
export class TransmuteRuntime extends EventTarget {
  #runtimeInit: RuntimeInitOptions;
  #engineInit: ConstructorParameters<typeof TransmuteEngine>[1];
  #readyContext: ReadyContext;
  #openedDocuments: JSARDOM<TransmuteNativeDocument>[] = [];

  constructor(runtimeInitOptions: RuntimeInitOptions, engineInitOptions?: ConstructorParameters<typeof TransmuteEngine>[1]) {
    super();

    this.#runtimeInit = runtimeInitOptions;
    this.#engineInit = engineInitOptions;
    this.#readyContext = ReadyContext.FromString(env.getReadyContext());

    messaging.addEventListener('dispose', () => {
      logger.info('Received dispose event, stopping the runtime instance.');
      this.stop();
    });
    messaging.addEventListener('load', (e: CustomEvent) => {
      try {
        const data = JSON.parse(e.detail);
        this.#onload(data.uri, data.channelId, data.containerPose);
      } catch (error) {
        this.dispatchEvent(new ErrorEvent('error', { error }));
        logger.error(`Failed to parse the load event data: ${error?.stack || error}`);
      }
    });
    messaging.addEventListener('firstFrameReady', (e: CustomEvent) => {
      const channelId = e.detail;
      if (channelId) {
        const dom = this.#openedDocuments.find(doc => doc.id === channelId);
        if (dom) {
          const engine = dom.document.scene.getEngine() as unknown as TransmuteEngine;
          engine.watchChanges();
        }
      }
    });
    messaging.addEventListener('context', this.#createDocumentEventHandler((dom, data) => {
      if (data.containerPose) {
        const pose = data.containerPose as XRPoseInit;
        dom.nativeDocument._setContainerPose(dom, pose.position, pose.rotation);
      }
    }));
    messaging.addEventListener('input', (messageEvent: CustomEvent) => {
      try {
        let eventToDispatch: JSARInputEvent;
        const data = JSON.parse(messageEvent.detail);
        switch (data['Type']) {
          case 'mouse':
            break;
          case 'raycast':
            {
              const detail = data['Data'];
              let uvCoord: BABYLON.Vector2;
              if (detail['UvCoord']) {
                const { x, y } = detail['UvCoord'];
                uvCoord = new BABYLON.Vector2(x, y);
              }
              eventToDispatch = new JSARInputEvent('raycast', {
                sourceId: detail['RaycastSourceId'],
                sourceType: detail['RaycastSourceType'],
                targetSpatialElementInternalGuid: detail['TargetGuid'],
                uvCoord,
              });
            }
            break;
          case 'handpress':
          case 'handrelease':
            {
              const detail = data['Data'];
              eventToDispatch = new JSARInputEvent('raycast_action', {
                type: data['Type'] === 'handpress' ? 'down' : 'up',
                sourceId: detail['HandType'],
              });
            }
            break;
          case 'handtracking':
            {
              const detail = data['Data'];
              eventToDispatch = new JSARInputEvent('handtracking', {
                handId: detail['Type'],
                joints: detail['Joints'],
                pose: detail['ThisPose'],
                gesture: detail['Gesture'],
                orientation: detail['Orientation'],
              });
            }
            break;
          default:
            logger.info(`Received an unknown input event: ${messageEvent.detail}`);
            break;
        }

        if (eventToDispatch) {
          // TODO: should we return the value by the documents?
          for (const dom of this.#openedDocuments) {
            dom.dispatchInputEvent(eventToDispatch);
          }
        }
      } catch (err) {
        logger.warn(`failed to parse input event message: ${messageEvent}(${typeof messageEvent}})`);
        logger.warn(err?.stack || err?.message || err);
        return false;
      }
    });
  }

  get documents() {
    return this.#openedDocuments;
  }

  get readyContext() {
    return this.#readyContext;
  }

  start() {
    env.markRuntimeAvailable(`version=${process.env['JSAR_VERSION']},jsardom=${JSARDOM.version},babylonjs=${BABYLON.Engine.Version}`);
  }

  load(url: string, channelId: string, containerPose: XRPoseInit) {
    return this.#onload(url, channelId, containerPose);
  }

  /**
   * Read the text file.
   * @param pathname the file path.
   * @returns the file content.
   */
  readTextFile(pathname: string): Promise<string> {
    return this.#runtimeInit.onReadTextFile(pathname);
  }

  /**
   * Read the binary file.
   * @param pathname the file path.
   * @returns the file content.
   */
  readBinaryFile(pathname: string): Promise<ArrayBuffer> {
    return this.#runtimeInit.onReadBinaryFile(pathname);
  }

  async isResourceCached(uri: string): Promise<[boolean, string?]> {
    return this.#runtimeInit.isResourceCached ? await this.#runtimeInit.isResourceCached(this, uri) : [false];
  }

  async cacheResource(uri: string, content: string | NodeJS.ArrayBufferView) {
    return this.#runtimeInit.cacheResource ? await this.#runtimeInit.cacheResource(this, uri, content) : undefined;
  }

  /**
   * Should use the cached resource or not.
   * @param resourceUri the origin resource uri.
   * @param cachePath the cached resource path.
   * @returns check their md5 values and return true if they are the same or something went wrong.
   */
  async shouldUseResourceCache(resourceUri: string, cachePath: string): Promise<boolean> {
    if (!resourceUri) {
      throw new TypeError('resourceUri is required');
    }
    if (!cachePath) {
      throw new TypeError('cachePath is required');
    }

    let useCache = true;
    try {
      const resp = await fetch(`${resourceUri}.md5`);
      if (resp.ok) {
        const onlineMd5 = await resp.text();  // server-side in base64 encoded.
        const localMd5 = Buffer.from(await this.readTextFile(`${cachePath}.md5`), 'hex').toString('base64');
        if (onlineMd5 !== localMd5) {
          useCache = false;
        }
      }
    } catch (err) {
      logger.warn(`failed to fetch the md5 file for ${resourceUri}, the error is: ${err}`);
    }

    if (useCache) {
      logger.info(`use the cached resource(${cachePath}) for "${resourceUri}".`);
    } else {
      logger.info(`use the online resource for "${resourceUri}".`);
    }
    return useCache;
  }

  writeConsoleLine(line: string) {
    if (typeof this.#runtimeInit.onWriteConsoleLine === 'function') {
      return this.#runtimeInit.onWriteConsoleLine(line);
    }
  }

  /**
   * Run the XSML document, it returns a promise which will be resolved when the xsml document is loaded and serialized for
   * the first frame.
   *
   * @param url the XSML document URL.
   * @param options the options for running the XSML document.
   */
  async runXsml(url: string, options: ScriptRunOptions): Promise<JSARDOM<TransmuteNativeDocument>> {
    let activeDom: JSARDOM<TransmuteNativeDocument>;

    /** Reset the engine firstly */
    // this.#engine.reset();

    /** 
     * Distroy the present documents.
     * TODO: mutiple documents support?
     */
    if (this.#openedDocuments.length > 0) {
      if (options.channelId === undefined) {
        for (let dom of this.#openedDocuments) {
          await dom.unload();
        }
        this.#openedDocuments = [];
      } else {
        this.#openedDocuments = this.#openedDocuments.filter(dom => {
          if (dom.id === options.channelId) {
            dom.unload();
            return false;
          } else {
            return true;
          }
        });
      }
    }

    try {
      const nativeDocument = new TransmuteNativeDocument(options.channelId, {
        runtime: this,
        engineInit: this.#engineInit,
      });
      const dom = new JSARDOM(url, {
        id: options.channelId,
        nativeDocument,
      });
      this.#openedDocuments.push(dom);
      await dom.load();
      logger.info(`XSML document is loaded: ${dom.id}, basePath: ${dom.document.baseURI}`);

      // FIXME: resolving the ready states for multiple documents.
      await dom.waitForSpaceReady();  // wait for the space ready.
      await nativeDocument.ready(dom.document);

      // Dispatch the `documentcontentloaded` event?
      this.dispatchEvent(new DocumentContentLoadedEvent(dom.document));
      activeDom = dom;

    } catch (e) {
      if (typeof options?.onError === 'function') {
        options.onError(e);
      } else {
        throw e;
      }
    }
    return activeDom;
  }

  /**
   * Stop the runtime.
   */
  async stop() {
    const docSize = this.#openedDocuments.length;
    logger.info(`Stopping the Transmute Runtime (still ${docSize} documents).`);

    // Dispose the opened documents.
    await Promise.all(
      this.#openedDocuments.map(async dom => {
        await dom.unload();
      })
    );
    this.#openedDocuments = [];
    logger.info('Transmute Runtime exited.');

    // Dispose the messaging system.
    messaging.dispose();
  }

  /**
   * Handle the loading document event, it returns the loaded document if success.
   * @param url the document url.
   * @param channelId the channel id, it is used for the communication between the main thread and the worker thread.
   * @returns the loaded document.
   */
  async #onload(url: string, channelId: string, containerPose: XRPoseInit): Promise<JSARDOM<TransmuteNativeDocument>> {
    let dom: JSARDOM<TransmuteNativeDocument>;
    let errorOccurred = false;
    let message: string;

    try {
      logger.info(`start loading XSML url=${url}`);
      dom = await this.runXsml(url, {
        channelId,
        containerPose,
      });
    } catch (e) {
      message = `${e.stack || e.message || e}`.slice(0, 256);
      logger.error(`Failed to load xsml: ${url}, the error: ${message}`);
      errorOccurred = true;
    } finally {
      // Checking for JSAR_RUN_MODE for the later behaviour of running.
      if (process.env.JSAR_RUN_MODE === 'EXIT_ON_FIRST_FRAME') {
        logger.info(`JS_RUN_MODE is EXIT_ON_FIRST_FRAME, exiting because the first frame is rendered or occurs some errors.`);
        this.stop();

        if (errorOccurred) {
          throw new TypeError(`Failed to load XSML(${url}), error: ${message}.`);
        }
      } else {
        // If error occured and owns a channel id, mark the gom buffer as errored to tell the channel buffer is errored.
        if (errorOccurred && channelId) {
          env.markGomBufferAsErrored(channelId, 1);
        }
      }
    }
    return dom;
  }

  #createDocumentEventHandler(fn: (dom: JSARDOM<TransmuteNativeDocument>, data: any) => void) {
    return (e: CustomEvent) => {
      try {
        const data = JSON.parse(e.detail);
        const channelId = data.channelId;
        if (channelId) {
          const document = this.#openedDocuments.find(doc => doc.id === channelId);
          if (document) {
            fn(document, data);
          }
        }
      } catch (err) {
        logger.error(`Failed to parse the event data: ${err}`, e.detail);
      }
    };
  }
}
