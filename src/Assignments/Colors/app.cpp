//
// Created by pbialas on 25.09.2020.
//

#include "app.h"

#include <iostream>
#include <vector>
#include <tuple>

#include "Application/utils.h"

void SimpleShapeApplication::init() {
    // A utility function that reads the shader sources, compiles them and creates the program object
    // As everything in OpenGL we reference program by an integer "handle".
    auto program = xe::utils::create_program(
            {{GL_VERTEX_SHADER,   std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
             {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"}});

    if (!program) {
        std::cerr << "Invalid program" << std::endl;
        exit(-1);
    }

    // A vector containing the x,y,z vertex coordinates for the triangle and square.
    std::vector<GLfloat> vertices = {
            -0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 0.1f,
            0.5f, 0.0f, 0.0f, 0.0f, 0.8f, 0.2f,
            0.0f, 0.5f, 0.0f, 0.0f, 0.1f, 0.3f,
            -0.5f, 0.0f, 0.0f, 0.0f, 0.8f, 0.4f,
            0.5f, 0.0f, 0.0f, 0.0f, 0.7f, 0.3f,
            0.5f, -0.5f, 0.0f, 0.0f, 0.6f, 0.2f,
            -0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.1f
    };

    // Generating the buffer and loading the vertex data into it.
    GLuint v_buffer_handle;
    glGenBuffers(1, &v_buffer_handle);
    OGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle));
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // This setups a Vertex Array Object (VAO) that  encapsulates
    // the state of all vertex buffers needed for rendering
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);

    // and this specifies how the data is layout in the buffer.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    //end of vao "recording"

    // Setting the background color of the rendering window,
    // I suggest not to use white or black for better debuging.
    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    // This setups an OpenGL vieport of the size of the whole rendering window.
    auto[w, h] = frame_buffer_size();
    glViewport(0, 0, w, h);

    glUseProgram(program);
}

//This functions is called every frame and does the actual rendering.
void SimpleShapeApplication::frame() {
    // Binding the VAO will setup all the required vertex buffers.
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawArrays(GL_TRIANGLE_FAN, 3, 4);
    glBindVertexArray(0);
}
