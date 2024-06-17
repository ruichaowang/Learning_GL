#version 330 core
out vec3 FragColor;

in vec2 TexCoords;
in vec4 model_position;

uniform sampler2D camera_texture;
uniform vec2 focal_lengths;
uniform vec2 cammera_principal_point;

void main() {
    float pz = model_position.z;
    if (abs(pz) < 0.00001) {
        pz = 0.1;
    }
    if (model_position.z < 0.0) {
        discard;
    }

    vec2 viewp = vec2(model_position.x/pz, model_position.y/pz);
    vec2 final_point = viewp * focal_lengths + cammera_principal_point;
    FragColor = texture(camera_texture, final_point).rgb;
    // FragColor = texture(material.diffuse, TexCoords).rgb;  // 此行为测试使用
}