#pragma once

#include <string>
#include <client/dom/dom_scripting.hpp>

#include "./html_element.hpp"

namespace dom
{
  enum class HTMLScriptCrossOrigin
  {
    Anonymous,
    UseCredentials
  };

  enum class HTMLScriptFetchPriority
  {
    High,
    Low,
    Auto
  };

  class HTMLScriptElement : public HTMLElement
  {
  public:
    using HTMLElement::HTMLElement;
    HTMLScriptElement(std::shared_ptr<Document> ownerDocument)
        : HTMLElement("SCRIPT", ownerDocument)
    {
    }

  public:
    // Attribute is not set (default), an empty string, or a JavaScript MIME type indicates a "classic script".
    inline bool isClassicScript()
    {
      return type.empty() ||
             type == "text/javascript" ||
             type == "application/javascript";
    }
    inline bool isModuleScript()
    {
      return type == "module";
    }
    inline bool isImportMap()
    {
      return type == "importmap";
    }
    inline void setAsync(bool value)
    {
      async = value;
      if (async == true)
        setAttribute("async", "async");
      else
        removeAttribute("async");
    }
    inline void setSrc(const string &value)
    {
      src = value;
      setAttribute("src", value);
    }

  protected:
    void createdCallback(bool from_scripting) override;
    void connectedCallback() override;
    void beforeLoadedCallback() override;

  private:
    bool isRenderable() const override final
    {
      return false;
    }
    void loadSource();
    void compileScript(const string &source, bool isTypeScript);
    void scheduleScriptExecution();
    void executeScript();

  public:
    /**
     * A boolean value that controls how the script should be executed. For classic scripts, if the async property is set to true, the external
     * script will be fetched in parallel to parsing and evaluated as soon as it is available. For module scripts, if the async property is set
     * to true, the script and all their dependencies will be fetched in parallel to parsing and evaluated as soon as they are available.
     */
    bool async = false;
    /**
     * A boolean value that controls how the script should be executed. For classic scripts, if the defer property is set to true, the external
     * script will be executed after the document has been parsed, but before firing `DOMContentLoaded` event. For module scripts, the defer
     * property has no effect.
     */
    bool defer = false;
    /**
     * A string of a keyword specifying the CORS mode to use when fetching the resource.
     */
    HTMLScriptCrossOrigin crossOrigin = HTMLScriptCrossOrigin::Anonymous;
    /**
     * A string of a keyword specifying the fetch priority for the resource.
     */
    HTMLScriptFetchPriority fetchPriority = HTMLScriptFetchPriority::Auto;
    /**
     * A string representing the URL of an external script; this can be used as an alternative to embedding a script directly within a document.
     * It reflects the src attribute of the <script> element.
     */
    string src;
    /**
     * A string that joins and returns the contents of all Text nodes inside the <script> element (ignoring other nodes like comments) in tree order.
     * On setting, it acts the same way as the Node.textContent property.
     */
    string text;
    /**
     * A string representing the type of the script. It reflects the type attribute of the <script> element.
     */
    string type;

  private:
    shared_ptr<dom::DOMScript> compiledScript;
    bool scriptCompiled = false;
    bool scriptExecutedOnce = false;
    bool scriptExecutionScheduled = false;
  };
}
