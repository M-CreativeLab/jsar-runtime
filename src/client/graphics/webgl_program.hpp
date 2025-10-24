#pragma once

#include <map>
#include <unordered_map>
#include <chrono>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <common/command_buffers/details/program.hpp>

#include "./webgl_object.hpp"
#include "./webgl_shader.hpp"
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
      void attachShader(std::shared_ptr<WebGLShader> shader);
      void detachShader(std::shared_ptr<WebGLShader> shader);
      void link();

      // Returns if this program is incomplete, it means the program response has not been received from channel peer.
      bool isIncomplete() const
      {
        return incomplete_;
      }
      // Waits for the program to be completed, it will block until the program response is received from channel peer.
      void waitForCompleted(const std::string& label = "", int timeout = 2000) const;
      // Calls setCompleted() when the program response is received from channel peer and updates the link status.
      void setCompleted(bool linkStatus);

      void setLinkStatus(bool linkStatus);
      void setValidateStatus(bool validateStatus);
      bool getLinkStatus(bool sync = false) const;
      bool getValidateStatus(bool sync = false) const;

      size_t countActiveAttribs() const;
      WebGLActiveInfo getActiveAttrib(int index);
      void setActiveAttrib(int index, const commandbuffers::ActiveInfo &activeInfo);
      bool hasActiveAttrib(int index);
      size_t countActiveUniforms() const;
      WebGLActiveInfo getActiveUniform(int index);
      void setActiveUniform(int index, const commandbuffers::ActiveInfo &activeInfo);
      bool hasActiveUniform(int index);
      void setAttribLocation(const std::string &name, int location);
      void resetAttribLocations();
      bool hasAttribLocation(const std::string &name);
      const WebGLAttribLocation &getAttribLocation(const std::string &name);
      std::vector<commandbuffers::AttribLocation> getAttribLocations() const;

      void setUniformLocation(const std::string &name, int location)
      {
        uniformLocations_[name] = WebGLUniformLocation(id, location, name);
      }
      bool hasUniformLocation(const std::string &name)
      {
        return uniformLocations_.find(name) != uniformLocations_.end();
      }
      const WebGLUniformLocation &getUniformLocation(const std::string &name)
      {
        return uniformLocations_[name];
      }
      void setUniformBlockIndex(const std::string &name, int index)
      {
        uniformBlockIndices_[name] = index;
      }
      bool hasUniformBlockIndex(const std::string &name)
      {
        return uniformBlockIndices_.find(name) != uniformBlockIndices_.end();
      }
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
      std::shared_ptr<WebGLShader> vertexShader_;
      std::shared_ptr<WebGLShader> fragmentShader_;

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
