#pragma once

#include <string>
#include <client/dom/node.hpp>

namespace client_cssom
{
  class StyleSheet
  {
  public:
    StyleSheet() = default;
    virtual ~StyleSheet() = default;

  public:
    bool disabled;
    inline const std::string &href() const
    {
      return href_;
    }
    inline std::shared_ptr<dom::Node> ownerNode() const
    {
      return ownerNode_.lock();
    }
    inline const dom::Node &ownerNodeChecked() const
    {
      return *ownerNode();
    }
    inline std::shared_ptr<StyleSheet> parentStyleSheet() const
    {
      return parentStyleSheet_.lock();
    }
    inline const std::string &title() const
    {
      return title_;
    }
    inline const std::string &type() const
    {
      return type_;
    }

  private:
    std::string href_;
    std::weak_ptr<dom::Node> ownerNode_;
    std::weak_ptr<StyleSheet> parentStyleSheet_;
    std::string title_;
    std::string type_;
  };
}
