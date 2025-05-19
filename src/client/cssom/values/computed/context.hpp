#pragma once

#include <glm/glm.hpp>
#include <client/cssom/media_queries.hpp>
#include <client/cssom/computed_style.hpp>
#include <client/html/html_element.hpp>

namespace client_cssom::values::computed
{
  // Context for computed value conversion.
  class Context
  {
  public:
    static Context From(const dom::HTMLElement &html_element)
    {
      Context context;
      context.reset_style_ = html_element.defaultStyleRef();
      return context;
    }

  public:
    Context() {}

  public:
    float fontSize() const { return device_.rootFontSize(); }
    int fontWeight() const { return 400; }
    float lineHeight() const { return device_.rootLineHeight(); }
    glm::uvec4 baseViewport() const { return glm::uvec4(1920, 1080, 0, 0); }

  private:
    Device device_;
    ComputedStyle inherited_style_;
    ComputedStyle reset_style_;

    bool in_media_query_ = false;
    bool in_container_query_ = false;
  };
}
