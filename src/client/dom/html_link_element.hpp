#pragma once

#include <string>
#include <client/cssom/stylesheet.hpp>

#include "./html_element.hpp"

namespace dom
{
  class HTMLLinkElement final : public HTMLElement
  {
    using HTMLElement::HTMLElement;

  public:
#define _CONTENT_TYPE_MAP(XX) \
  XX(Audio, "audio")          \
  XX(Document, "document")    \
  XX(Embed, "embed")          \
  XX(Fetch, "fetch")          \
  XX(Font, "font")            \
  XX(Image, "image")          \
  XX(Object, "object")        \
  XX(Script, "script")        \
  XX(Style, "style")          \
  XX(Track, "track")          \
  XX(Video, "video")          \
  XX(Worker, "worker")

#define _REL_TYPE_MAP(XX)            \
  XX(Alternate, "alternate")         \
  XX(Author, "author")               \
  XX(Bookmark, "bookmark")           \
  XX(Canonical, "canonical")         \
  XX(DnsPrefetch, "dns-prefetch")    \
  XX(External, "external")           \
  XX(Expect, "expect")               \
  XX(Help, "help")                   \
  XX(Icon, "icon")                   \
  XX(License, "license")             \
  XX(Manifest, "manifest")           \
  XX(Modulepreload, "modulepreload") \
  XX(Next, "next")                   \
  XX(NoFollow, "nofollow")           \
  XX(NoOpener, "noopener")           \
  XX(NoReferrer, "noreferrer")       \
  XX(Opener, "opener")               \
  XX(Pingback, "pingback")           \
  XX(Preconnect, "preconnect")       \
  XX(Prefetch, "prefetch")           \
  XX(Preload, "preload")             \
  XX(Prerender, "prerender")         \
  XX(Prev, "prev")                   \
  XX(Search, "search")               \
  XX(Stylesheet, "stylesheet")       \
  XX(Tag, "tag")                     \
  XX(TermsOfService, "terms-of-service")

    enum class ContentType
    {
#define XX(NAME, _) k##NAME,
      _CONTENT_TYPE_MAP(XX)
#undef XX
    };

    enum class RelType
    {
#define XX(NAME, _) k##NAME,
      _REL_TYPE_MAP(XX)
#undef XX
    };

  private:
    static std::string ToString(ContentType type)
    {
      switch (type)
      {
#define XX(NAME, STR)        \
  case ContentType::k##NAME: \
    return STR;
        _CONTENT_TYPE_MAP(XX)
#undef XX
      }
      return "";
    }

    static std::string ToString(RelType type)
    {
      switch (type)
      {
#define XX(NAME, STR)    \
  case RelType::k##NAME: \
    return STR;
        _REL_TYPE_MAP(XX)
#undef XX
      }
      return "";
    }

    static ContentType InitContentType(const std::string &str)
    {
#define XX(NAME, STR) \
  if (str == STR)     \
    return ContentType::k##NAME;
      _CONTENT_TYPE_MAP(XX)
#undef XX
      return ContentType::kDocument;
    }

    static RelType InitRelType(const std::string &str)
    {
#define XX(NAME, STR) \
  if (str == STR)     \
    return RelType::k##NAME;
      _REL_TYPE_MAP(XX)
#undef XX
      return RelType::kAlternate;
    }

#undef _CONTENT_TYPE_MAP
#undef _REL_TYPE_MAP

  public:
    inline ContentType as() const { return asType_; }
    inline void setAs(ContentType asType)
    {
      asType_ = asType;
      setAttribute("as", ToString(asType));
    }
    inline bool blocking() const { return blocking_; }
    inline void setBlocking(bool blocking)
    {
      blocking_ = blocking;
      setAttribute("blocking", blocking ? "true" : "false");
    }
    inline bool disabled() const { return disabled_; }
    inline void setDisabled(bool disabled)
    {
      disabled_ = disabled;
      setAttribute("disabled", disabled ? "true" : "false");
    }
    inline std::string href() const { return getAttribute("href"); }
    inline void setHref(const std::string &href) { setAttribute("href", href); }
    inline std::string hreflang() const { return getAttribute("hreflang"); }
    inline void setHreflang(const std::string &hreflang) { setAttribute("hreflang", hreflang); }
    inline std::string integrity() const { return getAttribute("integrity"); }
    inline void setIntegrity(const std::string &integrity) { setAttribute("integrity", integrity); }
    inline RelType rel() const { return relType_; }
    inline void setRel(RelType relType)
    {
      relType_ = relType;
      setAttribute("rel", ToString(relType));
    }

  private:
    void createdCallback() override;
    void connectedCallback() override;
    void disconnectedCallback() override;

  private:
    bool getBoolAttribute(const std::string &name);
    void onResourceLoaded(const std::string &source);
    void loadStyleSheet(const std::string &cssText);

  private:
    ContentType asType_;
    RelType relType_;
    bool blocking_;
    bool disabled_;
    std::shared_ptr<client_cssom::StyleSheet> styleSheet_ = nullptr;
  };
} // namespace dom
