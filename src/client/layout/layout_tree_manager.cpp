#include "./layout_tree_manager.hpp"
#include "./layout_object.hpp"
#include "./layout_view.hpp"
#include "./formatting_contexts.hpp"

namespace client_layout
{
  using namespace std;

  LayoutTreeManager::LayoutTreeManager(shared_ptr<LayoutView> view)
      : view_(view)
  {
  }

  void LayoutTreeManager::registerLayoutObject(shared_ptr<LayoutObject> object,
                                               shared_ptr<FormattingContext> context)
  {
    if (isManaged(object))
    {
      return; // Already registered
    }

    auto node = createLayoutNode(object, context);
    object_to_node_[object.get()] = node;

    // Set as root if it's the first object (typically LayoutView)
    if (root_ == nullptr)
    {
      root_ = node;
    }
  }

  void LayoutTreeManager::unregisterLayoutObject(shared_ptr<LayoutObject> object)
  {
    auto it = object_to_node_.find(object.get());
    if (it == object_to_node_.end())
    {
      return; // Not managed
    }

    auto node = it->second;

    // Remove from parent's children
    if (auto parent = node->parent.lock())
    {
      auto &children = parent->children;
      children.erase(
        remove_if(children.begin(), children.end(), [&node](const weak_ptr<LayoutNode> &child)
                  { return child.lock() == node; }),
        children.end());
    }

    // Clear parent references for all children
    for (auto &child : node->children)
    {
      child->parent.reset();
    }

    object_to_node_.erase(it);

    if (root_ == node)
    {
      root_.reset();
    }
  }

  void LayoutTreeManager::addChild(shared_ptr<LayoutObject> parent,
                                   shared_ptr<LayoutObject> child,
                                   shared_ptr<LayoutObject> beforeSibling)
  {
    auto parentNode = findLayoutNode(parent);
    auto childNode = findLayoutNode(child);

    if (!parentNode || !childNode)
    {
      return; // Objects not managed
    }

    // Remove child from previous parent if exists
    if (auto oldParent = childNode->parent.lock())
    {
      removeChild(oldParent->object, child);
    }

    // Determine insertion position
    size_t position = parentNode->children.size();
    if (beforeSibling)
    {
      auto beforeNode = findLayoutNode(beforeSibling);
      if (beforeNode && beforeNode->parent.lock() == parentNode)
      {
        auto it = find(parentNode->children.begin(), parentNode->children.end(), beforeNode);
        if (it != parentNode->children.end())
        {
          position = distance(parentNode->children.begin(), it);
        }
      }
    }

    // Insert child at position
    parentNode->children.insert(parentNode->children.begin() + position, childNode);
    childNode->parent = parentNode;

    // Handle integration between different formatting context types
    integrateChildToParent(parentNode, childNode, position);

    // Mark both parent and child as needing layout
    parentNode->needs_layout = true;
    childNode->needs_layout = true;
  }

  void LayoutTreeManager::removeChild(shared_ptr<LayoutObject> parent,
                                      shared_ptr<LayoutObject> child)
  {
    auto parentNode = findLayoutNode(parent);
    auto childNode = findLayoutNode(child);

    if (!parentNode || !childNode)
    {
      return; // Objects not managed
    }

    // Remove from parent's children
    auto &children = parentNode->children;
    children.erase(
      remove(children.begin(), children.end(), childNode),
      children.end());

    childNode->parent.reset();
    parentNode->needs_layout = true;
  }

  bool LayoutTreeManager::computeLayout(shared_ptr<LayoutObject> root,
                                        const ConstraintSpace &space)
  {
    auto rootNode = findLayoutNode(root);
    if (!rootNode)
    {
      return false;
    }

    return coordinateLayout(rootNode, space);
  }

  Fragment LayoutTreeManager::getLayoutResult(shared_ptr<LayoutObject> object) const
  {
    auto node = findLayoutNode(object);
    if (!node)
    {
      return Fragment(); // Empty fragment for unmanaged objects
    }

    return node->computed_fragment;
  }

  bool LayoutTreeManager::isManaged(shared_ptr<LayoutObject> object) const
  {
    return object_to_node_.find(object.get()) != object_to_node_.end();
  }

  void LayoutTreeManager::debugPrint() const
  {
    printf("=== Layout Tree Manager Debug ===\n");
    printf("Managed objects: %zu\n", object_to_node_.size());

    if (root_)
    {
      function<void(shared_ptr<LayoutNode>, int)> printNode =
        [&](shared_ptr<LayoutNode> node, int depth)
      {
        string indent(depth * 2, ' ');
        printf("%s%s (context: %s)\n",
               indent.c_str(),
               node->object->debugName().c_str(),
               node->context->type.isInline() ? "Inline" : node->context->type.isFlex() ? "Flex"
                                                         : node->context->type.isGrid() ? "Grid"
                                                                                        : "Block");

        for (auto &child : node->children)
        {
          printNode(child, depth + 1);
        }
      };

      printNode(root_, 0);
    }
    printf("=== End Layout Tree Manager Debug ===\n");
  }

  bool LayoutTreeManager::coordinateLayout(shared_ptr<LayoutNode> node,
                                           const ConstraintSpace &space)
  {
    if (!node->needs_layout)
    {
      return true; // Layout is up to date
    }

    // Compute layout for this node's formatting context
    auto result = node->context->computeLayout(space);
    if (!result || result->status() != LayoutResult::kSuccess)
    {
      return false;
    }

    node->computed_fragment = result->fragment();

    // For pure CSS contexts (inline, block), handle children directly
    if (node->context->isInline() || node->context->isBlock())
    {
      // Pure CSS layout contexts manage their children internally
      // or coordinate through the layout tree manager

      // Compute layout for children with appropriate constraint spaces
      for (auto &child : node->children)
      {
        ConstraintSpace childSpace = space; // TODO: Derive from parent's result
        if (!coordinateLayout(child, childSpace))
        {
          return false;
        }
      }
    }
    // For taffy-based contexts (flex, grid), they handle their own children
    // but we still need to coordinate with the tree manager

    node->needs_layout = false;
    return true;
  }

  void LayoutTreeManager::integrateChildToParent(shared_ptr<LayoutNode> parent,
                                                 shared_ptr<LayoutNode> child,
                                                 size_t position)
  {
    // Handle integration between different formatting context types

    if (parent->context->isInline() && child->context->isInline())
    {
      // Inline parent, inline child - pure CSS inline layout
      // The parent's inline formatting context will handle the child
    }
    else if (parent->context->isBlock() && child->context->isInline())
    {
      // Block parent, inline child - inline child flows within block
      // Block formatting context needs to account for inline children
    }
    else if (parent->context->isInline() && child->context->isBlock())
    {
      // Inline parent, block child - creates anonymous block wrapper
      // This is a complex case that needs special handling
    }
    else if (auto taffyParent = dynamic_cast<TaffyBasedFormattingContext *>(parent->context.get()))
    {
      // Taffy-based parent with any child type
      // Need to integrate the child into taffy's layout computation
      if (child->context->isInline() || child->context->isBlock())
      {
        // Pure CSS child in taffy parent - create placeholder or special handling
        // This is where the architectural integration happens
      }
    }

    // Mark both as needing layout after integration
    parent->needs_layout = true;
    child->needs_layout = true;
  }

  shared_ptr<LayoutTreeManager::LayoutNode>
  LayoutTreeManager::createLayoutNode(shared_ptr<LayoutObject> object,
                                      shared_ptr<FormattingContext> context)
  {
    auto node = make_shared<LayoutNode>();
    node->object = object;
    node->context = context;
    node->needs_layout = true;
    return node;
  }

  shared_ptr<LayoutTreeManager::LayoutNode>
  LayoutTreeManager::findLayoutNode(shared_ptr<LayoutObject> object) const
  {
    auto it = object_to_node_.find(object.get());
    return (it != object_to_node_.end()) ? it->second : nullptr;
  }
}