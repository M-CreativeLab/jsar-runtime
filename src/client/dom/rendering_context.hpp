#pragma once

#include <functional>
#include <string>
#include <memory>
#include <v8.h>

#include "./dom_parser.hpp"
#include "./dom_scripting.hpp"
#include "./document.hpp"

namespace dom
{
  typedef std::function<void(const v8::FunctionCallbackInfo<v8::Value> &)> ResponseCallback;
  typedef std::function<void(const std::string &)> StringResponseCallback;

  class DocumentRenderingContext : public std::enable_shared_from_this<DocumentRenderingContext>
  {
  public:
    DocumentRenderingContext() : scriptingContext(make_shared<DOMScriptingContext>()), isolate(v8::Isolate::GetCurrent())
    {
    }

  public:
    /**
     * Create a new document.
     *
     * @param url The url of the document.
     * @param type The type of the document.
     * @returns The created document.
     */
    template <typename DocumentType>
    shared_ptr<DocumentType> create(const std::string &url, DOMParsingType type)
    {
      shared_ptr<DocumentType> document;
      if (type == DOMParsingType::HTML)
        document = make_shared<DocumentType>(shared_from_this(), true);
      else
        throw std::runtime_error("Unsupported document type");

      document->setUrl(url);
      documents.push_back(document);
      return document;
    }

    /**
     * Open the given document, it will start loading the document.
     *
     * @param document The document to open.
     */
    template <typename DocumentType>
    void open(shared_ptr<DocumentType> document)
    {
      document->open();
    }

    /**
     * Set the resource loader value.
     *
     * @param value The `ResourceLoader` value to set.
     */
    void setResourceLoaderValue(v8::Local<v8::Value> value);

    /**
     * Fetch the resource from the given URL.
     *
     * @param url The URL of the resource to fetch.
     * @param responseType The type of the response to expect: "string", "arraybuffer" or "json".
     * @param callback The callback to call when the resource is fetched.
     */
    void fetchResource(const std::string &url, const std::string &responseType, const ResponseCallback &callback);

    /**
     * Fetch the text source type of the resource from the given URL.
     *
     * @param url The URL of the resource to fetch.
     * @param callback The callback to call when the resource is fetched.
     */
    void fetchTextSourceResource(const std::string &url, const StringResponseCallback &callback);

    /**
     * Create a new script object from this context.
     * 
     * @param url The URL of the script.
     * @param type The type of the script: classic or module.
     * @returns The new DOM script object.
     */
    shared_ptr<DOMScript> createScript(const std::string &url, SourceTextType type);

    /**
     * Parse the input json string and update the import map.
     * 
     * @param json The JSON string of the import map.
     * @returns Whether the input json is valid.
     */
    inline bool updateImportMap(const std::string &json)
    {
      return scriptingContext->updateImportMapFromJSON(json);
    }

  public:
    shared_ptr<DOMScriptingContext> scriptingContext;
    vector<shared_ptr<Document>> documents;

  private:
    v8::Isolate *isolate;
    v8::Global<v8::Object> resourceLoaderValue;
  };
}
