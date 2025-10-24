#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <client/scripting_base/v8_object_holder.hpp>
#include <client/fileapi/blob.hpp>

namespace endor
{
  namespace client_fileapi
  {
    class BlobURLRegistry
    {
    public:
      BlobURLRegistry();

    public:
      std::string createObjectURL(std::shared_ptr<Blob> blob);
      void revokeObjectURL(const std::string &url);
      std::shared_ptr<Blob> get(const std::string &url) const;

    private:
      std::unordered_map<std::string, std::shared_ptr<Blob>> url_to_blob_map_;
    };
  }
} // namespace endor
