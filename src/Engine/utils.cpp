//
// Created by Piotr Białas on 13/12/2021.
//

#include "utils.h"


void uniform_block_binding(GLuint program, const std::string &name, GLuint binding) {
    auto u_index = glGetUniformBlockIndex(program, name.c_str());
    if (u_index != -1) {
        glUniformBlockBinding(program, u_index, binding);
    }
}

