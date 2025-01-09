#pragma once

// Include bindings for various modules
#include <bindings/browser/binding.hpp>
#include <bindings/canvas/binding.hpp>
#include <bindings/cssom/binding.hpp>
#include <bindings/dom/binding.hpp>
#include <bindings/env/binding.hpp>
#include <bindings/math3d/binding.hpp>
#include <bindings/messaging/binding.hpp>
#include <bindings/renderer/binding.hpp>
#include <bindings/webgl/binding.hpp>
#include <bindings/webxr/binding.hpp>

/**
 * @def TR_NAPI_MODULE_MAP
 * Defines a macro to map N-API modules to their respective bindings.
 * This macro is used to generate code for initializing and registering N-API modules.
 *
 * Usage:
 *   TR_NAPI_MODULE_MAP(MODULE) \
 *     MODULE(browser)           \
 *     MODULE(canvas)            \
 *     MODULE(cssom)             \
 *     MODULE(dom)               \
 *     MODULE(env)               \
 *     MODULE(messaging)         \
 *     MODULE(math3d)            \
 *     MODULE(renderer)          \
 *     MODULE(webgl)             \
 *     MODULE(webxr)
 */
#define TR_NAPI_MODULE_MAP(MODULE) \
  MODULE(browser)                  \
  MODULE(canvas)                   \
  MODULE(cssom)                    \
  MODULE(dom)                      \
  MODULE(env)                      \
  MODULE(messaging)                \
  MODULE(math3d)                   \
  MODULE(renderer)                 \
  MODULE(webgl)                    \
  MODULE(webxr)
