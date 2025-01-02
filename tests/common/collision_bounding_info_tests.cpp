#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <common/collision/culling/bounding_info.hpp>
#include <common/math3d/plane.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

using namespace collision::culling;
using namespace math3d;

TEST_CASE("TrBoundingInfo reConstruct method", "[TrBoundingInfo]") {
  glm::vec3 min(0.0f, 0.0f, 0.0f);
  glm::vec3 max(1.0f, 1.0f, 1.0f);
  glm::mat4 worldMatrix = glm::mat4(1.0f);

  TrBoundingInfo boundingInfo(min, max, worldMatrix);
  boundingInfo.reConstruct(min, max, worldMatrix);

  REQUIRE(boundingInfo.boundingBox.minimum == min);
  REQUIRE(boundingInfo.boundingBox.maximum == max);
  REQUIRE(boundingInfo.boundingSphere.minimum == min);
  REQUIRE(boundingInfo.boundingSphere.maximum == max);
}

TEST_CASE("TrBoundingInfo update method", "[TrBoundingInfo]") {
  glm::vec3 min(0.0f, 0.0f, 0.0f);
  glm::vec3 max(1.0f, 1.0f, 1.0f);
  glm::mat4 worldMatrix = glm::mat4(1.0f);

  TrBoundingInfo boundingInfo(min, max, worldMatrix);
  glm::mat4 newWorldMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
  boundingInfo.update(newWorldMatrix);

  REQUIRE(boundingInfo.boundingBox.centerWorld == glm::vec3(1.5f, 1.5f, 1.5f));
  REQUIRE(boundingInfo.boundingSphere.centerWorld == glm::vec3(1.5f, 1.5f, 1.5f));
}

TEST_CASE("TrBoundingInfo isInFrustum method", "[TrBoundingInfo]") {
  glm::vec3 min(0.0f, 0.0f, 0.0f);
  glm::vec3 max(1.0f, 1.0f, 1.0f);
  glm::mat4 worldMatrix = glm::mat4(1.0f);

  TrBoundingInfo boundingInfo(min, max, worldMatrix);

  std::array<TrPlane, 6> frustumPlanes = {
    TrPlane(glm::vec3(1.0f, 0.0f, 0.0f), 1.0f),
    TrPlane(glm::vec3(-1.0f, 0.0f, 0.0f), 1.0f),
    TrPlane(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f),
    TrPlane(glm::vec3(0.0f, -1.0f, 0.0f), 1.0f),
    TrPlane(glm::vec3(0.0f, 0.0f, 1.0f), 1.0f),
    TrPlane(glm::vec3(0.0f, 0.0f, -1.0f), 1.0f)
  };

  REQUIRE(boundingInfo.isInFrustum(frustumPlanes));
}

TEST_CASE("TrBoundingInfo isCompletelyInFrustum method", "[TrBoundingInfo]") {
  glm::vec3 min(0.0f, 0.0f, 0.0f);
  glm::vec3 max(1.0f, 1.0f, 1.0f);
  glm::mat4 worldMatrix = glm::mat4(1.0f);

  TrBoundingInfo boundingInfo(min, max, worldMatrix);

  std::array<TrPlane, 6> frustumPlanes = {
    TrPlane(glm::vec3(1.0f, 0.0f, 0.0f), 1.0f),
    TrPlane(glm::vec3(-1.0f, 0.0f, 0.0f), 1.0f),
    TrPlane(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f),
    TrPlane(glm::vec3(0.0f, -1.0f, 0.0f), 1.0f),
    TrPlane(glm::vec3(0.0f, 0.0f, 1.0f), 1.0f),
    TrPlane(glm::vec3(0.0f, 0.0f, -1.0f), 1.0f)
  };

  REQUIRE(boundingInfo.isCompletelyInFrustum(frustumPlanes));
}

TEST_CASE("TrBoundingInfo isInFrustum method failure case", "[TrBoundingInfo]") {
  glm::vec3 min(0.0f, 0.0f, 0.0f);
  glm::vec3 max(1.0f, 1.0f, 1.0f);
  glm::mat4 worldMatrix = glm::mat4(1.0f);

  TrBoundingInfo boundingInfo(min, max, worldMatrix);

  std::array<TrPlane, 6> frustumPlanes = {
    TrPlane(glm::vec3(1.0f, 0.0f, 0.0f), -2.0f),
    TrPlane(glm::vec3(-1.0f, 0.0f, 0.0f), -2.0f),
    TrPlane(glm::vec3(0.0f, 1.0f, 0.0f), -2.0f),
    TrPlane(glm::vec3(0.0f, -1.0f, 0.0f), -2.0f),
    TrPlane(glm::vec3(0.0f, 0.0f, 1.0f), -2.0f),
    TrPlane(glm::vec3(0.0f, 0.0f, -1.0f), -2.0f)
  };

  REQUIRE_FALSE(boundingInfo.isInFrustum(frustumPlanes));
}

TEST_CASE("TrBoundingInfo isCompletelyInFrustum method failure case", "[TrBoundingInfo]") {
  glm::vec3 min(0.0f, 0.0f, 0.0f);
  glm::vec3 max(1.0f, 1.0f, 1.0f);
  glm::mat4 worldMatrix = glm::mat4(1.0f);

  TrBoundingInfo boundingInfo(min, max, worldMatrix);

  std::array<TrPlane, 6> frustumPlanes = {
    TrPlane(glm::vec3(1.0f, 0.0f, 0.0f), -2.0f),
    TrPlane(glm::vec3(-1.0f, 0.0f, 0.0f), -2.0f),
    TrPlane(glm::vec3(0.0f, 1.0f, 0.0f), -2.0f),
    TrPlane(glm::vec3(0.0f, -1.0f, 0.0f), -2.0f),
    TrPlane(glm::vec3(0.0f, 0.0f, 1.0f), -2.0f),
    TrPlane(glm::vec3(0.0f, 0.0f, -1.0f), -2.0f)
  };

  REQUIRE_FALSE(boundingInfo.isCompletelyInFrustum(frustumPlanes));
}
