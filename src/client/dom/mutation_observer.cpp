#include "./mutation_observer.hpp"

namespace dom
{
  using namespace std;

  MutationRecord MutationRecord::OnAddChild(std::shared_ptr<Node> target, std::shared_ptr<Node> addedNode)
  {
    MutationRecord record(MutationType::ChildList, target);
    record.addedNodes.push_back(addedNode);
    return record;
  }

  MutationRecord MutationRecord::OnRemoveChild(std::shared_ptr<Node> target, std::shared_ptr<Node> removedNode)
  {
    MutationRecord record(MutationType::ChildList, target);
    record.removedNodes.push_back(removedNode);
    return record;
  }

  MutationRecord MutationRecord::OnReplaceChild(std::shared_ptr<Node> target,
                                                std::shared_ptr<Node> newChild,
                                                std::shared_ptr<Node> oldChild)
  {
    MutationRecord record(MutationType::ChildList, target);
    record.addedNodes.push_back(newChild);
    record.removedNodes.push_back(oldChild);
    return record;
  }

  MutationRecord MutationRecord::OnAttributeChanged(std::shared_ptr<Node> target,
                                                    const string &attributeName,
                                                    const string &oldValue)
  {
    MutationRecord record(MutationType::Attributes, target);
    record.attributeName = attributeName;
    record.oldValue = oldValue;
    return record;
  }

  MutationRecord MutationRecord::OnTextContentChanged(std::shared_ptr<Node> target, const string &oldValue)
  {
    MutationRecord record(MutationType::CharacterData, target);
    record.oldValue = oldValue;
    return record;
  }

  MutationRecord::MutationRecord(MutationType type, std::shared_ptr<Node> target)
      : type(type),
        target(target)
  {
    assert(target != nullptr && "The target node is null when creating a mutation record.");
  }

  bool MutationRecord::isTarget(const shared_ptr<Node> node) const
  {
    auto targetRef = target.lock();
    if (targetRef == nullptr)
      return false;
    return targetRef.get() == node.get();
  }

  MutationObserver::MutationObserver(MutationCallback callback)
      : callback_(callback)
  {
  }

  MutationObserver::~MutationObserver()
  {
    disconnect();
  }

  void MutationObserver::disconnect()
  {
    if (observedNode_ == nullptr)
      return;

    observedNode_->removeMutationObserver(shared_from_this());
    observedNode_.reset();
  }

  bool MutationObserver::observe(std::shared_ptr<Node> target, const MutationObserver::ObserveOptions &options)
  {
    if (TR_UNLIKELY(target == nullptr))
      throw invalid_argument("target must not be null");
    if (TR_UNLIKELY(observedNode_ != nullptr))
      throw runtime_error("The mutation observer is already observing a node.");

    // At a minimum, the options object must contain at least one of the following properties:
    // - childList
    // - attributes
    // - characterData
    if (!options.childList && options.attributes == false && options.characterData == false)
      return false;

    if (target->addMutationObserver(shared_from_this()))
    {
      observedNode_ = target;
      options_ = options;
      return true;
    }
    else
    {
      return false;
    }
  }

  std::vector<MutationRecord> MutationObserver::takeRecords()
  {
    unique_lock<shared_mutex> lock(recordsMutex_);
    vector<MutationRecord> records = recordsQueue_;
    recordsQueue_.clear();
    return records;
  }

  bool MutationObserver::queueRecord(const MutationRecord &record)
  {
    if (!record.isTarget(observedNode_) && options_.subtree == false)
      return false;
    if (record.type == MutationType::Attributes && options_.attributes == false)
      return false;
    if (record.type == MutationType::CharacterData && options_.characterData == false)
      return false;
    if (record.type == MutationType::ChildList && options_.childList == false)
      return false;

    unique_lock<shared_mutex> lock(recordsMutex_);
    MutationRecord newRecord = record;
    newRecord.target = observedNode_;
    recordsQueue_.push_back(newRecord);
    return true;
  }
}
