#pragma once

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string_view>
#include <memory>

extern "C"
{
  const uint8_t *get_libnode_ptr();
  uintptr_t get_libnode_size();
  const uint8_t *get_libnode_md5_ptr();
  uintptr_t get_libnode_md5_size();
  const uint8_t *get_jsbootstrap_ptr(int jsframework_name);
  uintptr_t get_jsbootstrap_size(int jsframework_name);
  const uint8_t *get_jsbundle_ptr(int id);
  uintptr_t get_jsbundle_size(int id);
  int32_t carbonite_decompress_binary(const uint8_t *input_ptr,
                                      size_t input_len,
                                      uint8_t **output_ptr,
                                      size_t *output_len);
  void carbonite_release_memory(uint8_t *ptr, size_t len);
}

/**
 * The name of the JavaScript framework to bootstrap runtime: Babylon or Three.js.
 */
enum class JSFrameworkName
{
  BABYLON = 0,
  THREE = 1
};

enum class JSBundles
{
  MainEntry = 0,
  WebWorkersEntry = 1,
};

class JSBundle
{
public:
  /**
   * Get the pointer to the JavaScript bootstrap source code.
   *
   * @param jsframework_name The name of the JavaScript framework.
   * @returns The pointer to the JavaScript bootstrap source code.
   * Get the str pointer to the libnode source code.
   */
  static inline const uint8_t *GetBootstrapSourcePtr(JSFrameworkName jsframework_name = JSFrameworkName::BABYLON)
  {
    return get_jsbootstrap_ptr(static_cast<int>(jsframework_name));
  }

  /**
   * Get the size of the JavaScript bootstrap source code.
   *
   * @param jsframework_name The name of the JavaScript framework.
   * @returns The size of the JavaScript bootstrap source code.
   */
  static inline uintptr_t GetBootstrapSourceSize(JSFrameworkName jsframework_name = JSFrameworkName::BABYLON)
  {
    return get_jsbootstrap_size(static_cast<int>(jsframework_name));
  }

  /**
   * Get the pointer to the JavaScript client entry source code.
   *
   * @returns The pointer to the JavaScript client entry source code.
   */
  static inline const uint8_t *GetClientEntrySourcePtr(JSBundles id = JSBundles::MainEntry)
  {
    return get_jsbundle_ptr(static_cast<int>(id));
  }

  /**
   * Get the size of the JavaScript client entry source code.
   *
   * @returns The size of the JavaScript client entry source code.
   */
  static inline uintptr_t GetClientEntrySourceSize(JSBundles id = JSBundles::MainEntry)
  {
    return get_jsbundle_size(static_cast<int>(id));
  }
};

namespace carbonite
{
  class StringReference
  {
  public:
    StringReference(const unsigned char *ptr, size_t len)
        : ptr_(const_cast<unsigned char *>(ptr)),
          len_(len)
    {
    }
    StringReference(const StringReference &) = default;

  public:
    StringReference &operator=(const StringReference &) = default;

  public:
    inline unsigned char *data() { return ptr_; }
    inline const unsigned char *data() const { return ptr_; }
    inline size_t size() const { return len_; }

  private:
    unsigned char *ptr_;
    size_t len_;
  };

  static inline std::shared_ptr<StringReference> decompressBinary(const unsigned char *binaryPtr, size_t binaryLen)
  {
    uint8_t *outputPtr = nullptr;
    size_t outputLen = 0;
    if (carbonite_decompress_binary(binaryPtr, binaryLen, &outputPtr, &outputLen) != 0)
      return nullptr;

    StringReference result(reinterpret_cast<unsigned char *>(outputPtr), outputLen);
    auto deleter = [outputPtr, outputLen](StringReference *ref)
    {
      assert(ref->data() == outputPtr);
      assert(ref->size() == outputLen);
      carbonite_release_memory(reinterpret_cast<uint8_t *>(ref->data()), ref->size());
      delete ref;
    };
    return std::shared_ptr<StringReference>(new StringReference(result), deleter);
  }
}
