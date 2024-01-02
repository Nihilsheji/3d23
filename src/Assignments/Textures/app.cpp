//
// Created by pbialas on 25.09.2020.
// Ponieważ nie mogłem sprawić żeby zaczęło działać wraz z instrukcją zaimplementowałem rozwiązanie bez użycia klas wymienionych w instrukcji
//
#define STB_IMAGE_IMPLEMENTATION  1

#include "3rdParty/stb/stb_image.h"
#include "app.h"
#include "camera.h"

#include <iostream>
#include <vector>
#include <tuple>

#include "Application/utils.h"
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

struct ModifierBlock {
    glm::vec4 Kd;
    GLboolean use_map_Kd;
};

void SimpleShapeApplication::init() {
    // A utility function that reads the shader sources, compiles them and creates the program object
    // As everything in OpenGL we reference program by an integer "handle".
    auto program = xe::utils::create_program(
        { {GL_VERTEX_SHADER,   std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
         {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"} });

    if (!program) {
        std::cerr << "Invalid program" << std::endl;
        exit(-1);
    }

    std::vector<GLfloat> vertices = {
            // F
            -0.5f, -0.5f, -0.5f,    0.191f, 0.5f,
            0.0f, 0.5f, 0.0f,       0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,     0.5f, 0.809f,
            // L
            -0.5f, -0.5f, 0.5f,     0.5f, 0.809f,
            0.0f, 0.5f, 0.0f,       1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,    0.809f, 0.5f,
            // R
            0.5f, -0.5f, -0.5f,     0.5f, 0.191f,
            0.0f, 0.5f, 0.0f,       0.0f, 0.0f,
            0.5f, -0.5f, 0.5f,      0.191f, 0.5f,
            // B
            0.5f, -0.5f, 0.5f,      0.809f, 0.5f,
            0.0f, 0.5f, 0.0f,       1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f,     0.5f, 0.191f,
            // D1
            -0.5f, -0.5f, -0.5f,    0.191f, 0.5f,
            0.5f, -0.5f, -0.5f,     0.5f, 0.809f,
            -0.5f, -0.5f, 0.5f,     0.5f, 0.191f,
            // D2
            0.5f, -0.5f, 0.5f,      0.809f, 0.5f,
            -0.5f, -0.5f, 0.5f,     0.5f, 0.809f,
            0.5f, -0.5f, -0.5f,     0.5f, 0.191f,
    };

    indices = {
            0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17
    };

    set_camera(new Camera());
    set_controler(new CameraControler(camera()));

    auto [w, h] = frame_buffer_size();

    auto aspect_ = (float)w / h;
    auto fov_ = glm::pi<float>() / 4.0;
    auto near_ = 0.1f;
    auto far_ = 100.0f;

    camera_->look_at(
        glm::vec3(5.0f, 5.0f, 5.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    camera_->perspective(fov_, aspect_, near_, far_);

    M_ = glm::mat4(1.0f);

    GLuint indicesBuffer, verticesBuffer, uniformBuffer;

    glUseProgram(program);

    glGenBuffers(1, &indicesBuffer);
    OGL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer));
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenBuffers(1, &verticesBuffer);
    OGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer));
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &PVM);
    OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, PVM));
    glBufferData(GL_UNIFORM_BUFFER, 16 * sizeof(float), nullptr, GL_STATIC_DRAW);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Load texture
    stbi_set_flip_vertically_on_load(true);
    GLint width, height, channels;
    auto texture_file = std::string(ROOT_DIR) + "/Models/multicolor.png";
    auto img = stbi_load(texture_file.c_str(), &width, &height, &channels, 0);

    // Setup texture
    GLenum format;

    if (channels == 3)
        format = GL_RGB;
    else if (channels == 4) {
        format = GL_RGBA;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0); // Set active texture
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, img);

    glBindTexture(GL_TEXTURE_2D, 0);

    glUseProgram(program);

    // Set value for sampler uniform
    uniform_map_Kd_location_ = glGetUniformLocation(program, "map_Kd");// Sets the value of the uniform

    ModifierBlock modifierData;
    modifierData.Kd = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // Example data
    modifierData.use_map_Kd = GL_TRUE;

    if (texture > 0) {
        OGL_CALL(glUniform1i(uniform_map_Kd_location_, 0)); // Bind texture to slot number 0
        modifierData.use_map_Kd = GL_TRUE;
    }

    //Setup textures
    glGenBuffers(1, &color_uniform_buffer_); // Generate buffer

    glBindBuffer(GL_UNIFORM_BUFFER, color_uniform_buffer_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(ModifierBlock), &modifierData, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // This setups a Vertex Array Object (VAO) that  encapsulates
    // the state of all vertex buffers needed for rendering
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    // and this specifies how the data is layout in the buffer.
    glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer);

    // Setup attrib pointer for vertice coordinates
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(0));
    glEnableVertexAttribArray(0);

    // Setup attrib pointer for texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);

    // Bind uniform buffers
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, color_uniform_buffer_);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, PVM);

    glBindBuffer(GL_UNIFORM_BUFFER, PVM);
    glBindBuffer(GL_UNIFORM_BUFFER, color_uniform_buffer_);

    glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
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
    auto pvm = camera_->projection() * camera_->view() * glm::mat4(1.0f);
    glBindBuffer(GL_UNIFORM_BUFFER, PVM);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &pvm[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Binding the VAO will setup all the required vertex buffers.
    glBindVertexArray(vao_);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, reinterpret_cast<GLvoid*>(0));
    glBindVertexArray(0);
}

void SimpleShapeApplication::framebuffer_resize_callback(int w, int h) {
    Application::framebuffer_resize_callback(w, h);
    glViewport(0, 0, w, h);
    camera_->set_aspect((float)w / h);
}

void SimpleShapeApplication::mouse_button_callback(int button, int action, int mods) {
    Application::mouse_button_callback(button, action, mods);

    if (controler_) {
        double x, y;
        glfwGetCursorPos(window_, &x, &y);

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
            controler_->LMB_pressed(x, y);

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
            controler_->LMB_released(x, y);
    }

}

void SimpleShapeApplication::cursor_position_callback(double x, double y) {
    Application::cursor_position_callback(x, y);
    if (controler_) {
        controler_->mouse_moved(x, y);
    }
}