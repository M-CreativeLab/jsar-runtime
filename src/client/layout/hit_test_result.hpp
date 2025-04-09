#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <client/dom/types.hpp>

#include "./hit_test_request.hpp"

namespace client_layout
{
  class HitTestResult
  {
  public:
    HitTestResult();
    HitTestResult(const HitTestResult &);
    ~HitTestResult();

  public:
    std::shared_ptr<dom::Node> innerNode() const;
    std::shared_ptr<dom::Element> innerElement() const;

    void setNodeAndPosition(std::shared_ptr<dom::Node> node,
                            const glm::vec3 &p)
    {
      local_point_ = p;
      setInnerNode(node);
    }

    const HitTestRequest& getHitTestRequest() const { return request_; }
    void setInnerNode(std::shared_ptr<dom::Node> node);

  private:
    HitTestRequest request_;
    bool cacheable_;

    std::weak_ptr<dom::Node> inner_node_;
    std::weak_ptr<dom::Element> inner_element_;

    glm::vec3 local_point_;
  };
}
