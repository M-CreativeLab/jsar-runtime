#pragma once

#include <stdint.h>

extern "C"
{
  /**
   * Get the str pointer to the JavaScript bootstrap source code.
   */
  const uint8_t *get_jsbootstrap_ptr();
  
  /**
   * Get the size of the JavaScript bootstrap source code.
   */
  uintptr_t get_jsbootstrap_size();

  /**
   * Get the str pointer to the JavaScript bundle source code.
   */
  const uint8_t *get_jsbundle_ptr();

  /**
   * Get the size of the JavaScript bundle source code.
   */
  uintptr_t get_jsbundle_size();
}
