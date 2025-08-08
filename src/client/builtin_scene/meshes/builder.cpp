#include "./builder.hpp"
#include "../gaussian_splats_mesh.hpp"

namespace builtin_scene::meshes
{
  std::shared_ptr<GaussianSplatsMesh> MeshBuilder::CreateGaussianSplatsMesh()
  {
    return std::make_shared<GaussianSplatsMesh>();
  }
}