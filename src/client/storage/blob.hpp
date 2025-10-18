#pragma once

#include <string>
#include <vector>
#include <future>
#include <client/scripting_base/v8_object_holder.hpp>

namespace client_storage
{
  using Uint8Array = std::vector<uint8_t>;

  class Blob : public scripting_base::JSObjectHolder
  {
  public:
    Blob();

  public:
    std::promise<Uint8Array> bytes() const;
    std::promise<std::string> text() const;

  private:
    size_t size_;
    std::string type_;
    Uint8Array bytes_;
  };
}
