#pragma once

#include <memory>
#include <vector>
#include <string>
#include <client/graphics/webgl_context.hpp>
#include <client/graphics/webgl_program.hpp>
#include <client/graphics/webgl_shader.hpp>

#include "./asset.hpp"
#include "./shader_base.hpp"

namespace builtin_scene
{
  class Mesh3d;
  class Material
  {
  public:
    static inline std::vector<std::string> GlobalDefines = {};
    /* The define to require multiview */
    static constexpr const char *kRequireMultiviewDefine = "REQUIRE_MULTIVIEW";

  public:
    /**
     * Set the global defines for all materials later created.
     *
     * @param define The define to set.
     */
    static void SetGlobalDefines(const std::string &define);

    /**
     * Unset the global defines for all materials later created.
     *
     * @param define The define to unset.
     */
    static void UnsetGlobalDefines(const std::string &define);

    /**
     * Set the multiview is required for all materials later created.
     *
     * @param required Whether the multiview is required.
     */
    static inline void SetMultiviewRequired(bool required)
    {
      required
          ? SetGlobalDefines(kRequireMultiviewDefine)
          : UnsetGlobalDefines(kRequireMultiviewDefine);
    }

    /**
     * Create a new instance of the material.
     *
     * @tparam MaterialType The type of the material.
     * @tparam Args The types of the arguments for the constructor of the material.
     * @param args The arguments for the constructor of the material.
     */
    template <typename MaterialType, typename... Args>
    static inline std::shared_ptr<MaterialType> Make(Args &&...args)
    {
      return std::make_shared<MaterialType>(std::forward<Args>(args)...);
    }

  public:
    Material()
    {
    }
    virtual ~Material() = default;

  public:
    /**
     * @returns The name of the material.
     */
    virtual const std::string name() const
    {
      return "Material";
    }
    /**
     * @returns The list of defines for the material.
     */
    virtual const std::vector<std::string> defines() const
    {
      return {};
    }
    /**
     * @returns The vertex shader for the material.
     */
    virtual ShaderRef vertexShader()
    {
      return ShaderRef(client_graphics::WebGLShaderType::kVertex, "shaders/mesh.vert");
    }
    /**
     * @returns The fragment shader for the material.
     */
    virtual ShaderRef fragmentShader()
    {
      return ShaderRef(client_graphics::WebGLShaderType::kFragment, "materials/default.frag");
    }
    /**
     * Initialize the material with the given program.
     *
     * @param glContext The WebGL context to initialize the material with.
     * @param program The WebGL program to initialize the material with.
     * @returns Whether the material is initialized successfully.
     */
    virtual bool initialize(std::shared_ptr<client_graphics::WebGL2Context> glContext,
                            std::shared_ptr<client_graphics::WebGLProgram> program)
    {
      if (TR_UNLIKELY(glContext == nullptr || program == nullptr))
        return false;
      glContext_ = glContext;
      return true;
    }
    /**
     * Called before drawing the mesh with the material.
     */
    virtual void onBeforeDrawMesh(std::shared_ptr<client_graphics::WebGLProgram> program, std::shared_ptr<Mesh3d> mesh) {}
    virtual void onAfterDrawMesh(std::shared_ptr<client_graphics::WebGLProgram> program, std::shared_ptr<Mesh3d> mesh) {}

  public:
    /**
     * @returns The list of defines for the material, which includes the global defines.
     */
    const std::vector<std::string> getDefinesWithGlobals() const
    {
      return mixDefines(defines(), GlobalDefines);
    }
    /**
     * @returns Whether the material is opaque.
     */
    inline bool isOpaque() const
    {
      return isOpaque_;
    }

  protected:
    /**
     * Mix the defines with the base defines of the material.
     *
     * @param baseDefines The base defines of the material.
     * @param definesToAdd The list of defines to add to the base defines.
     * @returns The mixed defines.
     */
    const std::vector<std::string> mixDefines(const std::vector<std::string> &baseDefines,
                                              const std::vector<std::string> &definesToAdd) const;

  protected:
    std::weak_ptr<client_graphics::WebGL2Context> glContext_;
    bool isOpaque_ = true;
  };
}
