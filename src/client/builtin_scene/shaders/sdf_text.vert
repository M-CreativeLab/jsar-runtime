uniform mat4 viewProjection;
uniform mat4 model;

in vec3 position;
in vec2 uv;

out vec2 vUv;

void main() {
  vUv = uv;
  gl_Position = viewProjection * model * vec4(position, 1.0);
}