#include "./layout_object_child_list.hpp"
#include "./layout_object.hpp"

namespace client_layout
{
  using namespace std;

  LayoutObjectChildList::iterator &LayoutObjectChildList::iterator::operator++()
  {
    if (current_ != nullptr)
      current_ = current_->nextSibling();
    return *this;
  }

  LayoutObjectChildList::iterator LayoutObjectChildList::iterator::operator++(int)
  {
    auto tmp = *this;
    ++(*this);
    return tmp;
  }

  LayoutObjectChildList::const_iterator &LayoutObjectChildList::const_iterator::operator++()
  {
    if (current_ != nullptr)
      current_ = current_->nextSibling();
    return *this;
  }

  LayoutObjectChildList::const_iterator LayoutObjectChildList::const_iterator::operator++(int)
  {
    auto tmp = *this;
    ++(*this);
    return tmp;
  }

  shared_ptr<LayoutObject> LayoutObjectChildList::replaceChildNode(shared_ptr<LayoutObject> owner,
                                                                   shared_ptr<LayoutObject> newChild,
                                                                   shared_ptr<LayoutObject> oldChild)
  {
    assert(oldChild->parent() != nullptr && "The oldChild to be replaced must have a parent.");
    assert(oldChild->parent() == owner);
    assert(shared_from_this() == owner->virtualChildren());

    if (TR_UNLIKELY(oldChild == newChild)) // No need to replace the same child.
      return newChild;

    if (oldChild->prevSibling())
      oldChild->prevSibling()->setNextSibling(newChild);
    if (oldChild->nextSibling())
      oldChild->nextSibling()->setPrevSibling(newChild);

    if (firstChild() == oldChild)
      firstChild_ = newChild;
    if (lastChild() == oldChild)
      lastChild_ = newChild;

    newChild->setParent(owner);
    newChild->setPrevSibling(oldChild->prevSibling());
    newChild->setNextSibling(oldChild->nextSibling());

    oldChild->setParent(nullptr);
    oldChild->setPrevSibling(nullptr);
    oldChild->setNextSibling(nullptr);

    return oldChild;
  }

  shared_ptr<LayoutObject> LayoutObjectChildList::removeChildNode(shared_ptr<LayoutObject> owner,
                                                                  shared_ptr<LayoutObject> oldChild)
  {
    assert(oldChild->parent() != nullptr && "The oldChild to be removed must have a parent.");
    assert(oldChild->parent() == owner);
    assert(shared_from_this() == owner->virtualChildren());

    if (oldChild->prevSibling())
      oldChild->prevSibling()->setNextSibling(oldChild->nextSibling());
    if (oldChild->nextSibling())
      oldChild->nextSibling()->setPrevSibling(oldChild->prevSibling());

    if (firstChild() == oldChild)
      firstChild_ = oldChild->nextSibling();
    if (lastChild() == oldChild)
      lastChild_ = oldChild->prevSibling();

    oldChild->setParent(nullptr);
    oldChild->setPrevSibling(nullptr);
    oldChild->setNextSibling(nullptr);

    return oldChild;
  }

  void LayoutObjectChildList::insertChildNode(shared_ptr<LayoutObject> owner,
                                              shared_ptr<LayoutObject> newChild,
                                              shared_ptr<LayoutObject> beforeChild)
  {
    assert(newChild->parent() == nullptr && owner != nullptr &&
           "The newChild to be inserted must not have a parent.");
    assert(shared_from_this() == owner->virtualChildren());

    while (beforeChild && beforeChild->parent() != owner)
      beforeChild = beforeChild->parent();

    if (beforeChild && beforeChild->parent() != owner)
      assert(false && "The beforeChild is not a child of the owner.");

    newChild->setParent(owner);

    if (firstChild() == beforeChild)
      firstChild_ = newChild;

    if (beforeChild)
    {
      auto prevSibling = beforeChild->prevSibling();
      if (prevSibling)
        prevSibling->setNextSibling(newChild);
      newChild->setPrevSibling(prevSibling);
      newChild->setNextSibling(beforeChild);
      beforeChild->setPrevSibling(newChild);
    }
    else
    {
      if (lastChild())
        lastChild()->setNextSibling(newChild);
      newChild->setPrevSibling(lastChild());
      lastChild_ = newChild;
    }
  }
}
