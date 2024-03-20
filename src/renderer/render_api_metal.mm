#include "crates/jsar_shaders.h"
#include "render_api.hpp"
#include "runtime/platform_base.hpp"

// Metal implementation of RenderAPI.

#if SUPPORT_METAL

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#include <Unity/IUnityGraphicsMetal.h>
#include <Unity/IUnityLog.h>
#import <simd/simd.h>
#include <vector>

typedef struct {
  matrix_float4x4 worldMatrix;
} AppData;

@interface MTLProgramObject : NSObject
@property(nonatomic) bool linked;
@property(nonatomic, strong) MTLRenderPipelineDescriptor *descriptor;
@property(nonatomic, strong) id<MTLRenderPipelineState> pipeline;
@end

@interface MTLShaderObject : NSObject
@property(nonatomic) int type;
@property(nonatomic, strong) id<MTLLibrary> library;
@property(nonatomic, strong) id<MTLFunction> function;
@end

@interface MTLBufferObject : NSObject
@property(nonatomic) int size;
@property(nonatomic) int target;
@property(nonatomic, strong) id<MTLBuffer> buffer;
@end

@implementation MTLProgramObject
- (instancetype)init {
  self = [super init];
  if (self) {
    self.linked = false;
    self.descriptor = nil;
    self.pipeline = nil;
  }
  return self;
}
@end

@implementation MTLShaderObject
- (instancetype)initWithType:(int)type {
  self = [super init];
  if (self) {
    self.type = type;
    self.library = nil;
    self.function = nil;
  }
  return self;
}
@end

@implementation MTLBufferObject
- (instancetype)initWithSize:(int)size metalDevice:(id<MTLDevice>)metalDevice {
  self = [super init];
  if (self) {
#if UNITY_OSX
    MTLResourceOptions options =
        MTLResourceCPUCacheModeDefaultCache | MTLResourceStorageModeManaged;
#else
    MTLResourceOptions options = MTLResourceOptionCPUCacheModeDefault;
#endif
    self.size = size;
    self.buffer = [metalDevice newBufferWithLength:size options:options];
  }
  return self;
}
@end

class RenderAPI_Metal : public RenderAPI {
public:
  RenderAPI_Metal();
  virtual ~RenderAPI_Metal() {}

  virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type,
                                  IUnityInterfaces *interfaces);
  virtual bool GetUsesReverseZ() { return true; }
  virtual void DrawSimpleTriangles(const float worldMatrix[16],
                                   int triangleCount,
                                   const void *verticesFloat3Byte4);
  virtual int GetDrawingBufferWidth();
  virtual int GetDrawingBufferHeight();
  virtual int CreateProgram();
  virtual void LinkProgram(int program);
  virtual void UseProgram(int program);
  virtual void AttachShader(int program, int shader);
  virtual void DetachShader(int program, int shader);
  virtual int CreateShader(int type);
  virtual void DeleteShader(int shader);
  virtual void ShaderSource(int shader, const char *source, uint32_t length);
  virtual void CompileShader(int shader);
  virtual int CreateBuffer();
  virtual void BindBuffer(int target, int buffer);
  virtual void EnableVertexAttribArray(int index);
  virtual void VertexAttribPointer(int index, int size, int type,
                                   bool normalized, int stride,
                                   const void *offset);
  virtual void DrawArrays(int mode, int first, int count);
  virtual void DrawElements(int mode, int count, int type, const void *indices);
  virtual void ClearColor(float r, float g, float b, float a);
  virtual void ClearDepth(float depth);
  virtual void ClearStencil(uint32_t stencil);
  virtual void Clear(uint32_t mask);
  virtual void Enable(uint32_t cap);

  virtual void StartFrame();
  virtual void EndFrame();
	void ExecuteCommandBuffer();

private:
  id<MTLRenderCommandEncoder> CreateCommandEncoder();

private:
  IUnityLog *m_UnityLog;
  IUnityGraphicsMetal *m_MetalGraphics;

  MTLRenderPassDescriptor *m_RunPassDescriptor = nil;
  MTLVertexDescriptor *m_VertexDescriptor = nil;
  id<MTLRenderCommandEncoder> m_CurrentCommandEncoder = nil;
  std::vector<MTLShaderObject *> m_Shaders;
  std::vector<MTLProgramObject *> m_Programs;
  std::vector<MTLBufferObject *> m_Buffers;
  id<MTLRenderPipelineState> m_PipelineStateInNextFrame = nil;

  // state
  int m_DrawingBufferWidth;
  int m_DrawingBufferHeight;
  MTLClearColor m_ClearColor;
  double m_ClearDepth = 0.0;
  uint32_t m_ClearStencil = 0;

  // opengl buffers
  id<MTLBuffer> m_VertexBuffer;
  id<MTLBuffer> m_IndexBuffer;
  id<MTLBuffer> m_ConstantBuffer;
  AppData m_AppData;

  id<MTLDepthStencilState> m_DepthStencil;
  id<MTLRenderPipelineState> m_Pipeline;

  simd_float4x4 m_ModelMatrix;
  simd_float4x4 m_ViewMatrix;
  simd_float4x4 m_ProjectionMatrix;
};

RenderAPI *CreateRenderAPI_Metal() { return new RenderAPI_Metal(); }

static Class MTLVertexDescriptorClass;
static Class MTLRenderPipelineDescriptorClass;
static Class MTLDepthStencilDescriptorClass;
// const int kVertexSize = 12 + 4;

RenderAPI_Metal::RenderAPI_Metal() {
  m_VertexDescriptor = [MTLVertexDescriptorClass vertexDescriptor];
}

void RenderAPI_Metal::ExecuteCommandBuffer() {
  // TODO
}

id<MTLRenderCommandEncoder> RenderAPI_Metal::CreateCommandEncoder() {
  if (m_CurrentCommandEncoder != nil) {
    return m_CurrentCommandEncoder;
  }
  id<MTLCommandBuffer> commandBuffer =
      (id<MTLCommandBuffer>)m_MetalGraphics->CurrentCommandBuffer();
  m_CurrentCommandEncoder =
      [commandBuffer renderCommandEncoderWithDescriptor:m_RunPassDescriptor];

  // Set the states
  if (m_PipelineStateInNextFrame != nil)
    [m_CurrentCommandEncoder setRenderPipelineState:m_PipelineStateInNextFrame];

  // Bind buffers
  if (m_VertexBuffer != nil)
    [m_CurrentCommandEncoder setVertexBuffer:m_VertexBuffer offset:0 atIndex:1];

  return m_CurrentCommandEncoder;
}

void RenderAPI_Metal::ProcessDeviceEvent(UnityGfxDeviceEventType type,
                                         IUnityInterfaces *interfaces) {
  if (type == kUnityGfxDeviceEventInitialize) {
    m_UnityLog = interfaces->Get<IUnityLog>();
    m_MetalGraphics = interfaces->Get<IUnityGraphicsMetal>();
    MTLVertexDescriptorClass = NSClassFromString(@"MTLVertexDescriptor");
    MTLRenderPipelineDescriptorClass =
        NSClassFromString(@"MTLRenderPipelineDescriptor");
    MTLDepthStencilDescriptorClass =
        NSClassFromString(@"MTLDepthStencilDescriptor");
  } else if (type == kUnityGfxDeviceEventShutdown) {
    //@TODO: release resources
  }
}

int RenderAPI_Metal::GetDrawingBufferWidth() { return m_DrawingBufferWidth; }

int RenderAPI_Metal::GetDrawingBufferHeight() { return m_DrawingBufferHeight; }

int RenderAPI_Metal::CreateProgram() {
  MTLProgramObject *programObject = [[MTLProgramObject alloc] init];
  MTLRenderPipelineDescriptor *descriptor =
      [[MTLRenderPipelineDescriptorClass alloc] init];
  programObject.descriptor = descriptor;
  m_Programs.push_back(programObject);
  return m_Programs.size() - 1;
}

void RenderAPI_Metal::LinkProgram(int program) {
  MTLProgramObject *programObject = m_Programs[program];
  if (programObject.linked) {
    ::fprintf(stderr, "Program already linked\n");
    return;
  }
  if (programObject.descriptor.vertexFunction == nil) {
    ::fprintf(stderr, "Program has no vertex shader\n");
    return;
  }
  if (programObject.descriptor.fragmentFunction == nil) {
    ::fprintf(stderr, "Program has no fragment shader\n");
    return;
  }
  programObject.linked = true;
}

void RenderAPI_Metal::UseProgram(int program) {
  MTLProgramObject *programObject = m_Programs[program];
  if (programObject.linked == false) {
    ::fprintf(stderr, "Program not linked\n");
    return;
  }

  NSError *error = nil;
  programObject.descriptor.vertexDescriptor = m_VertexDescriptor;
  programObject.pipeline = [m_MetalGraphics->MetalDevice()
      newRenderPipelineStateWithDescriptor:programObject.descriptor
                                     error:&error];
  if (error != nil) {
    NSString *desc = [error localizedDescription];
    NSString *reason = [error localizedFailureReason];
    ::fprintf(stderr, "%s\n%s\n\n", desc ? [desc UTF8String] : "<unknown>",
              reason ? [reason UTF8String] : "");
  }
  m_PipelineStateInNextFrame = programObject.pipeline;
}

void RenderAPI_Metal::AttachShader(int program, int shader) {
  MTLProgramObject *programObject = m_Programs[program];
  MTLShaderObject *shaderObject = m_Shaders[shader];
  if (programObject == nil) {
    ::fprintf(stderr, "Invalid program id\n");
    return;
  }
  if (shaderObject == nil) {
    ::fprintf(stderr, "Invalid shader id\n");
    return;
  }
  if (shaderObject.function == nil) {
    ::fprintf(stderr, "Shader not compiled\n");
    return;
  }
  if (shaderObject.type == WEBGL_VERTEX_SHADER) {
    programObject.descriptor.vertexFunction = shaderObject.function;
  } else if (shaderObject.type == WEBGL_FRAGMENT_SHADER) {
    programObject.descriptor.fragmentFunction = shaderObject.function;
  } else {
    ::fprintf(stderr, "Invalid shader type\n");
  }
}

void RenderAPI_Metal::DetachShader(int program, int shader) {
  // Nothing to do
  // TODO: add shader functions to program and implement the state validation?
}

int RenderAPI_Metal::CreateShader(int type) {
  MTLShaderObject *shaderObject = [[MTLShaderObject alloc] initWithType:type];
  m_Shaders.push_back(shaderObject);
  return m_Shaders.size() - 1;
}

void RenderAPI_Metal::DeleteShader(int shader) {
  // TODO: release shader resources
}

void RenderAPI_Metal::ShaderSource(int shaderId, const char *source,
                                   uint32_t length) {
  MTLShaderObject *shaderObject = m_Shaders[shaderId];
  if (shaderObject == nil) {
    ::fprintf(stderr, "Invalid shader id\n");
    return;
  }
  if (shaderObject.library != nil) {
    ::fprintf(stderr, "Shader already has source\n");
    return;
  }

  const uint8_t *mslSource;
  size_t mslSize = 0;
  int code = glsl_to_msl(shaderObject.type, (const uint8_t *)source, length,
                         &mslSource, &mslSize);
  if (code != 0) {
    ::fprintf(stderr, "Failed to convert GLSL to MSL\n");
    return;
  }
  NSString *srcStr = [[NSString alloc] initWithBytes:mslSource
                                              length:mslSize
                                            encoding:NSASCIIStringEncoding];
  NSError *error = nil;
  shaderObject.library =
      [m_MetalGraphics->MetalDevice() newLibraryWithSource:srcStr
                                                   options:nil
                                                     error:&error];
  delete[] mslSource;

  if (error != nil) {
    NSString *desc = [error localizedDescription];
    NSString *reason = [error localizedFailureReason];
    ::fprintf(stderr, "%s\n%s\n\n", desc ? [desc UTF8String] : "<unknown>",
              reason ? [reason UTF8String] : "");
  }
}

void RenderAPI_Metal::CompileShader(int shader) {
  MTLShaderObject *shaderObject = m_Shaders[shader];
  if (shaderObject == nil) {
    ::fprintf(stderr, "Invalid shader id\n");
    return;
  }
  if (shaderObject.library == nil) {
    ::fprintf(stderr, "Shader has no source\n");
    return;
  }
  shaderObject.function = [shaderObject.library newFunctionWithName:@"main_"];
  if (shaderObject.function == nil) {
    ::fprintf(stderr, "Failed to compile shader\n");
  }
}

int RenderAPI_Metal::CreateBuffer() {
  id<MTLDevice> metalDevice = m_MetalGraphics->MetalDevice();
  MTLBufferObject *bufferObject =
      [[MTLBufferObject alloc] initWithSize:2048 metalDevice:metalDevice];
  m_Buffers.push_back(bufferObject);
  return m_Buffers.size() - 1;
}

void RenderAPI_Metal::BindBuffer(int target, int buffer) {
  MTLBufferObject *bufferObject = m_Buffers[buffer];
  if (bufferObject == nil) {
    ::fprintf(stderr, "Invalid buffer id\n");
    return;
  }
  if (bufferObject.buffer == nil) {
    ::fprintf(stderr, "Buffer not created\n");
    return;
  }
  if (target == WEBGL_ARRAY_BUFFER) {
    m_VertexBuffer = bufferObject.buffer;
  } else if (target == WEBGL_ELEMENT_ARRAY_BUFFER) {
    m_IndexBuffer = bufferObject.buffer;
  }
}

void RenderAPI_Metal::EnableVertexAttribArray(int index) {
  // Do nothing
}

void RenderAPI_Metal::VertexAttribPointer(int index, int size, int type,
                                          bool normalized, int stride,
                                          const void *offset) {
  switch (type) {
  case WEBGL_BYTE:
    m_VertexDescriptor.attributes[index].format = MTLVertexFormatChar2;
    break;
  case WEBGL_UNSIGNED_BYTE:
    m_VertexDescriptor.attributes[index].format = MTLVertexFormatUChar2;
    break;
  case WEBGL_SHORT:
    m_VertexDescriptor.attributes[index].format = MTLVertexFormatShort2;
    break;
  case WEBGL_UNSIGNED_SHORT:
    m_VertexDescriptor.attributes[index].format = MTLVertexFormatUShort2;
    break;
  case WEBGL_FLOAT:
    m_VertexDescriptor.attributes[index].format = MTLVertexFormatFloat2;
    break;
  default:
    break;
  }
  m_VertexDescriptor.layouts[index].stride = stride;
  m_VertexDescriptor.layouts[index].stepFunction =
      MTLVertexStepFunctionPerVertex;
  m_VertexDescriptor.layouts[index].stepRate = 1;
}

void RenderAPI_Metal::DrawArrays(int mode, int first, int count) {
  CreateCommandEncoder();

  MTLPrimitiveType type;
  switch (mode) {
  case WEBGL_POINTS:
    type = MTLPrimitiveTypePoint;
    break;
  case WEBGL_LINES:
    type = MTLPrimitiveTypeLine;
    break;
  case WEBGL_LINE_STRIP:
    type = MTLPrimitiveTypeLineStrip;
    break;
  case WEBGL_TRIANGLES:
    type = MTLPrimitiveTypeTriangle;
    break;
  case WEBGL_TRIANGLE_STRIP:
    type = MTLPrimitiveTypeTriangleStrip;
    break;
  default:
    break;
  }

  if (m_VertexBuffer == nil) {
    ::fprintf(stderr, "No vertex buffer bound\n");
    return;
  }
  [m_CurrentCommandEncoder drawPrimitives:type
                              vertexStart:first
                              vertexCount:count];
}

void RenderAPI_Metal::DrawElements(int mode, int count, int type,
                                   const void *indices) {
  // CreateCommandEncoder();
  // [m_CurrentCommandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
  //                                     indexCount:count
  //                                      indexType:MTLIndexTypeUInt16
  //                                    indexBuffer:m_VertexBuffer
  //                              indexBufferOffset:0];
}

void RenderAPI_Metal::ClearColor(float r, float g, float b, float a) {
  m_ClearColor = MTLClearColorMake(r, g, b, a);
}

void RenderAPI_Metal::ClearDepth(float depth) { m_ClearDepth = (double)depth; }

void RenderAPI_Metal::ClearStencil(uint32_t stencil) {
  m_ClearStencil = stencil;
}

void RenderAPI_Metal::Clear(uint32_t mask) {
  if (mask & WEBGL_COLOR_BUFFER_BIT) {
    m_RunPassDescriptor.colorAttachments[0].clearColor = m_ClearColor;
    m_RunPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
    m_RunPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
  }
  if (mask & WEBGL_DEPTH_BUFFER_BIT) {
    m_RunPassDescriptor.depthAttachment.clearDepth = m_ClearDepth;
    m_RunPassDescriptor.depthAttachment.loadAction = MTLLoadActionClear;
  }
  if (mask & WEBGL_STENCIL_BUFFER_BIT) {
    m_RunPassDescriptor.stencilAttachment.clearStencil = m_ClearStencil;
    m_RunPassDescriptor.stencilAttachment.loadAction = MTLLoadActionClear;
  }
}

void RenderAPI_Metal::Enable(uint32_t cap) {
  // TODO: implement
}

void RenderAPI_Metal::StartFrame() {
  m_RunPassDescriptor = m_MetalGraphics->CurrentRenderPassDescriptor();
  // Save the states in the original render pass descriptor
  m_DrawingBufferWidth =
      static_cast<int>(m_RunPassDescriptor.colorAttachments[0].texture.width);
  m_DrawingBufferHeight =
      static_cast<int>(m_RunPassDescriptor.colorAttachments[0].texture.height);

  // Set default clear color
  m_ClearColor = MTLClearColorMake(0.0, 0.0, 0.0, 1.0);
  m_MetalGraphics->EndCurrentCommandEncoder();
}

void RenderAPI_Metal::EndFrame() {
  CreateCommandEncoder();
  [m_CurrentCommandEncoder endEncoding];
  m_CurrentCommandEncoder = nil;
}

simd_float4x4 convertToMatrix(float *source_array) {
  return (matrix_float4x4){
      .columns[0] = {source_array[0], source_array[1], source_array[2],
                     source_array[3]},
      .columns[1] = {source_array[4], source_array[5], source_array[6],
                     source_array[7]},
      .columns[2] = {source_array[8], source_array[9], source_array[10],
                     source_array[11]},
      .columns[3] = {source_array[12], source_array[13], source_array[14],
                     source_array[15]},
  };
}

void RenderAPI_Metal::DrawSimpleTriangles(const float worldMatrix[16],
                                          int triangleCount,
                                          const void *verticesFloat3Byte4) {
  MTLRenderPassDescriptor *renderPass =
      m_MetalGraphics->CurrentRenderPassDescriptor();
  m_MetalGraphics->EndCurrentCommandEncoder();

  // Update vertex and constant buffers
  //@TODO: we don't do any synchronization here :)

  //   const int vbSize = triangleCount * 3 * kVertexSize;
  //   const int cbSize = sizeof(AppData);
  //   m_AppData.worldMatrix = convertToMatrix((float *)worldMatrix);

  //   ::memcpy(m_VertexBuffer.contents, verticesFloat3Byte4, vbSize);
  //   // ::memcpy(m_ConstantBuffer.contents, worldMatrix, cbSize);
  //   ::memcpy(m_ConstantBuffer.contents, &m_AppData, cbSize);

  // #if UNITY_OSX
  //   [m_VertexBuffer didModifyRange:NSMakeRange(0, vbSize)];
  //   [m_ConstantBuffer didModifyRange:NSMakeRange(0, cbSize)];
  // #endif

  // id<MTLRenderCommandEncoder> cmd =
  //     (id<MTLRenderCommandEncoder>)m_MetalGraphics->CurrentCommandEncoder();
  renderPass.colorAttachments[0].clearColor = MTLClearColorMake(1, 0, 0, 1);
  renderPass.colorAttachments[0].loadAction = MTLLoadActionClear;
  renderPass.colorAttachments[0].storeAction = MTLStoreActionStore;

  id<MTLCommandBuffer> commandBuffer =
      (id<MTLCommandBuffer>)m_MetalGraphics->CurrentCommandBuffer();
  id<MTLRenderCommandEncoder> cmd =
      [commandBuffer renderCommandEncoderWithDescriptor:renderPass];

  // Setup rendering state
  // [cmd setRenderPipelineState:m_Pipeline];
  // [cmd setDepthStencilState:m_DepthStencil];
  // [cmd setCullMode:MTLCullModeNone];

  // // Bind buffers
  // [cmd setVertexBuffer:m_VertexBuffer offset:0 atIndex:1];
  // [cmd setVertexBuffer:m_ConstantBuffer offset:0 atIndex:0];

  // // Draw
  // [cmd drawPrimitives:MTLPrimitiveTypeTriangle
  //         vertexStart:0
  //         vertexCount:triangleCount * 3];
  [cmd endEncoding];
}

#endif // #if SUPPORT_METAL
