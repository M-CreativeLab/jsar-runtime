#include <client/url/url_search_params.hpp>

using namespace std;

namespace client_url
{
  URLSearchParams::URLSearchParams()
  {
  }

  URLSearchParams::URLSearchParams(const string &query)
  {
    size_t start = 0;
    while (start < query.size())
    {
      size_t end = query.find('&', start);
      if (end == string::npos)
      {
        end = query.size();
      }

      size_t eq = query.find('=', start);
      if (eq != string::npos && eq < end)
      {
        string name = query.substr(start, eq - start);
        string value = query.substr(eq + 1, end - eq - 1);
        append(name, value);
      }
      else
      {
        string name = query.substr(start, end - start);
        append(name, "");
      }

      start = end + 1;
    }
  }

  void URLSearchParams::append(const string &name, const string &value)
  {
    emplace_back(name, value);
  }

  void URLSearchParams::remove(const string &name, optional<string> value)
  {
    if (value.has_value())
    {
      auto it = begin();
      while (it != end())
      {
        if (it->first == name && it->second == value.value())
        {
          it = erase(it);
        }
        else
        {
          ++it;
        }
      }
    }
    else
    {
      auto it = begin();
      while (it != end())
      {
        if (it->first == name)
        {
          it = erase(it);
        }
        else
        {
          ++it;
        }
      }
    }
  }

  string URLSearchParams::get(const string &name) const
  {
    for (const auto &pair : *this)
    {
      if (pair.first == name)
        return pair.second;
    }
    return "";
  }

  vector<string> URLSearchParams::getAll(const string &name) const
  {
    vector<string> values;
    for (const auto &pair : *this)
    {
      if (pair.first == name)
        values.push_back(pair.second);
    }
    return values;
  }

  bool URLSearchParams::has(const string &name) const
  {
    for (const auto &pair : *this)
    {
      if (pair.first == name)
        return true;
    }
    return false;
  }

  void URLSearchParams::set(const string &name, const string &value)
  {
    // Remove all existing entries with the same name
    remove(name);

    // Add the new entry
    append(name, value);
  }

  string URLSearchParams::toString() const
  {
    string result;
    for (size_t i = 0; i < size(); ++i)
    {
      const auto &pair = (*this)[i];
      result += pair.first + "=" + pair.second;
      if (i < size() - 1)
      {
        result += "&";
      }
    }
    return result;
  }

  vector<string> URLSearchParams::keys() const
  {
    vector<string> result;
    for (const auto &pair : *this)
      result.push_back(pair.first);
    return result;
  }

  vector<string> URLSearchParams::values() const
  {
    vector<string> result;
    for (const auto &pair : *this)
      result.push_back(pair.second);
    return result;
  }
}
