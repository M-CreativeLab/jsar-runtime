#include <client/fetch/headers.hpp>

namespace endor
{
  namespace client_fetch
  {
    Headers::Headers()
    {
    }

    void Headers::append(const std::string &name, const std::string &value)
    {
      auto it = find(name);
      if (it != end())
      {
        it->second += ", " + value;
      }
      else
      {
        insert({name, value});
      }
    }

    void Headers::remove(const std::string &name)
    {
      erase(name);
    }

    std::string Headers::get(const std::string &name) const
    {
      auto it = find(name);
      if (it != end())
      {
        return it->second;
      }
      return "";
    }

    bool Headers::has(const std::string &name) const
    {
      return find(name) != end();
    }

    void Headers::set(const std::string &name, const std::string &value)
    {
      (*this)[name] = value;
    }
  }
} // namespace endor
