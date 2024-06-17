#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aInstancePos; // 实例位置

out vec3 FragPos;
out vec2 TexCoords;
out vec4 model_position;

uniform vec3 position;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 extrinsic_matrix;

void main() {
  FragPos = aPos + aInstancePos;
  //   FragPos = aPos + position;
  TexCoords = aTexCoords;
  gl_Position = projection * view * vec4(FragPos, 1.0);

  model_position = extrinsic_matrix * vec4(FragPos, 1.0);
  model_position.xyz = model_position.xyz / model_position.w;
}