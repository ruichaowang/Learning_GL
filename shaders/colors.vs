#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 model_position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 extrinsic_matrix;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));

    Normal = mat3(transpose(inverse(model))) * aNormal;  // 可以删掉
    TexCoords = aTexCoords;                              // 这个也删掉，没用
    
    gl_Position = projection * view * vec4(FragPos, 1.0);

    model_position = extrinsic_matrix * vec4(aPos, 1.0);
}