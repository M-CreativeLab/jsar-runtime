#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <common/collision/culling/bounding_box.hpp>
#include <common/math3d/plane.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

using namespace collision::culling;
using namespace math3d;

TEST_CASE("TrBoundingBox reConstruct method", "[TrBoundingBox]") {
  glm::vec3 min(0.0f, 0.0f, 0.0f);
  glm::vec3 max(1.0f, 1.0f, 1.0f);
  glm::mat4 worldMatrix = glm::mat4(1.0f);

  TrBoundingBox boundingBox(min, max, worldMatrix);
  boundingBox.reConstruct(min, max, worldMatrix);

  REQUIRE(boundingBox.minimum == min);
  REQUIRE(boundingBox.maximum == max);
  REQUIRE(boundingBox.center == glm::vec3(0.5f, 0.5f, 0.5f));
  REQUIRE(boundingBox.extendSize == glm::vec3(0.5f, 0.5f, 0.5f));
}

TEST_CASE("TrBoundingBox scale method", "[TrBoundingBox]") {
  glm::vec3 min(0.0f, 0.0f, 0.0f);
  glm::vec3 max(1.0f, 1.0f, 1.0f);
  glm::mat4 worldMatrix = glm::mat4(1.0f);

  TrBoundingBox boundingBox(min, max, worldMatrix);
  boundingBox.scale(2.0f);

  REQUIRE(boundingBox.minimum == glm::vec3(-0.5f, -0.5f, -0.5f));
  REQUIRE(boundingBox.maximum == glm::vec3(1.5f, 1.5f, 1.5f));
  REQUIRE(boundingBox.center == glm::vec3(0.5f, 0.5f, 0.5f));
  REQUIRE(boundingBox.extendSize == glm::vec3(1.0f, 1.0f, 1.0f));
}

TEST_CASE("TrBoundingBox isInFrustum method", "[TrBoundingBox]") {
  glm::vec3 min(0.0f, 0.0f, 0.0f);
  glm::vec3 max(1.0f, 1.0f, 1.0f);
  glm::mat4 worldMatrix = glm::mat4(1.0f);

  TrBoundingBox boundingBox(min, max, worldMatrix);

  std::array<TrPlane, 6> frustumPlanes = {
    TrPlane(glm::vec3(1.0f, 0.0f, 0.0f), 1.0f),
    TrPlane(glm::vec3(-1.0f, 0.0f, 0.0f), 1.0f),
    TrPlane(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f),
    TrPlane(glm::vec3(0.0f, -1.0f, 0.0f), 1.0f),
    TrPlane(glm::vec3(0.0f, 0.0f, 1.0f), 1.0f),
    TrPlane(glm::vec3(0.0f, 0.0f, -1.0f), 1.0f)
  };

  REQUIRE(boundingBox.isInFrustum(frustumPlanes));
}

TEST_CASE("TrBoundingBox isCompletelyInFrustum method", "[TrBoundingBox]") {
  glm::vec3 min(0.0f, 0.0f, 0.0f);
  glm::vec3 max(1.0f, 1.0f, 1.0f);
  glm::mat4 worldMatrix = glm::mat4(1.0f);

  TrBoundingBox boundingBox(min, max, worldMatrix);

  std::array<TrPlane, 6> frustumPlanes = {
    TrPlane(glm::vec3(1.0f, 0.0f, 0.0f), 1.0f),
    TrPlane(glm::vec3(-1.0f, 0.0f, 0.0f), 1.0f),
    TrPlane(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f),
    TrPlane(glm::vec3(0.0f, -1.0f, 0.0f), 1.0f),
    TrPlane(glm::vec3(0.0f, 0.0f, 1.0f), 1.0f),
    TrPlane(glm::vec3(0.0f, 0.0f, -1.0f), 1.0f)
  };

  REQUIRE(boundingBox.isCompletelyInFrustum(frustumPlanes));
}

TEST_CASE("TrBoundingBox isInFrustum method failure case", "[TrBoundingBox]") {
  glm::vec3 min(0.0f, 0.0f, 0.0f);
  glm::vec3 max(1.0f, 1.0f, 1.0f);
  glm::mat4 worldMatrix = glm::mat4(1.0f);

  TrBoundingBox boundingBox(min, max, worldMatrix);

  std::array<TrPlane, 6> frustumPlanes = {
    TrPlane(glm::vec3(1.0f, 0.0f, 0.0f), -2.0f),
    TrPlane(glm::vec3(-1.0f, 0.0f, 0.0f), -2.0f),
    TrPlane(glm::vec3(0.0f, 1.0f, 0.0f), -2.0f),
    TrPlane(glm::vec3(0.0f, -1.0f, 0.0f), -2.0f),
    TrPlane(glm::vec3(0.0f, 0.0f, 1.0f), -2.0f),
    TrPlane(glm::vec3(0.0f, 0.0f, -1.0f), -2.0f)
  };

  REQUIRE_FALSE(boundingBox.isInFrustum(frustumPlanes));
}

TEST_CASE("TrBoundingBox isCompletelyInFrustum method failure case", "[TrBoundingBox]") {
  glm::vec3 min(0.0f, 0.0f, 0.0f);
  glm::vec3 max(1.0f, 1.0f, 1.0f);
  glm::mat4 worldMatrix = glm::mat4(1.0f);

  TrBoundingBox boundingBox(min, max, worldMatrix);

  std::array<TrPlane, 6> frustumPlanes = {
    TrPlane(glm::vec3(1.0f, 0.0f, 0.0f), -2.0f),
    TrPlane(glm::vec3(-1.0f, 0.0f, 0.0f), -2.0f),
    TrPlane(glm::vec3(0.0f, 1.0f, 0.0f), -2.0f),
    TrPlane(glm::vec3(0.0f, -1.0f, 0.0f), -2.0f),
    TrPlane(glm::vec3(0.0f, 0.0f, 1.0f), -2.0f),
    TrPlane(glm::vec3(0.0f, 0.0f, -1.0f), -2.0f)
  };

  REQUIRE_FALSE(boundingBox.isCompletelyInFrustum(frustumPlanes));
}
