#define CATCH_CONFIG_MAIN
#include "./catch2/catch_amalgamated.hpp"
#include <math/matrix.hpp>
#include <math/quat.hpp>
#include <math/vectors.hpp>

using namespace math;
using namespace Catch;

TEST_CASE("Matrix Creation", "[matrix]")
{
  glm::mat4 identity = matrix::Identity;
  glm::mat4 origin = matrix::Origin;

  REQUIRE(identity == glm::identity<glm::mat4>());
  REQUIRE(origin == glm::identity<glm::mat4>());

  float translation[] = {1.0f, 2.0f, 3.0f};
  float rotation[] = {0.0f, 0.0f, 0.0f, 1.0f};
  float scale[] = {1.0f, 1.0f, 1.0f};
  glm::mat4 trsMatrix = CreateMatrixFromTRS(translation, rotation, scale);

  REQUIRE(trsMatrix[3][0] == 1.0f);
  REQUIRE(trsMatrix[3][1] == 2.0f);
  REQUIRE(trsMatrix[3][2] == 3.0f);
}

TEST_CASE("Quaternion Operations", "[quat]")
{
  Quat identity = Quat::Identity();
  Quat zero = Quat::Zero();
  Quat nan = Quat::Nan();

  REQUIRE(identity == glm::quat(1, 0, 0, 0));
  REQUIRE(zero == glm::quat(0, 0, 0, 0));

  Quat quat = Quat::FromXYZW(1.0f, 0.0f, 0.0f, 0.0f);
  REQUIRE(quat.x == 1.0f);
  REQUIRE(quat.y == 0.0f);
  REQUIRE(quat.z == 0.0f);
  REQUIRE(quat.w == 0.0f);

  Vec3 axis(0.0f, 1.0f, 0.0f);
  float angle = glm::radians(90.0f);
  Quat rotationQuat = Quat::FromAxisAngle(axis, angle);
  REQUIRE(rotationQuat.x == Approx(0.0f).margin(0.0001f));
  REQUIRE(rotationQuat.y == Approx(0.7071f).margin(0.0001f));
  REQUIRE(rotationQuat.z == Approx(0.0f).margin(0.0001f));
  REQUIRE(rotationQuat.w == Approx(0.7071f).margin(0.0001f));
}

TEST_CASE("Vector Operations", "[vectors]")
{
  Vec3 identity = Vec3::Identity();
  Vec3 one = Vec3::One();

  REQUIRE(identity.x == 0.0f);
  REQUIRE(identity.y == 0.0f);
  REQUIRE(identity.z == 0.0f);

  REQUIRE(one.x == 1.0f);
  REQUIRE(one.y == 1.0f);
  REQUIRE(one.z == 1.0f);

  Vec3 vec(1.0f, 2.0f, 3.0f);
  REQUIRE(vec.length() == Approx(3.7417f).margin(0.0001f));

  Vec3 normalizedVec = vec.normalize();
  REQUIRE(normalizedVec.x == Approx(0.2673f).margin(0.0001f));
  REQUIRE(normalizedVec.y == Approx(0.5345f).margin(0.0001f));
  REQUIRE(normalizedVec.z == Approx(0.8018f).margin(0.0001f));

  Vec3 orthogonalVec = vec.anyOrthogonalVector();
  REQUIRE(orthogonalVec.dot(vec) == Approx(0.0f).margin(0.0001f));

  Vec3 orthonormalVec = normalizedVec.anyOrthonormalVector();
  REQUIRE(orthonormalVec.dot(normalizedVec) == Approx(0.0f).margin(0.0001f));
  REQUIRE(orthonormalVec.length() == Approx(1.0f).margin(0.0001f));

  REQUIRE(vec.isNormalized() == false);
  REQUIRE(normalizedVec.isNormalized() == true);

  Vec3 vec2(4.0f, 5.0f, 6.0f);
  REQUIRE(vec.dot(vec2) == Approx(32.0f).margin(0.0001f));

  Vec3 crossVec = vec.cross(vec2);
  REQUIRE(crossVec.x == Approx(-3.0f).margin(0.0001f));
  REQUIRE(crossVec.y == Approx(6.0f).margin(0.0001f));
  REQUIRE(crossVec.z == Approx(-3.0f).margin(0.0001f));
}
