#include "./layout_inline.hpp"
#include "./layout_view.hpp"

namespace client_layout
{
  using namespace std;

  LayoutInline::LayoutInline(shared_ptr<dom::Node> node)
      : LayoutBoxModelObject(node)
      , children_(make_shared<LayoutObjectChildList>())
  {
  }

  shared_ptr<LayoutObject> LayoutInline::firstChild() const
  {
    assert(children() != nullptr && "The children list is not set.");
    assert(children() == virtualChildren());
    return children()->firstChild();
  }

  shared_ptr<LayoutObject> LayoutInline::lastChild() const
  {
    assert(children() != nullptr && "The children list is not set.");
    assert(children() == virtualChildren());
    return children()->lastChild();
  }

  bool LayoutInline::nodeAtPoint(HitTestResult &r,
                                 const HitTestRay &ray,
                                 const glm::vec3 &accumulatedOffset,
                                 HitTestPhase phase)
  {
    if (!mayIntersect(r, ray, accumulatedOffset))
      return false;

    bool skipChildren = false;
    // TODO(yorkie): support set `skipChildren` from the hit test request.
    if (!skipChildren && hitTestChildren(r, ray, accumulatedOffset, phase))
      return true;

    auto hitDistance = ray.intersectsPlane(physicalBorderBoxFront());
    if (!hitDistance.has_value())
      return false;

    // TODO(yorkie): implement the more accurate hit test? Such as testing for the radius of the box, or other shapes.
    if (node()->isElement())
    {
      glm::vec3 hitPoint = ray.origin + ray.direction * hitDistance.value();
      r.setNodeAndPosition(node(), hitPoint);
    }
    return true;
  }

  bool LayoutInline::mayIntersect(const HitTestResult &r,
                                  const HitTestRay &ray,
                                  const glm::vec3 &accumulatedOffset) const
  {
    geometry::BoundingBox box = physicalBorderBoxRect();
    auto min = box.minimumWorld;
    auto max = box.maximumWorld;
    return ray.intersectsBoxMinMax(min, max);
  }

  bool LayoutInline::hitTestChildren(HitTestResult &r,
                                     const HitTestRay &ray,
                                     const glm::vec3 &accumulatedOffset,
                                     HitTestPhase phase)
  {
    for (auto child = lastChild(); child;
         child = child->prevSibling())
    {
      if (child->isText()) // Text nodes are not hit-testable.
        continue;

      glm::vec3 childAccumulatedOffset = accumulatedOffset;
      if (child->nodeAtPoint(r, ray, childAccumulatedOffset, phase))
        return true;
    }
    return false;
  }
}
