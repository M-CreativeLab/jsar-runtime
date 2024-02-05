#include "render_api.hpp"
#include "runtime/platform_base.hpp"

// Metal implementation of RenderAPI.

#if SUPPORT_METAL

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#include <Unity/IUnityGraphicsMetal.h>
#include <Unity/IUnityLog.h>
#import <simd/simd.h>

typedef struct {
  matrix_float4x4 worldMatrix;
} AppData;

@interface MTLProgramObject : NSObject
@property(nonatomic) bool linked;
@property(nonatomic, strong) MTLRenderPipelineDescriptor *descriptor;
@property(nonatomic, strong) id<MTLRenderPipelineState> pipeline;
@end

@interface MTLShaderObject : NSObject
@property(nonatomic, strong) id<MTLLibrary> library;
@property(nonatomic, strong) id<MTLFunction> function;
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
- (instancetype)init {
  self = [super init];
  if (self) {
    self.library = nil;
    self.function = nil;
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
  virtual void ShaderSource(int shader, const char *source);
  virtual void CompileShader(int shader);
  virtual int CreateBuffer();
  virtual void BindBuffer(int target, int buffer);
  virtual void EnableVertexAttribArray(int index);
  virtual void VertexAttribPointer(int index, int size, int type,
                                   bool normalized, int stride,
                                   const void *offset);
  virtual void DrawArrays(int mode, int first, int count);
  virtual void DrawElements(int mode, int count, int type, const void *indices);
  virtual void SetViewport(int x, int y, int w, int h);
  virtual void SetScissor(int x, int y, int w, int h);
  virtual void ClearColor(float r, float g, float b, float a);
  virtual void ClearDepth(float depth);
  virtual void ClearStencil(uint32_t stencil);
  virtual void Clear(uint32_t mask);
  virtual void Enable(uint32_t cap);

  virtual void StartFrame();
  virtual void EndFrame();

private:
  void CreateResources();
  id<MTLRenderCommandEncoder> CreateCommandEncoder();

private:
  IUnityLog *m_UnityLog;
  IUnityGraphicsMetal *m_MetalGraphics;

  MTLRenderPassDescriptor *m_RunPassDescriptor;
  id<MTLRenderCommandEncoder> m_CurrentCommandEncoder = nil;
  NSMutableArray<MTLShaderObject *> *m_Shaders = [NSMutableArray array];
  NSMutableArray<MTLProgramObject *> *m_Programs = [NSMutableArray array];
  id<MTLRenderPipelineState> m_PipelineStateInNextFrame = nil;

  // state
  int m_DrawingBufferWidth;
  int m_DrawingBufferHeight;
  MTLClearColor m_ClearColor;
  double m_ClearDepth = 0.0;
  uint32_t m_ClearStencil = 0;

  id<MTLBuffer> m_VertexBuffer;
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
const int kVertexSize = 12 + 4;
// Simple vertex & fragment shader source
static const char kShaderSource[] =
    "#include <metal_stdlib>\n"
    "using namespace metal;\n"
    "struct AppData\n"
    "{\n"
    "    float4x4 worldMatrix;\n"
    "};\n"
    "struct Vertex\n"
    "{\n"
    "    float3 pos [[attribute(0)]];\n"
    "    float4 color [[attribute(1)]];\n"
    "};\n"
    "struct VSOutput\n"
    "{\n"
    "    float4 pos [[position]];\n"
    "    half4  color;\n"
    "};\n"
    "struct FSOutput\n"
    "{\n"
    "    half4 frag_data [[color(0)]];\n"
    "};\n"
    "vertex VSOutput vertexMain(Vertex input [[stage_in]], constant AppData& "
    "my_cb [[buffer(0)]])\n"
    "{\n"
    "    VSOutput out = { my_cb.worldMatrix * float4(input.pos.xyz, 1), "
    "(half4)input.color };\n"
    "    return out;\n"
    "}\n"
    "fragment FSOutput fragmentMain(VSOutput input [[stage_in]])\n"
    "{\n"
    "    FSOutput out = { input.color };\n"
    "    return out;\n"
    "}\n";

RenderAPI_Metal::RenderAPI_Metal() {}

void RenderAPI_Metal::CreateResources() {
  id<MTLDevice> metalDevice = m_MetalGraphics->MetalDevice();
  NSError *error = nil;

  // Create shaders
  NSString *srcStr = [[NSString alloc] initWithBytes:kShaderSource
                                              length:sizeof(kShaderSource)
                                            encoding:NSASCIIStringEncoding];
  id<MTLLibrary> shaderLibrary = [metalDevice newLibraryWithSource:srcStr
                                                           options:nil
                                                             error:&error];
  if (error != nil) {
    NSString *desc = [error localizedDescription];
    NSString *reason = [error localizedFailureReason];
    ::fprintf(stderr, "%s\n%s\n\n", desc ? [desc UTF8String] : "<unknown>",
              reason ? [reason UTF8String] : "");
  }

  id<MTLFunction> vertexFunction =
      [shaderLibrary newFunctionWithName:@"vertexMain"];
  id<MTLFunction> fragmentFunction =
      [shaderLibrary newFunctionWithName:@"fragmentMain"];

  // Vertex / Constant buffers
#if UNITY_OSX
  MTLResourceOptions bufferOptions =
      MTLResourceCPUCacheModeDefaultCache | MTLResourceStorageModeManaged;
#else
  MTLResourceOptions bufferOptions = MTLResourceOptionCPUCacheModeDefault;
#endif

  m_VertexBuffer = [metalDevice newBufferWithLength:1024 options:bufferOptions];
  m_VertexBuffer.label = @"PluginVB";
  m_ConstantBuffer = [metalDevice newBufferWithLength:sizeof(AppData)
                                              options:bufferOptions];
  m_ConstantBuffer.label = @"PluginCB";

  // Vertex layout
  MTLVertexDescriptor *vertexDesc = [MTLVertexDescriptorClass vertexDescriptor];
  vertexDesc.attributes[0].format = MTLVertexFormatFloat3;
  vertexDesc.attributes[0].offset = 0;
  vertexDesc.attributes[0].bufferIndex = 1;
  vertexDesc.attributes[1].format = MTLVertexFormatUChar4Normalized;
  vertexDesc.attributes[1].offset = 3 * sizeof(float);
  vertexDesc.attributes[1].bufferIndex = 1;
  vertexDesc.layouts[1].stride = kVertexSize;
  vertexDesc.layouts[1].stepFunction = MTLVertexStepFunctionPerVertex;
  vertexDesc.layouts[1].stepRate = 1;

  // Pipeline
  MTLRenderPipelineDescriptor *pipeDesc =
      [[MTLRenderPipelineDescriptorClass alloc] init];
  // Let's assume we're rendering into BGRA8Unorm...
  pipeDesc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;

  pipeDesc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
  pipeDesc.stencilAttachmentPixelFormat = MTLPixelFormatDepth32Float_Stencil8;

  pipeDesc.sampleCount = 1;
  pipeDesc.colorAttachments[0].blendingEnabled = NO;

  pipeDesc.vertexFunction = vertexFunction;
  pipeDesc.fragmentFunction = fragmentFunction;
  pipeDesc.vertexDescriptor = vertexDesc;

  m_Pipeline = [metalDevice newRenderPipelineStateWithDescriptor:pipeDesc
                                                           error:&error];
  if (error != nil) {
    ::fprintf(stderr, "Metal: Error creating pipeline state: %s\n%s\n",
              [[error localizedDescription] UTF8String],
              [[error localizedFailureReason] UTF8String]);
    error = nil;
  }

  // Depth/Stencil state
  MTLDepthStencilDescriptor *depthDesc =
      [[MTLDepthStencilDescriptorClass alloc] init];
  depthDesc.depthCompareFunction = GetUsesReverseZ()
                                       ? MTLCompareFunctionGreaterEqual
                                       : MTLCompareFunctionLessEqual;
  depthDesc.depthWriteEnabled = false;
  m_DepthStencil = [metalDevice newDepthStencilStateWithDescriptor:depthDesc];
}

id<MTLRenderCommandEncoder> RenderAPI_Metal::CreateCommandEncoder() {
  if (m_CurrentCommandEncoder != nil) {
    return m_CurrentCommandEncoder;
  }
  id<MTLCommandBuffer> commandBuffer =
      (id<MTLCommandBuffer>)m_MetalGraphics->CurrentCommandBuffer();
  m_CurrentCommandEncoder =
      [commandBuffer renderCommandEncoderWithDescriptor:m_RunPassDescriptor];
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
    CreateResources();
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
  [m_Programs addObject:programObject];
  return (int)[m_Programs count] - 1;
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

  NSError *error = nil;
  programObject.pipeline = [m_MetalGraphics->MetalDevice()
      newRenderPipelineStateWithDescriptor:programObject.descriptor
                                     error:&error];
  if (error != nil) {
    NSString *desc = [error localizedDescription];
    NSString *reason = [error localizedFailureReason];
    ::fprintf(stderr, "%s\n%s\n\n", desc ? [desc UTF8String] : "<unknown>",
              reason ? [reason UTF8String] : "");
  }
  programObject.linked = true;
}

void RenderAPI_Metal::UseProgram(int program) {
  MTLProgramObject *programObject = m_Programs[program];
  if (programObject.pipeline == nil) {
    ::fprintf(stderr, "Program not linked\n");
    return;
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
  if (programObject.descriptor.vertexFunction == nil) {
    programObject.descriptor.vertexFunction = shaderObject.function;
  } else if (programObject.descriptor.fragmentFunction == nil) {
    programObject.descriptor.fragmentFunction = shaderObject.function;
  } else {
    ::fprintf(stderr, "Program already has vertex and fragment shaders\n");
  }
}

void RenderAPI_Metal::DetachShader(int program, int shader) {
  // Nothing to do
  // TODO: add shader functions to program and implement the state validation?
}

int RenderAPI_Metal::CreateShader(int type) {
  MTLShaderObject *shaderObject = [[MTLShaderObject alloc] init];
  [m_Shaders addObject:shaderObject];
  return (int)[m_Shaders count] - 1;
}

void RenderAPI_Metal::DeleteShader(int shader) {
  // TODO: release shader resources
}

void RenderAPI_Metal::ShaderSource(int shaderId, const char *source) {
  MTLShaderObject *shaderObject = m_Shaders[shaderId];
  if (shaderObject == nil) {
    ::fprintf(stderr, "Invalid shader id\n");
    return;
  }
  if (shaderObject.library != nil) {
    ::fprintf(stderr, "Shader already has source\n");
    return;
  }
  NSString *srcStr = [[NSString alloc] initWithBytes:source
                                              length:strlen(source)
                                            encoding:NSASCIIStringEncoding];
  NSError *error = nil;
  shaderObject.library =
      [m_MetalGraphics->MetalDevice() newLibraryWithSource:srcStr
                                                   options:nil
                                                     error:&error];
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
  shaderObject.function = [shaderObject.library newFunctionWithName:@"main"];
  if (shaderObject.function == nil) {
    ::fprintf(stderr, "Failed to compile shader\n");
  }
}

int RenderAPI_Metal::CreateBuffer() { return 0; }

void RenderAPI_Metal::BindBuffer(int target, int buffer) {
  // Do nothing
}

void RenderAPI_Metal::EnableVertexAttribArray(int index) {
  // Do nothing
}

void RenderAPI_Metal::VertexAttribPointer(int index, int size, int type,
                                          bool normalized, int stride,
                                          const void *offset) {
  // Do nothing
}

void RenderAPI_Metal::DrawArrays(int mode, int first, int count) {
  CreateCommandEncoder();
  [m_CurrentCommandEncoder drawPrimitives:MTLPrimitiveTypeTriangle
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

void RenderAPI_Metal::SetViewport(int x, int y, int w, int h) {
  CreateCommandEncoder();
  [m_CurrentCommandEncoder
      setViewport:(MTLViewport){(double)x, (double)y, (double)w, (double)h, 0.0,
                                1.0}];
}

void RenderAPI_Metal::SetScissor(int x, int y, int w, int h) {
  MTLScissorRect scissorRect = {(NSUInteger)x, (NSUInteger)y, (NSUInteger)w,
                                (NSUInteger)h};
  CreateCommandEncoder();
  [m_CurrentCommandEncoder setScissorRect:scissorRect];
}

void RenderAPI_Metal::ClearColor(float r, float g, float b, float a) {
  m_ClearColor = MTLClearColorMake(r, g, b, a);
}

void RenderAPI_Metal::ClearDepth(float depth) { m_ClearDepth = (double)depth; }

void RenderAPI_Metal::ClearStencil(uint32_t stencil) {
  m_ClearStencil = stencil;
}

void RenderAPI_Metal::Clear(uint32_t mask) {
  if (mask & COLOR_BUFFER_BIT) {
    m_RunPassDescriptor.colorAttachments[0].clearColor = m_ClearColor;
    m_RunPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
    m_RunPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
  }
  if (mask & DEPTH_BUFFER_BIT) {
    m_RunPassDescriptor.depthAttachment.clearDepth = m_ClearDepth;
    m_RunPassDescriptor.depthAttachment.loadAction = MTLLoadActionClear;
  }
  if (mask & STENCIL_BUFFER_BIT) {
    m_RunPassDescriptor.stencilAttachment.clearStencil = m_ClearStencil;
    m_RunPassDescriptor.stencilAttachment.loadAction = MTLLoadActionClear;
  }
}

void RenderAPI_Metal::Enable(uint32_t cap) {
  switch (cap) {
  case BLEND:
    // Do nothing
    break;
  case SCISSOR_TEST:
    // Do nothing
    break;
  default:
    break;
  }
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
