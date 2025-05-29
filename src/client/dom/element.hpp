#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <client/animation/animatable.hpp>
#include <client/animation/animation.hpp>
#include <client/animation/animation_timeline.hpp>
#include <client/animation/element_animations.hpp>
#include <client/animation/keyframes.hpp>
#include <client/builtin_scene/scene.hpp>
#include <client/builtin_scene/ecs.hpp>
#include <client/cssom/box_bounding.hpp>
#include <client/cssom/computed_style.hpp>
#include <client/cssom/css_style_declaration.hpp>
#include <client/layout/layout_box.hpp>

#include "./node.hpp"
#include "./dom_token_list.hpp"
#include "./events/mouse_event.hpp"
#include "./geometry/dom_rect.hpp"

#define TYPED_ELEMENT_MAP(XX)         \
  XX("audio", HTMLAudioElement)       \
  XX("body", HTMLBodyElement)         \
  XX("canvas", HTMLCanvasElement)     \
  XX("div", HTMLDivElement)           \
  XX("head", HTMLHeadElement)         \
  XX("html", HTMLHtmlElement)         \
  XX("img", HTMLImageElement)         \
  XX("link", HTMLLinkElement)         \
  XX("meta", HTMLMetaElement)         \
  XX("h1", HTMLHeadingElement)        \
  XX("h2", HTMLHeadingElement)        \
  XX("h3", HTMLHeadingElement)        \
  XX("h4", HTMLHeadingElement)        \
  XX("h5", HTMLHeadingElement)        \
  XX("h6", HTMLHeadingElement)        \
  XX("p", HTMLParagraphElement)       \
  XX("script", HTMLScriptElement)     \
  XX("span", HTMLSpanElement)         \
  XX("style", HTMLStyleElement)       \
  XX("template", HTMLTemplateElement) \
  XX("main", HTMLSectionElement)      \
  XX("section", HTMLSectionElement)   \
  XX("header", HTMLSectionElement)    \
  XX("footer", HTMLSectionElement)    \
  XX("video", HTMLVideoElement)

namespace dom
{
  struct CheckVisibilityOptions
  {
    bool checkOpacity = false;
    bool checkVisibilityCSS = false;
    bool contentVisibilityAuto = false;
    bool opacityProperty = false;
    bool visibilityProperty = false;
  };

  class Attr;
  class Document;
  class Element : public Node,
                  public Animatable,
                  virtual public client_cssom::BoxBounding
  {
    friend class DocumentEventDispatcher;
    friend class RenderHTMLDocument;

  public:
    /**
     * Create a new `Element` object from a `pugi::xml_node`, which is used to create and initialize an element from the
     *  XML parser.
     *
     * @param node The `pugi::xml_node` object.
     * @param ownerDocument The owner document of the element.
     * @returns The created `Element` object.
     */
    static std::shared_ptr<Element> CreateElement(pugi::xml_node node, std::shared_ptr<Document> ownerDocument);

    /**
     * Create a new `Element` object from a tag name, which is used to create an element from scripts such as:
     * `document.createElement('div')`.
     *
     * @param tagName The tag name of the element.
     * @param ownerDocument The owner document of the element.
     * @returns The created `Element` object.
     */
    static std::shared_ptr<Element> CreateElement(std::string namespaceURI, std::string tagName,
                                                  std::shared_ptr<Document> ownerDocument);

    /**
     * Clone the given element and return a new element with the same properties.
     *
     * @param srcElement The element to clone.
     * @returns The cloned element in `std::shared_ptr<Node>`.
     */
    static std::shared_ptr<Node> CloneElement(std::shared_ptr<Node> srcElement);

  public:
    Element(std::string tagName, std::optional<std::shared_ptr<Document>> ownerDocument);
    Element(pugi::xml_node node, std::shared_ptr<Document> ownerDocument);
    Element(const Element &other);
    ~Element() = default;

  public:
    std::shared_ptr<Element> getAnimationTarget() override;

    void before(std::vector<std::shared_ptr<Node>> nodes);
    void before(std::string text);
    inline void before(std::shared_ptr<Node> node) { before(std::vector<std::shared_ptr<Node>>{node}); }
    void after(std::vector<std::shared_ptr<Node>> nodes);
    void after(std::string text);
    inline void after(std::shared_ptr<Node> node) { after(std::vector<std::shared_ptr<Node>>{node}); }
    std::string getAttribute(const std::string &name) const;
    std::vector<std::string> getAttributeNames() const;
    std::shared_ptr<Attr> getAttributeNode(const std::string &name) const;
    Attr &getAttributeNodeChecked(const std::string &name) const;
    bool hasAttribute(const std::string &name) const;
    bool hasAttributes() const;
    /**
     * Sets the attribute value of the element.
     *
     * @param name The name of the attribute.
     * @param value The value of the attribute.
     * @param notify Whether to notify the attribute change.
     */
    void setAttribute(const std::string &name, const std::string &value, bool notify = true);
    void setAttributeNode(std::shared_ptr<Attr> attr);
    void remove();
    void removeAttribute(const std::string &name);
    /**
     * The method returns a `DOMRect` object providing information about the size of an element and its position
     * relative to the viewport.
     *
     * @returns The `DOMRect` object which is the smallest rectangle which contains the entire element, including
     *          its padding and border-width.
     */
    [[nodiscard]] geometry::DOMRect getBoundingClientRect() const;
    /**
     * The `getClientRects()` method returns a collection of `DOMRect` objects that indicate the bounding rectangles
     * for each CSS border box in a client.
     *
     * @returns a collection of `DOMRect` objects, one for each CSS border box associated with the element.
     */
    [[nodiscard]] std::vector<geometry::DOMRect> getClientRects() const;
    /**
     * It checks whether the element is visible.
     *
     * @param options The options to check the visibility.
     * @returns `true` if the element is visible, otherwise `false`.
     */
    [[nodiscard]] bool checkVisibility(CheckVisibilityOptions options) const;

    enum ScrollBehavior
    {
      kScrollBehaviorAuto,
      kScrollBehaviorSmooth,
      kScrollBehaviorInstant
    };
    struct ScrollOptions
    {
      float top = 0;
      float left = 0;
      ScrollBehavior behavior = kScrollBehaviorAuto;
    };

    // Scrolls the element to the given position.
    inline void scroll(const ScrollOptions &opts) { scrollTo(opts); }
    // Scrolls an element to the given position.
    void scrollTo(const ScrollOptions &);
    // Scrolls an element by the given amount.
    void scrollBy(const ScrollOptions &);

    std::shared_ptr<ElementAnimations> getElementAnimations() { return element_animations_; }
    bool hasAnimations() const
    {
      return element_animations_ != nullptr && !element_animations_->isEmpty();
    }

    inline bool hasAdoptedStyle() const { return adoptedStyle_ != nullptr; }
    inline const client_cssom::ComputedStyle &adoptedStyleRef() const
    {
      assert(adoptedStyle_ != nullptr && "The adopted style should not be null.");
      return *adoptedStyle_;
    }
    const client_cssom::CSSStyleDeclaration &defaultStyleRef() const { return defaultStyle_; }

    std::shared_ptr<const client_layout::LayoutBoxModelObject> principalBox() const { return principalBox_; }
    std::shared_ptr<client_layout::LayoutBoxModelObject> principalBox() { return principalBox_; }

  public:
    bool isElement() const override final { return true; }

    bool isHovered() const { return is_hovered_; }
    bool isFocused() const { return is_focused_; }

    /**
     * Returns true if the element's tag name is the same as the given tag name ignoring case.
     *
     * @param expectedTagName The expected tag name.
     * @returns True if the element's tag name is the same as the given tag name ignoring case.
     */
    bool is(const std::string expectedTagName);
    void setId(const std::string &id);
    std::string getInnerHTML();
    void setInnerHTML(const std::string &markup);
    std::string getOuterHTML();
    void setOuterHTML(const std::string &markup);

  protected: // Node lifecycle callbacks
    void connectedCallback() override;
    void disconnectedCallback() override;

  protected: // Element lifecycle callbacks
    /**
     * When the element is created each time.
     */
    virtual void createdCallback();
    /**
     * When the element is moved to a new document each time.
     */
    virtual void adoptedCallback();
    /**
     * When the element's attribute is changed.
     *
     * @param name The name of the attribute.
     * @param oldValue The old value of the attribute.
     * @param newValue The new value of the attribute.
     */
    virtual void attributeChangedCallback(const string &name, const string &oldValue, const string &newValue);
    /**
     * When the element's `classList` is changed.
     *
     * @param newClassList The new class list of the element.
     */
    virtual void classListChangedCallback(const DOMTokenList &newClassList);
    /**
     * When the element action state is changed, such as `:hover`, `:active`, `:focus`, etc.
     */
    virtual void actionStateChangedCallback();
    /**
     * When the element's adopted style is updated.
     */
    virtual void styleAdoptedCallback();

  protected:
    // Initialize the CSS boxes of the element.
    void initCSSBoxes();
    // Set the CSS boxes of the element from the current display style.
    void reinitCSSBoxes();
    // Clear all the CSS boxes of the element.
    void resetCSSBoxes(bool skipCheck = false);

    // Adopt the specified style to the element, it will copy the style properties to the element's adopted style, and
    // update the layout node's style.
    bool adoptStyle(const client_cssom::ComputedStyle &newStyle);

    /**
     * A utility method to use the scene weak reference safely.
     *
     * @param callback The callback function to use the scene.
     */
    void useSceneWithCallback(const std::function<void(builtin_scene::Scene &)> &callback);

    // Dispatch the event to the element, it will do bubbles and capture phase dispatching.
    void dispatchEventInternal(std::shared_ptr<dom::Event>);
    void simulateMouseDown(const glm::vec3 &hitPointInWorld);
    void simulateMouseUp(const glm::vec3 &hitPointInWorld);
    void simulateMouseMove(const glm::vec3 &hitPointInWorld);
    void simulateMouseOut(const glm::vec3 &hitPointInWorld);
    void simulateMouseOver(const glm::vec3 &hitPointInWorld);
    void simulateMouseEnter(const glm::vec3 &hitPointInWorld);
    void simulateMouseLeave(const glm::vec3 &hitPointInWorld);
    void simulateClick(const glm::vec3 &hitPointInWorld);
    void simulateScrollWithOffset(float offsetX, float offsetY);

  private:
    bool adoptStyleDirectly(const client_cssom::ComputedStyle &newStyle);
    bool setActionState(bool &state, bool value);

  public:
    std::string id;
    std::string namespaceURI;
    std::string tagName;
    std::string localName;
    std::string prefix;

    inline const std::string &className() const { return classList_.value(); }
    inline void setClassName(const std::string &className)
    {
      setAttribute("class", className);
    }
    inline const DOMTokenList &classList() const { return classList_; }
    inline DOMTokenList &classList() { return classList_; }

    std::shared_ptr<Element> firstElementChild() const;
    std::shared_ptr<Element> lastElementChild() const;

  protected:
    DOMTokenList classList_;
    std::unordered_map<std::string, std::shared_ptr<Attr>> attributeNodes_;
    client_cssom::CSSStyleDeclaration defaultStyle_;

  private:
    std::unique_ptr<client_cssom::ComputedStyle> adoptedStyle_;
    std::weak_ptr<builtin_scene::Scene> scene_;
    std::shared_ptr<ElementAnimations> element_animations_;
    std::vector<std::shared_ptr<client_layout::LayoutBoxModelObject>> boxes_;
    std::shared_ptr<client_layout::LayoutBoxModelObject> principalBox_;
    std::string currentDisplayStr_ = "block";
    bool is_hovered_ = false;
    bool is_focused_ = false;
    bool is_active_ = false;
  };
}
