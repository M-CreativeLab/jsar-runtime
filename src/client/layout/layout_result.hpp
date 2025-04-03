#pragma once

#include <stdint.h>

#include "./fragment.hpp"

namespace client_layout
{
  /**
   * The `LayoutResult` stores the resulting data from layout computation. It includes the geometry information which
   * is used to:
   *
   * - Set the transformation matrix for element rendering.
   * - Hit testings.
   */
  class LayoutResult
  {
    friend class TaffyBasedFormattingContext;
    friend class InlineFormattingContext;

  public:
    enum EStatus : uint8_t
    {
      kSuccess = 0,
      kRelayoutRequired,
    };

  public:
    LayoutResult() = delete;
    LayoutResult(const dom::geometry::DOMRect &rect);
    ~LayoutResult() = default;

  public:
    inline EStatus status() const { return status_; }
    inline const Fragment &fragment() const { return fragment_; }

    bool needsResize(const Fragment &) const;
    bool needsRelayout(const Fragment &) const;

  private:
    inline EStatus &status() { return status_; }
    inline Fragment &fragment() { return fragment_; }

  private:
    EStatus status_ = kSuccess;
    Fragment fragment_;
  };
}
