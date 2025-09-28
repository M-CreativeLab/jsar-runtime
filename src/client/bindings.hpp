#pragma once

// Include bindings for various modules
#include <bindings/browser/binding.hpp>
#include <bindings/cssom/binding.hpp>
#include <bindings/dom/binding.hpp>
#include <bindings/env/binding.hpp>
#include <bindings/messaging/binding.hpp>
#include <bindings/renderer/binding.hpp>

/**
 * @def TR_NAPI_MODULE_MAP
 * Defines a macro to map N-API modules to their respective bindings.
 * This macro is used to generate code for initializing and registering N-API modules.
 */
#define TR_NAPI_MODULE_MAP(MODULE) \
  MODULE(browser)                  \
  MODULE(cssom)                    \
  MODULE(dom)                      \
  MODULE(env)                      \
  MODULE(messaging)                \
  MODULE(renderer)
