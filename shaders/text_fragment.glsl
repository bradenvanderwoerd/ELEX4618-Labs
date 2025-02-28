#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D text;
uniform vec3 textColor;

void main() {
    float alpha = texture(text, TexCoords).r;  // Read the red channel
    FragColor = vec4(textColor, alpha); // Use alpha for transparency
}