#pragma once

#include <memory>
#include <assert.h>
#include <common/utility.hpp>

namespace client_layout
{
  class LayoutObject;
  class LayoutObjectChildList : public std::enable_shared_from_this<LayoutObjectChildList>
  {
    TR_DISALLOW_NEW();

  public:
    class iterator
    {
    public:
      explicit iterator(std::shared_ptr<LayoutObject> current)
          : current_(current)
      {
      }

      LayoutObject &operator*() const { return *current_; }
      iterator &operator++();
      iterator operator++(int);
      friend bool operator==(const iterator &lhs, const iterator &rhs)
      {
        return lhs.current_ == rhs.current_;
      }
      friend bool operator!=(const iterator &lhs, const iterator &rhs) { return !(lhs == rhs); }

    private:
      std::shared_ptr<LayoutObject> current_;
    };

    class const_iterator
    {
    public:
      explicit const_iterator(std::shared_ptr<LayoutObject> current)
          : current_(current)
      {
      }

      const LayoutObject &operator*() const { return *current_; }
      const_iterator &operator++();
      const_iterator operator++(int);
      friend bool operator==(const const_iterator &lhs, const const_iterator &rhs)
      {
        return lhs.current_ == rhs.current_;
      }
      friend bool operator!=(const const_iterator &lhs, const const_iterator &rhs) { return !(lhs == rhs); }

    private:
      std::shared_ptr<const LayoutObject> current_;
    };

  public:
    LayoutObjectChildList() : firstChild_(), lastChild_() {}

  public:
    std::shared_ptr<LayoutObject> firstChild() const { return firstChild_.lock(); }
    std::shared_ptr<LayoutObject> lastChild() const { return lastChild_.lock(); }

  public:
    iterator begin() { return iterator(firstChild_.lock()); }
    iterator end() { return iterator(nullptr); }
    const_iterator begin() const { return const_iterator(firstChild_.lock()); }
    const_iterator end() const { return const_iterator(nullptr); }
    const_iterator cbegin() const { return const_iterator(firstChild_.lock()); }
    const_iterator cend() const { return const_iterator(nullptr); }

  public:
    // Replace the old child with the new child.
    std::shared_ptr<LayoutObject> replaceChildNode(std::shared_ptr<LayoutObject> owner,
                                                   std::shared_ptr<LayoutObject> newChild,
                                                   std::shared_ptr<LayoutObject> oldChild);
    std::shared_ptr<LayoutObject> removeChildNode(std::shared_ptr<LayoutObject> owner,
                                                  std::shared_ptr<LayoutObject> child);
    void insertChildNode(std::shared_ptr<LayoutObject> owner,
                         std::shared_ptr<LayoutObject> newChild,
                         std::shared_ptr<LayoutObject> beforeChild);
    inline void appendChildNode(std::shared_ptr<LayoutObject> owner,
                                std::shared_ptr<LayoutObject> newChild)
    {
      insertChildNode(owner, newChild, nullptr);
    }

  private:
    std::weak_ptr<LayoutObject> firstChild_;
    std::weak_ptr<LayoutObject> lastChild_;
  };
}
