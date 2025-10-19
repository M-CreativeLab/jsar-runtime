#include <client/fileapi/blob.hpp>

using namespace std;

namespace client_fileapi
{
  Blob::Blob()
  {
  }

  promise<Uint8Array> Blob::bytes() const
  {
    promise<Uint8Array> promise;
    promise.set_value(bytes_);
    return promise;
  }

  promise<string> Blob::text() const
  {
    promise<string> promise;
    string text(bytes_.begin(), bytes_.end());
    promise.set_value(text);
    return promise;
  }
}
