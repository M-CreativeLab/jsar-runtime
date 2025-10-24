#pragma once

#include <unordered_map>
#include <command_buffers/details/program.hpp>
#include "./common.hpp"

namespace gles
{
  class GLProgram
  {
  public:
    GLProgram(GLuint id)
        : id(id)
    {
    }
    ~GLProgram()
    {
      if (id > 0)
      {
        glDeleteProgram(id);
        id = 0;
      }
    }

    operator GLuint() const
    {
      return id;
    }

    /**
     * Links the program and checks for errors.
     * 
     * @throws std::runtime_error if linking fails.
     */
    void link() const
    {
      glLinkProgram(id);

      GLenum status;
      glGetProgramiv(id, GL_LINK_STATUS, (GLint *)&status);
      if (status == GL_FALSE)
      {
        GLint errorLength;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &errorLength);
        GLchar *errorStr = new GLchar[errorLength];
        glGetProgramInfoLog(id, errorLength, NULL, errorStr);

        std::string msg = "Failed to link program(" + std::to_string(id) + "): " + errorStr;
        delete[] errorStr;
        throw std::runtime_error(msg);
      }
    }
    void validate() const
    {
      glValidateProgram(id);
    }

    GLint updateAttribLocation(int index, const char *name)
    {
      assert(index >= 0);
      attrib_locations_map[index] = glGetAttribLocation(id, name);
      return attrib_locations_map[index];
    }
    GLint getAttribLocation(int index) const
    {
      auto it = attrib_locations_map.find(index);
      if (it != attrib_locations_map.end())
        return it->second;
      return -1;
    }

  public:
    GLuint id;

  private:
    std::unordered_map<int, GLint> attrib_locations_map;
  };
}
