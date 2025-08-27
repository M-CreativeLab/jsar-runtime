#pragma once

#include <functional>
#include <string>
#include <memory>
#include <deque>
#include <unordered_map>
#include <v8.h>

#include "./dom_parser.hpp"
#include "./dom_scripting.hpp"
#include "./runtime_context.hpp"
#include "./document.hpp"

namespace dom
{
  enum class InputType
  {
    URL,
    Source,
  };

  // Forward declaration to avoid circular dependency
  class HTMLScriptElement;

  // Script execution handle to decouple from HTMLScriptElement
  struct ScriptExecutionHandle
  {
    std::size_t scriptId;
    std::weak_ptr<HTMLScriptElement> scriptElement;

    ScriptExecutionHandle(std::size_t id, std::weak_ptr<HTMLScriptElement> element)
        : scriptId(id)
        , scriptElement(element)
    {
    }
  };
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
     * @param base_uri The base URI of the document, used for resolving relative URLs.
     * @returns The created document.
     */
    template <typename DocumentType>
    shared_ptr<DocumentType> create(const std::string &url,
                                    DOMParsingType type,
                                    InputType input_type,
                                    const std::string &base_uri)
    {
      shared_ptr<DocumentType> document;
      if (type == DOMParsingType::HTML)
        document = make_shared<DocumentType>(getSharedPtr<BrowsingContext>(), true);
      else
        throw std::runtime_error("Unsupported document type");

      if (input_type == InputType::Source)
      {
        document->setSource(url);
        document->baseURI = base_uri;
      }
      else if (input_type == InputType::URL)
      {
        document->setUrl(url);
      }
      else
      {
        assert(false && "The input type must be url or source");
      }

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

    /**
     * Register a script for document-order execution.
     * Returns a unique script ID for tracking.
     */
    std::size_t registerScriptForExecution(std::shared_ptr<HTMLScriptElement> script);

    /**
     * Try to execute the next script in the queue if it's ready.
     */
    void tryExecuteNextScript();

    /**
     * Notify that a script has completed execution.
     */
    void notifyScriptExecutionComplete(std::size_t scriptId);

  public:
    vector<shared_ptr<Document>> documents;

  private:
    // Script execution queue using handles to avoid circular dependency
    std::deque<ScriptExecutionHandle> scriptExecutionQueue;
    std::size_t nextScriptId = 1;
  };
}
