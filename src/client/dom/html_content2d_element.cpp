#include <skia/include/core/SkImageInfo.h>
#include <client/builtin_scene/ecs-inl.hpp>
#include <client/builtin_scene/web_content.hpp>

#include "./html_content2d_element.hpp"
#include "./document.hpp"

namespace dom
{
  using namespace std;
  using namespace pugi;

  void HTMLContent2dElement::createdCallback()
  {
    HTMLPlaneElement::createdCallback();
    content2d_ = make_unique<Content2d>(shared_from_this());
  }

  void HTMLContent2dElement::connectedCallback()
  {
    HTMLPlaneElement::connectedCallback();
    content2d().onNodeConnected();
  }

  void HTMLContent2dElement::disconnectedCallback()
  {
    content2d().onNodeDisconnected(); // Disconnect the Content2d first
    HTMLPlaneElement::disconnectedCallback();
  }

  void HTMLContent2dElement::onLayoutChanged()
  {
    HTMLPlaneElement::onLayoutChanged();
    content2d().onLayoutSizeChanged();
  }

  void HTMLContent2dElement::onAdoptedStyleChanged()
  {
    HTMLPlaneElement::onAdoptedStyleChanged();
    content2d().onAdoptedStyleChanged();
  }
}
