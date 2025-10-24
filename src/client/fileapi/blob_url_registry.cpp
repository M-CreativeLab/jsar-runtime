#include <array>
#include <client/fileapi/blob_url_registry.hpp>

// FIXME(Yorkie): forcily define cpp_lib_span to fix build error for android r25
#if defined(__ANDROID__)
#include <cstddef>
// Check if the compiler supports <span>
#if __has_include(<span>) && !defined(__cpp_lib_span)
#define __cpp_lib_span 202002L
#endif
#endif
#include <stduuid/uuid.h>

using namespace std;

namespace endor
{
  namespace client_fileapi
  {
    static uuids::uuid_random_generator GetIdGenerator()
    {
      random_device device;
      auto seed_data = array<int, mt19937::state_size>{};
      generate(begin(seed_data), end(seed_data), ref(device));
      seed_seq seq(begin(seed_data), end(seed_data));
      mt19937 generator(seq);
      return uuids::uuid_random_generator(generator);
    }

    BlobURLRegistry::BlobURLRegistry()
    {
    }

    string BlobURLRegistry::createObjectURL(shared_ptr<Blob> blob)
    {
      // Generate a random UUID for the blob URL
      uuids::uuid_random_generator generate = GetIdGenerator();
      uuids::uuid blob_id = generate();
      assert(!blob_id.is_nil() && "Failed to generate blob id");

      string url = "blob:" + to_string(blob_id);
      url_to_blob_map_[url] = blob;
      return url;
    }

    void BlobURLRegistry::revokeObjectURL(const string &url)
    {
      auto it = url_to_blob_map_.find(url);
      if (it != url_to_blob_map_.end())
        url_to_blob_map_.erase(it);
    }

    shared_ptr<Blob> BlobURLRegistry::get(const string &url) const
    {
      auto it = url_to_blob_map_.find(url);
      if (it != url_to_blob_map_.end())
        return it->second;
      return nullptr;
    }
  }
} // namespace endor
