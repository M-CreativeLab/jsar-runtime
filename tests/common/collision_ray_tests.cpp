#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <common/collision/ray.hpp>
#include <common/collision/culling/bounding_box.hpp>
#include <common/collision/culling/bounding_sphere.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

using namespace collision;
using namespace collision::culling;

TEST_CASE("TrRay intersectsBoxMinMax method", "[TrRay]") {
  glm::vec3 origin(0.0f, 0.0f, 0.0f);
  glm::vec3 direction(1.0f, 1.0f, 1.0f);
  TrRay ray(origin, direction);

  glm::vec3 min(1.0f, 1.0f, 1.0f);
  glm::vec3 max(2.0f, 2.0f, 2.0f);

  REQUIRE(ray.intersectsBoxMinMax(min, max));
}

TEST_CASE("TrRay intersectsBox method", "[TrRay]") {
  glm::vec3 origin(0.0f, 0.0f, 0.0f);
  glm::vec3 direction(1.0f, 1.0f, 1.0f);
  TrRay ray(origin, direction);

  glm::vec3 min(1.0f, 1.0f, 1.0f);
  glm::vec3 max(2.0f, 2.0f, 2.0f);
  glm::mat4 worldMatrix = glm::mat4(1.0f);
  TrBoundingBox box(min, max, worldMatrix);

  REQUIRE(ray.intersectsBox(box));
}

TEST_CASE("TrRay intersectsSphere method", "[TrRay]") {
  glm::vec3 origin(0.0f, 0.0f, 0.0f);
  glm::vec3 direction(1.0f, 1.0f, 1.0f);
  TrRay ray(origin, direction);

  glm::vec3 min(1.0f, 1.0f, 1.0f);
  glm::vec3 max(2.0f, 2.0f, 2.0f);
  glm::mat4 worldMatrix = glm::mat4(1.0f);
  TrBoundingSphere sphere(min, max, worldMatrix);

  REQUIRE(ray.intersectsSphere(sphere, 0.0f));
}

TEST_CASE("TrRay update method", "[TrRay]") {
  glm::vec3 origin(0.0f, 0.0f, 0.0f);
  glm::vec3 direction(1.0f, 1.0f, 1.0f);
  TrRay ray;
  ray.update(origin, direction, 2.0f);

  REQUIRE(ray.origin == origin);
  REQUIRE(ray.direction == direction);
  REQUIRE(ray.length == 2.0f);
}
