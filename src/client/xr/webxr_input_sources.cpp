#include <set>
#include "./common.hpp"
#include "./webxr_input_sources.hpp"
#include "./webxr_session.hpp"
#include "./device.hpp"

using namespace std;

namespace client_xr
{
  XRInputSource::XRInputSource(shared_ptr<XRSession> session, xr::TrXRInputSource *inputSourceData)
      : session_(session),
        inputSourceData_(inputSourceData)
  {
  }

  bool XRInputSource::dispatchSelectOrSqueezeEvents(std::shared_ptr<XRFrame> frame)
  {
    assert(session_ != nullptr);

    if (inputSourceData_->primaryActionPressed == true)
    {
      // When the primary action is pressed.
      if (primaryActionPressed_ == false)
      {
        // When the primary action is pressed for the first time.
        primaryActionPressed_ = true;
        session_->onPrimaryActionStart(shared_from_this(), frame);
      }
    }
    else
    {
      // When the primary action is not pressed.
      if (primaryActionPressed_ == true)
      {
        // When the primary action is released.
        primaryActionPressed_ = false;
        session_->onPrimaryActionEnd(shared_from_this(), frame);
      }
    }

    if (inputSourceData_->squeezeActionPressed == true)
    {
      // When the squeeze action is pressed.
      if (squeezeActionPressed_ == false)
      {
        // When the squeeze action is pressed for the first time.
        squeezeActionPressed_ = true;
        session_->onSqueezeActionStart(shared_from_this(), frame);
      }
    }
    else
    {
      // When the squeeze action is not pressed.
      if (squeezeActionPressed_ == true)
      {
        // When the squeeze action is released.
        squeezeActionPressed_ = false;
        session_->onSqueezeActionEnd(shared_from_this(), frame);
      }
    }
    return true;
  }

  XRInputSourceArray::XRInputSourceArray(shared_ptr<XRSession> session)
      : session_(session),
        device_(session->device())
  {
  }

  shared_ptr<XRInputSource> XRInputSourceArray::getInputSourceById(int id)
  {
    for (auto &inputSource : *this)
    {
      if (inputSource->inputSourceData_->id == id)
        return inputSource;
    }
    return nullptr;
  }

  void checkInputSourceEnabledAndInsertTo(set<int> &targetSet, xr::TrXRInputSource *inputSource)
  {
    if (inputSource != nullptr && inputSource->enabled)
      targetSet.insert(inputSource->id);
  }

  void XRInputSourceArray::updateInputSources(std::shared_ptr<XRFrame> frame,
                                              std::shared_ptr<XRSession> session,
                                              InputSourcesChangedCallback onChangedCallback)
  {
    auto inputSourcesZone = device_->inputSourcesZone();

    /**
     * 1. Prepare sets including: added, removed, new, and old.
     */
    set<int> addedInputSourceIds;
    set<int> removedInputSourceIds;
    set<int> newInputSourceIds;
    {
      checkInputSourceEnabledAndInsertTo(newInputSourceIds, inputSourcesZone->getGazeInputSource());
      checkInputSourceEnabledAndInsertTo(newInputSourceIds, inputSourcesZone->getMainControllerInputSource());
      checkInputSourceEnabledAndInsertTo(newInputSourceIds, inputSourcesZone->getTransientPointerInputSource());
      checkInputSourceEnabledAndInsertTo(newInputSourceIds, inputSourcesZone->getHandInputSource(xr::TrHandness::Left));
      checkInputSourceEnabledAndInsertTo(newInputSourceIds, inputSourcesZone->getHandInputSource(xr::TrHandness::Right));
      checkInputSourceEnabledAndInsertTo(newInputSourceIds, inputSourcesZone->getScreenInputSource(0));
      checkInputSourceEnabledAndInsertTo(newInputSourceIds, inputSourcesZone->getScreenInputSource(1));
    }
    set<int> currentInputSourceIds;
    {
      // Fetch current input sources from the array.
      for (uint32_t i = 0; i < size(); i++)
      {
        auto inputSource = at(i);
        currentInputSourceIds.insert(inputSource->inputSourceData_->id);
      }
    }

    /**
     * 2. Compare the new and old sets to get added and removed sets.
     */
    if (currentInputSourceIds.size() == 0)
    {
      addedInputSourceIds = newInputSourceIds;
    }
    else
    {
      set_difference(newInputSourceIds.begin(), newInputSourceIds.end(),
                     currentInputSourceIds.begin(), currentInputSourceIds.end(),
                     inserter(addedInputSourceIds, addedInputSourceIds.begin()));
      set_difference(currentInputSourceIds.begin(), currentInputSourceIds.end(),
                     newInputSourceIds.begin(), newInputSourceIds.end(),
                     inserter(removedInputSourceIds, removedInputSourceIds.begin()));
    }

    /**
     * 3. Update the new set to the current set.
     */
    currentInputSourceIds = newInputSourceIds;

    /**
     * 4. Process the removed input sources firstly.
     */
    if (removedInputSourceIds.size() > 0)
    {
      vector<shared_ptr<XRInputSource>> removed;
      for (auto id : removedInputSourceIds)
      {
        auto inputSource = getInputSourceById(id);
        if (inputSource != nullptr)
          removed.push_back(inputSource);
      }
      onChangedCallback({}, removed);
    }

    /**
     * 5. Reset the array based on current input sources.
     *
     * NOTE: This is safe because we have already processed the removed input sources.
     */
    if (removedInputSourceIds.size() > 0 || addedInputSourceIds.size() > 0)
    {
      // Only update the JavaScript array object when there is a change, otherwise no need to update.
      vector<shared_ptr<XRInputSource>> tmpArray;
      for (int id : currentInputSourceIds)
      {
        auto inputSource = getInputSourceById(id);
        if (inputSource != nullptr)
        {
          // Reuse the existed input source object.
          // tmpArray.Set(tmpArray.Length(), inputSource->Value());
          tmpArray.push_back(inputSource);
        }
        else
        {
          auto newInputSource = make_shared<XRInputSource>(session, inputSourcesZone->getInputSourceById(id));
          tmpArray.push_back(newInputSource);
        }
      }
      clear();
      for (auto &inputSource : tmpArray)
        push_back(inputSource);
    }

    /**
     * 6. Process the added input sources.
     *
     * NOTE: We don't create any new input source objects here, just use instances in the above step.
     */
    if (addedInputSourceIds.size() > 0)
    {
      vector<shared_ptr<XRInputSource>> added;
      for (auto id : addedInputSourceIds)
      {
        auto inputSource = getInputSourceById(id);
        if (inputSource != nullptr)
          added.push_back(inputSource);
      }
      if (added.size() > 0)
        onChangedCallback(added, {});
    }

    /**
     * 7. Dispatch `select` and `squeeze` events for all existed input sources.
     */
    for (uint32_t i = 0; i < size(); i++)
    {
      auto inputSource = at(i);
      if (inputSource != nullptr)
        inputSource->dispatchSelectOrSqueezeEvents(frame);
      else
        std::cerr << "failed to dispatch events on XRInputSource(" << i << "): value is not an object." << std::endl;
    }
  }
}
