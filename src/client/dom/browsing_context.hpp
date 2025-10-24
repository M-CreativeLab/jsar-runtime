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

namespace endor
{
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
      uint32_t scriptId;
      std::weak_ptr<HTMLScriptElement> scriptElement;

      ScriptExecutionHandle(uint32_t id, std::weak_ptr<HTMLScriptElement> element)
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
     * Get the active document, which is the last document in the documents list.
     */
      inline std::shared_ptr<Document> getActiveDocument()
      {
        assert(activeDocument != nullptr && "The active document is null");
        return activeDocument;
      }

      /**
     * Open the given document, it will start loading the document.
     *
     * @param document The document to open.
     */
      template <typename DocumentType>
      void open(shared_ptr<DocumentType> document)
      {
        // Open the document to start loading
        document->open();

        // Set the active document if it's an HTML document
        if constexpr (std::is_same<DocumentType, HTMLDocument>::value)
          activeDocument = std::static_pointer_cast<HTMLDocument>(document);
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
      uint32_t registerScriptForExecution(std::shared_ptr<HTMLScriptElement> script);

      /**
     * Try to execute the next script in the queue if it's ready.
     */
      void tryExecuteNextScript();

      /**
     * Notify that a script has completed execution.
     */
      void notifyScriptExecutionComplete(uint32_t script_id);

    public:
      vector<shared_ptr<Document>> documents;
      shared_ptr<HTMLDocument> activeDocument = nullptr;

    private:
      // Script execution queue using handles to avoid circular dependency
      std::deque<ScriptExecutionHandle> script_execution_queue;
    };
  }
} // namespace endor
