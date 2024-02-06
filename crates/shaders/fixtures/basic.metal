// language: metal1.0
#include <metal_stdlib>
#include <simd/simd.h>

using metal::uint;

struct type_2 {
    metal::float4 member;
};

void main_1(
    thread metal::float2& aVertexPosition_1,
    constant metal::float2& uScalingFactor,
    constant metal::float2& uRotationVector,
    thread metal::float4& gl_Position
) {
    metal::float2 rotatedPosition = {};
    metal::float2 _e3 = aVertexPosition_1;
    metal::float2 _e5 = uRotationVector;
    metal::float2 _e8 = aVertexPosition_1;
    metal::float2 _e10 = uRotationVector;
    metal::float2 _e14 = aVertexPosition_1;
    metal::float2 _e16 = uRotationVector;
    metal::float2 _e19 = aVertexPosition_1;
    metal::float2 _e21 = uRotationVector;
    rotatedPosition = metal::float2((_e3.x * _e5.y) + (_e8.y * _e10.x), (_e14.y * _e16.y) - (_e19.x * _e21.x));
    metal::float2 _e28 = rotatedPosition;
    metal::float2 _e29 = uScalingFactor;
    metal::float2 _e30 = _e28 * _e29;
    gl_Position = metal::float4(_e30.x, _e30.y, 0.0, 1.0);
    return;
}

struct main_Input {
    metal::float2 aVertexPosition [[attribute(0)]];
};
struct main_Output {
    metal::float4 member [[position]];
};
vertex main_Output main_(
  main_Input varyings [[stage_in]]
, constant metal::float2& uScalingFactor [[user(fake0)]]
, constant metal::float2& uRotationVector [[user(fake0)]]
) {
    metal::float2 aVertexPosition_1 = {};
    metal::float4 gl_Position = {};
    const auto aVertexPosition = varyings.aVertexPosition;
    aVertexPosition_1 = aVertexPosition;
    main_1(aVertexPosition_1, uScalingFactor, uRotationVector, gl_Position);
    metal::float4 _e9 = gl_Position;
    const auto _tmp = type_2 {_e9};
    return main_Output { _tmp.member };
}
