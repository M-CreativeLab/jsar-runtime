#pragma once

namespace client_layout
{
  /**
   * @brief HitTestRay is used to perform hit testing in the layout view.
   *
   * The ray is defined in the local space of the layout view, which is a right-handed coordinate system with the
   * following axes:
   *
   * - X-axis: Right (+X)
   * - Y-axis: Up (-Y)
   * - Z-axis: Depth (+Z)
   */
  class HitTestRay
  {
  public:
    HitTestRay();

  private:
    // TODO
  };
}
