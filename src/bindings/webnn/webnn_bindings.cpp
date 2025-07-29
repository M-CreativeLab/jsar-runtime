/**
 * WebNN JavaScript bindings for JSAR
 * 
 * This file provides the bridge between JavaScript WebNN API and C++ backend
 */

#include "webnn_backend.hpp"
#include "../base.hpp"
#include <node_api.h>

namespace jsar {
namespace webnn {

// Helper function to convert JS array to std::vector
std::vector<uint32_t> jsArrayToVector(napi_env env, napi_value jsArray) {
    std::vector<uint32_t> result;
    
    uint32_t length;
    if (napi_get_array_length(env, jsArray, &length) != napi_ok) {
        return result;
    }
    
    result.reserve(length);
    for (uint32_t i = 0; i < length; i++) {
        napi_value element;
        uint32_t value;
        if (napi_get_element(env, jsArray, i, &element) == napi_ok &&
            napi_get_value_uint32(env, element, &value) == napi_ok) {
            result.push_back(value);
        }
    }
    
    return result;
}

// Helper function to convert std::vector to JS array
napi_value vectorToJSArray(napi_env env, const std::vector<uint32_t>& vec) {
    napi_value jsArray;
    if (napi_create_array_with_length(env, vec.size(), &jsArray) != napi_ok) {
        return nullptr;
    }
    
    for (size_t i = 0; i < vec.size(); i++) {
        napi_value element;
        if (napi_create_uint32(env, vec[i], &element) == napi_ok) {
            napi_set_element(env, jsArray, i, element);
        }
    }
    
    return jsArray;
}

// Native function: createMLContext
napi_value createMLContextNative(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    
    uint32_t deviceType = 0; // Default to CPU
    if (argc > 0) {
        napi_get_value_uint32(env, args[0], &deviceType);
    }
    
    auto context = createMLContext(static_cast<MLDeviceType>(deviceType));
    
    // For now, return a simple object representing the context
    napi_value result;
    napi_create_object(env, &result);
    
    napi_value deviceTypeValue;
    napi_create_uint32(env, deviceType, &deviceTypeValue);
    napi_set_named_property(env, result, "deviceType", deviceTypeValue);
    
    return result;
}

// Native function: createMLGraphBuilder
napi_value createMLGraphBuilderNative(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    
    // For simplicity, we'll create a graph builder without storing the context reference
    auto context = createMLContext(MLDeviceType::CPU);
    auto builder = createMLGraphBuilder(context);
    
    napi_value result;
    napi_create_object(env, &result);
    
    // Add a property to indicate this is a graph builder
    napi_value typeValue;
    napi_create_string_utf8(env, "MLGraphBuilder", NAPI_AUTO_LENGTH, &typeValue);
    napi_set_named_property(env, result, "type", typeValue);
    
    return result;
}

// Native function: validateMatMulDimensions
napi_value validateMatMulDimensionsNative(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    
    if (argc < 2) {
        napi_value result;
        napi_create_object(env, &result);
        
        napi_value validValue;
        napi_get_boolean(env, false, &validValue);
        napi_set_named_property(env, result, "valid", validValue);
        
        napi_value errorValue;
        napi_create_string_utf8(env, "Insufficient arguments", NAPI_AUTO_LENGTH, &errorValue);
        napi_set_named_property(env, result, "error", errorValue);
        
        return result;
    }
    
    auto dimsA = jsArrayToVector(env, args[0]);
    auto dimsB = jsArrayToVector(env, args[1]);
    
    napi_value result;
    napi_create_object(env, &result);
    
    bool valid = true;
    std::string error;
    
    if (dimsA.size() != 2 || dimsB.size() != 2) {
        valid = false;
        error = "MatMul operands must be 2D tensors";
    } else if (dimsA[1] != dimsB[0]) {
        valid = false;
        error = "MatMul dimension mismatch";
    }
    
    napi_value validValue;
    napi_get_boolean(env, valid, &validValue);
    napi_set_named_property(env, result, "valid", validValue);
    
    if (!valid) {
        napi_value errorValue;
        napi_create_string_utf8(env, error.c_str(), NAPI_AUTO_LENGTH, &errorValue);
        napi_set_named_property(env, result, "error", errorValue);
    } else {
        // Return result dimensions
        std::vector<uint32_t> resultDims = {dimsA[0], dimsB[1]};
        napi_value resultDimsValue = vectorToJSArray(env, resultDims);
        napi_set_named_property(env, result, "resultDimensions", resultDimsValue);
    }
    
    return result;
}

// Module initialization
napi_value Init(napi_env env, napi_value exports) {
    napi_value createMLContextFn, createMLGraphBuilderFn, validateMatMulDimensionsFn;
    
    napi_create_function(env, nullptr, 0, createMLContextNative, nullptr, &createMLContextFn);
    napi_create_function(env, nullptr, 0, createMLGraphBuilderNative, nullptr, &createMLGraphBuilderFn);
    napi_create_function(env, nullptr, 0, validateMatMulDimensionsNative, nullptr, &validateMatMulDimensionsFn);
    
    napi_set_named_property(env, exports, "createMLContext", createMLContextFn);
    napi_set_named_property(env, exports, "createMLGraphBuilder", createMLGraphBuilderFn);
    napi_set_named_property(env, exports, "validateMatMulDimensions", validateMatMulDimensionsFn);
    
    return exports;
}

NAPI_MODULE(webnn, Init)

} // namespace webnn
} // namespace jsar