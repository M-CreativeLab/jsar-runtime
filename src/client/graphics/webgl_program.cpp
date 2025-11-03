#include <chrono>
#include "./webgl_program.hpp"

namespace endor
{
  namespace client_graphics
  {
    using namespace std;

    void WebGLProgram::attachShader(shared_ptr<WebGLShader> shader)
    {
      switch (shader->type)
      {
      case WebGLShaderType::kVertex:
        vertexShader_ = shader;
        glslang_program_.addShader(shader->glslangShader());
        break;
      case WebGLShaderType::kFragment:
        fragmentShader_ = shader;
        glslang_program_.addShader(shader->glslangShader());
        break;
      default:
        throw runtime_error("Unknown WebGLShaderType");
      }
    }

    void WebGLProgram::detachShader(shared_ptr<WebGLShader> shader)
    {
      switch (shader->type)
      {
      case WebGLShaderType::kVertex:
        vertexShader_ = nullptr;
        break;
      case WebGLShaderType::kFragment:
        fragmentShader_ = nullptr;
        break;
      default:
        throw runtime_error("Unknown WebGLShaderType");
      }
    }

    void WebGLProgram::link()
    {
      if (!glslang_program_.link(EShMsgDefault))
      {
        linkStatus_ = false;
        return;
      }

      // Build reflection to get active attributes and uniforms.
      int opts = EShReflectionDefault |
                 EShReflectionBasicArraySuffix |
                 EShReflectionIntermediateIO;
      if (!glslang_program_.buildReflection(opts))
      {
        throw runtime_error("Program link failed: " + string(glslang_program_.getInfoLog()));
      }

      {
        // Add active attributes.
        int loc = 0;
        int size = glslang_program_.getNumLiveAttributes();
        for (int i = 0; i < size; ++i)
        {
          const auto &ref = glslang_program_.getPipeInput(i);
          auto type = ref.glDefineType;
          int size = ref.size;
          string name = ref.name;
          if (size > 1 && name.find("[0]") == string::npos)
          {
            // For array uniforms, append "[0]" to the name.
            name += "[0]";
          }

          activeAttribs_[i] = WebGLActiveInfo(name, type, size);
          attribLocations_[name] = WebGLAttribLocation(id, loc, name);

          // Update location for next attribute.
          int slots = 1;
          switch (type)
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
          loc += slots;
        }
      }
      {
        // Add active uniforms.
        int loc = 0;
        int size = glslang_program_.getNumLiveUniformVariables();
        for (int i = 0; i < size; ++i)
        {
          const auto &ref = glslang_program_.getUniform(i);
          auto type = ref.glDefineType;
          int size = ref.size;
          string name = ref.name;
          if (size > 1 && name.find("[0]") == string::npos)
          {
            // For array uniforms, append "[0]" to the name.
            name += "[0]";
          }

          activeUniforms_[i] = WebGLActiveInfo(name, type, size);
          uniformLocations_[name] = WebGLUniformLocation(id, loc, name);
          loc += size;
        }
      }
      {
        // Add uniform block indices.
        int size = glslang_program_.getNumLiveUniformBlocks();
        for (int i = 0; i < size; ++i)
        {
          const auto &ref = glslang_program_.getUniformBlock(i);
          uniformBlockIndices_[ref.name] = i;
        }
      }

      // Mark as complete.
      linkStatus_ = true;
      validateStatus_ = true;
    }

    const char *WebGLProgram::getInfoLog()
    {
      return glslang_program_.getInfoLog();
    }

    void WebGLProgram::waitForCompleted(const string &label, int timeout) const
    {
      chrono::steady_clock::time_point startTime = chrono::steady_clock::now();
      unique_lock<mutex> lock(setCompletedMutex_);
      setCompletedCv_.wait_for(lock, chrono::milliseconds(timeout), [this]()
                               { return !incomplete_; });

      auto duration = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - startTime).count();
      cerr << "Waited for program " << id << " (" << label << ") completion for " << duration << " ms." << endl;
    }

    void WebGLProgram::setCompleted(bool linkStatus)
    {
      linkStatus_ = linkStatus;
      incomplete_ = false;
      {
        unique_lock<mutex> lock(setCompletedMutex_);
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
      return linkStatus_;
    }

    bool WebGLProgram::getValidateStatus(bool sync) const
    {
      return validateStatus_;
    }

    size_t WebGLProgram::countActiveAttribs() const
    {
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

    void WebGLProgram::setAttribLocation(const string &name, int location)
    {
      attribLocations_[name] = WebGLAttribLocation(id, location, name);
    }

    bool WebGLProgram::hasAttribLocation(const string &name)
    {
      return attribLocations_.find(name) != attribLocations_.end();
    }

    const WebGLAttribLocation &WebGLProgram::getAttribLocation(const string &name)
    {
      return attribLocations_[name];
    }

    vector<commandbuffers::AttribLocation> WebGLProgram::getAttribLocationsInfo() const
    {
      vector<commandbuffers::AttribLocation> list;
      for (const auto &pair : attribLocations_)
      {
        auto loc = pair.second;
        list.push_back(commandbuffers::AttribLocation(loc.name, loc.index.value_or(-1)));
      }
      return list;
    }

    void WebGLProgram::setUniformLocation(const string &name, int location)
    {
      uniformLocations_[name] = WebGLUniformLocation(id, location, name);
    }

    bool WebGLProgram::hasUniformLocation(const string &name)
    {
      return uniformLocations_.find(name) != uniformLocations_.end();
    }

    const WebGLUniformLocation &WebGLProgram::getUniformLocation(const string &name)
    {
      return uniformLocations_[name];
    }

    vector<commandbuffers::UniformLocation> WebGLProgram::getUniformLocationsInfo() const
    {
      vector<commandbuffers::UniformLocation> list;
      for (const auto &pair : uniformLocations_)
      {
        auto loc = pair.second;
        list.push_back(commandbuffers::UniformLocation(loc.name, loc.index.value_or(-1)));
      }
      return list;
    }

    void WebGLProgram::setUniformBlockIndex(const string &name, int index)
    {
      uniformBlockIndices_[name] = index;
    }
    bool WebGLProgram::hasUniformBlockIndex(const string &name)
    {
      return uniformBlockIndices_.find(name) != uniformBlockIndices_.end();
    }
    int WebGLProgram::getUniformBlockIndex(const string &name)
    {
      return uniformBlockIndices_[name];
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
