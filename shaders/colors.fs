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
    vec3 oclr = texture(material.diffuse, final_point).rgb;
    FragColor = oclr;

    // FragColor = vec3(cammera_principal_point,0.0);  //测试参数是否正确

  //   FragColor = vec4(0.0, 1.0, 1.0); // 青色 (Cyan)，无光照效果,
  // 但是轮廓没有了，
}