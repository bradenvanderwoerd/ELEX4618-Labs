#version 410 core

in vec3 vertex_color;
in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;

out vec4 FragColor;

uniform vec3 light_pos;
uniform vec3 view_pos; // Camera position
uniform vec3 light_color;
uniform float shininess;         // Glossiness factor
uniform float specular_strength;  // Strength of specular reflection
uniform sampler2D shadow_map;    // Shadow map texture

float calculate_shadow(vec4 fragPosLightSpace) {
    // Perform perspective divide to get [0,1] NDC coordinates
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; // Transform from [-1,1] to [0,1]

    // Get closest depth from shadow map
    float closestDepth = texture(shadow_map, projCoords.xy).r;
    float currentDepth = projCoords.z; // Depth of the fragment

    // Apply bias to reduce shadow acne
    float bias = max(0.05 * (1.0 - dot(Normal, normalize(light_pos - FragPos))), 0.005);
    
    // Shadow factor (1.0 = fully lit, 0.0 = fully shadowed)
    float shadow = (currentDepth - bias) > closestDepth ? 0.0 : 1.0;
    
    return shadow;
}

void main() {
    // Ambient lighting
    float ambient_strength = 0.2;
    vec3 ambient = ambient_strength * light_color;

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 light_dir = normalize(light_pos - FragPos);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * light_color;

    // Specular lighting
    vec3 view_dir = normalize(view_pos - FragPos);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
    vec3 specular = specular_strength * spec * light_color;

    // Calculate shadow
    float shadow = calculate_shadow(FragPosLightSpace);

    // Apply shadow factor
    vec3 result = (ambient + (diffuse + specular) * shadow) * vertex_color;
    //FragColor = vec4(result, 1.0);
    FragColor = vec4(vec3(texture(shadow_map, FragPosLightSpace.xy).r), 1.0);
}