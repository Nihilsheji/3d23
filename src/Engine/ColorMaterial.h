//
// Created by Piotr Białas on 02/11/2021.
//

#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

#include "Application/utils.h"
#include "Material.h"

namespace xe {
    class ColorMaterial : public Material {
    public:
        ColorMaterial(const glm::vec4 color, GLuint texture, GLuint texture_unit) : Kd_(color), texture_(texture),
                                                                                    texture_unit_(texture_unit) {}
        ColorMaterial(const glm::vec4 color) : Kd_(color) {}

        void bind();

        void unbind();

        static void init();

        static GLuint program() { return shader_; }

        void set_texture(GLuint tex) { texture_ = tex; }

        void set_texture_unit(GLuint tex_unit) { texture_unit_ = tex_unit; }

        GLuint get_texture() { return texture_; };

        GLuint get_texture_unit() { return texture_unit_; };

    private:
        static GLuint shader_;
        static GLuint color_uniform_buffer_;
        static GLint uniform_map_Kd_location_;

        GLuint texture_ = 0;
        GLuint texture_unit_ = 0;
        glm::vec4 Kd_;
    };
}