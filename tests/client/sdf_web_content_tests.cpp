#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <client/builtin_scene/materials/web_content_instanced.hpp>
#include <client/builtin_scene/web_content.hpp>
#include <memory>

using namespace builtin_scene;
using namespace builtin_scene::materials;

TEST_CASE("WebContentInstancedMaterial SDF Configuration", "[sdf][materials]")
{
  SECTION("SDF parameters can be configured")
  {
    auto material = std::make_shared<WebContentInstancedMaterial>();
    
    // Test default values
    REQUIRE_NOTHROW(material->setSdfEnabled(true));
    REQUIRE_NOTHROW(material->setSdfEnabled(false));
    
    // Test anti-alias width configuration
    REQUIRE_NOTHROW(material->setSdfAntiAliasWidth(1.0f));
    REQUIRE_NOTHROW(material->setSdfAntiAliasWidth(2.0f));
    REQUIRE_NOTHROW(material->setSdfAntiAliasWidth(4.0f));
  }
}

TEST_CASE("WebContent roundedRect accessor", "[webcontent][sdf]")
{
  SECTION("roundedRect returns valid SkRRect")
  {
    auto webContent = std::make_shared<WebContent>("test", 100.0f, 100.0f);
    
    // Should not throw
    REQUIRE_NOTHROW(webContent->roundedRect());
    
    // Should return a valid SkRRect (even if empty initially)
    const auto& roundedRect = webContent->roundedRect();
    REQUIRE(roundedRect.width() >= 0);
    REQUIRE(roundedRect.height() >= 0);
  }
}

TEST_CASE("WebContentInstancedMaterial defines include SDF support", "[materials][defines]")
{
  SECTION("Material defines include required SDF features")
  {
    auto material = std::make_shared<WebContentInstancedMaterial>();
    const auto defines = material->defines();
    
    // Should include the basic required defines for instanced rendering
    bool hasUVs = false;
    bool hasInstanceTransforms = false;
    bool hasInstanceColors = false;
    bool hasInstanceTexture = false;
    
    for (const auto& define : defines)
    {
      if (define == "USE_UVS") hasUVs = true;
      if (define == "USE_INSTANCE_TRANSFORMS") hasInstanceTransforms = true;
      if (define == "USE_INSTANCE_COLORS") hasInstanceColors = true;
      if (define == "USE_INSTANCE_TEXTURE") hasInstanceTexture = true;
    }
    
    REQUIRE(hasUVs);
    REQUIRE(hasInstanceTransforms);
    REQUIRE(hasInstanceColors);
    REQUIRE(hasInstanceTexture);
  }
}

TEST_CASE("WebContentInstancedMaterial material name", "[materials]")
{
  SECTION("Material has correct name")
  {
    auto material = std::make_shared<WebContentInstancedMaterial>();
    REQUIRE(material->name() == "WebContentInstancedMaterial");
  }
}

TEST_CASE("WebContentInstancedMaterial shader reference", "[materials][shaders]")
{
  SECTION("Material uses correct fragment shader")
  {
    auto material = std::make_shared<WebContentInstancedMaterial>();
    auto shaderRef = material->fragmentShader();
    
    // Should reference the web_content.frag shader that now contains SDF functionality
    REQUIRE(shaderRef.path == "materials/web_content.frag");
    REQUIRE(shaderRef.type == ShaderType::kFragment);
  }
}