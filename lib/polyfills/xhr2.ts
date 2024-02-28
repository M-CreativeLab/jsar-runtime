import * as http from 'node:http';
import * as https from 'node:https';
import * as os from 'node:os';
import * as url from 'node:url';
import { resolveObjectURL } from 'node:buffer';

import * as logger from '../bindings/logger';

class SecurityError extends Error { }
class InvalidStateError extends Error { }
class NetworkError extends Error { }

class ProgressEventImpl {
  type: any;
  target: any;
  currentTarget: any;
  lengthComputable: boolean;
  loaded: number;
  total: number;

  constructor(type) {
    this.type = type;
    this.target = null;
    this.currentTarget = null;
    this.lengthComputable = false;
    this.loaded = 0;
    this.total = 0;
  }
};

class XMLHttpRequestEventTargetImpl implements XMLHttpRequestEventTarget {
  onloadstart: (this: XMLHttpRequest, ev: ProgressEvent<EventTarget>) => any;
  onabort: (this: XMLHttpRequest, ev: ProgressEvent<EventTarget>) => any;
  onerror: (this: XMLHttpRequest, ev: ProgressEvent<EventTarget>) => any;
  onload: (this: XMLHttpRequest, ev: ProgressEvent<EventTarget>) => any;
  onloadend: (this: XMLHttpRequest, ev: ProgressEvent<EventTarget>) => any;
  onprogress: (this: XMLHttpRequest, ev: ProgressEvent<EventTarget>) => any;
  ontimeout: (this: XMLHttpRequest, ev: ProgressEvent<EventTarget>) => any;
  _listeners: { [key: string]: Array<(this: XMLHttpRequest, ev: ProgressEvent<EventTarget>) => any> } = {};

  // @private
  // This is an abstract class and should not be instantiated directly.
  constructor() {
    this.onloadstart = null;
    this.onprogress = null;
    this.onabort = null;
    this.onerror = null;
    this.onload = null;
    this.ontimeout = null;
    this.onloadend = null;
  }

  // Adds a new-style listener for one of the XHR events.

  // @see http://www.w3.org/TR/XMLHttpRequest/#events

  // @param {String} eventType an XHR event type, such as 'readystatechange'
  // @param {function(ProgressEvent)} listener function that will be called when
  //   the event fires
  // @return {undefined} undefined
  addEventListener(eventType: string, listener) {
    var base;
    eventType = eventType.toLowerCase();
    (base = this._listeners)[eventType] || (base[eventType] = []);
    this._listeners[eventType].push(listener);
    return void 0;
  }

  // Removes an event listener added by calling addEventListener.

  // @param {String} eventType an XHR event type, such as 'readystatechange'
  // @param {function(ProgressEvent)} listener the value passed in a previous
  //   call to addEventListener.
  // @return {undefined} undefined
  removeEventListener(eventType: string, listener) {
    let index: number;
    eventType = eventType.toLowerCase();
    if (this._listeners[eventType]) {
      index = this._listeners[eventType].indexOf(listener);
      if (index !== -1) {
        this._listeners[eventType].splice(index, 1);
      }
    }
    return void 0;
  }

  // Calls all the listeners for an event.

  // @param {ProgressEvent} event the event to be dispatched
  // @return {undefined} undefined
  dispatchEvent(event) {
    let eventType, j, len, listener, listeners;
    event.currentTarget = event.target = this;
    eventType = event.type;
    if (listeners = this._listeners[eventType]) {
      for (j = 0, len = listeners.length; j < len; j++) {
        listener = listeners[j];
        listener.call(this, event);
      }
    }
    if (listener = this[`on${eventType}`]) {
      listener.call(this, event);
    }
    return void 0;
  }
}

export class XMLHttpRequestUploadImpl extends XMLHttpRequestEventTargetImpl implements XMLHttpRequestUpload {
  private _request: XMLHttpRequestImpl;
  private _contentType: string;
  private _body: Buffer;

  constructor(request: XMLHttpRequest) {
    super();
    this._request = <XMLHttpRequestImpl>request;
    this._reset();
  }

  _reset() {
    this._contentType = null;
    this._body = null;
    return void 0;
  }

  _setData(data) {
    var body, i, j, k, offset, ref, ref1, view;
    if (typeof data === 'undefined' || data === null) {
      return;
    }
    if (typeof data === 'string') {
      // DOMString
      if (data.length !== 0) {
        this._contentType = 'text/plain;charset=UTF-8';
      }
      this._body = Buffer.from(data, 'utf8');
    } else if (Buffer.isBuffer(data)) {
      // node.js Buffer
      this._body = data;
    } else if (data instanceof ArrayBuffer) {
      // ArrayBuffer arguments were supported in an old revision of the spec.
      body = Buffer.alloc(data.byteLength);
      view = new Uint8Array(data);
      for (i = j = 0, ref = data.byteLength; (0 <= ref ? j < ref : j > ref); i = 0 <= ref ? ++j : --j) {
        body[i] = view[i];
      }
      this._body = body;
    } else if (data.buffer && data.buffer instanceof ArrayBuffer) {
      // ArrayBufferView
      body = Buffer.alloc(data.byteLength);
      offset = data.byteOffset;
      view = new Uint8Array(data.buffer);
      for (i = k = 0, ref1 = data.byteLength; (0 <= ref1 ? k < ref1 : k > ref1); i = 0 <= ref1 ? ++k : --k) {
        body[i] = view[i + offset];
      }
      this._body = body;
    } else {
      // NOTE: diverging from the XHR specification of coercing everything else
      //       to Strings via toString() because that behavior masks bugs and is
      //       rarely useful
      throw new Error(`Unsupported send() data ${data}`);
    }
    return void 0;
  }

  _finalizeHeaders(headers, loweredHeaders) {
    if (this._contentType) {
      if (!('content-type' in loweredHeaders)) {
        headers['Content-Type'] = this._contentType;
      }
    }
    if (this._body) {
      // Restricted headers can't be set by the user, no need to check
      // loweredHeaders.
      headers['Content-Length'] = this._body.length.toString();
    }
    return void 0;
  }

  _startUpload(request) {
    if (this._body) {
      request.write(this._body);
    }
    request.end();
    return void 0;
  }
}

export class XMLHttpRequestImpl extends XMLHttpRequestEventTargetImpl implements XMLHttpRequest {
  onreadystatechange: (this: XMLHttpRequest, ev: Event) => any;
  readyState: number;
  response: any;
  responseText: string;
  responseType: XMLHttpRequestResponseType;
  responseURL: string;
  responseXML: Document;
  status: number;
  statusText: string;
  timeout: number;
  get upload(): XMLHttpRequestUpload {
    return this._upload;
  }
  withCredentials: boolean;

  readonly UNSENT: 0 = 0;
  static readonly UNSENT = 0;
  readonly OPENED: 1 = 1;
  static readonly OPENED = 1;
  readonly HEADERS_RECEIVED: 2 = 2;
  static readonly HEADERS_RECEIVED = 2;
  readonly LOADING: 3 = 3;
  static readonly LOADING = 3;
  readonly DONE: 4 = 4;
  static readonly DONE = 4;

  private _anonymous: boolean;
  private _method: string;
  private _url: url.UrlWithStringQuery;
  private _blobFromUrl: Blob;
  private _sync: boolean;
  private _headers: { [key: string]: string };
  private _loweredHeaders: { [key: string]: string };
  private _mimeOverride: string;
  private _request: http.ClientRequest;
  private _response: http.ServerResponse;
  private _responseParts: Array<any>;
  private _responseHeaders: { [key: string]: string };
  private _aborting: boolean;
  private _error: Error;
  private _loadedBytes: number;
  private _totalBytes: number;
  private _lengthComputable: boolean;
  private _restrictedMethods: { [key: string]: boolean } = {
    'CONNECT': true,
    'TRACE': true,
    'TRACK': true
  };
  private _restrictedHeaders: { [key: string]: boolean } = {
    'accept-charset': true,
    'accept-encoding': true,
    'access-control-request-headers': true,
    'access-control-request-method': true,
    'connection': true,
    'content-length': true,
    'cookie': true,
    'cookie2': true,
    'date': true,
    'dnt': true,
    'expect': true,
    'host': true,
    'keep-alive': true,
    'origin': true,
    'referer': true,
    'te': true,
    'trailer': true,
    'transfer-encoding': true,
    'upgrade': true,
    'via': true
  };
  private _privateHeaders: { [key: string]: boolean } = {};
  private _upload: XMLHttpRequestUploadImpl;
  private _userAgent: string = `Mozilla/5.0 (${os.type()} ${os.arch()}) ` + `node.js/${process.versions.node} v8/${process.versions.v8}`;
  private nodejsBaseUrl: string = null;
  private nodejsHttpAgent: http.Agent = http.globalAgent;
  private nodejsHttpsAgent: https.Agent = https.globalAgent;

  // Creates a new request.
  // @param {Object} options one or more of the options below
  // @option options {Boolean} anon if true, the request's anonymous flag
  //   will be set
  // @see http://www.w3.org/TR/XMLHttpRequest/#constructors
  // @see http://www.w3.org/TR/XMLHttpRequest/#anonymous-flag
  constructor() {
    super();
    this.onreadystatechange = null;
    // this._anonymous = options && options.anon;
    this.readyState = XMLHttpRequest.UNSENT;
    this.response = null;
    this.responseText = '';
    this.responseType = '';
    this.responseURL = '';
    this.status = 0;
    this.statusText = '';
    this.timeout = 0;
    this._upload = new XMLHttpRequestUploadImpl(this);
    this._method = null; // String
    this._url = null; // Return value of url.parse()
    this._sync = false;
    this._headers = null; // Object<String, String>
    this._loweredHeaders = null; // Object<lowercase String, String>
    this._mimeOverride = null;
    this._request = null; // http.ClientRequest
    this._response = null; // http.ClientResponse
    this._responseParts = null; // Array<Buffer, String>
    this._responseHeaders = null; // Object<lowercase String, String>
    this._aborting = null;
    this._error = null;
    this._loadedBytes = 0;
    this._totalBytes = 0;
    this._lengthComputable = false;
  }

  // Sets the XHR's method, URL, synchronous flag, and authentication params.
  // @param {String} method the HTTP method to be used
  // @param {String} url the URL that the request will be made to
  // @param {?Boolean} async if false, the XHR should be processed
  //   synchronously; true by default
  // @param {?String} user the user credential to be used in HTTP basic
  //   authentication
  // @param {?String} password the password credential to be used in HTTP basic
  //   authentication
  // @return {undefined} undefined
  // @throw {SecurityError} method is not one of the allowed methods
  // @throw {SyntaxError} urlString is not a valid URL
  // @throw {Error} the URL contains an unsupported protocol; the supported
  //   protocols are file, http and https
  // @see http://www.w3.org/TR/XMLHttpRequest/#the-open()-method
  open(method: string, url: string | URL): void;
  open(method: string, url: string | URL, async: boolean, username?: string, password?: string): void;
  open(method: string, url: string | URL, async?: boolean, username?: string, password?: string) {
    const urlStr = typeof url === 'string' ? url : url.toString();
    let xhrUrl;
    method = method.toUpperCase();
    if (method in this._restrictedMethods) {
      throw new SecurityError(`HTTP method ${method} is not allowed in XHR`);
    }
    xhrUrl = this._parseUrl(urlStr);
    if (async === void 0) {
      async = true;
    }
    switch (this.readyState) {
      case XMLHttpRequest.UNSENT:
      case XMLHttpRequest.OPENED:
      case XMLHttpRequest.DONE:
        // Nothing to do here.
        null;
        break;
      case XMLHttpRequest.HEADERS_RECEIVED:
      case XMLHttpRequest.LOADING:
        // TODO(pwnall): terminate abort(), terminate send()
        null;
    }
    this._method = method;
    this._url = xhrUrl;
    if (this._url.protocol === 'blob:') {
      this._blobFromUrl = resolveObjectURL(urlStr);
      logger.info(`created a blob from url:`, this._blobFromUrl, this._url, urlStr);
    }
    this._sync = !async;
    this._headers = {};
    this._loweredHeaders = {};
    this._mimeOverride = null;
    this._setReadyState(XMLHttpRequest.OPENED);
    this._request = null;
    this._response = null;
    this.status = 0;
    this.statusText = '';
    this._responseParts = [];
    this._responseHeaders = null;
    this._loadedBytes = 0;
    this._totalBytes = 0;
    this._lengthComputable = false;
    return void 0;
  }

  // Appends a header to the list of author request headers.

  // @param {String} name the HTTP header name
  // @param {String} value the HTTP header value
  // @return {undefined} undefined
  // @throw {InvalidStateError} readyState is not OPENED
  // @throw {SyntaxError} name is not a valid HTTP header name or value is not
  //   a valid HTTP header value
  // @see http://www.w3.org/TR/XMLHttpRequest/#the-setrequestheader()-method
  setRequestHeader(name: string, value: string) {
    let loweredName: string;
    if (this.readyState !== XMLHttpRequest.OPENED) {
      throw new InvalidStateError("XHR readyState must be OPENED");
    }
    loweredName = name.toLowerCase();
    if (this._restrictedHeaders[loweredName] || /^sec\-/.test(loweredName) || /^proxy-/.test(loweredName)) {
      console.warn(`Refused to set unsafe header \"${name}\"`);
      return void 0;
    }
    value = value.toString();
    if (loweredName in this._loweredHeaders) {
      // Combine value with the existing header value.
      name = this._loweredHeaders[loweredName];
      this._headers[name] = this._headers[name] + ', ' + value;
    } else {
      // New header.
      this._loweredHeaders[loweredName] = name;
      this._headers[name] = value;
    }
    return void 0;
  }

  // Initiates the request.

  // @param {?String, ?ArrayBufferView} data the data to be sent; ignored for
  //   GET and HEAD requests
  // @return {undefined} undefined
  // @throw {InvalidStateError} readyState is not OPENED
  // @see http://www.w3.org/TR/XMLHttpRequest/#the-send()-method
  send(data: string | ArrayBufferView) {
    if (this.readyState !== XMLHttpRequest.OPENED) {
      throw new InvalidStateError("XHR readyState must be OPENED");
    }
    if (this._request) {
      throw new InvalidStateError("send() already called");
    }
    switch (this._url.protocol) {
      case 'file:':
        this._sendFile();
        break;
      case 'blob:':
        this._sendBlob();
        break;
      case 'http:':
      case 'https:':
        this._sendHttp(data);
        break;
      default:
        throw new NetworkError(`Unsupported protocol ${this._url.protocol}`);
    }
    return void 0;
  }

  // Cancels the network activity performed by this request.

  // @return {undefined} undefined
  // @see http://www.w3.org/TR/XMLHttpRequest/#the-abort()-method
  abort() {
    if (!this._request) {
      return;
    }
    this._request.abort();
    this._setError();
    this._dispatchProgress('abort');
    this._dispatchProgress('loadend');
    return void 0;
  }

  // Returns a header value in the HTTP response for this XHR.

  // @param {String} name case-insensitive HTTP header name
  // @return {?String} value the value of the header whose name matches the
  //   given name, or null if there is no such header
  // @see http://www.w3.org/TR/XMLHttpRequest/#the-getresponseheader()-method
  getResponseHeader(name: string) {
    let loweredName: string;
    if (!this._responseHeaders) {
      return null;
    }
    loweredName = name.toLowerCase();
    if (loweredName in this._responseHeaders) {
      return this._responseHeaders[loweredName];
    } else {
      return null;
    }
  }

  // Returns all the HTTP headers in this XHR's response.

  // @return {String} header lines separated by CR LF, where each header line
  //   has the name and value separated by a ": " (colon, space); the empty
  //   string is returned if the headers are not available
  // @see http://www.w3.org/TR/XMLHttpRequest/#the-getallresponseheaders()-method
  getAllResponseHeaders() {
    let lines, name, value;
    if (!this._responseHeaders) {
      return '';
    }
    lines = (function () {
      var ref, results;
      ref = this._responseHeaders;
      results = [];
      for (name in ref) {
        value = ref[name];
        results.push(`${name}: ${value}`);
      }
      return results;
    }).call(this);
    return lines.join("\r\n");
  }

  // Overrides the Content-Type

  // @return {undefined} undefined
  // @see http://www.w3.org/TR/XMLHttpRequest/#the-overridemimetype()-method
  overrideMimeType(newMimeType: string) {
    if (this.readyState === XMLHttpRequest.LOADING || this.readyState === XMLHttpRequest.DONE) {
      throw new InvalidStateError("overrideMimeType() not allowed in LOADING or DONE");
    }
    this._mimeOverride = newMimeType.toLowerCase();
    return void 0;
  }

  // Network configuration not exposed in the XHR API.

  // Although the XMLHttpRequest specification calls itself "ECMAScript HTTP",
  // it assumes that requests are always performed in the context of a browser
  // application, where some network parameters are set by the browser user and
  // should not be modified by Web applications. This API provides access to
  // these network parameters.

  // NOTE: this is not in the XMLHttpRequest API, and will not work in
  // browsers.  It is a stable node-xhr2 API.

  // @param {Object} options one or more of the options below
  // @option options {?http.Agent} httpAgent the value for the nodejsHttpAgent
  //   property (the agent used for HTTP requests)
  // @option options {?https.Agent} httpsAgent the value for the
  //   nodejsHttpsAgent property (the agent used for HTTPS requests)
  // @return {undefined} undefined
  nodejsSet(options) {
    let baseUrl, parsedUrl;
    if ('httpAgent' in options) {
      this.nodejsHttpAgent = options.httpAgent;
    }
    if ('httpsAgent' in options) {
      this.nodejsHttpsAgent = options.httpsAgent;
    }
    if ('baseUrl' in options) {
      baseUrl = options.baseUrl;
      if (baseUrl !== null) {
        parsedUrl = url.parse(baseUrl, false, true);
        if (!parsedUrl.protocol) {
          throw new SyntaxError("baseUrl must be an absolute URL");
        }
      }
      this.nodejsBaseUrl = baseUrl;
    }
    return void 0;
  }

  // Default settings for the network configuration not exposed in the XHR API.

  // NOTE: this is not in the XMLHttpRequest API, and will not work in
  // browsers.  It is a stable node-xhr2 API.

  // @param {Object} options one or more of the options below
  // @option options {?http.Agent} httpAgent the default value for the
  //   nodejsHttpAgent property (the agent used for HTTP requests)
  // @option options {https.Agent} httpsAgent the default value for the
  //   nodejsHttpsAgent property (the agent used for HTTPS requests)
  // @return {undefined} undefined
  // @see XMLHttpRequest.nodejsSet
  static nodejsSet(options) {
    // "this" will be set to XMLHttpRequest.prototype, so the instance nodejsSet
    // operates on default property values.
    XMLHttpRequestImpl.prototype.nodejsSet(options);
    return void 0;
  }

  // Sets the readyState property and fires the readystatechange event.

  // @private
  // @param {Number} newReadyState the new value of readyState
  // @return {undefined} undefined
  private _setReadyState(newReadyState: number) {
    this.readyState = newReadyState;
    const event = new ProgressEventImpl('readystatechange');
    this.dispatchEvent(event);
    return void 0;
  }

  // XMLHttpRequest#send() implementation for the file: protocol.
  private _sendFile() {
    if (this._method !== 'GET') {
      throw new NetworkError('The file protocol only supports GET');
    }
    throw new Error("Protocol file: not implemented");
  }

  private _sendBlob() {
    if (!this._blobFromUrl) {
      throw new NetworkError('The blob URL does not exist');
    }
    this._setReadyState(XMLHttpRequest.LOADING);
    this._dispatchProgress('loadstart');
    this._blobFromUrl.arrayBuffer().then(arrayBuffer => {
      this.status = 200;
      this.statusText = 'OK';
      this.responseText = null;
      this.response = arrayBuffer;
      this._request = null;
      this._response = null;
      this._blobFromUrl = null;
      this._setReadyState(XMLHttpRequest.DONE);
      this._dispatchProgress('load');
      this._dispatchProgress('loadend');
    });
  }

  // XMLHttpRequest#send() implementation for the http: and https: protocols.

  // @private
  // This method sets the instance variables and calls _sendHxxpRequest(), which
  // is responsible for building a node.js request and firing it off. The code
  // in _sendHxxpRequest() is separated off so it can be reused when handling
  // redirects.

  // @see http://www.w3.org/TR/XMLHttpRequest/#infrastructure-for-the-send()-method
  private _sendHttp(data: string | ArrayBufferView) {
    if (this._sync) {
      throw new Error("Synchronous XHR processing not implemented");
    }
    if ((data != null) && (this._method === 'GET' || this._method === 'HEAD')) {
      console.warn(`Discarding entity body for ${this._method} requests`);
      data = null;
    } else {
      // Send Content-Length: 0
      data || (data = '');
    }
    // NOTE: this is called before finalizeHeaders so that the uploader can
    //       figure out Content-Length and Content-Type.
    this._upload._setData(data);
    this._finalizeHeaders();
    this._sendHxxpRequest();
    return void 0;
  }

  // Sets up and fires off a HTTP/HTTPS request using the node.js API.

  // @private
  // This method contains the bulk of the XMLHttpRequest#send() implementation,
  // and is also used to issue new HTTP requests when handling HTTP redirects.

  // @see http://www.w3.org/TR/XMLHttpRequest/#infrastructure-for-the-send()-method
  _sendHxxpRequest() {
    var agent, hxxp, request;
    if (this._url.protocol === 'http:') {
      hxxp = http;
      agent = this.nodejsHttpAgent;
    } else {
      hxxp = https;
      agent = this.nodejsHttpsAgent;
    }
    request = hxxp.request({
      hostname: this._url.hostname,
      port: this._url.port,
      path: this._url.path,
      auth: this._url.auth,
      method: this._method,
      headers: this._headers,
      agent: agent
    });
    this._request = request;
    if (this.timeout) {
      request.setTimeout(this.timeout, () => {
        return this._onHttpTimeout(request);
      });
    }
    request.on('response', (response) => {
      return this._onHttpResponse(request, response);
    });
    request.on('error', (error) => {
      return this._onHttpRequestError(request, error);
    });
    this._upload._startUpload(request);
    if (this._request === request) { // An http error might have already fired.
      this._dispatchProgress('loadstart');
    }
    return void 0;
  }

  // Fills in the restricted HTTP headers with default values.

  // This is called right before the HTTP request is sent off.

  // @private
  // @return {undefined} undefined
  _finalizeHeaders() {
    var base;
    this._headers['Connection'] = 'keep-alive';
    this._headers['Host'] = this._url.host;
    if (this._anonymous) {
      this._headers['Referer'] = 'about:blank';
    }
    (base = this._headers)['User-Agent'] || (base['User-Agent'] = this._userAgent);
    this._upload._finalizeHeaders(this._headers, this._loweredHeaders);
    return void 0;
  }

  // Called when the headers of an HTTP response have been received.

  // @private
  // @param {http.ClientRequest} request the node.js ClientRequest instance that
  //   produced this response
  // @param {http.ClientResponse} response the node.js ClientResponse instance
  //   passed to
  _onHttpResponse(request, response) {
    var lengthString;
    if (this._request !== request) {
      return;
    }
    // Transparent redirection handling.
    switch (response.statusCode) {
      case 301:
      case 302:
      case 303:
      case 307:
      case 308:
        this._url = this._parseUrl(response.headers['location']);
        this._method = 'GET';
        if ('content-type' in this._loweredHeaders) {
          delete this._headers[this._loweredHeaders['content-type']];
          delete this._loweredHeaders['content-type'];
        }
        // XMLHttpRequestUpload#_finalizeHeaders() sets Content-Type directly.
        if ('Content-Type' in this._headers) {
          delete this._headers['Content-Type'];
        }
        // Restricted headers can't be set by the user, no need to check
        // loweredHeaders.
        delete this._headers['Content-Length'];
        this._upload._reset();
        this._finalizeHeaders();
        this._sendHxxpRequest();
        return;
    }
    this._response = response;
    this._response.on('data', (data) => {
      return this._onHttpResponseData(response, data);
    });
    this._response.on('end', () => {
      return this._onHttpResponseEnd(response);
    });
    this._response.on('close', () => {
      return this._onHttpResponseClose(response);
    });
    this.responseURL = this._url.href.split('#')[0];
    this.status = this._response.statusCode;
    this.statusText = http.STATUS_CODES[this.status];
    this._parseResponseHeaders(response);
    if (lengthString = this._responseHeaders['content-length']) {
      this._totalBytes = parseInt(lengthString);
      this._lengthComputable = true;
    } else {
      this._lengthComputable = false;
    }
    return this._setReadyState(XMLHttpRequest.HEADERS_RECEIVED);
  }

  // Called when some data has been received on a HTTP connection.

  // @private
  // @param {http.ClientResponse} response the node.js ClientResponse instance
  //   that fired this event
  // @param {String, Buffer} data the data that has been received
  _onHttpResponseData(response: http.ServerResponse, data) {
    if (this._response !== response) {
      return;
    }
    this._responseParts.push(data);
    this._loadedBytes += data.length;
    if (this.readyState !== XMLHttpRequest.LOADING) {
      this._setReadyState(XMLHttpRequest.LOADING);
    }
    return this._dispatchProgress('progress');
  }

  // Called when the HTTP request finished processing.

  // @private
  // @param {http.ClientResponse} response the node.js ClientResponse instance
  //   that fired this event
  _onHttpResponseEnd(response) {
    if (this._response !== response) {
      return;
    }
    this._parseResponse();
    this._request = null;
    this._response = null;
    this._setReadyState(XMLHttpRequest.DONE);
    this._dispatchProgress('load');
    return this._dispatchProgress('loadend');
  }

  // Called when the underlying HTTP connection was closed prematurely.

  // If this method is called, it will be called after or instead of
  // onHttpResponseEnd.

  // @private
  // @param {http.ClientResponse} response the node.js ClientResponse instance
  //   that fired this event
  _onHttpResponseClose(response) {
    var request;
    if (this._response !== response) {
      return;
    }
    request = this._request;
    this._setError();
    request.abort();
    this._setReadyState(XMLHttpRequest.DONE);
    this._dispatchProgress('error');
    return this._dispatchProgress('loadend');
  }

  // Called when the timeout set on the HTTP socket expires.

  // @private
  // @param {http.ClientRequest} request the node.js ClientRequest instance that
  //   fired this event
  _onHttpTimeout(request) {
    if (this._request !== request) {
      return;
    }
    this._setError();
    request.abort();
    this._setReadyState(XMLHttpRequest.DONE);
    this._dispatchProgress('timeout');
    return this._dispatchProgress('loadend');
  }

  // Called when something wrong happens on the HTTP socket

  // @private
  // @param {http.ClientRequest} request the node.js ClientRequest instance that
  //   fired this event
  // @param {Error} error emitted exception
  _onHttpRequestError(request, error) {
    if (this._request !== request) {
      return;
    }
    this._setError();
    request.abort();
    this._setReadyState(XMLHttpRequest.DONE);
    this._dispatchProgress('error');
    return this._dispatchProgress('loadend');
  }

  private _dispatchProgress(eventType) {
    var event;
    event = new ProgressEventImpl(eventType);
    event.lengthComputable = this._lengthComputable;
    event.loaded = this._loadedBytes;
    event.total = this._totalBytes;
    this.dispatchEvent(event);
    return void 0;
  }

  // Sets up the XHR to reflect the fact that an error has occurred.

  // The possible errors are a network error, a timeout, or an abort.

  // @private
  _setError() {
    this._request = null;
    this._response = null;
    this._responseHeaders = null;
    this._responseParts = null;
    return void 0;
  }

  // Parses a request URL string.

  // @private
  // This method is a thin wrapper around url.parse() that normalizes HTTP
  // user/password credentials. It is used to parse the URL string passed to
  // XMLHttpRequest#open() and the URLs in the Location headers of HTTP redirect
  // responses.

  // @param {String} urlString the URL to be parsed
  // @return {Object} parsed URL
  _parseUrl(urlString: string) {
    let absoluteUrlString: string,
      index: number,
      password: string,
      user: string,
      xhrUrl: url.UrlWithStringQuery;

    if (this.nodejsBaseUrl === null) {
      absoluteUrlString = urlString;
    } else {
      absoluteUrlString = url.resolve(this.nodejsBaseUrl, urlString);
    }
    xhrUrl = url.parse(absoluteUrlString, false, true);
    xhrUrl.hash = null;
    if (xhrUrl.auth && ((typeof user !== "undefined" && user !== null) || (typeof password !== "undefined" && password !== null))) {
      index = xhrUrl.auth.indexOf(':');
      if (index === -1) {
        if (!user) {
          user = xhrUrl.auth;
        }
      } else {
        // TODO
        // if (!user) {
        //   user = xhrUrl.substring(0, index);
        // }
        // if (!password) {
        //   password = xhrUrl.substring(index + 1);
        // }
      }
    }
    if (user || password) {
      xhrUrl.auth = `${user}:${password}`;
    }
    return xhrUrl;
  }

  // Reads the headers from a node.js ClientResponse instance.

  // @private
  // @param {http.ClientResponse} response the response whose headers will be
  //   imported into this XMLHttpRequest's state
  // @return {undefined} undefined
  // @see http://www.w3.org/TR/XMLHttpRequest/#the-getresponseheader()-method
  // @see http://www.w3.org/TR/XMLHttpRequest/#the-getallresponseheaders()-method
  _parseResponseHeaders(response) {
    var loweredName, name, ref, value;
    this._responseHeaders = {};
    ref = response.headers;
    for (name in ref) {
      value = ref[name];
      loweredName = name.toLowerCase();
      if (this._privateHeaders[loweredName]) {
        continue;
      }
      if (this._mimeOverride !== null && loweredName === 'content-type') {
        value = this._mimeOverride;
      }
      this._responseHeaders[loweredName] = value;
    }
    if (this._mimeOverride !== null && !('content-type' in this._responseHeaders)) {
      this._responseHeaders['content-type'] = this._mimeOverride;
    }
    return void 0;
  }

  // Sets the response and responseText properties when an XHR completes.

  // @private
  // @return {undefined} undefined
  _parseResponse() {
    let arrayBuffer, buffer, i, j, jsonError, ref, view;
    if (Buffer.concat) {
      buffer = Buffer.concat(this._responseParts);
    } else {
      // node 0.6
      buffer = this._concatBuffers(this._responseParts);
    }
    this._responseParts = null;
    switch (this.responseType) {
      case 'text':
        this._parseTextResponse(buffer);
        break;
      case 'json':
        this.responseText = null;
        try {
          this.response = JSON.parse(buffer.toString('utf-8'));
        } catch (error1) {
          jsonError = error1;
          this.response = null;
        }
        break;
      case 'arraybuffer':
        this.responseText = null;
        arrayBuffer = new ArrayBuffer(buffer.length);
        view = new Uint8Array(arrayBuffer);
        for (i = j = 0, ref = buffer.length; (0 <= ref ? j < ref : j > ref); i = 0 <= ref ? ++j : --j) {
          view[i] = buffer[i];
        }
        this.response = arrayBuffer;
        break;
      default:
        // TODO(pwnall): content-base detection
        this._parseTextResponse(buffer);
    }
    return void 0;
  }

  // Sets response and responseText for a 'text' response type.

  // @private
  // @param {Buffer} buffer the node.js Buffer containing the binary response
  // @return {undefined} undefined
  _parseTextResponse(buffer) {
    var e;
    try {
      this.responseText = buffer.toString(this._parseResponseEncoding());
    } catch (error1) {
      e = error1;
      // Unknown encoding.
      this.responseText = buffer.toString('binary');
    }
    this.response = this.responseText;
    return void 0;
  }

  // Figures out the string encoding of the XHR's response.

  // This is called to determine the encoding when responseText is set.

  // @private
  // @return {String} a string encoding, e.g. 'utf-8'
  _parseResponseEncoding() {
    var contentType, encoding, match;
    encoding = null;
    if (contentType = this._responseHeaders['content-type']) {
      if (match = /\;\s*charset\=(.*)$/.exec(contentType)) {
        return match[1];
      }
    }
    return 'utf-8';
  }

  // Buffer.concat implementation for node 0.6.

  // @private
  // @param {Array<Buffer>} buffers the buffers whose contents will be merged
  // @return {Buffer} same as Buffer.concat(buffers) in node 0.8 and above
  _concatBuffers(buffers) {
    var buffer, j, k, len, len1, length, target;
    if (buffers.length === 0) {
      return Buffer.alloc(0);
    }
    if (buffers.length === 1) {
      return buffers[0];
    }
    length = 0;
    for (j = 0, len = buffers.length; j < len; j++) {
      buffer = buffers[j];
      length += buffer.length;
    }
    target = Buffer.alloc(length);
    length = 0;
    for (k = 0, len1 = buffers.length; k < len1; k++) {
      buffer = buffers[k];
      buffer.copy(target, length);
      length += buffer.length;
    }
    return target;
  }
}
