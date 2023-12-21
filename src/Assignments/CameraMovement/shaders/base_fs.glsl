#version 430

layout(location=0) out vec4 vFragColor;

layout(std140, binding = 0) uniform Modifier {
    float strength;
    vec3  color_modifier; 
};

in vec3 color;

void main() {
    vFragColor = vec4(color.r * color_modifier.r * strength, color.g * color_modifier.g * strength,  color.b * color_modifier.b * strength, 1.0);
}


