#include "./gaussian_splatting_renderer.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <cstring>

namespace renderer
{
  TrGaussianSplattingRenderer::TrGaussianSplattingRenderer()
      : needsSorting_(false)
      , shaderProgram_(0)
      , vertexBuffer_(0)
      , indexBuffer_(0)
  {
  }

  TrGaussianSplattingRenderer::~TrGaussianSplattingRenderer()
  {
    shutdownGL();
  }

  bool TrGaussianSplattingRenderer::loadModel(const std::string &filepath)
  {
    // Determine file type from extension
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos)
    {
      std::string ext = filepath.substr(dotPos);
      if (ext == ".gsplat")
      {
        return loadGSplatFile(filepath);
      }
      else if (ext == ".ply")
      {
        return loadPlyFile(filepath);
      }
    }
    
    std::cerr << "Unsupported 3DGS file format: " << filepath << std::endl;
    return false;
  }

  void TrGaussianSplattingRenderer::addSplats(const std::vector<GaussianSplat> &splats)
  {
    std::lock_guard<std::mutex> lock(splatsMutex_);
    splats_.insert(splats_.end(), splats.begin(), splats.end());
    needsSorting_ = true;
  }

  void TrGaussianSplattingRenderer::clearSplats()
  {
    std::lock_guard<std::mutex> lock(splatsMutex_);
    splats_.clear();
    needsSorting_ = false;
  }

  size_t TrGaussianSplattingRenderer::getSplatCount() const
  {
    std::lock_guard<std::mutex> lock(splatsMutex_);
    return splats_.size();
  }

  void TrGaussianSplattingRenderer::sortSplats(const float viewMatrix[16])
  {
    if (!needsSorting_)
      return;

    std::lock_guard<std::mutex> lock(splatsMutex_);
    
    // Compute depths for all splats
    computeDepths(viewMatrix);
    
    // Sort by depth (back to front for correct alpha blending)
    sortByDepth();
    
    needsSorting_ = false;
  }

  void TrGaussianSplattingRenderer::render(const float projectionMatrix[16], const float viewMatrix[16])
  {
    // TODO: Implement WebGL2 rendering
    // This is a placeholder that should be implemented with:
    // 1. WebGL2 context setup
    // 2. Gaussian splatting shaders
    // 3. Vertex buffer management
    // 4. Instanced rendering of ellipsoids
    
    std::lock_guard<std::mutex> lock(splatsMutex_);
    
    if (splats_.empty())
      return;
    
    // For now, just log that we would render the splats
    std::cout << "Rendering " << splats_.size() << " gaussian splats (placeholder)" << std::endl;
  }

  bool TrGaussianSplattingRenderer::initializeGL()
  {
    // TODO: Initialize WebGL2 shaders and buffers
    // This should create:
    // 1. Vertex and fragment shaders for gaussian splatting
    // 2. Vertex buffer objects
    // 3. Texture resources if needed
    
    // Placeholder implementation
    return true;
  }

  void TrGaussianSplattingRenderer::shutdownGL()
  {
    // TODO: Clean up WebGL2 resources
    shaderProgram_ = 0;
    vertexBuffer_ = 0;
    indexBuffer_ = 0;
  }

  bool TrGaussianSplattingRenderer::loadGSplatFile(const std::string &filepath)
  {
    // TODO: Implement .gsplat file parsing
    // This is a placeholder for the actual file format parsing
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open())
    {
      std::cerr << "Failed to open .gsplat file: " << filepath << std::endl;
      return false;
    }
    
    // Placeholder: Create a simple test splat
    GaussianSplat testSplat = {};
    testSplat.position[0] = 0.0f;
    testSplat.position[1] = 0.0f;
    testSplat.position[2] = 0.0f;
    testSplat.rotation[3] = 1.0f; // w = 1 for identity quaternion
    testSplat.scale[0] = testSplat.scale[1] = testSplat.scale[2] = 1.0f;
    testSplat.color[0] = testSplat.color[1] = testSplat.color[2] = 1.0f;
    testSplat.opacity = 1.0f;
    
    std::lock_guard<std::mutex> lock(splatsMutex_);
    splats_.push_back(testSplat);
    needsSorting_ = true;
    
    return true;
  }

  bool TrGaussianSplattingRenderer::loadPlyFile(const std::string &filepath)
  {
    // TODO: Implement .ply file parsing for 3DGS
    // PLY files are a common format for 3D Gaussian Splatting data
    std::ifstream file(filepath);
    if (!file.is_open())
    {
      std::cerr << "Failed to open .ply file: " << filepath << std::endl;
      return false;
    }
    
    // Placeholder implementation
    return false;
  }

  void TrGaussianSplattingRenderer::computeDepths(const float viewMatrix[16])
  {
    for (auto &splat : splats_)
    {
      // Transform position by view matrix and compute depth (z component)
      float x = splat.position[0];
      float y = splat.position[1];
      float z = splat.position[2];
      
      // Apply view matrix transformation (simplified)
      splat.depth = viewMatrix[2] * x + viewMatrix[6] * y + viewMatrix[10] * z + viewMatrix[14];
    }
  }

  void TrGaussianSplattingRenderer::sortByDepth()
  {
    // Sort back to front (higher depth first) for correct alpha blending
    std::sort(splats_.begin(), splats_.end(), 
              [](const GaussianSplat &a, const GaussianSplat &b) {
                return a.depth > b.depth;
              });
  }
}