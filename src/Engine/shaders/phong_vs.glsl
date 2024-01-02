#version 430

layout(location=0) in vec4 a_vertex_position;
layout(location=1) in vec2 a_vertex_texcoords;
layout(location=2) in vec2 a_vertex_texcoords_1;
layout(location=3) in vec2 a_vertex_texcoords_2;
layout(location=4) in vec2 a_vertex_texcoords_3;
layout(location=5) in vec3 a_vertex_normal;

layout(std140, binding = 1) uniform Transformations {
    mat4 PVM;
};

layout(std140, binding=2) uniform Matrices {
    mat4 VM;
    mat3 N;
};

out vec4 vertex_position;
out vec3 vertex_normal;
out vec2 vertex_texcoords;
out vec3 vertex_coords_in_viewspace;
out vec3 vertex_normal_in_viewspace;

void main() {
    vertex_normal = a_vertex_normal;
    vertex_position = a_vertex_position;
    vertex_texcoords = a_vertex_texcoords;

    vec4 vertex_coords_in_viewspace4 = VM * a_vertex_position;
    vertex_coords_in_viewspace = vertex_coords_in_viewspace4.xyz/vertex_coords_in_viewspace4.w;
    vertex_normal_in_viewspace = normalize(N * a_vertex_normal);
    gl_Position =  PVM * a_vertex_position;
}
