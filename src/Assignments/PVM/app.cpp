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

float strength = 0.9f;
float color[3] = { 0.0f, 0.5f, 0.5f };
float theta = 1.0 * glm::pi<float>() / 6.0f;
auto cs = std::cos(theta);
auto ss = std::sin(theta);
glm::mat2 rot{cs, ss, -ss, cs};
glm::vec2 trans{0.0, -0.25};
glm::vec2 scale{0.5, 0.5};
glm::mat4 PVM;

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
            -0.5f, 0.0f, 0.0f, 0.8f, 0.5f, 0.1f,
            0.0f, 0.5f, 0.0f, 0.8f, 0.5f, 0.3f,
            0.5f, 0.0f, 0.0f, 0.9f, 0.5f, 0.2f,
            0.5f, -0.5f, 0.0f, 0.9f, 0.5f, 0.2f,
            -0.5f, -0.5f, 0.0f, 0.6f, 0.5f, 0.1f
    };

    GLuint indices[] = {
        0, 1, 2, 
        0, 2, 3, 
        0, 3, 4
    };

    // Set up Projeciton - View - Model matrices
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(-3.0f, 3.0f, 4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::perspective(45.0f, 650.0f / 480.0f, 0.1f, 100.0f);

    PVM = projection * view * model;

    // Generating the buffer and loading the vertex data into it.
    GLuint v_buffer_handle, ebo, ubo, transformations_ubo;
    glGenBuffers(1, &v_buffer_handle);
    glGenBuffers(1, &ebo);
    glGenBuffers(1, &ubo);
    glGenBuffers(1, &transformations_ubo);
    OGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle));
    
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, 12 * sizeof(float), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 2 * sizeof(float), &strength);
    glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(float), 8 * sizeof(float), &color);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBuffer(GL_UNIFORM_BUFFER, transformations_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), &PVM, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // This setups a Vertex Array Object (VAO) that  encapsulates
    // the state of all vertex buffers needed for rendering
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, transformations_ubo);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);

    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, transformations_ubo);

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
    glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
