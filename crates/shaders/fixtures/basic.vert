in vec2 aVertexPosition;
layout(binding = 0) uniform vec2 uScalingFactor;
layout(binding = 1) uniform vec2 uRotationVector;

void main() {
  vec2 rotatedPosition = vec2(
    aVertexPosition.x * uRotationVector.y +
    aVertexPosition.y * uRotationVector.x,
    aVertexPosition.y * uRotationVector.y -
    aVertexPosition.x * uRotationVector.x
  );
  gl_Position = vec4(rotatedPosition * uScalingFactor, 0.0, 1.0);
}