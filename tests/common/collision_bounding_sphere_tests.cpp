#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <common/collision/culling/bounding_sphere.hpp>
#include <common/math3d/plane.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

using namespace collision::culling;
using namespace math3d;
using namespace Catch;

TEST_CASE("TrBoundingSphere reConstruct method", "[TrBoundingSphere]") {
  glm::vec3 min(0.0f, 0.0f, 0.0f);
  glm::vec3 max(1.0f, 1.0f, 1.0f);
  glm::mat4 worldMatrix = glm::mat4(1.0f);

  TrBoundingSphere boundingSphere(min, max, worldMatrix);
  boundingSphere.reConstruct(min, max, worldMatrix);

  REQUIRE(boundingSphere.minimum == min);
  REQUIRE(boundingSphere.maximum == max);
  REQUIRE(boundingSphere.center == glm::vec3(0.5f, 0.5f, 0.5f));
  REQUIRE(boundingSphere.radius == Approx(0.866f).margin(0.001f));
}

TEST_CASE("TrBoundingSphere scale method", "[TrBoundingSphere]") {
  glm::vec3 min(0.0f, 0.0f, 0.0f);
  glm::vec3 max(1.0f, 1.0f, 1.0f);
  glm::mat4 worldMatrix = glm::mat4(1.0f);

  TrBoundingSphere boundingSphere(min, max, worldMatrix);
  boundingSphere.scale(2.0f);

  // REQUIRE(boundingSphere.minimum == glm::vec3(0.0f, 0.0f, 0.0f));
  // REQUIRE(boundingSphere.maximum == glm::vec3(2.0f, 2.0f, 2.0f));
  // REQUIRE(boundingSphere.center == glm::vec3(1.0f, 1.0f, 1.0f));
  // REQUIRE(boundingSphere.radius == Approx(1.732f).margin(0.001f));
}

TEST_CASE("TrBoundingSphere isInFrustum method", "[TrBoundingSphere]") {
  glm::vec3 min(0.0f, 0.0f, 0.0f);
  glm::vec3 max(1.0f, 1.0f, 1.0f);
  glm::mat4 worldMatrix = glm::mat4(1.0f);

  TrBoundingSphere boundingSphere(min, max, worldMatrix);

  std::array<TrPlane, 6> frustumPlanes = {
    TrPlane(glm::vec3(1.0f, 0.0f, 0.0f), 1.0f),
    TrPlane(glm::vec3(-1.0f, 0.0f, 0.0f), 1.0f),
    TrPlane(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f),
    TrPlane(glm::vec3(0.0f, -1.0f, 0.0f), 1.0f),
    TrPlane(glm::vec3(0.0f, 0.0f, 1.0f), 1.0f),
    TrPlane(glm::vec3(0.0f, 0.0f, -1.0f), 1.0f)
  };

  REQUIRE(boundingSphere.isInFrustum(frustumPlanes));
}

TEST_CASE("TrBoundingSphere isCompletelyInFrustum method", "[TrBoundingSphere]") {
  glm::vec3 min(0.0f, 0.0f, 0.0f);
  glm::vec3 max(1.0f, 1.0f, 1.0f);
  glm::mat4 worldMatrix = glm::mat4(1.0f);

  TrBoundingSphere boundingSphere(min, max, worldMatrix);

  std::array<TrPlane, 6> frustumPlanes = {
    TrPlane(glm::vec3(1.0f, 0.0f, 0.0f), 1.0f),
    TrPlane(glm::vec3(-1.0f, 0.0f, 0.0f), 1.0f),
    TrPlane(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f),
    TrPlane(glm::vec3(0.0f, -1.0f, 0.0f), 1.0f),
    TrPlane(glm::vec3(0.0f, 0.0f, 1.0f), 1.0f),
    TrPlane(glm::vec3(0.0f, 0.0f, -1.0f), 1.0f)
  };

  REQUIRE(boundingSphere.isCompletelyInFrustum(frustumPlanes));
}

TEST_CASE("TrBoundingSphere isInFrustum method failure case", "[TrBoundingSphere]") {
  glm::vec3 min(0.0f, 0.0f, 0.0f);
  glm::vec3 max(1.0f, 1.0f, 1.0f);
  glm::mat4 worldMatrix = glm::mat4(1.0f);

  TrBoundingSphere boundingSphere(min, max, worldMatrix);

  std::array<TrPlane, 6> frustumPlanes = {
    TrPlane(glm::vec3(1.0f, 0.0f, 0.0f), -2.0f),
    TrPlane(glm::vec3(-1.0f, 0.0f, 0.0f), -2.0f),
    TrPlane(glm::vec3(0.0f, 1.0f, 0.0f), -2.0f),
    TrPlane(glm::vec3(0.0f, -1.0f, 0.0f), -2.0f),
    TrPlane(glm::vec3(0.0f, 0.0f, 1.0f), -2.0f),
    TrPlane(glm::vec3(0.0f, 0.0f, -1.0f), -2.0f)
  };

  REQUIRE_FALSE(boundingSphere.isInFrustum(frustumPlanes));
}

TEST_CASE("TrBoundingSphere isCompletelyInFrustum method failure case", "[TrBoundingSphere]") {
  glm::vec3 min(0.0f, 0.0f, 0.0f);
  glm::vec3 max(1.0f, 1.0f, 1.0f);
  glm::mat4 worldMatrix = glm::mat4(1.0f);

  TrBoundingSphere boundingSphere(min, max, worldMatrix);

  std::array<TrPlane, 6> frustumPlanes = {
    TrPlane(glm::vec3(1.0f, 0.0f, 0.0f), -2.0f),
    TrPlane(glm::vec3(-1.0f, 0.0f, 0.0f), -2.0f),
    TrPlane(glm::vec3(0.0f, 1.0f, 0.0f), -2.0f),
    TrPlane(glm::vec3(0.0f, -1.0f, 0.0f), -2.0f),
    TrPlane(glm::vec3(0.0f, 0.0f, 1.0f), -2.0f),
    TrPlane(glm::vec3(0.0f, 0.0f, -1.0f), -2.0f)
  };

  REQUIRE_FALSE(boundingSphere.isCompletelyInFrustum(frustumPlanes));
}
