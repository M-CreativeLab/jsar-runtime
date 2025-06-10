import path from 'node:path';
import crypto from 'node:crypto';
import fsPromises from 'node:fs/promises';
import { resolveObjectURL } from 'node:buffer';

import {
  type ResourceLoader as JSARResourceLoader,
} from '@yodaos-jsar/dom';
import { getClientContext, isResourcesCachingDisabled, getResourceCacheExpirationTime } from '@transmute/env';
import * as undici from 'undici';
import { CacheControl, parse as parseCacheControl } from 'cache-control-parser';
import { IncomingHttpHeaders } from 'undici/types/header';

type FetchReturnsMap = {
  json: object;
  string: string;
  arraybuffer: ArrayBuffer;
};
type FetchOptions = {
  accept?: string;
  cookieJar?: any;
  referrer?: string;
  headers?: undici.HeadersInit;
};

function hash(algorithm: 'md5' | 'sha256', content: string | NodeJS.ArrayBufferView) {
  return crypto.createHash(algorithm)
    .update(content)
    .digest('hex');
}
const getHashOfUri = (uri: string) => hash('md5', uri);
const getOrigin = (url: string) => {
  let origin: string = '';
  try {
    let urlObject = new URL(url);
    origin = urlObject.hostname;
    // TODO(yorkie): support the port?
  } catch (_) {
    origin = 'anonymous';
    console.warn(`Use anonymous domain for the URL: ${url}`);
  }
  return origin;
};

/**
 * Parse the URL string is valid to create a new `URL` object.
 * @param url 
 * @returns 
 */
function canParseURL(url: string): boolean {
  try {
    new URL(url);
    return true;
  } catch (_e) {
    return false;
  }
}

/**
 * Make a new `Response` object from the given `ArrayBuffer`.
 * @param arraybuffer 
 * @param fromResp 
 * @returns 
 */
function makeResponse(arraybuffer: ArrayBuffer | Buffer,
  fromResp: Response = new Response(null, { status: 200, statusText: 'OK' })): Response {
  return new Response(arraybuffer, {
    status: fromResp.status,
    statusText: fromResp.statusText,
    headers: fromResp.headers,
  });
}

class Cache {
  #metadata: {
    [key: string]: string
  };
  #cacheControl: CacheControl;

  constructor() {
    this.#metadata = {};
  }

  protected initMetadata(metadata: { [key: string]: string }) {
    this.#metadata = metadata;

    if (this.#metadata['cache-control']) {
      this.#cacheControl = parseCacheControl(this.#metadata['cache-control']);
    }
  }

  is(control: 'no-cache' | 'no-store' | 'must-revalidate' | 'max-age'): boolean {
    if (!this.#cacheControl) {
      return false;
    } else if (control === 'no-cache') {
      return this.#cacheControl['no-cache'];
    } else if (control === 'no-store') {
      return this.#cacheControl['no-store'];
    } else if (control === 'must-revalidate') {
      return this.#cacheControl['must-revalidate'];
    } else if (control === 'max-age') {
      return this.#cacheControl['max-age'] > 0;
    } else {
      throw new TypeError(`Unknown cache control: ${control}`);
    }
  }

  expired(): boolean {
    // TODO(yorkie): support the `max-age` and `s-maxage` cache control.
    if (this.#metadata['expires']) {
      const expires = new Date(this.#metadata['expires']).getTime();
      return expires < Date.now();
    }

    // By default, we need to use heuristic caching to check if the resource is expired.
    if (this.has('last-modified') && this.has('date')) {
      const dateTime = this.date().getTime();
      const maxAge = (dateTime - this.lastModified().getTime()) * 0.1;
      return dateTime + maxAge < Date.now();
    }

    // By default, it's expired.
    return true;
  }

  has(key: 'last-modified' | 'date' | 'etag'): boolean {
    return this.#metadata[key] !== undefined;
  }

  lastModified(): Date {
    return new Date(this.#metadata['last-modified']);
  }

  date(): Date {
    return new Date(this.#metadata['date']);
  }

  etag(): string {
    return this.#metadata['etag'];
  }
}

class ReadableCache extends Cache {
  static async FromFile(filename: string): Promise<ReadableCache | null> {
    const cache = new ReadableCache(filename);
    const metadataPath = `${filename}.metadata`;
    try {
      const metadataFileStat = await fsPromises.stat(metadataPath);
      if (!metadataFileStat.isFile()) {
        throw new Error('The metadata file is not a file.');
      }
      const metadata = JSON.parse(await fsPromises.readFile(metadataPath, 'utf8'));
      cache.initMetadata(metadata);
    } catch (_) {
      // Ignore the error
    }
    return cache;
  }

  constructor(public filename: string) {
    super();
  }

  async readContentAsText(): Promise<string> {
    return fsPromises.readFile(this.filename, 'utf8');
  }

  async readContentAsBuffer(): Promise<Buffer> {
    return fsPromises.readFile(this.filename);
  }
}

type ResponseCacheInfo<D> = {
  useCache: boolean;
  storeCache: boolean;
  responseData: D & {
    statusCode?: number,
    headers?: Headers | IncomingHttpHeaders,
  };
};
type ResponseContentCallback<D> = (content: NodeJS.ArrayBufferView | string, info: ResponseCacheInfo<D>) => void;

function buildCacheInfoFrom<D>(response: ResponseCacheInfo<D>['responseData']): ResponseCacheInfo<D> {
  let useCache = response.statusCode === 304;
  let storeCache = true;
  if (!useCache && response.headers['cache-control']) {
    const cacheControl = parseCacheControl(response.headers['cache-control'].toString());
    if (cacheControl['no-cache']) {
      storeCache = false;
    }
  }

  return {
    useCache,
    storeCache,
    responseData: response,
  };
}

class CacheStorage {
  #rootDirectory: string;
  #localExpirationTime: number = getResourceCacheExpirationTime();
  #disabled: boolean = isResourcesCachingDisabled();

  constructor(rootDirectory: string) {
    this.#rootDirectory = rootDirectory;
  }

  async #ensureDir(dir: string) {
    try {
      const dirStat = await fsPromises.stat(dir);
      if (!dirStat.isDirectory()) {
        throw new TypeError(`The cache directory "${dir}" must be a directory.`);
      }
    } catch (_err) {
      await fsPromises.mkdir(dir, { recursive: true });
    }
  }

  async open() {
    if (this.#disabled) {
      return;
    }
    await this.#ensureDir(this.#rootDirectory);
  }

  async get(url: string): Promise<ReadableCache | null> {
    if (this.#disabled) {
      return null;
    }

    const key = getHashOfUri(url);
    const filename = path.join(this.#rootDirectory, getOrigin(url), key);
    try {
      const fstats = await fsPromises.stat(filename);
      if (!fstats.isFile()) {
        if (fstats.isDirectory()) {
          await fsPromises.rmdir(filename, { recursive: true });
        } else {
          await fsPromises.unlink(filename);
        }
        throw new Error('The cached file is not a file.');
      }
      if (fstats.mtimeMs < (Date.now() - this.#localExpirationTime)) {
        await fsPromises.unlink(filename);
        throw new Error('The cached file is expired.');
      }
      return ReadableCache.FromFile(filename);
    } catch (e) {
      if (e?.code !== 'ENOENT') {
        console.warn(`Failed to get Cache object for "${url}"`, e);
      }
    }
    return null;
  }

  async put(url: string, headers: any, content?: NodeJS.ArrayBufferView | string): Promise<void> {
    if (this.#disabled) {
      return;
    }

    const key = getHashOfUri(url);
    const originDir = path.join(this.#rootDirectory, getOrigin(url));
    await this.#ensureDir(originDir); // Ensure the origin directory exists.

    const filenames = {
      content: path.join(originDir, key),
      metadata: path.join(originDir, `${key}.metadata`),
      md5file: path.join(originDir, `${key}.md5`),
    };

    let fields: Record<string, string>;
    if (!headers || headers == null) {
      fields = {};
    } else if (typeof headers['entries'] === 'function') {
      fields = Object.fromEntries(headers.entries());
    } else if (Array.isArray(headers)) {
      fields = headers.reduce((obj, item, index) => {
        obj[index] = item;
        return obj;
      }, {} as Record<string, string>);
    } else {
      fields = headers;
    }

    let freshMetadata: { [key: string]: string } = {
      url,
      ...fields,
    };

    const writes: Array<Promise<void>> = [];
    if (content) {
      const contentMd5 = hash('md5', content);

      // Check the content's md5 hash.
      if (fields['content-md5']) {
        // If the content-md5 is provided, we need to check if the content's md5 hash is matched.
        const expectedMd5 = Buffer.from(fields['content-md5'], 'base64').toString('hex');
        if (expectedMd5 !== contentMd5) {
          console.warn(`The content(${url})'s md5 hash is not matched: ${expectedMd5} != ${contentMd5}`);
          return;
        }
      }

      writes.push(fsPromises.writeFile(filenames.content, content));
      writes.push(fsPromises.writeFile(filenames.md5file, contentMd5));

      freshMetadata['contentHash'] = contentMd5;
      writes.push(fsPromises.writeFile(filenames.metadata, JSON.stringify(freshMetadata, null, 2), 'utf8'));
    } else {
      // If the content is not provided, it means updating the metadata only.
      let localMetadata: { [key: string]: string };
      try {
        const metadataText = await fsPromises.readFile(filenames.metadata, 'utf8');
        localMetadata = JSON.parse(metadataText);
      } catch (_err) {
        localMetadata = {};
      }
      freshMetadata = {
        ...localMetadata,
        ...fields,
      };
      writes.push(fsPromises.writeFile(filenames.metadata, JSON.stringify(freshMetadata, null, 2), 'utf8'));
    }
    await Promise.all(writes);
  }

  async delete(url: string): Promise<void> {
    if (this.#disabled) {
      return;
    }
    // TODO(yorkie): implement a LRU to delete the cache.
  }

  async requestWithCache<R, D>(
    url: string,
    requestInit: RequestInit,
    impl: {
      readFile: (filename: string) => Promise<R>,
      sendRequest: (url: string, init: RequestInit) => Promise<ResponseCacheInfo<D>['responseData']>,
      readResponse: (info: ResponseCacheInfo<D>, url: string, onContentLoaded: ResponseContentCallback<D>) => Promise<R>
    }
  ): Promise<R> {
    const requestDirectly = async (url: string, init: RequestInit) => {
      const sentAt = performance.now();
      const resp = await impl.sendRequest(url, init);
      const body = await impl.readResponse(buildCacheInfoFrom(resp), url, (content, info) => {
        if (info.storeCache) {
          this.put(url, resp.headers, content);
        }
      });
      const elapsed = performance.now() - sentAt;
      console.info(`Loaded(forced) "${url}" in ${elapsed}ms`);
      return body;
    };

    if (this.#disabled) {
      return requestDirectly(url, requestInit);
    }

    const cache = await this.get(url);
    if (cache == null || !cache) {
      return requestDirectly(url, requestInit);
    }

    // Skip the cache if the 
    if (cache.is('no-store')) {
      return requestDirectly(url, requestInit);
    }

    if (cache.expired()) {
      const newInit: RequestInit = { ...requestInit };
      const putHeaderOnOptions = (key: string, value: string) => {
        if (!newInit.headers) {
          newInit.headers = {};
        }
        newInit.headers[key] = value;
      };
      if (cache.has('etag')) {
        putHeaderOnOptions('if-none-match', cache.etag());
      }
      if (cache.has('last-modified')) {
        putHeaderOnOptions('if-modified-since', cache.lastModified().toUTCString());
      }

      const sentAt = performance.now();
      const response = await impl.sendRequest(url, newInit);
      const info = buildCacheInfoFrom(response);

      if (info.useCache) {
        // The resource is not modified, so use the cached content.
        const r = impl.readFile(cache.filename);
        const elapsed = performance.now() - sentAt;
        console.info(`Loaded(from cache) "${url}" in ${elapsed}ms`);
        if (info.storeCache) {
          // Update the cache metadata only.
          this.put(url, response.headers);
        }
        return r;
      } else {
        const r = await impl.readResponse(info, url, (content, info) => {
          if (info.storeCache) {
            this.put(url, response.headers, content);
          }
        });
        const elapsed = performance.now() - sentAt;
        console.info(`Loaded "${url}" in ${elapsed}ms`);
        return r;
      }
    } else {
      return impl.readFile(cache.filename);
    }
  }
}

export class ResourceLoaderOnTransmute implements JSARResourceLoader {
  #cacheDirectory: string;
  #cacheStorage: CacheStorage;
  #defaultHeaders: Record<string, string> = {};
  #networkProxyAgent: undici.ProxyAgent;

  constructor() {
    this.#cacheDirectory = path.join(getClientContext().applicationCacheDirectory, '.res_cache');
    this.#cacheStorage = new CacheStorage(this.#cacheDirectory);
    this.#cacheStorage.open();

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
    options: RequestInit,
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
      return this.#readBlob(blob as Blob, returnsAs);
    }

    /**
     * Check if the URL is a data URL, and directly return the decoded content.
     */
    if (url.startsWith('data:')) {
      return this.#readDataURL(url, returnsAs);
    }

    /**
     * Resolve the relative URL when the URL is not starting with `http:`, `https:` or `file:`.
     */
    if (
      typeof globalThis.document !== 'undefined' &&
      (
        !url.startsWith('http:') &&
        !url.startsWith('https:') &&
        !url.startsWith('file:')
      )
    ) {
      url = new URL(url, globalThis.document.baseURI).href;
    }

    /**
     * Check if this URL is a valid URL.
     */
    if (!canParseURL(url)) {
      throw new TypeError(`Failed to fetch: Invalid URL("${url}")`);
    }
    const urlObj = new URL(url);
    if (urlObj.protocol === 'file:') {
      return this.#readFile(urlObj.pathname, returnsAs);
    } else {
      return this.#cacheStorage.requestWithCache(url, this.#getRequestInit(options), {
        readFile: (filename: string) => this.#readFile(filename, returnsAs),
        sendRequest: (url: string, init: RequestInit) => this.#sendRequest(url, init),
        readResponse: (...args) => this.#readBody(...args, returnsAs),
      });
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
    const forceFetch = fetch;  // Save the Node.js fetch implementation.

    return async function fetch(input: RequestInfo, init?: RequestInit): Promise<Response> {
      /**
       * TODO(yorkie): Support CORS.
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

      return self.#cacheStorage.requestWithCache(urlObj.href, self.#getRequestInit(init), {
        readFile: async (filename) => makeResponse(await self.#readFile(filename, 'arraybuffer')),
        sendRequest: forceFetch,
        readResponse: async (info, _url, onContentReady) => {
          const resp = info.responseData;
          if (resp.ok) {
            let arraybuffer: ArrayBuffer;
            try {
              arraybuffer = await resp.arrayBuffer();
              onContentReady(new Uint8Array(arraybuffer), info);
              return makeResponse(arraybuffer, resp);
            } catch (err) {
              console.warn(`Failed to read response body: ${err}`);
            }
          }
          return resp;
        }
      });
    }
  }

  setDefaultHeaders(rawHeaders: string) {
    // Parsing the raw headers into an object.
    if (rawHeaders) {
      const headers = rawHeaders.split('\r\n');
      for (const header of headers) {
        const [key, value] = header.split(':').map(s => s.trim());
        if (key && value) {
          this.#defaultHeaders[key] = value;
        }
      }
    }
  }

  #getRequestInit(init: RequestInit = {}): RequestInit {
    if (this.#defaultHeaders && Object.keys(this.#defaultHeaders).length > 0) {
      if (!init.headers) {
        init.headers = {};
      } else if (Array.isArray(init.headers)) {
        init.headers = Object.fromEntries(init.headers);
      }

      // Merge the custom headers into the request headers.
      for (const [key, value] of Object.entries(this.#defaultHeaders)) {
        if (value !== undefined && value !== null) {
          init.headers[key] = value;
        }
      }
    }
    return init;
  }

  async #sendRequest(url: string, options: FetchOptions): Promise<undici.Dispatcher.ResponseData> {
    const reqInit: undici.RequestInit = {
      ...(options == null ? {} : options),
    };
    const responseData = await undici.request(url, <any>{
      maxRedirections: 5,
      dispatcher: this.#networkProxyAgent || undici.getGlobalDispatcher(),
      ...reqInit
    });

    if (responseData.statusCode >= 400) {
      throw new Error(`Failed to fetch(${url}), statusCode=${responseData.statusCode}`);
    } else {
      return responseData;
    }
  }

  async #readBody<AsType extends keyof FetchReturnsMap>(
    info: ResponseCacheInfo<undici.Dispatcher.ResponseData>,
    requestUrl: string,
    onContentLoaded: ResponseContentCallback<undici.Dispatcher.ResponseData>,
    returnsAs: AsType
  ): Promise<FetchReturnsMap[AsType]> {
    const { responseData: response } = info;
    if (returnsAs === 'arraybuffer') {
      const arraybuffer = await response.body.arrayBuffer();
      onContentLoaded(new Uint8Array(arraybuffer), info);
      return arraybuffer as FetchReturnsMap[AsType];
    } else {
      const text = await response.body.text();
      onContentLoaded(text, info);
      if (returnsAs === 'string') {
        return text as FetchReturnsMap[AsType];
      } else if (returnsAs === 'json') {
        let obj: any;
        try {
          obj = JSON.parse(text);
        } catch (_) { }
        return obj as FetchReturnsMap[AsType];
      } else {
        throw new TypeError(`Unknown return type: "${returnsAs}"`);
      }
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
    const byteChars = isBase64 ? atob(data) : decodeURIComponent(data);
    const byteArray = new Uint8Array(byteChars.length);
    for (let i = 0; i < byteChars.length; i++) {
      byteArray[i] = byteChars.charCodeAt(i);
    }
    const blob = new Blob([byteArray], { type: mimeType });
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

  #readBinaryFile(pathname: string): Promise<Buffer> {
    return fsPromises.readFile(pathname);
  }
}
