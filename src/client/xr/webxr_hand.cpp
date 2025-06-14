#include "./webxr_hand.hpp"
#include "./webxr_spaces.hpp"
#include "./webxr_input_sources.hpp"

namespace client_xr
{
  using namespace std;

  XRHand::XRHand(shared_ptr<XRInputSource> inputSource)
  {
    for (auto joint : inputSource->inputSourceData_->joints)
      push_back(XRJointSpace::Make(inputSource, joint.index));
  }

  XRHand::XRHand(XRHand &that)
      : vector<shared_ptr<XRJointSpace>>(that)
      , inputSource_(that.inputSource_)
  {
  }

  shared_ptr<XRJointSpace> XRHand::get(client_xr::XRJointIndex index)
  {
    for (auto joint : *this)
      if (joint->index == index)
        return joint;
    return nullptr;
  }

  shared_ptr<XRJointSpace> XRHand::get(const string key)
  {
    for (auto joint : *this)
      if (joint->name == key)
        return joint;
    return nullptr;
  }

  vector<string> XRHand::keys()
  {
    vector<string> keys;
    for (auto joint : *this)
      keys.push_back(joint->name);
    return keys;
  }
}
