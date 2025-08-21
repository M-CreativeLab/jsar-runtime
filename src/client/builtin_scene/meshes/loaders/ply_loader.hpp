#pragma once

#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <client/builtin_scene/gaussian_splatting.hpp>
#include "./gaussian_splat_loader.hpp"

namespace builtin_scene::model_loaders
{
  /**
   * PLY file loader for 3D Gaussian Splatting models.
   * Follows the Spark implementation pattern:
   * https://github.com/sparkjsdev/spark/blob/main/src/ply.ts
   * 
   * PLY is a polygon file format that can store 3D model data including
   * vertex positions, colors, and properties for Gaussian splatting.
   */
  class PlyLoader : public GaussianSplatLoader
  {
  public:
    /**
     * PLY property types supported by the loader
     */
    enum class PropertyType
    {
      CHAR,
      UCHAR,
      SHORT,
      USHORT,
      INT,
      UINT,
      FLOAT,
      DOUBLE
    };

    /**
     * PLY property definition
     */
    struct PlyProperty
    {
      bool isList;
      PropertyType type;
      PropertyType countType; // For list properties
    };

    /**
     * PLY element definition
     */
    struct PlyElement
    {
      std::string name;
      int count;
      std::vector<std::pair<std::string, PlyProperty>> properties;
    };

    /**
     * Decode .ply file from data buffer using callback-based approach.
     * @param fileBytes Raw .ply file data
     * @param initNumSplats Callback called with total number of splats
     * @param splatCallback Callback called for each splat
     * @return true if loading was successful, false otherwise
     */
    static bool DecodePly(
      const std::vector<char> &fileBytes,
      std::function<void(int numSplats)> initNumSplats,
      SplatCallback splatCallback);

    // Override base class methods for progressive loading
    bool load(const std::vector<char> &data, std::vector<builtin_scene::GaussianSplat> &splats) override;
    bool initProgressiveLoading(const std::vector<char> &data, ProgressCallback progressCallback = nullptr) override;
    bool loadNextBatch(size_t batchSize, std::vector<builtin_scene::GaussianSplat> &splats) override;
    bool isProgressiveLoadingComplete() const override;
    int getTotalSplatCount() const override;
    int getLoadedSplatCount() const override;
    void resetProgressiveLoading() override;
    bool loadWithCallback(
      const std::vector<char> &data,
      std::function<void(int numSplats)> initNumSplats,
      SplatCallback splatCallback) override;

    /**
     * Convenience method to load splats into a vector (for backward compatibility).
     * @param data Raw .ply file data
     * @param splats Output vector to store loaded splats
     * @return true if loading was successful, false otherwise
     */
    static bool Load(const std::vector<char> &data, std::vector<builtin_scene::GaussianSplat> &splats);

  private:
    /**
     * Parse the PLY text header to extract element and property definitions.
     * @param fileBytes Raw file data
     * @param headerEnd Output: position where header ends and binary data begins
     * @param elements Output: map of element definitions
     * @param littleEndian Output: whether binary data is little-endian
     * @return true if header was parsed successfully
     */
    static bool ParseHeader(
      const std::vector<char> &fileBytes,
      size_t &headerEnd,
      std::unordered_map<std::string, PlyElement> &elements,
      bool &littleEndian);

    /**
     * Parse binary data for a specific element using its property definitions.
     * @param data Binary data starting at the element's data
     * @param element Element definition with properties
     * @param littleEndian Whether data is little-endian
     * @param callback Function to call for each item parsed
     * @return number of bytes consumed
     */
    static size_t ParseElementData(
      const char *data,
      const PlyElement &element,
      bool littleEndian,
      std::function<void(int index, const std::unordered_map<std::string, float> &item)> callback);

    /**
     * Get the size in bytes of a property type.
     * @param type Property type
     * @return Size in bytes
     */
    static size_t GetPropertyTypeSize(PropertyType type);

    /**
     * Parse a single property value from binary data.
     * @param data Binary data
     * @param offset Offset within data
     * @param type Property type
     * @param littleEndian Whether data is little-endian
     * @return Parsed value as float
     */
    static float ParsePropertyValue(const char *data, size_t offset, PropertyType type, bool littleEndian);

    /**
     * Convert property type string to PropertyType enum.
     * @param typeStr Property type string from PLY header
     * @return PropertyType enum value
     */
    static PropertyType StringToPropertyType(const std::string &typeStr);

    /**
     * Extract splat data from parsed vertex properties.
     * @param index Vertex index
     * @param properties Map of property name to value
     * @param splatCallback Callback to invoke with splat data
     */
    static void ExtractSplatData(
      int index,
      const std::unordered_map<std::string, float> &properties,
      SplatCallback splatCallback);

    /**
     * Read binary data with proper endianness handling.
     * @param data Raw binary data
     * @param offset Offset in bytes
     * @param value Output value
     * @param littleEndian Whether data is little-endian
     * @return true if read was successful, false if out of bounds
     */
    template <typename T>
    static bool ReadBinary(const char *data, size_t offset, T &value, bool littleEndian);

  private:
    // Progressive loading state
    std::vector<char> progressiveData_;
    ProgressCallback progressCallback_;
    int totalSplats_ = -1;
    int loadedSplats_ = 0;
    bool progressiveInitialized_ = false;

    // Parsed PLY structure for progressive loading
    size_t headerEnd_ = 0;
    std::unordered_map<std::string, PlyElement> elements_;
    bool littleEndian_ = true;
  };
}
