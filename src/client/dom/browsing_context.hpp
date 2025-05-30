#pragma once

#include <functional>
#include <string>
#include <memory>
#include <v8.h>

#include "./dom_parser.hpp"
#include "./dom_scripting.hpp"
#include "./runtime_context.hpp"
#include "./document.hpp"

namespace dom
{
  class BrowsingContext : public RuntimeContext
  {
  public:
    using RuntimeContext::RuntimeContext;

  public:
    /**
     * Create a new document.
     *
     * @param url The url of the document.
     * @param type The type of the document.
     * @param input_type The source of the document.
     * @returns The created document.
     */
    template <typename DocumentType>
    shared_ptr<DocumentType> create(const std::string &url, DOMParsingType type, InputType input_type)
    {
      shared_ptr<DocumentType> document;
      if (type == DOMParsingType::HTML)
        document = make_shared<DocumentType>(getSharedPtr<BrowsingContext>(), true);
      else
        throw std::runtime_error("Unsupported document type");
      if (input_type == InputType::Source)
        document->setSource(url, true);
      else if (input_type == InputType::URL)
        document->setUrl(url);
      else
        assert(false && "The input type must be url or source");
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
     * Create a new script object from this context.
     *
     * @param url The URL of the script.
     * @param type The type of the script: classic or module.
     * @returns The new DOM script object.
     */
    shared_ptr<DOMScript> createScript(const std::string &url, SourceTextType type)
    {
      return scriptingContext->create(getSharedPtr(), url, type);
    }

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
    vector<shared_ptr<Document>> documents;
  };
}
