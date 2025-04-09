#include <client/dom/node.hpp>
#include <client/dom/element.hpp>

#include "./hit_test_result.hpp"

namespace client_layout
{
  using namespace std;

  HitTestResult::HitTestResult()
      : request_(HitTestRequest::kReadOnly | HitTestRequest::kActive, nullptr),
        cacheable_(true)
  {
  }

  HitTestResult::HitTestResult(const HitTestResult &other)
      : request_(other.request_),
        cacheable_(other.cacheable_),
        inner_node_(other.inner_node_),
        inner_element_(other.inner_element_)
  {
  }

  HitTestResult::~HitTestResult() = default;

  shared_ptr<dom::Node> HitTestResult::innerNode() const
  {
    return inner_node_.lock();
  }

  shared_ptr<dom::Element> HitTestResult::innerElement() const
  {
    return inner_element_.lock();
  }

  void HitTestResult::setInnerNode(shared_ptr<dom::Node> node)
  {
    inner_node_ = node;
  }
}
