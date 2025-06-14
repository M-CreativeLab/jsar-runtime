#pragma once

#include <memory>
#include <ostream>
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
    std::shared_ptr<dom::Node> innerNode() const
    {
      return inner_node_.lock();
    }
    std::shared_ptr<dom::Element> innerElement() const
    {
      return inner_element_.lock();
    }
    glm::vec3 hitPoint() const
    {
      return hit_point_;
    }

    void setNodeAndPosition(std::shared_ptr<dom::Node> node, const glm::vec3 &p)
    {
      hit_point_ = p;
      setInnerNode(node);
    }

    const HitTestRequest &getHitTestRequest() const
    {
      return request_;
    }
    void setInnerNode(std::shared_ptr<dom::Node> node)
    {
      inner_node_ = node;
    }

  private:
    friend std::ostream &operator<<(std::ostream &os, const HitTestResult &r);

  private:
    HitTestRequest request_;
    bool cacheable_;

    std::weak_ptr<dom::Node> inner_node_;
    std::weak_ptr<dom::Element> inner_element_;
    glm::vec3 hit_point_;
  };
}
