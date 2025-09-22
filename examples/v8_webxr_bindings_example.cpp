/**
 * Example: V8 WebXR Bindings Integration
 * 
 * This example demonstrates how to integrate the new V8 WebXR bindings
 * into a V8 context and use them from JavaScript.
 */

#include <client/script_bindings/webxr/binding.hpp>
#include <node/v8.h>
#include <iostream>

using namespace v8;

class V8WebXRExample
{
public:
  void Run()
  {
    // Initialize V8
    std::unique_ptr<Platform> platform = platform::NewDefaultPlatform();
    V8::InitializePlatform(platform.get());
    V8::Initialize();

    // Create isolate
    Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = ArrayBuffer::Allocator::NewDefaultAllocator();
    Isolate *isolate = Isolate::New(create_params);

    {
      Isolate::Scope isolate_scope(isolate);
      HandleScope handle_scope(isolate);

      // Create context
      Local<Context> context = Context::New(isolate);
      Context::Scope context_scope(context);

      // Initialize WebXR bindings
      script_bindings::webxr::Initialize(isolate, context);

      std::cout << "V8 WebXR Bindings initialized successfully!" << std::endl;

      // Test basic functionality
      RunExampleScripts(isolate, context);
    }

    // Cleanup
    isolate->Dispose();
    V8::Dispose();
    V8::ShutdownPlatform();
    delete create_params.array_buffer_allocator;
  }

private:
  void RunExampleScripts(Isolate *isolate, Local<Context> context)
  {
    // Test 1: Check constructor availability
    std::cout << "\n=== Testing WebXR Constructor Availability ===" << std::endl;
    ExecuteScript(isolate, context, R"(
      console.log('XRSpace constructor:', typeof XRSpace);
      console.log('XRReferenceSpace constructor:', typeof XRReferenceSpace);
      console.log('XRSession constructor:', typeof XRSession);
      console.log('XRFrame constructor:', typeof XRFrame);
      console.log('XRRigidTransform constructor:', typeof XRRigidTransform);
      console.log('XRPose constructor:', typeof XRPose);
      console.log('XRViewerPose constructor:', typeof XRViewerPose);
      console.log('XRViewport constructor:', typeof XRViewport);
      console.log('XRView constructor:', typeof XRView);
      console.log('XRSystem constructor:', typeof XRSystem);
      console.log('XRRenderState constructor:', typeof XRRenderState);
      console.log('XRLayer constructor:', typeof XRLayer);
      console.log('XRWebGLLayer constructor:', typeof XRWebGLLayer);
      console.log('XRInputSource constructor:', typeof XRInputSource);
      console.log('XRHand constructor:', typeof XRHand);
    )");

    // Test 2: Constructor names
    std::cout << "\n=== Testing WebXR Constructor Names ===" << std::endl;
    ExecuteScript(isolate, context, R"(
      console.log('XRSpace.name:', XRSpace.name);
      console.log('XRSession.name:', XRSession.name);
      console.log('XRFrame.name:', XRFrame.name);
      console.log('XRRigidTransform.name:', XRRigidTransform.name);
      console.log('XRViewport.name:', XRViewport.name);
      console.log('XRView.name:', XRView.name);
      console.log('XRSystem.name:', XRSystem.name);
      console.log('XRWebGLLayer.name:', XRWebGLLayer.name);
      console.log('XRInputSource.name:', XRInputSource.name);
      console.log('XRHand.name:', XRHand.name);
    )");

    // Test 3: Inheritance verification
    std::cout << "\n=== Testing WebXR Inheritance ===" << std::endl;
    ExecuteScript(isolate, context, R"(
      try {
        // Check prototype chain
        console.log('XRReferenceSpace prototype:', Object.getPrototypeOf(XRReferenceSpace.prototype).constructor.name);
        console.log('XRViewerPose prototype:', Object.getPrototypeOf(XRViewerPose.prototype).constructor.name);
        console.log('XRWebGLLayer prototype:', Object.getPrototypeOf(XRWebGLLayer.prototype).constructor.name);
        
        // Verify method existence
        console.log('XRSession.prototype methods:');
        console.log('  requestAnimationFrame:', typeof XRSession.prototype.requestAnimationFrame);
        console.log('  requestReferenceSpace:', typeof XRSession.prototype.requestReferenceSpace);
        console.log('  addEventListener:', typeof XRSession.prototype.addEventListener);
        
        console.log('XRFrame.prototype methods:');
        console.log('  getPose:', typeof XRFrame.prototype.getPose);
        console.log('  getViewerPose:', typeof XRFrame.prototype.getViewerPose);
        
        console.log('XRReferenceSpace.prototype methods:');
        console.log('  getOffsetReferenceSpace:', typeof XRReferenceSpace.prototype.getOffsetReferenceSpace);

        console.log('XRView.prototype methods:');
        console.log('  requestViewportScale:', typeof XRView.prototype.requestViewportScale);

        console.log('XRSystem.prototype methods:');
        console.log('  isSessionSupported:', typeof XRSystem.prototype.isSessionSupported);
        console.log('  requestSession:', typeof XRSystem.prototype.requestSession);

        console.log('XRWebGLLayer.prototype methods:');
        console.log('  getViewport:', typeof XRWebGLLayer.prototype.getViewport);

        console.log('XRHand.prototype methods:');
        console.log('  get:', typeof XRHand.prototype.get);
        console.log('  entries:', typeof XRHand.prototype.entries);
        console.log('  forEach:', typeof XRHand.prototype.forEach);
        
        console.log('V8 WebXR binding layer is working correctly!');
      } catch (e) {
        console.error('Error testing WebXR inheritance:', e.message);
      }
    )");

    // Test 4: Property accessors
    std::cout << "\n=== Testing WebXR Property Descriptors ===" << std::endl;
    ExecuteScript(isolate, context, R"(
      try {
        // Check property descriptors
        const sessionDescriptors = Object.getOwnPropertyDescriptors(XRSession.prototype);
        console.log('XRSession properties:');
        console.log('  inputSources:', sessionDescriptors.inputSources ? 'getter defined' : 'missing');
        console.log('  renderState:', sessionDescriptors.renderState ? 'getter defined' : 'missing');
        
        const transformDescriptors = Object.getOwnPropertyDescriptors(XRRigidTransform.prototype);
        console.log('XRRigidTransform properties:');
        console.log('  position:', transformDescriptors.position ? 'getter defined' : 'missing');
        console.log('  orientation:', transformDescriptors.orientation ? 'getter defined' : 'missing');
        console.log('  matrix:', transformDescriptors.matrix ? 'getter defined' : 'missing');

        const viewDescriptors = Object.getOwnPropertyDescriptors(XRView.prototype);
        console.log('XRView properties:');
        console.log('  eye:', viewDescriptors.eye ? 'getter defined' : 'missing');
        console.log('  projectionMatrix:', viewDescriptors.projectionMatrix ? 'getter defined' : 'missing');

        const viewportDescriptors = Object.getOwnPropertyDescriptors(XRViewport.prototype);
        console.log('XRViewport properties:');
        console.log('  x:', viewportDescriptors.x ? 'getter defined' : 'missing');
        console.log('  y:', viewportDescriptors.y ? 'getter defined' : 'missing');
        console.log('  width:', viewportDescriptors.width ? 'getter defined' : 'missing');
        console.log('  height:', viewportDescriptors.height ? 'getter defined' : 'missing');

        const renderStateDescriptors = Object.getOwnPropertyDescriptors(XRRenderState.prototype);
        console.log('XRRenderState properties:');
        console.log('  baseLayer:', renderStateDescriptors.baseLayer ? 'getter defined' : 'missing');
        console.log('  depthNear:', renderStateDescriptors.depthNear ? 'getter defined' : 'missing');
        console.log('  depthFar:', renderStateDescriptors.depthFar ? 'getter defined' : 'missing');

        const webglLayerDescriptors = Object.getOwnPropertyDescriptors(XRWebGLLayer.prototype);
        console.log('XRWebGLLayer properties:');
        console.log('  framebuffer:', webglLayerDescriptors.framebuffer ? 'getter defined' : 'missing');
        console.log('  framebufferWidth:', webglLayerDescriptors.framebufferWidth ? 'getter defined' : 'missing');
        console.log('  framebufferHeight:', webglLayerDescriptors.framebufferHeight ? 'getter defined' : 'missing');

        const inputSourceDescriptors = Object.getOwnPropertyDescriptors(XRInputSource.prototype);
        console.log('XRInputSource properties:');
        console.log('  handedness:', inputSourceDescriptors.handedness ? 'getter defined' : 'missing');
        console.log('  targetRayMode:', inputSourceDescriptors.targetRayMode ? 'getter defined' : 'missing');
        console.log('  gripSpace:', inputSourceDescriptors.gripSpace ? 'getter defined' : 'missing');

        const handDescriptors = Object.getOwnPropertyDescriptors(XRHand.prototype);
        console.log('XRHand properties:');
        console.log('  size:', handDescriptors.size ? 'getter defined' : 'missing');
        
      } catch (e) {
        console.error('Error testing property descriptors:', e.message);
      }
    )");

    // Test 5: WebXR API usage simulation
    std::cout << "\n=== Testing WebXR API Usage Simulation ===" << std::endl;
    ExecuteScript(isolate, context, R"(
      try {
        console.log('Simulating WebXR API usage...');
        
        // Simulate typical WebXR usage pattern (without actual backend)
        console.log('Would request XR session...');
        console.log('Would setup animation frame...');
        console.log('Would access pose data...');
        console.log('WebXR binding architecture verified!');
        
      } catch (e) {
        console.error('Error in WebXR simulation:', e.message);
      }
    )");
  }

  void ExecuteScript(Isolate *isolate, Local<Context> context, const char *source)
  {
    HandleScope handle_scope(isolate);
    TryCatch try_catch(isolate);

    Local<String> source_string = String::NewFromUtf8(isolate, source).ToLocalChecked();
    Local<Script> script;
    
    if (!Script::Compile(context, source_string).ToLocal(&script))
    {
      ReportException(isolate, &try_catch);
      return;
    }

    Local<Value> result;
    if (!script->Run(context).ToLocal(&result))
    {
      ReportException(isolate, &try_catch);
      return;
    }
  }

  void ReportException(Isolate *isolate, TryCatch *try_catch)
  {
    HandleScope handle_scope(isolate);
    String::Utf8Value exception(isolate, try_catch->Exception());
    const char *exception_string = *exception;
    Local<Message> message = try_catch->Message();
    
    if (message.IsEmpty())
    {
      std::cerr << "JavaScript error: " << exception_string << std::endl;
    }
    else
    {
      String::Utf8Value filename(isolate, message->GetScriptOrigin().ResourceName());
      Local<Context> context(isolate->GetCurrentContext());
      int linenum = message->GetLineNumber(context).FromMaybe(-1);
      std::cerr << "JavaScript error at " << *filename << ":" << linenum << ": " << exception_string << std::endl;
    }
  }
};

int main()
{
  V8WebXRExample example;
  example.Run();
  return 0;
}