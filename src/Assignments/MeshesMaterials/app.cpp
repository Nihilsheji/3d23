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

#include "Application/utils.h"
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

#include "Engine/Mesh.h"
#include "Engine/ColorMaterial.h"
#include "Engine/texture.h"

struct ModifierBlock {
    glm::vec4 Kd;
    GLboolean use_map_Kd;
};

void SimpleShapeApplication::init() {
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

    auto texture = create_texture(std::string(ROOT_DIR) + "/Models/multicolor.png");

    xe::ColorMaterial::init();

    auto pyramid = new xe::Mesh;

    pyramid->allocate_vertex_buffer(vertices.size() * sizeof(GLfloat), GL_STATIC_DRAW);
    pyramid->load_vertices(0, vertices.size() * sizeof(GLfloat), vertices.data());
    pyramid->vertex_attrib_pointer(0, 3, GL_FLOAT, 5 * sizeof(GLfloat), 0);
    pyramid->vertex_attrib_pointer(1, 2, GL_FLOAT, 5 * sizeof(GLfloat), 3 * sizeof(GLfloat));

    pyramid->allocate_index_buffer(indices.size() * sizeof(GLfloat), GL_STATIC_DRAW);
    pyramid->load_indices(0, indices.size() * sizeof(GLfloat), indices.data());

    pyramid->add_submesh(0, 3, new xe::ColorMaterial({1.0f, 1.0f, 1.0f, 1.0f}, texture, 1) );
    pyramid->add_submesh(3, 6, new xe::ColorMaterial({1.0f, 1.0f, 1.0f, 1.0f}, texture, 2) );
    pyramid->add_submesh(6, 9, new xe::ColorMaterial({1.0f, 1.0f, 1.0f, 1.0f}, texture, 3) );
    pyramid->add_submesh(9, 12, new xe::ColorMaterial({1.0f, 1.0f, 1.0f, 1.0f}, texture, 4) );
    pyramid->add_submesh(12, 18, new xe::ColorMaterial({1.0f, 1.0f, 1.0f, 1.0f}, texture, 5) );
    add_submesh(pyramid);

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

    glGenBuffers(1, &PVM);
    OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, PVM));
    glBufferData(GL_UNIFORM_BUFFER, 16 * sizeof(float), nullptr, GL_STATIC_DRAW);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // This setups a Vertex Array Object (VAO) that  encapsulates
    // the state of all vertex buffers needed for rendering
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    // Bind uniform buffers
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, PVM);
    glBindBuffer(GL_UNIFORM_BUFFER, PVM);

    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //end of vao "recording"

    // Setting the background color of the rendering window,
    // I suggest not to use white or black for better debuging.
    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    // This setups an OpenGL vieport of the size of the whole rendering window.
    glViewport(0, 0, w, h);
}

//This functions is called every frame and does the actual rendering.
void SimpleShapeApplication::frame() {
    auto pvm = camera_->projection() * camera_->view() * glm::mat4(1.0f);
    glBindBuffer(GL_UNIFORM_BUFFER, PVM);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &pvm[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    for (auto m: meshes_) {
        m->draw();
    }
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