import path from 'node:path';
import crypto from 'node:crypto';
import fsPromises from 'node:fs/promises';
import { resolveObjectURL } from 'node:buffer';

import {
  type ResourceLoader as JSARResourceLoader,
} from '@yodaos-jsar/dom';
import { getClientContext, isResourcesCachingDisabled, getResourceCacheExpirationTime } from '@transmute/env';
import * as undici from 'undici';

type FetchReturnsMap = {
  json: object;
  string: string;
  arraybuffer: ArrayBuffer;
};
type FetchOptions = {
  accept?: string;
  cookieJar?: any;
  referrer?: string;
};

function hash(algorithm: 'md5' | 'sha256', content: string | NodeJS.ArrayBufferView) {
  return crypto.createHash(algorithm)
    .update(content)
    .digest('hex');
}
const getHashOfUri = (uri: string) => hash('sha256', uri);

function canParseURL(url: string): boolean {
  try {
    new URL(url);
    return true;
  } catch (_e) {
    return false;
  }
}

export class ResourceLoaderOnTransmute implements JSARResourceLoader {
  #clientContext = getClientContext();
  /**
   * The flag indicates if the resources caching is enabled.
   */
  #isCachingEnabled = !isResourcesCachingDisabled();
  #cacheExpirationTime = getResourceCacheExpirationTime();
  #cacheDirectory: string;
  #networkProxyAgent: undici.ProxyAgent;

  constructor() {
    if (this.#isCachingEnabled) {
      this.#cacheDirectory = path.join(this.#clientContext.applicationCacheDirectory, '.res_cache');
      fsPromises.mkdir(this.#cacheDirectory, { recursive: true })
        .catch((err) => {
          console.warn('failed to create cache directory', err);
        });
    }

    let proxyAddr: string = '';
    if (process.env['https_proxy']) {
      proxyAddr = process.env['https_proxy'];
    } else if (process.env['http_proxy']) {
      proxyAddr = process.env['http_proxy'];
    } else if (process.env['all_proxy']) {
      proxyAddr = process.env['all_proxy'];
    }
    if (proxyAddr !== '') {
      this.#networkProxyAgent = new undici.ProxyAgent(proxyAddr);
    }
  }

  /**
   * Fetch a resource from the network or file system.
   * 
   * @param url the resource URL.
   * @param options fetch options.
   * @param returnsAs expected return type.
   * @returns 
   */
  async fetch<AsType extends keyof FetchReturnsMap>(
    url: string,
    options: FetchOptions,
    returnsAs?: AsType
  ): Promise<FetchReturnsMap[AsType]> {
    if (typeof url !== 'string') {
      throw new TypeError('The URL must be a string, but got ' + typeof url);
    }

    /**
     * Check if the URL is a blob URL, and directly return the response from the resolved `Blob` object.
     */
    if (url.startsWith('blob:')) {
      const blob = resolveObjectURL(url);
      if (!blob) {
        throw new TypeError('Failed to resolve the blob URL: ' + url);
      }
      return this.#readBlob(blob, returnsAs);
    }

    /**
     * Check if the URL is a data URL, and directly return the decoded content.
     */
    if (url.startsWith('data:')) {
      return this.#readDataURL(url, returnsAs);
    }

    /**
     * Check if this URL is a valid URL.
     */
    if (!canParseURL(url)) {
      throw new TypeError(`Failed to fetch: Invalid URL ${url}`);
    }
    const urlObj = new URL(url);
    if (urlObj.protocol === 'file:') {
      return this.#readFile(urlObj.pathname, returnsAs);
    } else {
      if (!this.#isCachingEnabled) {
        return this.#requestFile(url, options, returnsAs);
      }
      const [isCached, cachedUrl] = await this.#isResourceCached(url);
      if (isCached && await this.#shouldUseResourceCache(url, cachedUrl)) {
        return this.#readFile(cachedUrl, returnsAs);
      } else {
        return this.#requestFile(url, options, returnsAs);
      }
    }
  }

  /**
   * Create a WHATWG fetch implementation.
   * 
   * @param baseURI The base URI to be used with relative URLs and CORS checks.
   * @returns the fetch(input, init?) function.
   */
  createWHATWGFetchImpl(baseURI: string): (input: RequestInfo, init?: RequestInit) => Promise<Response> {
    const self: ResourceLoaderOnTransmute = this;
    const fetchImpl = fetch;  // Save the Node.js fetch implementation.

    return async function fetch(input: RequestInfo, init?: RequestInit): Promise<Response> {
      /**
       * TODO: Support CORS.
       */
      let urlObj: URL;
      const url = typeof input === 'string' ? input : input.url;
      if (url.startsWith('http:') || url.startsWith('https:')) {
        urlObj = new URL(url);
      } else if (url.startsWith('/') || url.startsWith('./') || url.startsWith('../')) {
        urlObj = new URL(url, baseURI);
      } else {
        throw new TypeError(`Failed to fetch: Invalid URL ${input}`);
      }

      if (!self.#isCachingEnabled) {
        return fetchImpl(url, init);
      }
      const [isCached, cachedUrl] = await self.#isResourceCached(url);
      if (isCached && await self.#shouldUseResourceCache(url, cachedUrl)) {
        const cache: ArrayBuffer = await self.#readBinaryFile(cachedUrl);
        const readable = new ReadableStream({
          start(controller) {
            controller.enqueue(cache);
            controller.close();
          },
        });
        return new Response(readable, {
          status: 200,
          statusText: 'OK',
        });
      } else {
        return fetchImpl(url, init);
      }
    }
  }

  /**
   * Make a network request to fetch a given resource file.
   */
  async #requestFile<AsType extends keyof FetchReturnsMap>(
    url: string,
    options: FetchOptions,
    returnsAs?: AsType
  ): Promise<FetchReturnsMap[AsType]> {
    const reqInit: undici.RequestInit = {
      ...(options == null ? {} : options),
    };
    const resp = await undici.request(url, <any>{
      maxRedirections: 5,
      dispatcher: this.#networkProxyAgent || undici.getGlobalDispatcher(),
      ...reqInit
    });
    if (resp.statusCode >= 400) {
      throw new Error(`Failed to fetch(${url}), statusCode=${resp.statusCode}`);
    }

    if (returnsAs === 'string') {
      const str = await resp.body.text();
      this.#cacheResource(url, str);
      return str as FetchReturnsMap[AsType];
    } else if (returnsAs === 'json') {
      const obj = await resp.body.json() as any;
      this.#cacheResource(url, obj);
      return obj as FetchReturnsMap[AsType];
    } else if (returnsAs === 'arraybuffer') {
      const buf = await resp.body.arrayBuffer();
      this.#cacheResource(url, new Uint8Array(buf));
      return buf as FetchReturnsMap[AsType];
    }
  }

  /**
   * Read blob content with the specified return type.
   * @param blob the blob object.
   * @param returnsAs expected return type.
   * @returns the expected content.
   */
  async #readBlob<AsType extends keyof FetchReturnsMap>(blob: Blob, returnsAs: AsType): Promise<FetchReturnsMap[AsType]> {
    switch (returnsAs) {
      case 'string':
        return blob.text() as Promise<FetchReturnsMap[AsType]>;
      case 'json':
        return JSON.parse(await blob.text()) as FetchReturnsMap[AsType];
      case 'arraybuffer':
        return blob.arrayBuffer() as Promise<FetchReturnsMap[AsType]>;
      default:
        throw new TypeError(`Unknown return type: "${returnsAs}"`);
    }
  }

  /**
   * Read data URL such as `data:image/png;base64,...`.
   * @param url the data URL.
   * @param returnsAs expected return type.
   * @returns the expected content.
   */
  async #readDataURL<AsType extends keyof FetchReturnsMap>(url: string, returnsAs: AsType): Promise<FetchReturnsMap[AsType]> {
    const m = url.match(/^data:(.*?)(;base64)?,(.*)/);
    if (!m) {
      throw new TypeError(`Invalid data URL: ${url}`);
    }
    const [, mimeType, isBase64, data] = m;
    const blob = new Blob([
      isBase64 ? atob(data) : decodeURIComponent(data)],
      { type: mimeType });
    return this.#readBlob(blob, returnsAs);
  }

  /**
   * Read a file's content from the file system.
   * @param pathname the file pathname.
   * @param returnsAs expected return type.
   * @returns 
   */
  #readFile<AsType extends keyof FetchReturnsMap>(
    pathname: string,
    returnsAs: AsType
  ): Promise<FetchReturnsMap[AsType]> {
    /**
     * FIXME: This is a workaround for the issue that the path of the file URL on Windows starts with a slash.
     */
    let isWin: boolean;
    if (typeof navigator !== 'undefined') {
      isWin = navigator.platform?.indexOf('Win') > -1;
    } else {
      isWin = process.platform === 'win32';
    }
    if (isWin && pathname.startsWith('/')) {
      pathname = pathname.slice(1);
    }
    if (returnsAs === 'string') {
      return this.#readTextFile(pathname) as Promise<FetchReturnsMap[AsType]>;
    } else if (returnsAs === 'json') {
      return this.#readTextFile(pathname)
        .then((text) => JSON.parse(text)) as Promise<FetchReturnsMap[AsType]>;
    } else if (returnsAs === 'arraybuffer') {
      return this.#readBinaryFile(pathname) as Promise<FetchReturnsMap[AsType]>;
    } else {
      throw new TypeError(`Unknown return type: "${returnsAs}"`);
    }
  }

  #readTextFile(pathname: string): Promise<string> {
    return fsPromises.readFile(pathname, 'utf8');
  }

  #readBinaryFile(pathname: string): Promise<ArrayBuffer> {
    return fsPromises.readFile(pathname);
  }

  /**
   * Check if a resource is cached in the cache directory.
   * @param uri the resource uri
   * @returns the cached path and whether the resource is cached.
   */
  async #isResourceCached(uri: string): Promise<[boolean, string?]> {
    if (!this.#isCachingEnabled) {
      throw new TypeError('Disallow to check cache hit when caching is disabled.');
    }
    const cachedPath = path.join(this.#cacheDirectory, getHashOfUri(uri));
    try {
      const fstats = await fsPromises.stat(cachedPath);
      if (fstats.isDirectory()) {
        fsPromises.rmdir(cachedPath, { recursive: true });
        throw new Error('The cached file is a directory.');
      }
      if (fstats.mtimeMs < Date.now() - this.#cacheExpirationTime) {
        fsPromises.unlink(cachedPath);
        throw new Error('The cached file is expired.');
      }
      if (!fstats.isFile()) {
        fsPromises.unlink(cachedPath);
        throw new Error('The cached file is not a file.');
      }
      return [true, cachedPath];
    } catch (_err) {
      return [false];
    }
  }

  /**
   * Cache a resource to the cache directory.
   * @param uri the resource uri
   * @param content the resource content
   */
  async #cacheResource(uri: string, content: string | NodeJS.ArrayBufferView) {
    if (this.#isCachingEnabled === false) {
      return; // Don't cache if the caching is disabled.
    }
    const cacheDir = this.#cacheDirectory;
    const filename = getHashOfUri(uri);
    const contentPath = path.join(cacheDir, filename);
    const md5filePath = path.join(cacheDir, `${filename}.md5`);
    await Promise.all([
      fsPromises.writeFile(contentPath, content),
      fsPromises.writeFile(md5filePath, hash('md5', content)),
    ]);
  }

  /**
   * Check if the resource's cache should be used.
   * 
   * It first fetches the md5 value from the server-side, https://example.com/resource.md5, and compares it with the
   * local md5 value. If they are the same, the local cache is used; otherwise, the online resource is used.
   * 
   * @param resourceUri the resource uri
   * @param cachePath the cache path
   * @returns 
   */
  async #shouldUseResourceCache(resourceUri: string, cachePath: string): Promise<boolean> {
    if (!resourceUri) {
      throw new TypeError('resourceUri is required');
    }
    if (!cachePath) {
      throw new TypeError('cachePath is required');
    }

    let useCache = true;
    try {
      const resp = await (new Promise<undici.Dispatcher.ResponseData>((resolve, reject) => {
        /**
         * FIXME: I do this because the undici.request() seems not supporting custom timeout.
         */
        const requestTimer = setTimeout(() => reject(new Error('request timeout')), 1000);
        undici.request(`${resourceUri}.md5`, { dispatcher: this.#networkProxyAgent || undici.getGlobalDispatcher() }).then((data) => {
          clearTimeout(requestTimer);
          resolve(data);
        }, (err) => {
          clearTimeout(requestTimer);
          reject(err);
        });
      }));

      /**
       * Check if the MD5 values are same.
       */
      if (resp.statusCode === 200) {
        const [remote, local] = await Promise.all([
          resp.body.text(),
          this.#readTextFile(`${cachePath}.md5`)
        ]);
        if (remote !== Buffer.from(local, 'hex').toString('base64')) {
          useCache = false;
        }
      }
    } catch (err) {
      console.warn(`failed to fetch the md5 file for ${resourceUri}, the error is: ${err}`);
    }
    return useCache;
  }
}
