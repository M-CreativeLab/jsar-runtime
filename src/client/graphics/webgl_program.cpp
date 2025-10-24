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
        throw runtime_error("Unsupported shader type to attach to program.");
    }

    void WebGLProgram::detachShader(std::shared_ptr<WebGLShader> shader)
    {
      if (shader->type == WebGLShaderType::kVertex)
        vertexShader_ = nullptr;
      else if (shader->type == WebGLShaderType::kFragment)
        fragmentShader_ = nullptr;
      else
        throw runtime_error("Unsupported shader type to detach from program.");
    }

    void WebGLProgram::link()
    {
      if (!vertexShader_ || !fragmentShader_)
        throw runtime_error("Cannot link program without both vertex and fragment shaders.");

      string shaderSource = vertexShader_->source;
      auto attribsInfo = crates::webgl::GLSLAttributeParser::ParseAttributes(shaderSource);

      int loc = 0;
      for (const auto &attrib : attribsInfo)
      {
        attribLocations_[attrib.name] = WebGLAttribLocation(id, loc, attrib.name);

        // mat4 takes up 4 attribute locations
        int slots = 1;
        if (attrib.type_name == "mat2" ||
            attrib.type_name == "mat2x3" ||
            attrib.type_name == "mat2x4")
        {
          slots = 2;
        }
        else if (attrib.type_name == "mat3" ||
                 attrib.type_name == "mat3x2" ||
                 attrib.type_name == "mat3x4")
        {
          slots = 3;
        }
        else if (attrib.type_name == "mat4" ||
                 attrib.type_name == "mat4x2" ||
                 attrib.type_name == "mat4x3")
        {
          slots = 4;
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
