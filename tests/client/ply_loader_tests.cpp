#include "../catch2/catch.hpp"
#include <client/builtin_scene/meshes/loaders/ply_loader.hpp>
#include <cstring>

using namespace builtin_scene::model_loaders;

TEST_CASE("PLY Loader", "[ply_loader]")
{
  SECTION("Empty PLY data handling")
  {
    std::vector<char> emptyData;
    std::vector<builtin_scene::GaussianSplat> splats;

    bool result = PlyLoader::load(emptyData, splats);
    REQUIRE_FALSE(result);
    REQUIRE(splats.empty());
  }

  SECTION("Invalid PLY header handling")
  {
    std::string invalidHeader = "not_ply\nformat binary_little_endian 1.0\nend_header\n";
    std::vector<char> invalidData(invalidHeader.begin(), invalidHeader.end());
    std::vector<builtin_scene::GaussianSplat> splats;

    bool result = PlyLoader::load(invalidData, splats);
    REQUIRE_FALSE(result);
    REQUIRE(splats.empty());
  }

  SECTION("Minimal valid PLY header parsing")
  {
    std::string validHeader = 
      "ply\n"
      "format binary_little_endian 1.0\n"
      "element vertex 0\n"
      "property float x\n"
      "property float y\n"
      "property float z\n"
      "end_header\n";
    
    std::vector<char> validData(validHeader.begin(), validHeader.end());
    std::vector<builtin_scene::GaussianSplat> splats;

    bool result = PlyLoader::load(validData, splats);
    REQUIRE(result);
    REQUIRE(splats.empty()); // No vertex data, so no splats
  }

  SECTION("Callback-based loading with empty data")
  {
    std::vector<char> emptyData;
    int numSplatsCallback = -1;
    int splatCallbackCount = 0;

    bool result = PlyLoader::decodePly(emptyData,
      [&numSplatsCallback](int numSplats) {
        numSplatsCallback = numSplats;
      },
      [&splatCallbackCount](int index, float x, float y, float z, float scaleX, float scaleY, float scaleZ,
                           float quatX, float quatY, float quatZ, float quatW, float opacity, float r, float g, float b) {
        splatCallbackCount++;
      });

    REQUIRE_FALSE(result);
    REQUIRE(numSplatsCallback == -1); // Callback should not have been called
    REQUIRE(splatCallbackCount == 0);
  }
}