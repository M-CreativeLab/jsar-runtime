#include "gaussian_splat_loader.hpp"

namespace builtin_scene::model_loaders
{
  builtin_scene::GaussianSplat GaussianSplatLoader::createSplat(
    int index,
    float x,
    float y,
    float z,
    float scaleX,
    float scaleY,
    float scaleZ,
    float quatX,
    float quatY,
    float quatZ,
    float quatW,
    float opacity,
    float r,
    float g,
    float b)
  {
    builtin_scene::GaussianSplat splat;

    // Position
    splat.position[0] = x;
    splat.position[1] = y;
    splat.position[2] = z;

    // Color
    splat.color[0] = r;
    splat.color[1] = g;
    splat.color[2] = b;

    // Opacity
    splat.opacity = opacity;

    // Scale
    splat.scale[0] = scaleX;
    splat.scale[1] = scaleY;
    splat.scale[2] = scaleZ;

    // Rotation (quaternion)
    splat.rotation[0] = quatX;
    splat.rotation[1] = quatY;
    splat.rotation[2] = quatZ;
    splat.rotation[3] = quatW;

    return splat;
  }
}