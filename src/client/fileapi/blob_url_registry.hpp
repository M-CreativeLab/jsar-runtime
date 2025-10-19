#pragma once

#include <string>
#include <vector>
#include <client/scripting_base/v8_object_holder.hpp>
#include <client/fileapi/blob.hpp>

namespace client_fileapi
{
  class BlobURLRegistry
  {
  public:
    BlobURLRegistry();

  public:
    std::string createObjectURL(const Blob& blob);
    void revokeObjectURL(const std::string& url);

  private:
    std::unordered_map<std::string, Blob> url_to_blob_map_;
    std::unordered_map<Blob, std::string> blob_to_url_map_;
  };
}
