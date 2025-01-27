#pragma once

#include <optional>
#include <string>
#include <vector>

namespace dom
{
  class DOMTokenList : std::vector<std::string>
  {
  public:
    /**
     * Constructs a new DOMTokenList.
     *
     * @param input The initial value of the list.
     * @param supportedTokens The list of supported tokens.
     */
    DOMTokenList(std::string input = "",
                 std::vector<std::string> supportedTokens = {},
                 std::function<void(const DOMTokenList &)> updateCallback = nullptr)
        : std::vector<std::string>(),
          supportedTokens_(supportedTokens),
          updateCallback_(updateCallback)
    {
      resetFrom(input);
    }
    /**
     * Constructs a new DOMTokenList from another DOMTokenList and an initial value.
     *
     * @param other The other DOMTokenList to copy from.
     * @param input The initial value of the list.
     */
    DOMTokenList(DOMTokenList &other, std::string input)
        : std::vector<std::string>(),
          supportedTokens_(other.supportedTokens_),
          updateCallback_(other.updateCallback_)
    {
      resetFrom(input);
    }

  public:
    using std::vector<std::string>::begin;
    using std::vector<std::string>::end;

  public:
    /**
     * Gets the number of tokens in the list.
     *
     * @returns The number of tokens in the list.
     */
    inline size_t length() const { return size(); }
    /**
     * A stringifier property that returns the value of the list as a string.
     *
     * @returns The value of the list as a string.
     */
    inline const std::string &value() const
    {
      if (isDirty_ == true) // Update the value if it is dirty
      {
        std::string result;
        for (size_t i = 0; i < size(); i++)
        {
          if (i > 0)
            result += std::string(DOMTokenList::SEP);
          result += at(i);
        }
        value_ = result;
        isDirty_ = false;
      }
      return value_;
    }

  public:
    /**
     * Gets the item in the list by its index, or `nullopt` if the index is greater than or equal to the
     * list's length.
     *
     * @param index The index of the item to get.
     * @returns The `nullopt` or the item at the given index.
     */
    std::optional<std::string> item(size_t index) const
    {
      if (index < size())
      {
        return std::make_optional(at(index));
      }
      return std::nullopt;
    }
    /**
     * Checks if the list contains the given token.
     *
     * @param token The token to check.
     * @returns `true` if the list contains the given token, otherwise `false`.
     */
    bool contains(const std::string &token) const
    {
      return std::find(begin(), end(), token) != end();
    }
    /**
     * Adds the given token to the list.
     *
     * @param token The token to add.
     */
    void add(const std::string &token)
    {
      if (!contains(token))
      {
        push_back(token);
        markAsDirty();
      }
    }
    /**
     * Removes the given token from the list.
     *
     * @param token The token to remove.
     */
    void remove(const std::string &token)
    {
      auto it = std::find(begin(), end(), token);
      if (it != end())
      {
        erase(it);
        markAsDirty();
      }
    }
    /**
     * Replaces the given token in the list with the new token.
     *
     * @param oldToken The token to replace.
     * @param newToken The new token to replace with.
     * @returns `true` if the token was replaced, otherwise `false`.
     */
    bool replace(const std::string &oldToken, const std::string &newToken)
    {
      auto it = std::find(begin(), end(), oldToken);
      if (it != end())
      {
        *it = newToken;
        markAsDirty();
        return true;
      }
      return false;
    }
    /**
     * Checks if the list supports the given token.
     *
     * @param token The token to check.
     * @returns `true` if the list supports the given token, otherwise `false`.
     */
    bool supports(const std::string &token) const
    {
      return std::find(supportedTokens_.begin(), supportedTokens_.end(), token) != supportedTokens_.end();
    }
    /**
     * Toggles the given token in the list.
     *
     * @param token The token to toggle.
     * @returns `true` if the token was added, otherwise `false`.
     */
    inline bool toggle(const std::string &token)
    {
      if (contains(token))
      {
        remove(token);
        return false;
      }
      add(token);
      return true;
    }
    /**
     * It calls the callback given in parameter once for each value pair in the list, in insertion order.
     */
    void forEach(std::function<void(const std::string &token, size_t index, const DOMTokenList &list)> callback,
                 void *thisArg = nullptr) const
    {
      for (size_t i = 0; i < size(); i++)
      {
        callback(at(i), i, *this);
      }
    }
    /**
     * @returns The list of keys in the list.
     */
    std::vector<uint32_t> keys() const
    {
      std::vector<uint32_t> it;
      for (int i = 0; i < size(); i++)
        it.push_back(i);
      return it;
    }
    /**
     * @returns The list of values in the list.
     */
    std::vector<std::string> values() const
    {
      return *this;
    }

  public:
    operator std::string() const { return value(); }
    friend std::ostream &operator<<(std::ostream &os, const DOMTokenList &list)
    {
      os << list.value();
      return os;
    }

  private:
    void resetFrom(const std::string &input)
    {
      if (size() > 0)
        clear();

      if (!input.empty())
      {
        size_t start = 0;
        size_t end = 0;
        while (end < input.length())
        {
          end = input.find(SEP, start);
          if (end == std::string::npos)
            end = input.length();
          push_back(input.substr(start, end - start));
          start = end + 1;
        }
      }

      value_ = input;
      isDirty_ = false;
    }
    void markAsDirty()
    {
      isDirty_ = true;
      if (updateCallback_)
        updateCallback_(*this);
    }

  private:
    std::vector<std::string> supportedTokens_;
    std::function<void(const DOMTokenList &)> updateCallback_;
    mutable std::string value_;
    mutable bool isDirty_;

  private:
    static constexpr const char *const SEP = " ";
  };
}
