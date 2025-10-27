#include <crates/bindings.webgl.hpp>
#include "./webgl_program.hpp"

namespace endor
{
  namespace client_graphics
  {
    using namespace std;

    void WebGLProgram::attachShader(std::shared_ptr<WebGLShader> shader)
    {
      if (shader->type == WebGLShaderType::kVertex)
        vertexShader_ = shader;
      else if (shader->type == WebGLShaderType::kFragment)
        fragmentShader_ = shader;
      else
        assert(false && "Shader type should be either vertex or fragment.");
    }

    void WebGLProgram::detachShader(std::shared_ptr<WebGLShader> shader)
    {
      if (shader->type == WebGLShaderType::kVertex)
        vertexShader_ = nullptr;
      else if (shader->type == WebGLShaderType::kFragment)
        fragmentShader_ = nullptr;
      else
        assert(false && "Shader type should be either vertex or fragment.");
    }

    void WebGLProgram::link()
    {
      if (!vertexShader_ || !fragmentShader_)
        throw runtime_error("Cannot link program without both vertex and fragment shaders.");

      vector<crates::webgl::GLSLAttribute> attribsInfo;
      vector<crates::webgl::GLSLUniform> uniformsInfo;

      cerr << "Vertex Shader Source:\n"
           << vertexShader_->source << endl;
      if (!crates::webgl::GLSLShaderAnalyzer::Parse(vertexShader_->source, attribsInfo, uniformsInfo, true))
      {
        throw runtime_error("Failed to parse vertex shader source for attribute info.");
      }

      {
        vector<crates::webgl::GLSLAttribute> attribsInfo;
        vector<crates::webgl::GLSLUniform> fragment_uniformsInfo;

        cerr << "Fragment Shader Source:\n"
             << fragmentShader_->source << endl;
        if (!crates::webgl::GLSLShaderAnalyzer::Parse(fragmentShader_->source, attribsInfo, fragment_uniformsInfo, true))
        {
          throw runtime_error("Failed to parse fragment shader source for uniform info.");
        }

        for (const auto &uniform : fragment_uniformsInfo)
        {
          // Avoid duplicates
          auto it = std::find_if(uniformsInfo.begin(), uniformsInfo.end(), [&uniform](const crates::webgl::GLSLUniform &u)
                                 { return u.name == uniform.name; });
          if (it == uniformsInfo.end())
          {
            uniformsInfo.push_back(uniform);
          }
        }
      }

      int loc = 0;
      int activeIndex = 0;

      for (const auto &attrib : attribsInfo)
      {
        attribLocations_[attrib.name] = WebGLAttribLocation(id,
                                                            attrib.active ? loc : -1,
                                                            attrib.name);
        if (attrib.active)
        {
          activeAttribs_[activeIndex] = WebGLActiveInfo(attrib.name, attrib.type, attrib.size);
          activeIndex += 1;
        }

        // mat4 takes up 4 attribute locations
        int slots = 1;
        switch (attrib.type)
        {
        case WEBGL_FLOAT_MAT2:
        case WEBGL2_FLOAT_MAT2x3:
        case WEBGL2_FLOAT_MAT2x4:
          slots = 2;
          break;
        case WEBGL_FLOAT_MAT3:
        case WEBGL2_FLOAT_MAT3x2:
        case WEBGL2_FLOAT_MAT3x4:
          slots = 3;
          break;
        case WEBGL_FLOAT_MAT4:
        case WEBGL2_FLOAT_MAT4x2:
        case WEBGL2_FLOAT_MAT4x3:
          slots = 4;
          break;
        default:
          break;
        }

        if (slots > 1)
        {
          for (int i = 1; i < slots; ++i)
          {
            string name = attrib.name + "[" + to_string(i) + "]";
            attribLocations_[name] = WebGLAttribLocation(id, loc + i, name);
          }
        }

        // Advance location
        loc += slots;
      }

      loc = 0;
      activeIndex = 0;

      for (const auto &uniform : uniformsInfo)
      {
        uniformLocations_[uniform.name] = WebGLUniformLocation(id,
                                                               uniform.active ? loc : -1,
                                                               uniform.name);
        if (uniform.active || true)
        {
          string name = uniform.name;
          if (uniform.size > 1)
            name += "[0]";
          cerr << "Uniform[" << activeIndex << "]: (" << name << "), type: " << uniform.type << ", size: " << uniform.size << ", active: " << (uniform.active ? "true" : "false") << endl;

          activeUniforms_[activeIndex] = WebGLActiveInfo(uniform.name, uniform.size, uniform.type);
          activeIndex += 1;
        }

        // For array uniforms, add locations for each element
        if (uniform.size > 1)
        {
          for (int i = 1; i < uniform.size; ++i)
          {
            string name = uniform.name + "[" + to_string(i) + "]";
            uniformLocations_[name] = WebGLUniformLocation(id, loc + i, name);
          }
        }

        // Advance location
        loc += 1;
      }
    }

    void WebGLProgram::waitForCompleted(const string &label, int timeout) const
    {
      chrono::steady_clock::time_point startTime = chrono::steady_clock::now();
      std::unique_lock<std::mutex> lock(setCompletedMutex_);
      setCompletedCv_.wait_for(lock, std::chrono::milliseconds(timeout), [this]()
                               { return !incomplete_; });

      auto duration = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - startTime).count();
      cerr << "Waited for program " << id << " (" << label << ") completion for " << duration << " ms." << endl;
    }

    void WebGLProgram::setCompleted(bool linkStatus)
    {
      linkStatus_ = linkStatus;
      incomplete_ = false;
      {
        std::unique_lock<std::mutex> lock(setCompletedMutex_);
        setCompletedCv_.notify_all();
      }
    }

    void WebGLProgram::setLinkStatus(bool linkStatus)
    {
      linkStatus_ = linkStatus;
    }

    void WebGLProgram::setValidateStatus(bool validateStatus)
    {
      validateStatus_ = validateStatus;
    }

    bool WebGLProgram::getLinkStatus(bool sync) const
    {
      if (sync)
        waitForCompleted("getLinkStatus");
      return linkStatus_;
    }

    bool WebGLProgram::getValidateStatus(bool sync) const
    {
      if (sync)
        waitForCompleted("getValidateStatus");
      return validateStatus_;
    }

    size_t WebGLProgram::countActiveAttribs() const
    {
      waitForCompleted("countActiveAttribs");
      return activeAttribs_.size();
    }

    WebGLActiveInfo WebGLProgram::getActiveAttrib(int index)
    {
      return activeAttribs_[index];
    }

    void WebGLProgram::setActiveAttrib(int index, const commandbuffers::ActiveInfo &activeInfo)
    {
      activeAttribs_[index] = WebGLActiveInfo(activeInfo);
    }

    bool WebGLProgram::hasActiveAttrib(int index)
    {
      return activeAttribs_.find(index) != activeAttribs_.end();
    }

    size_t WebGLProgram::countActiveUniforms() const
    {
      waitForCompleted("countActiveUniforms");
      return activeUniforms_.size();
    }

    WebGLActiveInfo WebGLProgram::getActiveUniform(int index)
    {
      return activeUniforms_[index];
    }

    void WebGLProgram::setActiveUniform(int index, const commandbuffers::ActiveInfo &activeInfo)
    {
      activeUniforms_[index] = activeInfo;
    }

    bool WebGLProgram::hasActiveUniform(int index)
    {
      return activeUniforms_.find(index) != activeUniforms_.end();
    }

    void WebGLProgram::setAttribLocation(const std::string &name, int location)
    {
      attribLocations_[name] = WebGLAttribLocation(id, location, name);
    }

    void WebGLProgram::resetAttribLocations()
    {
      attribLocations_.clear();
    }

    bool WebGLProgram::hasAttribLocation(const std::string &name)
    {
      return attribLocations_.find(name) != attribLocations_.end();
    }

    const WebGLAttribLocation &WebGLProgram::getAttribLocation(const std::string &name)
    {
      return attribLocations_[name];
    }

    vector<commandbuffers::AttribLocation> WebGLProgram::getAttribLocations() const
    {
      vector<commandbuffers::AttribLocation> list;
      for (const auto &pair : attribLocations_)
      {
        auto loc = pair.second;
        list.push_back(commandbuffers::AttribLocation(loc.name, loc.index.value_or(-1)));
      }
      return list;
    }

    void WebGLProgram::printInfo() const
    {
      cout << "Program " << id << " info:" << endl;
      cout << "Link status: " << (linkStatus_ ? "true" : "false") << endl;
      cout << "Active attributes(" << activeAttribs_.size() << "):" << endl;
      for (auto &pair : activeAttribs_)
      {
        auto activeInfo = pair.second;
        cout << "  " << pair.first << ": " << activeInfo.name << ", type: " << activeInfo.type << ", size: " << activeInfo.size << endl;
      }

      cout << "Active uniforms(" << activeUniforms_.size() << "):" << endl;
      for (auto &pair : activeUniforms_)
      {
        auto activeInfo = pair.second;
        cout << "  " << pair.first << ": " << activeInfo.name << ", type: " << activeInfo.type << ", size: " << activeInfo.size << endl;
      }

      cout << "Attribute locations:" << endl;
      for (auto &pair : attribLocations_)
        cout << "  " << pair.first << ": " << pair.second.index.value_or(-1) << endl;

      cout << "Uniform locations:" << endl;
      for (auto &pair : uniformLocations_)
        cout << "  " << pair.first << ": " << pair.second.index.value_or(-1) << endl;
    }
  }
} // namespace endor
