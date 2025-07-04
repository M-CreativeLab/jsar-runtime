#pragma once

#include <unordered_set>
#include <unordered_map>

#include "./gpu_base.hpp"

namespace commandbuffers
{
  class GPUAdapterInfo
  {
  public:
    std::string architecture = "";
    std::string description = "";
    std::string device = "";
    std::string vendor = "";
    uint32_t subgroupMaxSize;
    uint32_t subgroupMinSize;
  };

  class GPUSupportedFeatures : public std::unordered_set<std::string>
  {
  public:
    GPUSupportedFeatures()
    {
      // TODO(yorkie): add required features
    }
  };

  class GPUSupportedLimits : public std::unordered_map<std::string, uint32_t>
  {
  public:
    GPUSupportedLimits()
    {
      insert({"maxTextureDimension1D", 8192});
      insert({"maxTextureDimension2D", 8192});
      insert({"maxTextureDimension3D", 2048});
      insert({"maxTextureArrayLayers", 256});
      insert({"maxBindGroups", 4});
      insert({"maxBindGroupEntries", 640});
    }

  public:
    uint32_t maxTextureDimension1D() const
    {
      return at("maxTextureDimension1D");
    }
    uint32_t maxTextureDimension2D() const
    {
      return at("maxTextureDimension2D");
    }
    uint32_t maxTextureDimension3D() const
    {
      return at("maxTextureDimension3D");
    }
    uint32_t maxTextureArrayLayers() const
    {
      return at("maxTextureArrayLayers");
    }
    uint32_t maxBindGroups() const
    {
      return at("maxBindGroups");
    }
    uint32_t maxBindGroupEntries() const
    {
      return at("maxBindGroupEntries");
    }
  };
}
