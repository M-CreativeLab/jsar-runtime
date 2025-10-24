#include "meshes.hpp"
#include "gaussian_splats_mesh.hpp"

namespace endor
{
  namespace builtin_scene
  {
    std::shared_ptr<GaussianSplatsMesh> MeshBuilder::CreateGaussianSplatsMesh()
    {
      auto splatsMesh = std::make_shared<GaussianSplatsMesh>();
      splatsMesh->build();
      return splatsMesh;
    }
  }
} // namespace endor