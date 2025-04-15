#include "./fragment.hpp"

namespace client_layout
{
  using namespace std;

  Fragment::Fragment(const dom::geometry::DOMRect &rect)
      : size_(rect.width(), rect.height(), 1.0f),
        content_size_(0.0f, 0.0f, 0.0f),
        position_(rect.x(), rect.y(), 0.0f),
        border_(0.0f, 0.0f, 0.0f, 0.0f),
        padding_(0.0f, 0.0f, 0.0f, 0.0f)
  {
  }

  Fragment::Fragment(const crates::layout2::Layout &nativeLayout)
      : size_(nativeLayout.width(), nativeLayout.height(), 1.0f),
        content_size_(nativeLayout.contentWidth(), nativeLayout.contentHeight(), 0.0f),
        position_(nativeLayout.left(), nativeLayout.top(), 0.0f)
  {
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

  ostream &operator<<(ostream &os, const Fragment &fragment)
  {
    os << "Fragment {" << endl
       << "    size: (" << fragment.width() << ", " << fragment.height() << endl
       << " content: (" << fragment.contentWidth() << ", " << fragment.contentHeight() << ")" << endl
       << "  border: " << fragment.border_ << endl
       << " padding: " << fragment.padding_ << endl
       << "}";
    return os;
  }
}
