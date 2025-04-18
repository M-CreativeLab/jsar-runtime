#include "./layout_block_flow.hpp"

namespace client_layout
{
  using namespace std;

  void LayoutBlockFlow::addChild(shared_ptr<LayoutObject> child,
                                 shared_ptr<LayoutObject> beforeChild)
  {
    LayoutBox::addChild(child, beforeChild);
  }

  void LayoutBlockFlow::removeChild(shared_ptr<LayoutObject> child)
  {
    LayoutBox::removeChild(child);
  }
}
