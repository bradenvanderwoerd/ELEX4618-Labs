#version 410
layout (location = 0) in vec3 aPos;  // Position
layout (location = 1) in vec3 aColor; // Color

uniform mat4 mvp;

out vec3 vertexColor; // Pass to fragment shader

void main() {
    gl_Position = mvp * vec4(aPos, 1.0);
    vertexColor = aColor; // Pass color to fragment shader
}