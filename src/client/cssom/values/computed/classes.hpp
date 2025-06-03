#pragma once

#include "./align.hpp"
#include "./angle.hpp"
#include "./animation.hpp"
#include "./border.hpp"
#include "./box.hpp"
#include "./common.hpp"
#include "./color.hpp"
#include "./easing.hpp"
#include "./flex.hpp"
#include "./font.hpp"
#include "./length.hpp"
#include "./length_percentage.hpp"
#include "./percentage.hpp"
#include "./position.hpp"
#include "./text.hpp"
#include "./time.hpp"
#include "./transform.hpp"

// Forward declarations that not included to avoid circular dependencies
namespace client_cssom::values::computed
{
  class Context;
}
