#pragma once

#include <map>
#include <unordered_map>
#include <chrono>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <common/command_buffers/details/program.hpp>

#include "./webgl_object.hpp"
#include "./webgl_active_info.hpp"
#include "./webgl_attrib_location.hpp"
#include "./webgl_uniform_location.hpp"

namespace endor
{
  namespace client_graphics
  {
    class WebGLProgram : public WebGLObject
    {
    public:
      WebGLProgram()
          : WebGLObject(WebGLObjectType::Program)
      {
      }

    public:
      // Returns if this program is incomplete, it means the program response has not been received from channel peer.
      bool isIncomplete() const
      {
        return incomplete_;
      }
      // Waits for the program to be completed, it will block until the program response is received from channel peer.
      void waitForCompleted(int timeout = 2000) const;
      // Calls setCompleted() when the program response is received from channel peer and updates the link status.
      void setCompleted(bool linkStatus);
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
     * It sets the validate status of the program.
     *
     * @param validateStatus The validate status of the program.
     */
      void setValidateStatus(bool validateStatus)
      {
        validateStatus_ = validateStatus;
      }
      /**
     * This method gets the current link status of the program.
     *
     * @param sync If `true`, it will wait for the program to be completed before returning the link status.
     * @returns The current link status of the program.
     */
      bool getLinkStatus(bool sync = false) const;
      /**
     * This method gets the current validate status of the program.
     *
     * @param sync If `true`, it will wait for the program to be completed before returning the validate status.
     * @returns The current validate status of the program.
     */
      bool getValidateStatus(bool sync = false) const;
      /**
     * @returns The number of active attributes in the program.
     */
      size_t countActiveAttribs() const
      {
        waitForCompleted();
        return activeAttribs_.size();
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
     * @returns The number of active uniforms in the program.
     */
      size_t countActiveUniforms() const
      {
        waitForCompleted();
        return activeUniforms_.size();
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
      void setActiveUniform(int index, const commandbuffers::ActiveInfo &activeInfo)
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
        attribLocations_[name] = WebGLAttribLocation(id, location, name);
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
      const WebGLAttribLocation &getAttribLocation(const std::string &name)
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
        uniformLocations_[name] = WebGLUniformLocation(id, location, name);
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
      const WebGLUniformLocation &getUniformLocation(const std::string &name)
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
      void printInfo() const;

    private:
      std::atomic<bool> incomplete_ = true;
      mutable std::mutex setCompletedMutex_;
      mutable std::condition_variable setCompletedCv_;

      bool linkStatus_ = false;
      bool validateStatus_ = false;
      std::map<int, WebGLActiveInfo> activeAttribs_;
      std::map<int, WebGLActiveInfo> activeUniforms_;
      std::unordered_map<std::string, WebGLAttribLocation> attribLocations_;
      std::unordered_map<std::string, WebGLUniformLocation> uniformLocations_;
      std::map<std::string, int> uniformBlockIndices_;
    };
  }
} // namespace endor
