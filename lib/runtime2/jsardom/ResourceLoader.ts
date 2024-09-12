import path from 'node:path';
import crypto from 'node:crypto';
import fsPromises from 'node:fs/promises';
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

  constructor() {
    if (this.#isCachingEnabled) {
      this.#cacheDirectory = path.join(this.#clientContext.applicationCacheDirectory, '.res_cache');
      fsPromises.mkdir(this.#cacheDirectory, { recursive: true })
        .catch((err) => {
          console.warn('failed to create cache directory', err);
        });
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
    const resp = await undici.request(url, <any>{ maxRedirections: 5, ...reqInit });
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
      const resp = await undici.request(`${resourceUri}.md5`, {
        headersTimeout: 500,
        bodyTimeout: 1000
      });
      if (resp.statusCode === 200) {
        const onlineMd5 = await resp.body.text();  // server-side in base64 encoded.
        const localMd5 = Buffer.from(await this.#readTextFile(`${cachePath}.md5`), 'hex').toString('base64');
        if (onlineMd5 !== localMd5) {
          useCache = false;
        }
      }
    } catch (err) {
      console.warn(`failed to fetch the md5 file for ${resourceUri}, the error is: ${err}`);
    }

    if (useCache) {
      console.info(`use the cached resource(${cachePath}) for "${resourceUri}".`);
    } else {
      console.info(`use the online resource for "${resourceUri}".`);
    }
    return useCache;
  }
}
