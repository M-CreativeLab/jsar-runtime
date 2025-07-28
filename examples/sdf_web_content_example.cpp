/**
 * Example: Using SDF-based Web Content Rendering
 * 
 * This example demonstrates how to configure and use the new SDF-based
 * rendering for Web Content planes to achieve superior anti-aliasing.
 */

#include <client/builtin_scene/web_content.hpp>
#include <client/builtin_scene/materials/web_content_instanced.hpp>
#include <memory>

namespace sdf_example
{
  using namespace builtin_scene;
  using namespace builtin_scene::materials;

  /**
   * Configure a web content material for optimal SDF rendering
   */
  void configureSdfMaterial(std::shared_ptr<WebContentInstancedMaterial> material)
  {
    // Enable SDF-based anti-aliasing
    material->setSdfEnabled(true);
    
    // Configure anti-alias width based on use case:
    // - 1.0f: Sharp edges, minimal blur
    // - 2.0f: Balanced quality (default)
    // - 4.0f: Soft edges, more blur
    material->setSdfAntiAliasWidth(2.0f);
  }

  /**
   * Create a web content element with rounded borders
   * This will benefit most from SDF rendering
   */
  std::shared_ptr<WebContent> createRoundedWebContent()
  {
    auto webContent = std::make_shared<WebContent>("rounded-example", 200.0f, 150.0f);
    
    // The border radius will be automatically extracted and passed to the SDF shader
    // when the content is rendered through the material system
    
    return webContent;
  }

  /**
   * Performance considerations for SDF rendering
   */
  void performanceOptimization(std::shared_ptr<WebContentInstancedMaterial> material)
  {
    // For performance-critical scenarios where edge quality is not important,
    // you can disable SDF rendering to save fragment shader cycles
    material->setSdfEnabled(false);
    
    // For high-quality rendering where visual fidelity is paramount,
    // enable SDF with wider anti-aliasing
    material->setSdfEnabled(true);
    material->setSdfAntiAliasWidth(3.0f);
  }

  /**
   * Quality comparison scenarios
   */
  void demonstrateQualityImprovements()
  {
    auto material = std::make_shared<WebContentInstancedMaterial>();
    
    // Scenario 1: Traditional rendering (for comparison)
    material->setSdfEnabled(false);
    // Result: Standard texture filtering, potential aliasing on edges
    
    // Scenario 2: SDF rendering enabled
    material->setSdfEnabled(true);
    material->setSdfAntiAliasWidth(2.0f);
    // Result: Mathematically smooth edges, superior anti-aliasing
    
    // The difference is most noticeable on:
    // - Rounded corners and borders
    // - Scaled or rotated content
    // - High DPI displays
    // - Zoomed interfaces
  }

} // namespace sdf_example