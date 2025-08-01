#include "./ply_parser.hpp"
#include <common/debug.hpp>
#include <sstream>
#include <iostream>

namespace builtin_scene::model_renderer
{
  static const char *LOG_TAG = "PlyParser";

  bool PlyParser::parse(const std::vector<char> &data, std::vector<GaussianSplat> &splats)
  {
    try {
      // Convert data to string stream
      std::string content(data.begin(), data.end());
      std::stringstream ss(content);
      
      // Create tinyply file instance
      tinyply::PlyFile plyFile;
      
      // Read PLY file
      plyFile.read(ss);
      
      // Request common properties for Gaussian Splatting
      std::shared_ptr<tinyply::PlyData> vertices;
      std::shared_ptr<tinyply::PlyData> colors;
      std::shared_ptr<tinyply::PlyData> opacities;
      std::shared_ptr<tinyply::PlyData> scales;
      std::shared_ptr<tinyply::PlyData> rotations;
      
      try {
        vertices = plyFile.request_properties_from_element("vertex", {"x", "y", "z"});
      } catch (const std::exception &e) {
        DEBUG(LOG_TAG, "Failed to get vertex positions: %s", e.what());
        return false;
      }
      
      try {
        colors = plyFile.request_properties_from_element("vertex", {"red", "green", "blue"});
      } catch (const std::exception &e) {
        // Try alternative color names
        try {
          colors = plyFile.request_properties_from_element("vertex", {"r", "g", "b"});
        } catch (const std::exception &e2) {
          DEBUG(LOG_TAG, "No color properties found, using default colors");
        }
      }
      
      try {
        opacities = plyFile.request_properties_from_element("vertex", {"opacity"});
      } catch (const std::exception &e) {
        try {
          opacities = plyFile.request_properties_from_element("vertex", {"alpha"});
        } catch (const std::exception &e2) {
          DEBUG(LOG_TAG, "No opacity properties found, using default opacity");
        }
      }
      
      try {
        scales = plyFile.request_properties_from_element("vertex", {"scale_0", "scale_1", "scale_2"});
      } catch (const std::exception &e) {
        try {
          scales = plyFile.request_properties_from_element("vertex", {"sx", "sy", "sz"});
        } catch (const std::exception &e2) {
          DEBUG(LOG_TAG, "No scale properties found, using default scales");
        }
      }
      
      try {
        rotations = plyFile.request_properties_from_element("vertex", {"rot_0", "rot_1", "rot_2", "rot_3"});
      } catch (const std::exception &e) {
        try {
          rotations = plyFile.request_properties_from_element("vertex", {"qx", "qy", "qz", "qw"});
        } catch (const std::exception &e2) {
          DEBUG(LOG_TAG, "No rotation properties found, using default rotations");
        }
      }
      
      // Parse vertex data (required for tinyply)
      plyFile.parse_vertex_data();
      
      if (!vertices || vertices->count == 0) {
        DEBUG(LOG_TAG, "No vertex data found in PLY file");
        return false;
      }
      
      splats.clear();
      splats.reserve(vertices->count);
      
      // Extract vertex positions
      auto position_data = vertices->get_data<float>();
      
      // Extract colors if available
      std::vector<float> color_data;
      if (colors && colors->count > 0) {
        color_data = colors->get_data<float>();
      }
      
      // Extract opacities if available
      std::vector<float> opacity_data;
      if (opacities && opacities->count > 0) {
        opacity_data = opacities->get_data<float>();
      }
      
      // Extract scales if available
      std::vector<float> scale_data;
      if (scales && scales->count > 0) {
        scale_data = scales->get_data<float>();
      }
      
      // Extract rotations if available
      std::vector<float> rotation_data;
      if (rotations && rotations->count > 0) {
        rotation_data = rotations->get_data<float>();
      }
      
      // Build splats from extracted data
      for (size_t i = 0; i < vertices->count; ++i) {
        GaussianSplat splat;
        
        // Position (required)
        if (i * 3 + 2 < position_data.size()) {
          splat.position[0] = position_data[i * 3 + 0];
          splat.position[1] = position_data[i * 3 + 1];
          splat.position[2] = position_data[i * 3 + 2];
        }
        
        // Color (default to white if not available)
        if (!color_data.empty() && i * 3 + 2 < color_data.size()) {
          splat.color[0] = color_data[i * 3 + 0] / 255.0f; // Normalize if in 0-255 range
          splat.color[1] = color_data[i * 3 + 1] / 255.0f;
          splat.color[2] = color_data[i * 3 + 2] / 255.0f;
        } else {
          splat.color[0] = 1.0f;
          splat.color[1] = 1.0f;
          splat.color[2] = 1.0f;
        }
        
        // Opacity (default to 1.0 if not available)
        if (!opacity_data.empty() && i < opacity_data.size()) {
          splat.opacity = opacity_data[i];
        } else {
          splat.opacity = 1.0f;
        }
        
        // Scale (default to 1.0 if not available)
        if (!scale_data.empty() && i * 3 + 2 < scale_data.size()) {
          splat.scale[0] = scale_data[i * 3 + 0];
          splat.scale[1] = scale_data[i * 3 + 1];
          splat.scale[2] = scale_data[i * 3 + 2];
        } else {
          splat.scale[0] = 1.0f;
          splat.scale[1] = 1.0f;
          splat.scale[2] = 1.0f;
        }
        
        // Rotation (default to identity quaternion if not available)
        if (!rotation_data.empty() && i * 4 + 3 < rotation_data.size()) {
          splat.rotation[0] = rotation_data[i * 4 + 0];
          splat.rotation[1] = rotation_data[i * 4 + 1];
          splat.rotation[2] = rotation_data[i * 4 + 2];
          splat.rotation[3] = rotation_data[i * 4 + 3];
        } else {
          splat.rotation[0] = 0.0f;
          splat.rotation[1] = 0.0f;
          splat.rotation[2] = 0.0f;
          splat.rotation[3] = 1.0f; // Identity quaternion
        }
        
        splats.push_back(splat);
      }
      
      DEBUG(LOG_TAG, "Successfully parsed PLY file with %zu splats using tinyply", splats.size());
      return true;
      
    } catch (const std::exception &e) {
      DEBUG(LOG_TAG, "Error parsing PLY file: %s", e.what());
      return false;
    }
  }

  float PlyParser::extractFloat(const std::shared_ptr<tinyply::PlyData> &plyData, 
                               size_t index, 
                               float defaultValue)
  {
    if (!plyData || !plyData->buffer || index >= plyData->count) {
      return defaultValue;
    }
    
    if (plyData->t == tinyply::Type::FLOAT32) {
      const float* data = reinterpret_cast<const float*>(plyData->buffer.get());
      return data[index];
    } else if (plyData->t == tinyply::Type::FLOAT64) {
      const double* data = reinterpret_cast<const double*>(plyData->buffer.get());
      return static_cast<float>(data[index]);
    }
    
    return defaultValue;
  }
}