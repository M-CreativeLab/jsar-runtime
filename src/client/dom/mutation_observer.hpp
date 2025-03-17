#pragma once

#include <vector>
#include <string>
#include <memory>
#include <optional>
#include <functional>
#include <shared_mutex>

#include "./node.hpp"
#include "./node_list.hpp"

namespace dom
{
  class MutationRecord;
  class MutationObserver;
  typedef std::function<void(const std::vector<const MutationRecord> &, MutationObserver &)> MutationCallback;

  enum class MutationType
  {
    Attributes,
    CharacterData,
    ChildList,
  };

  class MutationRecord
  {
    friend class MutationObserver;

  public:
    /**
     * A new `MutationRecord` when a child node is added.
     *
     * @param target The target node.
     * @param addedNode The added child node.
     */
    static MutationRecord OnAddChild(std::shared_ptr<Node> target, std::shared_ptr<Node> addedNode);
    /**
     * A new `MutationRecord` when a child node is removed.
     *
     * @param target The target node.
     * @param removedNode The removed child node.
     */
    static MutationRecord OnRemoveChild(std::shared_ptr<Node> target, std::shared_ptr<Node> removedNode);
    /**
     * A new `MutationRecord` when a child node is replaced.
     *
     * @param target The target node.
     * @param newChild The new child node.
     * @param oldChild The old child node.
     */
    static MutationRecord OnReplaceChild(std::shared_ptr<Node> target,
                                         std::shared_ptr<Node> newChild,
                                         std::shared_ptr<Node> oldChild);
    /**
     * A new `MutationRecord` when an attribute of a node is changed.
     *
     * @param target The target node.
     * @param attributeName The name of the attribute.
     * @param oldValue The old value of the attribute.
     */
    static MutationRecord OnAttributeChanged(std::shared_ptr<Node> target,
                                             const std::string &attributeName,
                                             const std::string &oldValue);
    /**
     * A new `MutationRecord` when the text content of a node is changed.
     *
     * @param target The target node.
     * @param oldValue The old value of the text content.
     */
    static MutationRecord OnTextContentChanged(std::shared_ptr<Node> target, const std::string &oldValue);

  private:
    MutationRecord(MutationType type, std::shared_ptr<Node> target);

  public:
    bool isTarget(std::shared_ptr<Node> node) const;

  public:
    friend std::ostream &operator<<(std::ostream &os, const MutationRecord &record)
    {
      os << "MutationRecord { type: ";
      switch (record.type)
      {
      case MutationType::Attributes:
        os << "Attributes";
        break;
      case MutationType::CharacterData:
        os << "CharacterData";
        break;
      case MutationType::ChildList:
        os << "ChildList";
        break;
      default:
        os << "Unknown";
        break;
      }
      os << ", target: " << record.target.lock()->nodeName;
      if (record.attributeName.has_value())
        os << ", attributeName: " << record.attributeName.value();
      if (record.attributeNamespace.has_value())
        os << ", attributeNamespace: " << record.attributeNamespace.value();
      if (record.oldValue.has_value())
        os << ", oldValue: " << record.oldValue.value();
      if (record.addedNodes.size() > 0)
      {
        os << ", addedNodes: [";
        for (auto node : record.addedNodes)
          os << node->nodeName << ", ";
        os << "]";
      }
      if (record.removedNodes.size() > 0)
      {
        os << ", removedNodes: [";
        for (auto node : record.removedNodes)
          os << node->nodeName << ", ";
        os << "]";
      }
      if (record.previousSibling.lock() != nullptr)
        os << ", previousSibling: " << record.previousSibling.lock()->nodeName;
      if (record.nextSibling.lock() != nullptr)
        os << ", nextSibling: " << record.nextSibling.lock()->nodeName;
      os << " }";
      return os;
    }

  public:
    MutationType type;
    NodeList<Node> addedNodes = NodeList<Node>::Empty();
    NodeList<Node> removedNodes = NodeList<Node>::Empty();
    std::optional<std::string> attributeName;
    std::optional<std::string> attributeNamespace;
    std::optional<std::string> oldValue;
    std::weak_ptr<Node> target;
    std::weak_ptr<Node> previousSibling;
    std::weak_ptr<Node> nextSibling;
  };

  class MutationObserver : public std::enable_shared_from_this<MutationObserver>
  {
    friend class Node;

  public:
    struct ObserveOptions
    {
      bool subtree = false;
      bool childList = false;
      bool attributes = false;
      std::vector<std::string> attributeFilter;
      bool attributeOldValue = false;
      bool characterData = false;
      bool characterDataOldValue = false;
    };

  public:
    MutationObserver(MutationCallback callback = nullptr);
    ~MutationObserver();

  public:
    /**
     * @returns `true` if the observer is observing the subtree.
     */
    inline bool isSubtreeObserved() const { return options_.subtree; }

  public:
    void disconnect();
    bool observe(std::shared_ptr<Node> target, const ObserveOptions &options);
    std::vector<MutationRecord> takeRecords();

  private:
    bool queueRecord(const MutationRecord &record);

  private:
    MutationCallback callback_;
    std::shared_ptr<Node> observedNode_;
    ObserveOptions options_;
    std::vector<MutationRecord> recordsQueue_;
    std::shared_mutex recordsMutex_;
  };
}
