//
// Created by pbialas on 25.09.2020.
//

#include "app.h"

#include <iostream>
#include <vector>
#include <tuple>

#include "Application/utils.h"
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"

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

    //Create each side separately for different colors

    std::vector<GLfloat> vertices = {
           -0.5f, 0.0f, -0.5f, 1.0f, 0.0f, 0.0f,   //0
           -0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f,    //1
           0.5f, 0.0f, -0.5f, 1.0f, 0.0f, 0.0f,    //2
           0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f,     //3

           -0.5f, 0.f, -0.5f, 0.0f, 1.0f, 0.0f,    //4
           0.f, 1.f, 0.f, 0.0f, 1.0f, 0.0f,        //5
           0.5f, 0.f, -0.5f, 0.0f, 1.0f, 0.0f,     //6

           0.5f, 0.f, -0.5f, 0.0f, 0.0f, 1.0f,     //7
           0.f, 1.f, 0.f, 0.0f, 0.0f, 1.0f,        //8
           0.5f, 0.f, 0.5f, 0.0f, 0.0f, 1.0f,      //9

           -0.5f, 0.f, 0.5f, 1.0f, 1.0f, 0.0f,     //10
           0.f, 1.f, 0.f, 1.0f, 1.0f, 0.0f,        //11
           0.5f, 0.f, 0.5f, 1.0f, 1.0f, 0.0f,      //12

           -0.5f, 0.f, 0.5f, 1.0f, 0.0f, 1.0f,     //13
           0.f, 1.f, 0.f, 1.0f, 0.0f, 1.0f,        //14
           -0.5f, 0.f, -0.5f, 1.0f, 0.0f, 1.0f,    //15
    };

    indices = {
            0, 2, 1,
            1, 2, 3,
            4, 5, 6,
            7, 8, 9,
            11, 10, 12,
            13, 14, 15,
    };

    float strength = 1.0;
    std::vector<GLfloat> color = { 0.9, 0.8, 0.7 };

    auto [w, h] = frame_buffer_size();
    aspect_ = (float)w / h;
    fov_ = glm::pi<float>() / 4.0;
    near_ = 0.1f;
    far_ = 100.0f;

    V_ = glm::lookAt(
        glm::vec3(5.0f, 5.0f, 5.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    P_ = glm::perspective(fov_, aspect_, near_, far_);
    M_ = glm::mat4(1.0f);

    GLuint indicesBuffer, verticesBuffer, uniformBuffer;

    glGenBuffers(1, &indicesBuffer);
    OGL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer));
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenBuffers(1, &verticesBuffer);
    OGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer));
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &uniformBuffer);
    OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer));
    glBufferData(GL_UNIFORM_BUFFER, 8 * sizeof(float), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float), &strength);
    glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(float), 3 * sizeof(float), color.data());
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glGenBuffers(1, &PVM);
    OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, PVM));
    glBufferData(GL_UNIFORM_BUFFER, 16 * sizeof(float), nullptr, GL_STATIC_DRAW);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // This setups a Vertex Array Object (VAO) that  encapsulates
    // the state of all vertex buffers needed for rendering
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniformBuffer);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, PVM);

    // and this specifies how the data is layout in the buffer.
    glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, PVM);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniformBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    //end of vao "recording"

    // Setting the background color of the rendering window,
    // I suggest not to use white or black for better debuging.
    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    // This setups an OpenGL vieport of the size of the whole rendering window.
    glViewport(0, 0, w, h);

    glUseProgram(program);
}

//This functions is called every frame and does the actual rendering.
void SimpleShapeApplication::frame() {
    auto pvm = P_ * V_ * M_;
    glBindBuffer(GL_UNIFORM_BUFFER, PVM);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &pvm[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Binding the VAO will setup all the required vertex buffers.
    glBindVertexArray(vao_);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, reinterpret_cast<GLvoid *>(0));
    glBindVertexArray(0);
}

void SimpleShapeApplication::framebuffer_resize_callback(int w, int h) {
    Application::framebuffer_resize_callback(w, h);
    glViewport(0, 0, w, h);
    aspect_ = (float)w / h;
    P_ = glm::perspective(fov_, aspect_, near_, far_);
}
