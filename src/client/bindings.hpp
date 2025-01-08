#pragma once

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

#define TR_NAPI_MODULE_MAP(XX) \
  XX(browser)                  \
  XX(canvas)                   \
  XX(cssom)                    \
  XX(dom)                      \
  XX(env)                      \
  XX(messaging)                \
  XX(math3d)                   \
  XX(renderer)                 \
  XX(webgl)                    \
  XX(webxr)
