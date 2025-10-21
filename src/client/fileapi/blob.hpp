#pragma once

#include <string>
#include <vector>
#include <future>
#include <client/scripting_base/v8_object_holder.hpp>

namespace client_fileapi
{
  class Blob : public scripting_base::JSObjectHolder
  {
  public:
    using Bytes = std::vector<uint8_t>;

  public:
    Blob();
    Blob(const Bytes &bytes, const std::string &type = "");

  public:
    size_t size() const
    {
      return size_;
    }
    std::string type() const
    {
      return type_;
    }
    std::promise<Bytes> bytes() const;
    std::promise<std::string> text() const;
    const Bytes &data() const
    {
      return bytes_;
    }

  private:
    size_t size_;
    std::string type_;
    Bytes bytes_;
  };
}
