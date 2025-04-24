#include "./fragment.hpp"

namespace client_layout
{
  using namespace std;

  Fragment::Fragment(const dom::geometry::DOMRect &rect)
      : size_(rect.width(), rect.height(), 1.0f),
        content_size_(std::nullopt),
        position_(rect.x(), rect.y(), 0.0f),
        border_(0.0f, 0.0f, 0.0f, 0.0f),
        padding_(0.0f, 0.0f, 0.0f, 0.0f)
  {
  }

  Fragment::Fragment(const crates::layout2::Layout &nativeLayout)
      : size_(nativeLayout.width(), nativeLayout.height(), 1.0f),
        position_(nativeLayout.left(), nativeLayout.top(), 0.0f)
  {
    if (nativeLayout.contentWidth() == 0 && nativeLayout.contentHeight() == 0)
    {
      content_size_ = std::nullopt;
    }
    else
    {
      content_size_ = glm::vec3(nativeLayout.contentWidth(),
                                nativeLayout.contentHeight(),
                                1.0f);
    }
    setBorder(nativeLayout.border());
    setPadding(nativeLayout.padding());
  }

  const Fragment Fragment::position(const Fragment &childFragment) const
  {
    // Use child fragment which copies the size.
    dom::geometry::DOMRect newRect = childFragment.getRect();

    // Set the position of the child fragment.
    newRect.x() = left() + childFragment.left();
    newRect.y() = top() + childFragment.top();

    // Returns a new fragment.
    return Fragment(newRect);
  }

  void Fragment::moveBy(float x, float y, float z)
  {
    position_ += glm::vec3(x, y, z);
  }

  bool Fragment::visibleInViewport(const geometry::Viewport3d &viewport) const
  {
    // Check if the fragment is within the viewport bounds.
    return left() < viewport.right() &&
           right() > viewport.left() &&
           top() < viewport.bottom() &&
           bottom() > viewport.top();
  }

  ostream &operator<<(ostream &os, const Fragment &fragment)
  {
    auto contentSize = fragment.content_size_.value_or(glm::vec3(0.0f));
    os << "Fragment {" << endl
       << "position: (" << fragment.position_.x << ", " << fragment.position_.y << ")" << endl
       << "    size: (" << fragment.size_.x << ", " << fragment.size_.y << ")" << endl
       << " content: (" << contentSize.x << ", " << contentSize.y << ")" << endl
       << "  border: " << fragment.border_ << endl
       << " padding: " << fragment.padding_ << endl
       << "}";
    return os;
  }
}
