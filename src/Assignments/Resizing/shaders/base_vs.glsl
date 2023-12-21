#version 430

layout(location=0) in  vec4 a_vertex_position;
layout(location=1) in  vec3 a_color;

layout(std140, binding = 1) uniform Transformations {
    mat4 PVM;
};

out vec3 color;

void main() {
    gl_Position = PVM * a_vertex_position;
    color = a_color;
}
