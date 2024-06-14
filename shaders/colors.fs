#version 330 core
out vec3 FragColor;

struct Material {
  sampler2D diffuse;
  vec3 specular;
  float shininess;
};
in vec2 TexCoords;
in vec4 model_position;

uniform Material material;
uniform vec2 focal_lengths;
uniform vec2 cammera_principal_point;

void main() {
  FragColor = texture(material.diffuse, TexCoords).rgb;

  // 这部分是贴图的逻辑
    float pz = model_position.z;
    if (abs(pz) < 0.00001) {
        pz = 0.1;
    }
    vec2 viewp = vec2(model_position.x/pz, model_position.y/pz);
    vec2 final_point = viewp * focal_lengths + cammera_principal_point;
    // final_point.x = 1.0 - final_point.x;
    // final_point.y = 1.0 - final_point.y;
    FragColor = texture(material.diffuse, final_point).rgb;
}