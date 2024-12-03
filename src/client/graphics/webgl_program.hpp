#pragma once

#include <map>
#include "common/command_buffers/details/program.hpp"
#include "./webgl_object.hpp"
#include "./webgl_active_info.hpp"
#include "./webgl_uniform_location.hpp"

namespace client_graphics
{
  class WebGLProgram : public WebGLObject
  {
  public:
    WebGLProgram() : WebGLObject(WebGLObjectType::Program)
    {
    }

  public:
    /**
     * It sets the link status of the program.
     *
     * @param linkStatus The link status of the program.
     */
    void setLinkStatus(bool linkStatus)
    {
      linkStatus_ = linkStatus;
    }
    /**
     * @returns THe current link status of the program.
     */
    bool getLinkStatus()
    {
      return linkStatus_;
    }
    /**
     * @returns The active attribute information at the given index.
     */
    WebGLActiveInfo getActiveAttrib(int index)
    {
      return activeAttribs_[index];
    }
    /**
     * It sets the active attribute information at the given index.
     *
     * @param index The index of the active attribute.
     * @param activeInfo The active attribute information.
     */
    void setActiveAttrib(int index, const commandbuffers::ActiveInfo &activeInfo)
    {
      activeAttribs_[index] = WebGLActiveInfo(activeInfo);
    }
    /**
     * @returns If the active attribute exists at the given index.
     */
    bool hasActiveAttrib(int index)
    {
      return activeAttribs_.find(index) != activeAttribs_.end();
    }
    /**
     * @param index The index of the active uniform.
     * @returns The active uniform information at the given index.
     */
    WebGLActiveInfo getActiveUniform(int index)
    {
      return activeUniforms_[index];
    }
    /**
     * It sets the active uniform information at the given index.
     *
     * @param index The index of the active uniform.
     * @param activeInfo The active uniform information.
     */
    void setActiveUniform(int index, commandbuffers::ActiveInfo &activeInfo)
    {
      activeUniforms_[index] = activeInfo;
    }
    /**
     * @param index The index of the active uniform.
     * @returns If the active uniform exists at the given index.
     */
    bool hasActiveUniform(int index)
    {
      return activeUniforms_.find(index) != activeUniforms_.end();
    }
    /**
     * It sets the attribute location for the given name.
     *
     * @param name The name of the attribute.
     * @param location The location of the attribute.
     */
    void setAttribLocation(const std::string &name, int location)
    {
      attribLocations_[name] = location;
    }
    /**
     * @param name The name of the attribute.
     * @returns If the attribute location exists for the given name.
     */
    bool hasAttribLocation(const std::string &name)
    {
      return attribLocations_.find(name) != attribLocations_.end();
    }
    /**
     * @param name The name of the attribute.
     * @returns The attribute location for the given name.
     */
    int getAttribLocation(const std::string &name)
    {
      return attribLocations_[name];
    }
    /**
     * It sets the uniform location for the given name.
     *
     * @param name The name of the uniform.
     * @param location The location of the uniform.
     */
    void setUniformLocation(const std::string &name, int location)
    {
      uniformLocations_[name] = WebGLUniformLocation(location, name);
    }
    /**
     * @param name The name of the uniform.
     * @returns If the uniform location exists for the given name.
     */
    bool hasUniformLocation(const std::string &name)
    {
      return uniformLocations_.find(name) != uniformLocations_.end();
    }
    /**
     * @param name The name of the uniform.
     * @returns The uniform location for the given name.
     */
    WebGLUniformLocation getUniformLocation(const std::string &name)
    {
      return uniformLocations_[name];
    }
    /**
     * It sets the uniform block index for the given name.
     *
     * @param name The name of the uniform block.
     * @param index The index of the uniform block.
     */
    void setUniformBlockIndex(const std::string &name, int index)
    {
      uniformBlockIndices_[name] = index;
    }
    /**
     * @param name The name of the uniform block.
     * @returns If the uniform block index exists for the given name.
     */
    bool hasUniformBlockIndex(const std::string &name)
    {
      return uniformBlockIndices_.find(name) != uniformBlockIndices_.end();
    }
    /**
     * @param name The name of the uniform block.
     * @returns The uniform block index for the given name.
     */
    int getUniformBlockIndex(const std::string &name)
    {
      return uniformBlockIndices_[name];
    }

  public:
    /**
     * It prints the information of the program to stdout, including:
     * 
     * - Link status
     * - Active attributes
     * - Active uniforms
     * - Attribute locations
     * - Uniform locations
     */
    void printInfo()
    {
      std::cout << "Program " << id << " info:" << std::endl;
      std::cout << "Link status: " << (linkStatus_ ? "true" : "false") << std::endl;
      std::cout << "Active attributes:" << std::endl;
      for (auto &pair : activeAttribs_)
      {
        auto activeInfo = pair.second;
        std::cout << "  " << pair.first << ": " << activeInfo.name << ", type: " << activeInfo.type << ", size: " << activeInfo.size << std::endl;
      }

      std::cout << "Active uniforms:" << std::endl;
      for (auto &pair : activeUniforms_)
      {
        auto activeInfo = pair.second;
        std::cout << "  " << pair.first << ": " << activeInfo.name << ", type: " << activeInfo.type << ", size: " << activeInfo.size << std::endl;
      }

      std::cout << "Attribute locations:" << std::endl;
      for (auto &pair : attribLocations_)
        std::cout << "  " << pair.first << ": " << pair.second << std::endl;

      std::cout << "Uniform locations:" << std::endl;
      for (auto &pair : uniformLocations_)
        std::cout << "  " << pair.first << ": " << pair.second.index << std::endl;
    }

  private:
    bool linkStatus_ = false;
    std::map<int, WebGLActiveInfo> activeAttribs_;
    std::map<int, WebGLActiveInfo> activeUniforms_;
    std::map<std::string, int> attribLocations_;
    std::map<std::string, WebGLUniformLocation> uniformLocations_;
    std::map<std::string, int> uniformBlockIndices_;
  };
}
