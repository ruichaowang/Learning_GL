#version 330 core

in vec3 FragPos;
in vec4 model_position;
out vec3 FragColor;

uniform sampler2D camera_texture;
uniform vec2 focal_lengths;
uniform vec2 camera_principal_point;
uniform int debug_discard;
uniform int debug_mesh;

void main() {
  float pz = model_position.z;
  if (abs(pz) < 0.00001) {
    pz = 0.1;
  }

  vec2 viewp = vec2(model_position.x / pz, model_position.y / pz);
  vec2 final_point = viewp * focal_lengths + camera_principal_point;

  if (debug_mesh == 1) {
    FragColor = vec3(0.5, 0.5, 0.1);
  } else {
    if (model_position.z < 0.0) {
      discard;
    }

    if (final_point.x < 0 || final_point.y < 0 || final_point.x > 1 ||
        final_point.y > 1) {
      discard;
    }
    FragColor = texture(camera_texture, final_point).rgb;
  }
}