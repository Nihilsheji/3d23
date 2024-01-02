#version 430

#define MAX_POINT_LIGHTS 24

struct PointLight {
    vec3 position_in_world_space;
    vec3 position_in_view_space;
    vec3 color;
    vec3 atn;
};

layout(location=0) out vec4 vFragColor;

layout(std140, binding = 0) uniform Material {
    vec4 Kd;
    vec3 Ks;
    vec4 Ka;
    bool use_map_Kd;
    float Ns;
} modifier;

layout(std140, binding=3) uniform Light {
    PointLight light[MAX_POINT_LIGHTS];
    int num_lights;
} p_light;

uniform sampler2D map_Kd;

uniform vec3 ambient_light;

in vec4 vertex_position;
in vec3 vertex_normal;
in vec2 vertex_texcoords;
in vec3 vertex_coords_in_viewspace;
in vec3 vertex_normal_in_viewspace;

void main() {
    float Ns = modifier.Ns;
    vec3 Ks = modifier.Ks;
    vec4 Kd = modifier.Kd;

    if (modifier.use_map_Kd) {
        Kd *= texture(map_Kd, vertex_texcoords);
    }

    vec3 normal = normalize(vertex_normal);
    vec3 diffuseLight;
    vec3 specularLight;

    for(int i = 0; i < p_light.num_lights; ++i)
    {
        PointLight light = p_light.light[i];

        // Diffuse
        vec3 lightDirection = normalize(light.position_in_world_space - vertex_position.xyz);
        float diffuse = max(dot(normal, lightDirection), 0.0f);
        diffuseLight += diffuse * light.color;

        // Specular
        vec3 viewDirection = -normalize(vertex_coords_in_viewspace);
        vec3 reflectDirection = reflect(-lightDirection, normal);
        float specular = pow(max(dot(viewDirection, reflectDirection), 0.0f), Ns);
        specularLight += specular * Ks;
    }

    vFragColor.a = Kd.a;
    vFragColor.rgb = Kd.rgb * (diffuseLight + ambient_light + specularLight);
}