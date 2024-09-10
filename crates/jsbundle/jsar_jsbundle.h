#pragma once

#include <stdint.h>

extern "C"
{
  const uint8_t *get_jsbootstrap_ptr(int jsframework_name);
  uintptr_t get_jsbootstrap_size(int jsframework_name);
  const uint8_t *get_jsbundle_ptr();
  uintptr_t get_jsbundle_size();
}

/**
 * The name of the JavaScript framework to bootstrap runtime: Babylon or Three.js.
 */
enum class JSFrameworkName
{
  BABYLON = 0,
  THREE = 1
};

class JSBundle
{
public:
  /**
   * Get the pointer to the JavaScript bootstrap source code.
   *
   * @param jsframework_name The name of the JavaScript framework.
   * @returns The pointer to the JavaScript bootstrap source code.
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
  static inline const uint8_t *GetClientEntrySourcePtr()
  {
    return get_jsbundle_ptr();
  }

  /**
   * Get the size of the JavaScript client entry source code.
   *
   * @returns The size of the JavaScript client entry source code.
   */
  static inline uintptr_t GetClientEntrySourceSize()
  {
    return get_jsbundle_size();
  }
};
