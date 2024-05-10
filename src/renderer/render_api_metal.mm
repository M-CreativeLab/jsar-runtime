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
  ~RenderAPI_Metal() {}

  void ProcessDeviceEvent(UnityGfxDeviceEventType type,
                          IUnityInterfaces *interfaces);
  bool GetUsesReverseZ() { return false; }

  bool SupportsWebGL2() { return false; }
  int GetDrawingBufferWidth() { return m_DrawingBufferWidth; }
  int GetDrawingBufferHeight() { return m_DrawingBufferHeight; }
  void ClearColor(float r, float g, float b, float a) {}
  void ClearDepth(float depth) {}
  void ClearStencil(uint32_t stencil) {}
  void Clear(uint32_t mask) {}
  void Enable(uint32_t cap) {}

  void StartFrame() {}
  void EndFrame() {}

  void StartXRFrame() {}
  void EndXRFrame() {}

  bool ExecuteCommandBuffer() { return true; }
  bool ExecuteCommandBuffer(vector<renderer::CommandBuffer *> &commandBuffers,
                            xr::DeviceFrame *deviceFrame, bool isDefaultQueue) {
    return true;
  }

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
};

RenderAPI *CreateRenderAPI_Metal() { return new RenderAPI_Metal(); }

static Class MTLVertexDescriptorClass;
static Class MTLRenderPipelineDescriptorClass;
static Class MTLDepthStencilDescriptorClass;
// const int kVertexSize = 12 + 4;

RenderAPI_Metal::RenderAPI_Metal() {
  m_VertexDescriptor = [MTLVertexDescriptorClass vertexDescriptor];
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

#endif // #if SUPPORT_METAL
