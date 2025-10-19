#include <client/fileapi/blob_url_registry.hpp>

namespace client_fileapi
{
  BlobURLRegistry::BlobURLRegistry()
  {
  }

  std::string BlobURLRegistry::createObjectURL(const Blob& blob)
  {
    // Generate a unique URL for the blob
    std::string url = "blob:" + std::to_string(reinterpret_cast<std::uintptr_t>(&blob));
    url_to_blob_map_[url] = blob;
    blob_to_url_map_[blob] = url;
    return url;
  }

  void BlobURLRegistry::revokeObjectURL(const std::string& url)
  {
    auto it = url_to_blob_map_.find(url);
    if (it != url_to_blob_map_.end())
    {
      blob_to_url_map_.erase(it->second);
      url_to_blob_map_.erase(it);
    }
  }
}