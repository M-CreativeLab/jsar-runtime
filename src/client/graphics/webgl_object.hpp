#pragma once
#include <map>
#include "idgen.hpp"

namespace client_graphics
{
  enum class WebGLObjectType
  {
    Buffer,
    Framebuffer,
    Program,
    Shader,
    Texture,
    Renderbuffer,
    VertexArray,
    Query,
    Sampler,
    TransformFeedback,
    UniformBuffer,
    StorageBuffer,
    FenceSync,
    Path,
    None
  };

  class WebGLObject
  {
  public:
    /**
     * Create a new `WebGLObject` with auto-generated id.
     * 
     * @param type The type of the `WebGLObject`.
     */
    WebGLObject(WebGLObjectType type)
        : type(type)
    {
      static TrIdGenerator idGen(0x10);
      id = idGen.get();
    }
    /**
     * Create a new `WebGLObject` with the given id.
     * 
     * @param type The type of the `WebGLObject`.
     * @param id The id of the `WebGLObject`.
     */
    WebGLObject(WebGLObjectType type, int id)
        : type(type)
        , id(id)
    {
    }

  public:
    /**
     * Mark this `WebGLObject` to be deleted, and it should not be used anymore.
     */
    void markDeleted()
    {
      deleted_ = true;
    }
    /**
     * @returns if this `WebGLObject` is marked to be deleted.
     */
    bool isDeleted()
    {
      return deleted_;
    }
    /**
     * @returns if this `WebGLObject` is valid.
     */
    bool isValid()
    {
      return !deleted_;
    }

  public:
    int id;
    WebGLObjectType type;

  protected:
    bool deleted_ = false;
  };
}
