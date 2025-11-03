#include <client/fileapi/blob.hpp>

using namespace std;

namespace endor
{
  namespace client_fileapi
  {
    Blob::Blob()
        : size_(0)
        , type_("")
        , bytes_()
    {
    }

    Blob::Blob(const Bytes &bytes, const string &type)
        : size_(bytes.size())
        , type_(type)
        , bytes_(bytes)
    {
    }

    promise<Blob::Bytes> Blob::bytes() const
    {
      promise<Bytes> promise;
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
} // namespace endor
