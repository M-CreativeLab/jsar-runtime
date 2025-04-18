#include <client/dom/node.hpp>
#include <client/dom/element.hpp>

#include "./hit_test_result.hpp"

namespace client_layout
{
  using namespace std;

  HitTestResult::HitTestResult()
      : request_(HitTestRequest::kReadOnly | HitTestRequest::kActive | HitTestRequest::kAvoidCache, nullptr),
        cacheable_(true)
  {
  }

  HitTestResult::HitTestResult(const HitTestResult &other)
      : request_(other.request_),
        cacheable_(other.cacheable_),
        inner_node_(other.inner_node_),
        inner_element_(other.inner_element_),
        hit_point_(other.hit_point_)
  {
  }

  HitTestResult::~HitTestResult() = default;

  ostream &operator<<(ostream &os, const HitTestResult &r)
  {
    auto innerNode = r.innerNode();
    auto hitPoint = r.hitPoint();
    os << "HitTestResult {" << endl
       << "  innerNode: " << (innerNode ? innerNode->toString() : "null") << endl
       << "   hitPoint: [" << hitPoint.x << ", " << hitPoint.y << ", " << hitPoint.z << "]" << endl
       << "  cacheable: " << (r.cacheable_ ? "true" : "false") << endl
       << "}";
    return os;
  }
}
