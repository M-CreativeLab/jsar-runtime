#pragma once

#include <atomic>
#include <mutex>
#include <condition_variable>

using namespace std;

namespace renderer
{
  enum CommandType
  {
    /** Program */
    kCommandTypeCreateProgram,
    kCommandTypeLinkProgram,
    kCommandTypeUseProgram,
    /** Shader */
    kCommandTypeAttachShader,
    kCommandTypeDetachShader,
    kCommandTypeCreateShader,
    kCommandTypeDeleteShader,
    kCommandTypeShaderSource,
    kCommandTypeCompileShader,
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
    kCommandTypeDepthFunc,
    kCommandTypeEnable,
    kCommandTypeDisable,
    kCommandTypeGetBooleanv,
    kCommandTypeGetIntegerv,
    kCommandTypeGetFloatv,
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
  };

  class UseProgramCommandBuffer : public CommandBuffer
  {
  public:
    UseProgramCommandBuffer(int id) : CommandBuffer(kCommandTypeUseProgram), m_ProgramId(id) {}
    ~UseProgramCommandBuffer() {}

  public:
    int m_ProgramId;
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

  class DepthFuncCommandBuffer : public CommandBuffer
  {
  public:
    DepthFuncCommandBuffer(int func) : CommandBuffer(kCommandTypeDepthFunc), m_Func(func) {}
    ~DepthFuncCommandBuffer() {}

  public:
    int m_Func;
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
}
