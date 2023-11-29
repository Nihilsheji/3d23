#version 410

layout(location=0) out vec4 vFragColor;

in vec3 color;

void main() {
    vFragColor = vec4(color, 1.0);
}
