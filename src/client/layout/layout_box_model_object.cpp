#include <glm/glm.hpp>
#include <client/dom/node.hpp>

#include "./layout_box_model_object.hpp"
#include "./layout_view.hpp"

namespace client_layout
{
  using namespace std;

  LayoutBoxModelObject::LayoutBoxModelObject(shared_ptr<dom::Node> node)
      : LayoutObject(node)
      , scrollable_area_(make_shared<client_scroll::ScrollableArea>())
  {
  }

  void LayoutBoxModelObject::setDisplay(const string &displayStr)
  {
    setDisplay(DisplayType::Make(displayStr));
  }

  void LayoutBoxModelObject::setDisplay(const DisplayType &display)
  {
    display_ = display;
    setFormattingContext(display_);
  }

  shared_ptr<client_scroll::ScrollableArea> LayoutBoxModelObject::getScrollableArea() const
  {
    return scrollable_area_;
  }

  void LayoutBoxModelObject::updateFromStyle()
  {
  }

  math3d::TrPlane LayoutBoxModelObject::physicalBorderBoxFront() const
  {
    auto transformComponent = getSceneComponent<builtin_scene::Transform>();
    auto latestMatrix = transformComponent->lastComputedMatrix();

    glm::vec4 originalNormal(0, 0, 1, 0);
    glm::vec4 originalPoint(0, 0, 0, 1);

    glm::vec3 normal = glm::normalize(glm::vec3(glm::inverse(glm::transpose(latestMatrix)) * originalNormal));
    glm::vec3 point = glm::vec3(latestMatrix * originalPoint);
    float distance = -glm::dot(normal, point);

    math3d::TrPlane plane(normal, distance);
    return plane;
  }

  geometry::BoundingBox LayoutBoxModelObject::physicalBorderBoxRect() const
  {
    auto transformComponent = getSceneComponent<builtin_scene::Transform>();
    auto min = glm::vec3(-0.5, -0.5, -0.5);
    auto max = min * -1.0f;
    return geometry::BoundingBox(min, max, transformComponent->lastComputedMatrix());
  }

  float LayoutBoxModelObject::getPaddingEdgeWidth(Edge index) const
  {
    auto nodeStyle = style();
    if (!nodeStyle.has_value())
      return 0.0f;
    const auto &length = nodeStyle->padding()[index];
    // TODO(yorkie): support percentage value?
    return length.getLength().px();
  }

  float LayoutBoxModelObject::getBorderEdgeWidth(Edge index) const
  {
    auto nodeStyle = style();
    if (!nodeStyle.has_value())
      return 0.0f;
    const auto &length = nodeStyle->borderWidth()[index];
    return length.value;
  }

  void LayoutBoxModelObject::styleDidChange()
  {
    LayoutObject::styleDidChange();

    updateFromStyle();
    updateLayer(containingScrollContainer(), true);
  }
}
