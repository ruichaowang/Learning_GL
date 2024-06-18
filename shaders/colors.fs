#version 330 core
out vec3 FragColor;

in vec2 TexCoords;
in vec4 model_position;

uniform sampler2D camera_texture;
uniform vec2 focal_lengths;
uniform vec2 cammera_principal_point;
uniform int debug_discard;

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
    
    if (debug_discard == 1) {
        if (final_point.x < 0 ||  final_point.y < 0) {
            discard;
        }
        if (final_point.x > 1 || final_point.y > 1) {
            discard;
        }
    }
    FragColor = texture(camera_texture, final_point).rgb;
    // FragColor = texture(camera_texture, TexCoords).rgb;  // 此行为测试使用
}