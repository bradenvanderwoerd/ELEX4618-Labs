#version 410 core

layout (location = 0) in vec3 a_pos;   // Position
layout (location = 1) in vec3 a_color; // Color
layout (location = 2) in vec3 a_normal; // Normal

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 light_space_matrix; // Light's projection * view

out vec3 vertex_color;
out vec3 Normal;
out vec3 FragPos;
out vec4 FragPosLightSpace; // Position in light space

void main() {
    FragPos = vec3(model * vec4(a_pos, 1.0));
    Normal = mat3(transpose(inverse(model))) * a_normal; // Transform normal correctly
    vertex_color = a_color;
    
    // Transform vertex position into light space
    FragPosLightSpace = light_space_matrix * vec4(FragPos, 1.0);
    
    gl_Position = projection * view * model * vec4(a_pos, 1.0);
}