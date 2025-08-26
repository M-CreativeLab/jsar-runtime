#pragma once

#include "./script_interfaces.hpp"
#include "../html/html_script_element.hpp"
#include <memory>
#include <string>

namespace dom
{
  /**
   * Adapter that wraps HTMLScriptElement to implement the IScript interface.
   * This decouples the script execution manager from the specific HTML element implementation.
   */
  class HTMLScriptAdapter : public IScript
  {
  public:
    /**
     * Constructor
     * @param element The HTMLScriptElement to wrap
     */
    explicit HTMLScriptAdapter(std::shared_ptr<HTMLScriptElement> element);
    ~HTMLScriptAdapter() = default;

    // IScript interface implementation
    ScriptType getType() const override;
    ScriptState getState() const override;
    std::string getSource() const override;
    bool isInline() const override;
    void load(std::function<void(bool success)> callback) override;
    void execute() override;
    std::string getId() const override;
    bool isReady() const override;

    /**
     * Get the underlying HTMLScriptElement
     * @return The wrapped element
     */
    std::shared_ptr<HTMLScriptElement> getElement() const
    {
      return element_;
    }

  private:
    std::shared_ptr<HTMLScriptElement> element_;
    mutable std::string cachedId_; // Cache the ID to avoid regenerating it
  };

  /**
   * Document adapter that implements IScriptExecutionContext interface.
   * This provides document-level operations needed by the script execution manager.
   */
  class DocumentScriptContext : public IScriptExecutionContext
  {
  public:
    /**
     * Constructor
     * @param document The document to wrap
     */
    explicit DocumentScriptContext(std::shared_ptr<Document> document);
    ~DocumentScriptContext() = default;

    // IScriptExecutionContext interface implementation
    void dispatchDOMContentLoaded() override;
    bool isParsingFinished() const override;
    void pauseParsing() override;
    void resumeParsing() override;

    /**
     * Get the underlying Document
     * @return The wrapped document
     */
    std::shared_ptr<Document> getDocument() const
    {
      auto doc = document_.lock();
      return doc;
    }

  private:
    std::weak_ptr<Document> document_;
  };
}
