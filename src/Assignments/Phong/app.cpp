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
#include "glm/gtc/matrix_transform.hpp"

#include "Engine/Mesh.h"
#include "Engine/Material.h"

#include "Engine/mesh_loader.h"
#include "Engine/ColorMaterial.h"
#include "Engine/PhongMaterial.h"
#include "Engine/lights.h"

struct ModifierBlock {
    glm::vec4 Kd;
    GLboolean use_map_Kd;
};

struct LightBlock {
    PointLight light[24];
    int num_lights;
};

struct MatricesBlock {
    glm::mat4 VM;
    glm::mat3 N;
};

void SimpleShapeApplication::init() {
    xe::ColorMaterial::init();
    xe::PhongMaterial::init();

    mesh = xe::load_mesh_from_obj(std::string(ROOT_DIR) + "/Models/square.obj", std::string(ROOT_DIR) + "/Models");

    add_submesh(mesh.get());

    auto light_1 = new PointLight({ 0.0, 0.5, 0.5 }, { 0.0, 1.0, 0.0 }, { 0.0, 0.0, 0.0 });
    auto light_2 = new PointLight({ -0.5, -0.5, 0.5 }, { 1.0, 0.0, 1.0 }, { 0.0, 0.0, 0.0 });
    auto light_3 = new PointLight({ 0.5, -0.5, 0.5 }, { 0.0, 0.0, 1.0 }, { 0.0, 0.0, 0.0 });

    add_light(*light_1);
    add_light(*light_2);
    add_light(*light_3);

    set_camera(new Camera());
    set_controler(new CameraControler(camera()));

    auto [w, h] = frame_buffer_size();

    auto aspect_ = (float)w / h;
    auto fov_ = glm::pi<float>() / 4.0;
    auto near_ = 0.1f;
    auto far_ = 100.0f;

    camera_->look_at(
        glm::vec3(0.0f, 0.0f, 5.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    camera_->perspective(fov_, aspect_, near_, far_);

    M_ = glm::mat4(1.0f);

    glGenBuffers(1, &PVM);
    OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, PVM));
    glBufferData(GL_UNIFORM_BUFFER, 16 * sizeof(float), nullptr, GL_STATIC_DRAW);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glGenBuffers(1, &Matrices);
    OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, Matrices));
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);

    glGenBuffers(1, &Lights);
    OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, Lights));
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LightBlock), nullptr, GL_STATIC_DRAW);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // This setups a Vertex Array Object (VAO) that  encapsulates
    // the state of all vertex buffers needed for rendering
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    // Bind uniform buffers
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, PVM);
    glBindBuffer(GL_UNIFORM_BUFFER, PVM);

    glBindBufferBase(GL_UNIFORM_BUFFER, 2, Matrices);
    glBindBuffer(GL_UNIFORM_BUFFER, Matrices);

    glBindBufferBase(GL_UNIFORM_BUFFER, 3, Lights);
    glBindBuffer(GL_UNIFORM_BUFFER, Lights);

    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //end of vao "recording"

    // Setting ambient light
    xe::PhongMaterial::set_ambient({ 0.3f, 0.3f, 0.3f });

    // Setting the background color of the rendering window,
    // I suggest not to use white or black for better debuging.
    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    // This setups an OpenGL vieport of the size of the whole rendering window.
    glViewport(0, 0, w, h);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

//This functions is called every frame and does the actual rendering.
void SimpleShapeApplication::frame() {
    auto pvm = camera_->projection() * camera_->view() * glm::mat4(1.0f);

    glBindBuffer(GL_UNIFORM_BUFFER, PVM);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &pvm[0]);

    auto vm = camera_->view() * glm::mat4(1.0f);
    auto R = glm::mat3(vm);
    auto N = glm::mat3(glm::cross(R[1], R[2]), glm::cross(R[2], R[0]), glm::cross(R[0], R[1]));

    MatricesBlock matricesBlock{};

    matricesBlock.N = N;
    matricesBlock.VM = vm;

    glBindBuffer(GL_UNIFORM_BUFFER, Matrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(MatricesBlock), &matricesBlock);

    glBindBuffer(GL_UNIFORM_BUFFER, Lights);

    LightBlock lightBlock{};

    lightBlock.num_lights = p_lights_.size();

    for(int i = 0; i < p_lights_.size(); i++) {
        lightBlock.light[i] = p_lights_[i];
    }

    int lightBlockSize = sizeof(LightBlock);

    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightBlock), &lightBlock);

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