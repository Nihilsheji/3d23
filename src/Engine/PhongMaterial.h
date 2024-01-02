//
// Created by Piotr Białas on 20/11/2021.
//

#pragma once

#include "Material.h"

#include <string>

namespace xe {
    class PhongMaterial : public Material {
    public:

        static void init();

        static GLuint program() { return shader_; }
        PhongMaterial(const glm::vec4 color, const glm::vec3 ambient, const glm::vec3 specular, float specularStrength) : Kd_(color), Ka_(ambient), Ks_(specular), Ns(specularStrength) {}

        PhongMaterial(const glm::vec4 color, const glm::vec3 ambient, const glm::vec3 specular, float specularStrength, GLuint texture, GLuint texture_unit) : Kd_(color), map_Kd_(texture),
                                                                                    map_Kd_unit_(texture_unit) {}

        PhongMaterial(const glm::vec4 color, GLuint texture, GLuint texture_unit) : Kd_(color), map_Kd_(texture),
                                                                                    map_Kd_unit_(texture_unit) {}

        PhongMaterial(const glm::vec4 color, GLuint texture) : PhongMaterial(color, texture, 0) {}

        PhongMaterial(const glm::vec4 color) : PhongMaterial(color, 0) {}

        void set_texture(GLuint tex) { map_Kd_ = tex; }

        static void set_ambient(const glm::vec3 ambient) {
            glUseProgram(program());
            glUniform3f(uniform_ambient_location_, ambient.x, ambient.y, ambient.z);
            glUseProgram(0);
        }

        void bind() override;

        void unbind() override;


    private:

        static GLuint shader_;
        static GLuint material_uniform_buffer_;
        static GLint uniform_map_Kd_location_;
        static GLint uniform_ambient_location_;

        glm::vec4 Kd_;
        GLuint map_Kd_ = 0;
        GLboolean use_map_Kd_ = 0;
        GLuint map_Kd_unit_ = 0;

        glm::vec3 Ka_;
        glm::vec3 Ks_;

        float Ns;
    };


    GLuint create_texture(const std::string &name);

}




