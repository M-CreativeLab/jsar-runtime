#pragma once

#include <glm/glm.hpp>

namespace math
{
  /**
   * The forward direction.
   */
  class Dir3 : public glm::vec3
  {
  public:
    /**
     * @returns The forward direction.
     */
    static Dir3 Up() { return Dir3(0.0f, 1.0f, 0.0f); }
    /**
     * @returns The forward direction.
     */
    static Dir3 Right() { return Dir3(1.0f, 0.0f, 0.0f); }
    /**
     * @returns The +Y direction.
     */
    static inline Dir3 Y() { return Up(); }
    /**
     * @returns The +X direction.
     */
    static inline Dir3 X() { return Right(); }

  public:
    using glm::vec3::vec3;
  };
}
