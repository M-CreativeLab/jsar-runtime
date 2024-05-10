#pragma once

#include <string>
#include <map>
#include "common.hpp"

class OpenGLTextureBinding
{
public:
	OpenGLTextureBinding(GLenum target, GLuint texture) : m_Target(target), m_Texture(texture) {}
	inline void Reset(GLenum target, GLuint texture)
	{
		m_Target = target;
		m_Texture = texture;
	}
	inline GLenum GetTarget() { return m_Target; }
	inline GLint GetTexture() { return m_Texture; }

public:
	GLenum m_Target;
	GLuint m_Texture;
};

class OpenGLContextStorage
{
public:
	OpenGLContextStorage(std::string name) : m_Name(name)
  {
  }
  ~OpenGLContextStorage()
	{
		ClearTextureBindings();
	}

  void RecordViewport(int x, int y, int w, int h);
  void RecordProgram(int program);
  void RecordArrayBuffer(int buffer);
  void RecordElementArrayBuffer(int buffer);
  void RecordFramebuffer(int buffer);
  void RecordRenderbuffer(int buffer);
  void RecordVertexArrayObject(int vao);
  void RecordActiveTextureUnit(int unit);
  void RecordTextureBindingWithUnit(GLenum target, GLuint texture);

  const char *GetName() { return m_Name.c_str(); }
  GLint GetProgram() { return m_ProgramId; }
  GLint GetArrayBuffer() { return m_ArrayBufferId; }
  GLint GetElementArrayBuffer() { return m_ElementArrayBufferId; }
  GLint GetFramebuffer() { return m_FramebufferId; }
  GLint GetRenderbuffer() { return m_RenderbufferId; }
  GLint GetVertexArrayObject() { return m_VertexArrayObjectId; }
  GLenum GetActiveTextureUnit() { return m_LastActiveTextureUnit; }

  void ResetProgram(int programToReset);

void Restore();
  void Print();
  void ClearTextureBindings();

protected:
  std::string m_Name;
  GLint m_Viewport[4] = {-1, -1, -1, -1};
  /** Program */
  GLint m_ProgramId = 0;
  /** Buffers */
  GLint m_ArrayBufferId = 0;
  GLint m_ElementArrayBufferId = 0;
  GLint m_FramebufferId = 0;
  GLint m_RenderbufferId = 0;
  /** Vertex Array Object */
  GLint m_VertexArrayObjectId = 0;
  /** Textures */
  GLenum m_LastActiveTextureUnit = GL_TEXTURE0;
  std::map<GLenum, OpenGLTextureBinding *> m_TextureBindingsWithUnit;
};

class OpenGLHostContextStorage : public OpenGLContextStorage
{
public:
	OpenGLHostContextStorage() : OpenGLContextStorage("Host") {}

public:
	void Restore();
	void Record();
	void RecordTextureBindingFromHost();

private:
	bool m_CullFaceEnabled;
	GLenum m_CullFace;
	GLenum m_FrontFace;
};
