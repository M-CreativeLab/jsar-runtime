#pragma once

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <map>

using namespace std;

namespace renderer
{
  enum CommandType
  {
    /** Program */
    kCommandTypeCreateProgram,
    kCommandTypeLinkProgram,
    kCommandTypeUseProgram,
    kCommandTypeGetProgramParameter,
    kCommandTypeGetProgramInfoLog,
    /** Shader */
    kCommandTypeAttachShader,
    kCommandTypeDetachShader,
    kCommandTypeCreateShader,
    kCommandTypeDeleteShader,
    kCommandTypeShaderSource,
    kCommandTypeCompileShader,
    kCommandTypeGetShaderSource,
    kCommandTypeGetShaderParameter,
    kCommandTypeGetShaderInfoLog,
    /** Buffer */
    kCommandTypeCreateBuffer,
    kCommandTypeBindBuffer,
    kCommandTypeBufferData,
    /** Texture */
    kCommandTypeCreateTexture,
    kCommandTypeBindTexture,
    kCommandTypeTexImage2D,
    kCommandTypeTexParameteri,
    kCommandTypeActiveTexture,
    kCommandTypeGenerateMipmap,
    /** Vertex Attribute */
    kCommandTypeEnableVertexAttribArray,
    kCommandTypeDisableVertexAttribArray,
    kCommandTypeVertexAttribPointer,
    kCommandTypeGetAttribLocation,
    kCommandTypeGetUniformLocation,
    /** Set Uniform */
    kCommandTypeUniform1f,
    kCommandTypeUniform1fv,
    kCommandTypeUniform1i,
    kCommandTypeUniform1iv,
    kCommandTypeUniform2f,
    kCommandTypeUniform2fv,
    kCommandTypeUniform2i,
    kCommandTypeUniform2iv,
    kCommandTypeUniform3f,
    kCommandTypeUniform3fv,
    kCommandTypeUniform3i,
    kCommandTypeUniform3iv,
    kCommandTypeUniform4f,
    kCommandTypeUniform4fv,
    kCommandTypeUniform4i,
    kCommandTypeUniform4iv,
    kCommandTypeUniformMatrix2fv,
    kCommandTypeUniformMatrix3fv,
    kCommandTypeUniformMatrix4fv,
    /** Draw */
    kCommandTypeDrawArrays,
    kCommandTypeDrawElements,
    /** Pixels */
    kCommandTypePixelStorei,
    /** Viewport & Scissor */
    kCommandTypeSetViewport,
    kCommandTypeSetScissor,
    /** Clear */
    kCommandTypeClear,
    kCommandTypeClearColor,
    kCommandTypeClearDepth,
    kCommandTypeClearStencil,
    /** Common */
    kCommandTypeDepthMask,
    kCommandTypeDepthFunc,
    kCommandTypeDepthRange,
    kCommandTypeStencilFunc,
    kCommandTypeStencilFuncSeparate,
    kCommandTypeStencilMask,
    kCommandTypeStencilMaskSeparate,
    kCommandTypeStencilOp,
    kCommandTypeStencilOpSeparate,
    kCommandTypeColorMask,
    kCommandTypeCullFace,
    kCommandTypeFrontFace,
    kCommandTypeEnable,
    kCommandTypeDisable,
    kCommandTypeGetBooleanv,
    kCommandTypeGetIntegerv,
    kCommandTypeGetFloatv,
    kCommandTypeGetString,
    kCommandTypeGetShaderPrecisionFormat,
    kCommandTypeGetError,
  };

  class CommandBuffer
  {
  public:
    CommandBuffer(CommandType type) : m_Type(type) {}
    ~CommandBuffer() {}
    CommandType GetType() { return m_Type; }
    void WaitFinished()
    {
      unique_lock<mutex> lock(m_MutexToFinish);
      m_ConditionOnFinished.wait(lock, [this]
                                 { return m_Finished.load(); });
    }
    void Finish()
    {
      unique_lock<mutex> lock(m_MutexToFinish);
      m_Finished.store(true);
      m_ConditionOnFinished.notify_all();
    }

  protected:
    CommandType m_Type;
    int m_CommandId;

  private:
    atomic<bool> m_Finished = false;
    mutex m_MutexToFinish;
    condition_variable m_ConditionOnFinished;
  };

  class CreateProgramCommandBuffer : public CommandBuffer
  {
  public:
    CreateProgramCommandBuffer() : CommandBuffer(kCommandTypeCreateProgram) {}
    ~CreateProgramCommandBuffer() {}

  public:
    int m_ProgramId = 0;
  };

  class LinkProgramCommandBuffer : public CommandBuffer
  {
  public:
    LinkProgramCommandBuffer(int id) : CommandBuffer(kCommandTypeLinkProgram), m_ProgramId(id) {}
    ~LinkProgramCommandBuffer() {}

  public:
    int m_ProgramId;
    std::map<std::string, int> m_UniformLocations;
  };

  class UseProgramCommandBuffer : public CommandBuffer
  {
  public:
    UseProgramCommandBuffer(int id) : CommandBuffer(kCommandTypeUseProgram), m_ProgramId(id) {}
    ~UseProgramCommandBuffer() {}

  public:
    int m_ProgramId;
  };

  class GetProgramParameterCommandBuffer : public CommandBuffer
  {
  public:
    GetProgramParameterCommandBuffer(int programId, int pname) : CommandBuffer(kCommandTypeGetProgramParameter),
                                                                 m_ProgramId(programId),
                                                                 m_Pname(pname) {}
    ~GetProgramParameterCommandBuffer() {}

  public:
    int m_ProgramId;
    int m_Pname;
    int m_Value;
  };

  class GetProgramInfoLogCommandBuffer : public CommandBuffer
  {
  public:
    GetProgramInfoLogCommandBuffer(int programId) : CommandBuffer(kCommandTypeGetProgramInfoLog),
                                                    m_ProgramId(programId) {}
    ~GetProgramInfoLogCommandBuffer()
    {
      delete[] m_InfoLog;
    }
    void CopyInfoLog(const char *infoLog, uint32_t length)
    {
      m_InfoLog = new char[length];
      memcpy((void *)m_InfoLog, infoLog, length);
    }

  public:
    int m_ProgramId;
    char *m_InfoLog;
  };

  class AttachShaderCommandBuffer : public CommandBuffer
  {
  public:
    AttachShaderCommandBuffer(int programId, int shaderId) : CommandBuffer(kCommandTypeAttachShader),
                                                             m_ProgramId(programId),
                                                             m_ShaderId(shaderId) {}
    ~AttachShaderCommandBuffer() {}

  public:
    int m_ProgramId;
    int m_ShaderId;
  };

  class DetachShaderCommandBuffer : public CommandBuffer
  {
  public:
    DetachShaderCommandBuffer(int programId, int shaderId) : CommandBuffer(kCommandTypeDetachShader),
                                                             m_ProgramId(programId),
                                                             m_ShaderId(shaderId) {}
    ~DetachShaderCommandBuffer() {}

  public:
    int m_ProgramId;
    int m_ShaderId;
  };

  class CreateShaderCommandBuffer : public CommandBuffer
  {
  public:
    CreateShaderCommandBuffer(int shaderType) : CommandBuffer(kCommandTypeCreateShader), m_ShaderType(shaderType) {}
    ~CreateShaderCommandBuffer() {}

  public:
    int m_ShaderType;
    int m_ShaderId = 0;
  };

  class DeleteShaderCommandBuffer : public CommandBuffer
  {
  public:
    DeleteShaderCommandBuffer(int shaderId) : CommandBuffer(kCommandTypeDeleteShader), m_ShaderId(shaderId) {}
    ~DeleteShaderCommandBuffer() {}

  public:
    int m_ShaderId;
  };

  class ShaderSourceCommandBuffer : public CommandBuffer
  {
  public:
    ShaderSourceCommandBuffer(int shaderId, const char *source, uint32_t length) : CommandBuffer(kCommandTypeShaderSource),
                                                                                   m_ShaderId(shaderId)
    {
      m_Length = length;
      m_Source = new char[length];
      memcpy((void *)m_Source, source, length);
    }
    ~ShaderSourceCommandBuffer()
    {
      delete[] m_Source;
    }

  public:
    int m_ShaderId;
    const char *m_Source;
    uint32_t m_Length;
  };

  class CompileShaderCommandBuffer : public CommandBuffer
  {
  public:
    CompileShaderCommandBuffer(int shaderId) : CommandBuffer(kCommandTypeCompileShader), m_ShaderId(shaderId) {}
    ~CompileShaderCommandBuffer() {}

  public:
    int m_ShaderId;
  };

  class GetShaderSourceCommandBuffer : public CommandBuffer
  {
  public:
    GetShaderSourceCommandBuffer(int shaderId) : CommandBuffer(kCommandTypeGetShaderSource), m_ShaderId(shaderId) {}
    ~GetShaderSourceCommandBuffer()
    {
      delete[] m_Source;
    }
    void CopySource(char *source, uint32_t length)
    {
      m_Source = new char[length];
      memcpy((void *)m_Source, source, length);
    }

  public:
    int m_ShaderId;
    char *m_Source;
  };

  class GetShaderParameterCommandBuffer : public CommandBuffer
  {
  public:
    GetShaderParameterCommandBuffer(int shaderId, int pname) : CommandBuffer(kCommandTypeGetShaderParameter),
                                                               m_ShaderId(shaderId),
                                                               m_Pname(pname) {}
    ~GetShaderParameterCommandBuffer() {}

  public:
    int m_ShaderId;
    int m_Pname;
    int m_Value;
  };

  class GetShaderInfoLogCommandBuffer : public CommandBuffer
  {
  public:
    GetShaderInfoLogCommandBuffer(int shaderId) : CommandBuffer(kCommandTypeGetShaderInfoLog),
                                                  m_ShaderId(shaderId) {}
    ~GetShaderInfoLogCommandBuffer()
    {
      delete[] m_InfoLog;
    }
    void CopyInfoLog(const char *infoLog, uint32_t length)
    {
      m_InfoLog = new char[length];
      memcpy((void *)m_InfoLog, infoLog, length);
    }

  public:
    int m_ShaderId;
    char *m_InfoLog;
  };

  class CreateBufferCommandBuffer : public CommandBuffer
  {
  public:
    CreateBufferCommandBuffer() : CommandBuffer(kCommandTypeCreateBuffer) {}
    ~CreateBufferCommandBuffer() {}

  public:
    int m_BufferId = 0;
  };

  class BindBufferCommandBuffer : public CommandBuffer
  {
  public:
    BindBufferCommandBuffer(int target, int buffer) : CommandBuffer(kCommandTypeBindBuffer),
                                                      m_Target(target),
                                                      m_Buffer(buffer) {}
    ~BindBufferCommandBuffer() {}

  public:
    int m_Target;
    int m_Buffer;
  };

  class BufferDataCommandBuffer : public CommandBuffer
  {
  public:
    BufferDataCommandBuffer(int target, int size, const void *data, int usage) : CommandBuffer(kCommandTypeBufferData),
                                                                                 m_Target(target),
                                                                                 m_Size(size),
                                                                                 m_Data(new char[size]),
                                                                                 m_Usage(usage)
    {
      memcpy((void *)m_Data, data, size);
    }
    ~BufferDataCommandBuffer()
    {
      delete[] m_Data;
    }

  public:
    int m_Target;
    int m_Size;
    const char *m_Data;
    int m_Usage;
  };

  class CreateTextureCommandBuffer : public CommandBuffer
  {
  public:
    CreateTextureCommandBuffer() : CommandBuffer(kCommandTypeCreateTexture) {}
    ~CreateTextureCommandBuffer() {}

  public:
    int m_TextureId = 0;
  };

  class BindTextureCommandBuffer : public CommandBuffer
  {
  public:
    BindTextureCommandBuffer(int target, int texture) : CommandBuffer(kCommandTypeBindTexture),
                                                        m_Target(target),
                                                        m_Texture(texture) {}
    ~BindTextureCommandBuffer() {}

  public:
    int m_Target;
    int m_Texture;
  };

  class TexImage2DCommandBuffer : public CommandBuffer
  {
  public:
    TexImage2DCommandBuffer(
        int target,
        int level,
        int internalformat,
        int width,
        int height,
        int border,
        int format,
        int type,
        size_t pixelsSize,
        const void *pixels) : CommandBuffer(kCommandTypeTexImage2D),
                              m_Target(target),
                              m_Level(level),
                              m_Internalformat(internalformat),
                              m_Width(width),
                              m_Height(height),
                              m_Border(border),
                              m_Format(format),
                              m_Type(type),
                              m_PixelsSize(pixelsSize),
                              m_Pixels(new char[pixelsSize])
    {
      memcpy((void *)m_Pixels, pixels, width * height * 4);
    }
    ~TexImage2DCommandBuffer()
    {
      delete[] m_Pixels;
    }

  public:
    int m_Target;
    int m_Level;
    int m_Internalformat;
    int m_Width;
    int m_Height;
    int m_Border;
    int m_Format;
    int m_Type;
    size_t m_PixelsSize;
    const char *m_Pixels;
  };

  class TexParameteriCommandBuffer : public CommandBuffer
  {
  public:
    TexParameteriCommandBuffer(int target, int pname, int param) : CommandBuffer(kCommandTypeTexParameteri),
                                                                   m_Target(target),
                                                                   m_Pname(pname),
                                                                   m_Param(param) {}
    ~TexParameteriCommandBuffer() {}

  public:
    int m_Target;
    int m_Pname;
    int m_Param;
  };

  class ActiveTextureCommandBuffer : public CommandBuffer
  {
  public:
    ActiveTextureCommandBuffer(int texture) : CommandBuffer(kCommandTypeActiveTexture),
                                              m_Texture(texture) {}
    ~ActiveTextureCommandBuffer() {}

  public:
    int m_Texture;
  };

  class GenerateMipmapCommandBuffer : public CommandBuffer
  {
  public:
    GenerateMipmapCommandBuffer(int target) : CommandBuffer(kCommandTypeGenerateMipmap),
                                              m_Target(target) {}
    ~GenerateMipmapCommandBuffer() {}

  public:
    int m_Target;
  };

  class EnableVertexAttribArrayCommandBuffer : public CommandBuffer
  {
  public:
    EnableVertexAttribArrayCommandBuffer(int index) : CommandBuffer(kCommandTypeEnableVertexAttribArray),
                                                      m_Index(index) {}
    ~EnableVertexAttribArrayCommandBuffer() {}

  public:
    int m_Index;
  };

  class DisableVertexAttribArrayCommandBuffer : public CommandBuffer
  {
  public:
    DisableVertexAttribArrayCommandBuffer(int index) : CommandBuffer(kCommandTypeDisableVertexAttribArray),
                                                      m_Index(index) {}
    ~DisableVertexAttribArrayCommandBuffer() {}

  public:
    int m_Index;
  };

  class VertexAttribPointerCommandBuffer : public CommandBuffer
  {
  public:
    VertexAttribPointerCommandBuffer(int index, int size, int type, bool normalized, int stride, const void *offset) : CommandBuffer(kCommandTypeVertexAttribPointer),
                                                                                                                       m_Index(index),
                                                                                                                       m_Size(size),
                                                                                                                       m_Type(type),
                                                                                                                       m_Normalized(normalized),
                                                                                                                       m_Stride(stride),
                                                                                                                       m_Offset(offset) {}
    ~VertexAttribPointerCommandBuffer() {}

  public:
    int m_Index;
    int m_Size;
    int m_Type;
    bool m_Normalized;
    int m_Stride;
    const void *m_Offset;
  };

  class GetAttribLocationCommandBuffer : public CommandBuffer
  {
  public:
    GetAttribLocationCommandBuffer(int program, const char *name) : CommandBuffer(kCommandTypeGetAttribLocation),
                                                                    m_Program(program),
                                                                    m_Name(new char[strlen(name) + 1])
    {
      strcpy((char *)m_Name, name);
    }
    ~GetAttribLocationCommandBuffer()
    {
      delete[] m_Name;
    }

  public:
    int m_Program;
    const char *m_Name;
    int m_Location;
  };

  class GetUniformLocationCommandBuffer : public CommandBuffer
  {
  public:
    GetUniformLocationCommandBuffer(int program, const char *name) : CommandBuffer(kCommandTypeGetUniformLocation),
                                                                     m_Program(program),
                                                                     m_Name(new char[strlen(name) + 1])
    {
      strcpy((char *)m_Name, name);
    }
    ~GetUniformLocationCommandBuffer()
    {
      delete[] m_Name;
    }

  public:
    int m_Program;
    const char *m_Name;
    int m_Location;
  };

  class Uniform1fCommandBuffer : public CommandBuffer
  {
  public:
    Uniform1fCommandBuffer(int location, float v0) : CommandBuffer(kCommandTypeUniform1f),
                                                     m_Location(location),
                                                     m_V0(v0) {}
    ~Uniform1fCommandBuffer() {}

  public:
    int m_Location;
    float m_V0;
  };

  class Uniform1fvCommandBuffer : public CommandBuffer
  {
  public:
    Uniform1fvCommandBuffer(int location, std::vector<float> values) : CommandBuffer(kCommandTypeUniform1fv),
                                                                       m_Location(location)
    {
      m_Value = new float[values.size()];
      for (int i = 0; i < values.size(); i++)
        m_Value[i] = values[i];
      m_Count = values.size();
    }
    ~Uniform1fvCommandBuffer()
    {
      delete[] m_Value;
    }

  public:
    int m_Location;
    int m_Count;
    float *m_Value;
  };

  class Uniform1iCommandBuffer : public CommandBuffer
  {
  public:
    Uniform1iCommandBuffer(int location, int v0) : CommandBuffer(kCommandTypeUniform1i),
                                                   m_Location(location),
                                                   m_V0(v0) {}
    ~Uniform1iCommandBuffer() {}

  public:
    int m_Location;
    int m_V0;
  };

  class Uniform1ivCommandBuffer : public CommandBuffer
  {
  public:
    Uniform1ivCommandBuffer(int location, std::vector<int> values) : CommandBuffer(kCommandTypeUniform1iv),
                                                                     m_Location(location)
    {
      m_Value = new int[values.size()];
      for (int i = 0; i < values.size(); i++)
        m_Value[i] = values[i];
      m_Count = values.size();
    }
    ~Uniform1ivCommandBuffer()
    {
      delete[] m_Value;
    }

  public:
    int m_Location;
    int m_Count;
    int *m_Value;
  };

  class Uniform2fCommandBuffer : public CommandBuffer
  {
  public:
    Uniform2fCommandBuffer(int location, float v0, float v1) : CommandBuffer(kCommandTypeUniform2f),
                                                               m_Location(location),
                                                               m_V0(v0),
                                                               m_V1(v1) {}
    ~Uniform2fCommandBuffer() {}

  public:
    int m_Location;
    float m_V0;
    float m_V1;
  };

  class Uniform2fvCommandBuffer : public CommandBuffer
  {
  public:
    Uniform2fvCommandBuffer(int location, std::vector<float> values) : CommandBuffer(kCommandTypeUniform2fv),
                                                                       m_Location(location)
    {
      m_Value = new float[values.size()];
      for (int i = 0; i < values.size(); i++)
        m_Value[i] = values[i];
      m_Count = values.size() / 2;
    }
    ~Uniform2fvCommandBuffer()
    {
      delete[] m_Value;
    }

  public:
    int m_Location;
    int m_Count;
    float *m_Value;
  };

  class Uniform2iCommandBuffer : public CommandBuffer
  {
  public:
    Uniform2iCommandBuffer(int location, int v0, int v1) : CommandBuffer(kCommandTypeUniform2i),
                                                           m_Location(location),
                                                           m_V0(v0),
                                                           m_V1(v1) {}
    ~Uniform2iCommandBuffer() {}

  public:
    int m_Location;
    int m_V0;
    int m_V1;
  };

  class Uniform2ivCommandBuffer : public CommandBuffer
  {
  public:
    Uniform2ivCommandBuffer(int location, std::vector<int> values) : CommandBuffer(kCommandTypeUniform2iv),
                                                                     m_Location(location)
    {
      m_Value = new int[values.size()];
      for (int i = 0; i < values.size(); i++)
        m_Value[i] = values[i];
      m_Count = values.size() / 2;
    }
    ~Uniform2ivCommandBuffer()
    {
      delete[] m_Value;
    }

  public:
    int m_Location;
    int m_Count;
    int *m_Value;
  };

  class Uniform3fCommandBuffer : public CommandBuffer
  {
  public:
    Uniform3fCommandBuffer(int location, float v0, float v1, float v2) : CommandBuffer(kCommandTypeUniform3f),
                                                                         m_Location(location),
                                                                         m_V0(v0),
                                                                         m_V1(v1),
                                                                         m_V2(v2) {}
    ~Uniform3fCommandBuffer() {}

  public:
    int m_Location;
    float m_V0;
    float m_V1;
    float m_V2;
  };

  class Uniform3fvCommandBuffer : public CommandBuffer
  {
  public:
    Uniform3fvCommandBuffer(int location, std::vector<float> values) : CommandBuffer(kCommandTypeUniform3fv),
                                                                       m_Location(location)
    {
      m_Value = new float[values.size()];
      for (int i = 0; i < values.size(); i++)
        m_Value[i] = values[i];
      m_Count = values.size() / 3;
    }
    ~Uniform3fvCommandBuffer()
    {
      delete[] m_Value;
    }

  public:
    int m_Location;
    int m_Count;
    float *m_Value;
  };

  class Uniform3iCommandBuffer : public CommandBuffer
  {
  public:
    Uniform3iCommandBuffer(int location, int v0, int v1, int v2) : CommandBuffer(kCommandTypeUniform3i),
                                                                   m_Location(location),
                                                                   m_V0(v0),
                                                                   m_V1(v1),
                                                                   m_V2(v2) {}
    ~Uniform3iCommandBuffer() {}

  public:
    int m_Location;
    int m_V0;
    int m_V1;
    int m_V2;
  };

  class Uniform3ivCommandBuffer : public CommandBuffer
  {
  public:
    Uniform3ivCommandBuffer(int location, std::vector<int> values) : CommandBuffer(kCommandTypeUniform3iv),
                                                                     m_Location(location)
    {
      m_Value = new int[values.size()];
      for (int i = 0; i < values.size(); i++)
        m_Value[i] = values[i];
      m_Count = values.size() / 3;
    }
    ~Uniform3ivCommandBuffer()
    {
      delete[] m_Value;
    }

  public:
    int m_Location;
    int m_Count;
    int *m_Value;
  };

  class Uniform4fCommandBuffer : public CommandBuffer
  {
  public:
    Uniform4fCommandBuffer(int location, float v0, float v1, float v2, float v3) : CommandBuffer(kCommandTypeUniform4f),
                                                                                   m_Location(location),
                                                                                   m_V0(v0),
                                                                                   m_V1(v1),
                                                                                   m_V2(v2),
                                                                                   m_V3(v3) {}
    ~Uniform4fCommandBuffer() {}

  public:
    int m_Location;
    float m_V0;
    float m_V1;
    float m_V2;
    float m_V3;
  };

  class Uniform4fvCommandBuffer : public CommandBuffer
  {
  public:
    Uniform4fvCommandBuffer(int location, std::vector<float> values) : CommandBuffer(kCommandTypeUniform4fv),
                                                                       m_Location(location)
    {
      m_Value = new float[values.size()];
      for (int i = 0; i < values.size(); i++)
        m_Value[i] = values[i];
      m_Count = values.size() / 4;
    }
    ~Uniform4fvCommandBuffer()
    {
      delete[] m_Value;
    }

  public:
    int m_Location;
    int m_Count;
    float *m_Value;
  };

  class Uniform4iCommandBuffer : public CommandBuffer
  {
  public:
    Uniform4iCommandBuffer(int location, int v0, int v1, int v2, int v3) : CommandBuffer(kCommandTypeUniform4i),
                                                                           m_Location(location),
                                                                           m_V0(v0),
                                                                           m_V1(v1),
                                                                           m_V2(v2),
                                                                           m_V3(v3) {}
    ~Uniform4iCommandBuffer() {}

  public:
    int m_Location;
    int m_V0;
    int m_V1;
    int m_V2;
    int m_V3;
  };

  class Uniform4ivCommandBuffer : public CommandBuffer
  {
  public:
    Uniform4ivCommandBuffer(int location, std::vector<int> values) : CommandBuffer(kCommandTypeUniform4iv),
                                                                     m_Location(location)
    {
      m_Value = new int[values.size()];
      for (int i = 0; i < values.size(); i++)
        m_Value[i] = values[i];
      m_Count = values.size() / 4;
    }
    ~Uniform4ivCommandBuffer()
    {
      delete[] m_Value;
    }

  public:
    int m_Location;
    int m_Count;
    int *m_Value;
  };

  class UniformMatrix2fvCommandBuffer : public CommandBuffer
  {
  public:
    UniformMatrix2fvCommandBuffer(int location, bool transpose, std::vector<float> values) : CommandBuffer(kCommandTypeUniformMatrix2fv),
                                                                                             m_Location(location),
                                                                                             m_Transpose(transpose)
    {
      m_Value = new float[values.size()];
      for (int i = 0; i < values.size(); i++)
        m_Value[i] = values[i];
      m_Count = 1; // webgl only supports 1 matrix
    }
    ~UniformMatrix2fvCommandBuffer()
    {
      delete[] m_Value;
    }

  public:
    int m_Location;
    int m_Count;
    bool m_Transpose;
    float *m_Value;
  };

  class UniformMatrix3fvCommandBuffer : public CommandBuffer
  {
  public:
    UniformMatrix3fvCommandBuffer(int location, bool transpose, std::vector<float> values) : CommandBuffer(kCommandTypeUniformMatrix3fv),
                                                                                             m_Location(location),
                                                                                             m_Transpose(transpose)
    {
      m_Value = new float[values.size()];
      for (int i = 0; i < values.size(); i++)
        m_Value[i] = values[i];
      m_Count = 1; // webgl only supports 1 matrix
    }
    ~UniformMatrix3fvCommandBuffer()
    {
      delete[] m_Value;
    }

  public:
    int m_Location;
    int m_Count;
    bool m_Transpose;
    float *m_Value;
  };

  class UniformMatrix4fvCommandBuffer : public CommandBuffer
  {
  public:
    UniformMatrix4fvCommandBuffer(int location, bool transpose, std::vector<float> values) : CommandBuffer(kCommandTypeUniformMatrix4fv),
                                                                                             m_Location(location),
                                                                                             m_Transpose(transpose)
    {
      m_Value = new float[values.size()];
      for (int i = 0; i < values.size(); i++)
        m_Value[i] = values[i];
      m_Count = 1; // webgl only supports 1 matrix
    }
    ~UniformMatrix4fvCommandBuffer()
    {
      delete[] m_Value;
    }

  public:
    int m_Location;
    int m_Count;
    bool m_Transpose;
    float *m_Value;
  };

  class DrawArraysCommandBuffer : public CommandBuffer
  {
  public:
    DrawArraysCommandBuffer(int mode, int first, int count) : CommandBuffer(kCommandTypeDrawArrays),
                                                              m_Mode(mode),
                                                              m_First(first),
                                                              m_Count(count) {}
    ~DrawArraysCommandBuffer() {}

  public:
    int m_Mode;
    int m_First;
    int m_Count;
  };

  class DrawElementsCommandBuffer : public CommandBuffer
  {
  public:
    DrawElementsCommandBuffer(int mode, int count, int type, const void *indices) : CommandBuffer(kCommandTypeDrawElements),
                                                                                    m_Mode(mode),
                                                                                    m_Count(count),
                                                                                    m_Type(type),
                                                                                    m_Indices(indices) {}
    ~DrawElementsCommandBuffer() {}

  public:
    int m_Mode;
    int m_Count;
    int m_Type;
    const void *m_Indices;
  };

  class PixelStoreiCommandBuffer : public CommandBuffer
  {
  public:
    PixelStoreiCommandBuffer(int pname, int param) : CommandBuffer(kCommandTypePixelStorei),
                                                     m_Pname(pname),
                                                     m_Param(param) {}
    ~PixelStoreiCommandBuffer() {}

  public:
    int m_Pname;
    int m_Param;
  };

  class SetViewportCommandBuffer : public CommandBuffer
  {
  public:
    SetViewportCommandBuffer(int x, int y, int width, int height) : CommandBuffer(kCommandTypeSetViewport),
                                                                    m_X(x),
                                                                    m_Y(y),
                                                                    m_Width(width),
                                                                    m_Height(height) {}
    ~SetViewportCommandBuffer() {}

  public:
    int m_X;
    int m_Y;
    int m_Width;
    int m_Height;
  };

  class SetScissorCommandBuffer : public CommandBuffer
  {
  public:
    SetScissorCommandBuffer(int x, int y, int width, int height) : CommandBuffer(kCommandTypeSetScissor),
                                                                   m_X(x),
                                                                   m_Y(y),
                                                                   m_Width(width),
                                                                   m_Height(height) {}
    ~SetScissorCommandBuffer() {}

  public:
    int m_X;
    int m_Y;
    int m_Width;
    int m_Height;
  };

  class ClearCommandBuffer : public CommandBuffer
  {
  public:
    ClearCommandBuffer(int mask) : CommandBuffer(kCommandTypeClear), m_Mask(mask){};
    ~ClearCommandBuffer();

  public:
    int m_Mask;
  };

  class ClearColorCommandBuffer : public CommandBuffer
  {
  public:
    ClearColorCommandBuffer(float r, float g, float b, float a) : CommandBuffer(kCommandTypeClearColor),
                                                                  m_R(r),
                                                                  m_G(g),
                                                                  m_B(b),
                                                                  m_A(a) {}
    ~ClearColorCommandBuffer() {}

  public:
    float m_R;
    float m_G;
    float m_B;
    float m_A;
  };

  class ClearDepthCommandBuffer : public CommandBuffer
  {
  public:
    ClearDepthCommandBuffer(float depth) : CommandBuffer(kCommandTypeClearDepth), m_Depth(depth) {}
    ~ClearDepthCommandBuffer() {}

  public:
    float m_Depth;
  };

  class ClearStencilCommandBuffer : public CommandBuffer
  {
  public:
    ClearStencilCommandBuffer(int stencil) : CommandBuffer(kCommandTypeClearStencil), m_Stencil(stencil) {}
    ~ClearStencilCommandBuffer() {}

  public:
    int m_Stencil;
  };

  class DepthMaskCommandBuffer : public CommandBuffer
  {
  public:
    DepthMaskCommandBuffer(bool flag) : CommandBuffer(kCommandTypeDepthMask), m_Flag(flag) {}
    ~DepthMaskCommandBuffer() {}

  public:
    bool m_Flag;
  };

  class DepthFuncCommandBuffer : public CommandBuffer
  {
  public:
    DepthFuncCommandBuffer(int func) : CommandBuffer(kCommandTypeDepthFunc), m_Func(func) {}
    ~DepthFuncCommandBuffer() {}

  public:
    int m_Func;
  };

  class DepthRangeCommandBuffer : public CommandBuffer
  {
  public:
    DepthRangeCommandBuffer(float near, float far) : CommandBuffer(kCommandTypeDepthRange),
                                                     m_Near(near),
                                                     m_Far(far) {}
    ~DepthRangeCommandBuffer() {}

  public:
    float m_Near;
    float m_Far;
  };

  class StencilFuncCommandBuffer : public CommandBuffer
  {
  public:
    StencilFuncCommandBuffer(int func, int ref, int mask) : CommandBuffer(kCommandTypeStencilFunc),
                                                            m_Func(func),
                                                            m_Ref(ref),
                                                            m_Mask(mask) {}
    ~StencilFuncCommandBuffer() {}

  public:
    int m_Func;
    int m_Ref;
    int m_Mask;
  };

  class StencilFuncSeparateCommandBuffer : public CommandBuffer
  {
  public:
    StencilFuncSeparateCommandBuffer(int face, int func, int ref, int mask) : CommandBuffer(kCommandTypeStencilFuncSeparate),
                                                                              m_Face(face),
                                                                              m_Func(func),
                                                                              m_Ref(ref),
                                                                              m_Mask(mask) {}
    ~StencilFuncSeparateCommandBuffer() {}

  public:
    int m_Face;
    int m_Func;
    int m_Ref;
    int m_Mask;
  };

  class StencilMaskCommandBuffer : public CommandBuffer
  {
  public:
    StencilMaskCommandBuffer(int mask) : CommandBuffer(kCommandTypeStencilMask), m_Mask(mask) {}
    ~StencilMaskCommandBuffer() {}

  public:
    int m_Mask;
  };

  class StencilMaskSeparateCommandBuffer : public CommandBuffer
  {
  public:
    StencilMaskSeparateCommandBuffer(int face, int mask) : CommandBuffer(kCommandTypeStencilMaskSeparate),
                                                           m_Face(face),
                                                           m_Mask(mask) {}
    ~StencilMaskSeparateCommandBuffer() {}

  public:
    int m_Face;
    int m_Mask;
  };

  class StencilOpCommandBuffer : public CommandBuffer
  {
  public:
    StencilOpCommandBuffer(int fail, int zfail, int zpass) : CommandBuffer(kCommandTypeStencilOp),
                                                             m_Fail(fail),
                                                             m_Zfail(zfail),
                                                             m_Zpass(zpass) {}
    ~StencilOpCommandBuffer() {}

  public:
    int m_Fail;
    int m_Zfail;
    int m_Zpass;
  };

  class StencilOpSeparateCommandBuffer : public CommandBuffer
  {
  public:
    StencilOpSeparateCommandBuffer(int face, int fail, int zfail, int zpass) : CommandBuffer(kCommandTypeStencilOpSeparate),
                                                                               m_Face(face),
                                                                               m_Fail(fail),
                                                                               m_Zfail(zfail),
                                                                               m_Zpass(zpass) {}
    ~StencilOpSeparateCommandBuffer() {}

  public:
    int m_Face;
    int m_Fail;
    int m_Zfail;
    int m_Zpass;
  };

  class ColorMaskCommandBuffer : public CommandBuffer
  {
  public:
    ColorMaskCommandBuffer(bool r, bool g, bool b, bool a) : CommandBuffer(kCommandTypeColorMask),
                                                             m_R(r),
                                                             m_G(g),
                                                             m_B(b),
                                                             m_A(a) {}
    ~ColorMaskCommandBuffer() {}

  public:
    bool m_R;
    bool m_G;
    bool m_B;
    bool m_A;
  };

  class CullFaceCommandBuffer : public CommandBuffer
  {
  public:
    CullFaceCommandBuffer(int mode) : CommandBuffer(kCommandTypeCullFace), m_Mode(mode) {}
    ~CullFaceCommandBuffer() {}

  public:
    int m_Mode;
  };

  class FrontFaceCommandBuffer : public CommandBuffer
  {
  public:
    FrontFaceCommandBuffer(int mode) : CommandBuffer(kCommandTypeFrontFace), m_Mode(mode) {}
    ~FrontFaceCommandBuffer() {}

  public:
    int m_Mode;
  };

  class EnableCommandBuffer : public CommandBuffer
  {
  public:
    EnableCommandBuffer(uint32_t cap) : CommandBuffer(kCommandTypeEnable), m_Cap(cap) {}
    ~EnableCommandBuffer() {}

  public:
    uint32_t m_Cap;
  };

  class DisableCommandBuffer : public CommandBuffer
  {
  public:
    DisableCommandBuffer(uint32_t cap) : CommandBuffer(kCommandTypeDisable), m_Cap(cap) {}
    ~DisableCommandBuffer() {}

  public:
    uint32_t m_Cap;
  };

  class GetBooleanvCommandBuffer : public CommandBuffer
  {
  public:
    GetBooleanvCommandBuffer(uint32_t pname) : CommandBuffer(kCommandTypeGetBooleanv), m_Pname(pname) {}
    ~GetBooleanvCommandBuffer() {}

  public:
    uint32_t m_Pname;
    bool m_Value;
  };

  class GetIntegervCommandBuffer : public CommandBuffer
  {
  public:
    GetIntegervCommandBuffer(uint32_t pname) : CommandBuffer(kCommandTypeGetIntegerv), m_Pname(pname) {}
    ~GetIntegervCommandBuffer() {}

  public:
    uint32_t m_Pname;
    int m_Value;
  };

  class GetFloatvCommandBuffer : public CommandBuffer
  {
  public:
    GetFloatvCommandBuffer(uint32_t pname) : CommandBuffer(kCommandTypeGetFloatv), m_Pname(pname) {}
    ~GetFloatvCommandBuffer() {}

  public:
    uint32_t m_Pname;
    float m_Value;
  };

  class GetStringCommandBuffer : public CommandBuffer
  {
  public:
    GetStringCommandBuffer(uint32_t pname) : CommandBuffer(kCommandTypeGetString), m_Pname(pname) {}
    ~GetStringCommandBuffer()
    {
      delete[] m_Value;
    }
    void CopyValue(const uint8_t *value)
    {
      m_Value = new char[strlen((const char *)value) + 1];
      strcpy((char *)m_Value, (const char *)value);
    }

  public:
    uint32_t m_Pname;
    const char *m_Value;
  };

  class GetShaderPrecisionFormatCommandBuffer : public CommandBuffer
  {
  public:
    GetShaderPrecisionFormatCommandBuffer(uint32_t shadertype, uint32_t precisiontype) : CommandBuffer(kCommandTypeGetShaderPrecisionFormat),
                                                                                         m_ShaderType(shadertype),
                                                                                         m_PrecisionType(precisiontype) {}
    ~GetShaderPrecisionFormatCommandBuffer() {}

  public:
    uint32_t m_ShaderType;
    uint32_t m_PrecisionType;
    int m_RangeMin;
    int m_RangeMax;
    int m_Precision;
  };

  class GetErrorCommandBuffer : public CommandBuffer
  {
  public:
    GetErrorCommandBuffer() : CommandBuffer(kCommandTypeGetError) {}
    ~GetErrorCommandBuffer() {}

  public:
    int m_Error;
  };
}
