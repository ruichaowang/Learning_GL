#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
in vec4 model_position;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform vec2 focal_lengths;
uniform vec2 cammera_principal_point;

void main()
{
    // 此方案是原始的
    //ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
  	
    //diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;  
    
    //specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);  
        
    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);


    // 这部分是贴图的逻辑
    // float pz = model_position.z;
    // if (abs(pz) < 0.00001) {
    //     pz = 1.0;
    // }
    // vec2 viewp = vec2(model_position.x/pz, model_position.y/pz);
    // vec2 final_point = viewp * focal_lengths + cammera_principal_point;
    // vec3 oclr = texture(material.diffuse, final_point).rgb;
    // FragColor = vec4(oclr, 1.0);

    // FragColor = vec4(0.0, 1.0, 1.0, 0.2); // 青色 (Cyan)，无光照效果, 但是轮廓没有了，
} 