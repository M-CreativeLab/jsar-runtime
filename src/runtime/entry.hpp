#pragma once

#include "math/matrix.hpp"
#include "base.hpp"
#include "logger.hpp"
#include "node_bootstrapper.hpp"

#ifndef TRANSMUTE_STANDALONE
/**
 * @brief It includes the unity-related headers
 */
#include "platform_base.hpp"
#include "renderer/render_api.hpp"
#include <Unity/IUnityGraphics.h>
#include <Unity/IUnityLog.h>
#endif

extern "C"
{
#ifndef TRANSMUTE_STANDALONE
  /**
   * Unity functions
   */
  DLL_PUBLIC void UnityPluginLoad(IUnityInterfaces *unityInterfaces);
  DLL_PUBLIC void UnityPluginUnload();
  DLL_PUBLIC UnityRenderingEvent TransmuteNative_GetRenderEventFunc();

  /**
   * Prepare the transmute native runtime.
   */
  DLL_PUBLIC void TransmuteNative_Prepare();

  /**
   * Initialize the XR device, this is required to execute the JSAR in XR device.
   */
  DLL_PUBLIC void TransmuteNative_InitializeXRDevice(bool enabled);

  /**
   * Fetch the event from the JavaScript side.
   */
  DLL_PUBLIC bool TransmuteNative_GetEventFromJavaScript(int *id, int *type, uint32_t *size);

  /**
   * Fetch the event data from the JavaScript side.
   */
  DLL_PUBLIC void TransmuteNative_GetEventDataFromJavaScript(const char *data);

  /**
   * Check if the JavaScript runtime is up.
   */
  DLL_PUBLIC bool TransmuteNative_IsRuntimeUp();

  /**
   * Check if the JavaScript runtime is available.
   */
  DLL_PUBLIC bool TransmuteNative_IsRuntimeAvailable();

  /**
   * Dispatch the runtime event.
   */
  DLL_PUBLIC void TransmuteNative_DispatchRuntimeEvent(int id);

  /**
   * Dispatch the native event.
   */
  DLL_PUBLIC void TransmuteNative_DispatchNativeEvent(int id, int type, const char *data);

  /**
   * Set the runtime initialization arguments.
   *
   * @param argJson The JSON string of the runtime initialization arguments.
   */
  DLL_PUBLIC void TransmuteNative_SetRuntimeInit(const char *argJson);

  /**
   * Set the viewport size for current rendering.
   *
   * @param w The width of the viewport.
   * @param h The height of the viewport.
   */
  DLL_PUBLIC void TransmuteNative_SetViewport(int w, int h);

  /**
   * Set the field of view for the current rendering.
   *
   * @param fov The field of view to be set.
   */
  DLL_PUBLIC void TransmuteNative_SetFov(float fov);

  /**
   * Set the time for the current rendering.
   *
   * @param time The time to be set.
   */
  DLL_PUBLIC void TransmuteNative_SetTime(float time);

  /**
   * Set the stereo rendering mode such as single-pass, multi-pass, etc.
   *
   * @param mode The stereo rendering mode to be set, the valid value is: MultiPass(0), SinglePass(1), SinglePassInstanced(2),
   *             SinglePassMultiview(3) and -1 for unknown.
   */
  DLL_PUBLIC void TransmuteNative_SetStereoRenderingMode(int mode);

  /**
   * Update the projection matrix for a specific eye.
   *
   * @param eyeId The eye id, 0 for left and 1 for right.
   * @param transform The projection matrix to be set, a valid transform is a 16-element float array that represents a 4x4 matrix,
   *                  and it's in column-major order.
   */
  DLL_PUBLIC bool TransmuteNative_SetViewerStereoProjectionMatrix(int eyeId, float *transform);

  /**
   * Update the viewer's transform matrix, namely the matrix that describes how to transform the viewer's model to the world space.
   * It will be used to calculate the viewer's view matrix.
   *
   * @param translation The translation part of the transform, a 3-element float array.
   * @param rotation The rotation part of the transform, a 4-element float array that represents a quaternion.
   */
  DLL_PUBLIC bool TransmuteNative_SetViewerTransformFromTRS(float *translation, float *rotation);

  /**
   * Update the viewer's stereo view matrix for a specific eye.
   *
   * @param eyeId The eye id, 0 for left and 1 for right.
   * @param translation The translation part of the transform, a 3-element float array.
   */
  DLL_PUBLIC bool TransmuteNative_SetViewerStereoViewMatrixFromTRS(int eyeId, float *translation, float *rotation);

  /**
   * Update the local transform matrix for a specific applet.
   *
   * @param id The applet id, namely the XR session id.
   * @param translation The translation part of the transform, a 3-element float array.
   * @param rotation The rotation part of the transform, a 4-element float array that represents a quaternion.
   */
  DLL_PUBLIC bool TransmuteNative_SetLocalTransformFromTRS(int id, float *translation, float *rotation);

  /**
   * __Input Source Updates__
   *
   * Input source represents the input device, such as the controller, hand, etc, in WebXR and OpenXR, it's called input source.
   * Every input source has a unique id, target ray space, grip space basically, and especially for the hand input source, it has
   * the handness, joint pose, etc.
   *
   * Note: as for the hand input source, we use its handness as its id.
   */

  /**
   * Update the hand input source's pose.
   *
   * @param handness The handness of the hand, 0 for left and 1 for right.
   * @param joint The joint index of the hand, 0 for wrist, 1 for thumb, 2 for index, 3 for middle, 4 for ring, 5 for pinky.
   * @param position The position of the joint, a 3-element float array.
   * @param orientation The orientation of the joint, a 4-element float array that represents a quaternion.
   * @param radius The radius of the joint, a float value.
   */
  DLL_PUBLIC void TransmuteNative_SetHandInputPose(int handness, int joint, float *position, float *orientation, float radius);

  /**
   * Update the target ray pose for the hand input source.
   *
   * @param handness The handness of the hand, 0 for left and 1 for right.
   * @param translation The translation part of the transform, a 3-element float array.
   * @param rotation The rotation part of the transform, a 4-element float array that represents a quaternion.
   */
  DLL_PUBLIC void TransmuteNative_SetHandInputRayPose(int handness, float *translation, float *rotation);

  /**
   * Update the grip pose for the hand input source.
   * 
   * @param handness The handness of the hand, 0 for left and 1 for right.
   * @param translation The translation part of the transform, a 3-element float array.
   * @param rotation The rotation part of the transform, a 4-element float array that represents a quaternion.
   */
  DLL_PUBLIC void TransmuteNative_SetHandInputGripPose(int handness, float *translation, float *rotation);

  /**
   * An action is a special type of event that's triggered by the input source, such as the controller button press, etc. Calling
   * this function will not trigger an event to the client side, it just updates the action state such as pressed, released, etc.
   * Then the client side will fetch the action state in a frame and dispatch the events accordingly.
   * 
   * See https://developer.mozilla.org/en-US/docs/Web/API/WebXR_Device_API/Inputs#actions for more details.
   * 
   * @param handness The handness of the hand, 0 for left and 1 for right.
   * @param action The action type: primary(0), squeeze(1).
   * @param state The action state: pressed(0), released(1).
   */
  DLL_PUBLIC void TransmuteNative_SetHandInputActionState(int handness, int actionType, int state);
#endif
}
