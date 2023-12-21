//
// Created by pbialas on 05.08.2020.
//

#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Application/application.h"
#include "Application/utils.h"

#include "camera.h"

class SimpleShapeApplication : public xe::Application
{
public:
    SimpleShapeApplication(int width, int height, std::string title, bool debug) : Application(width, height, title, debug) {}

    void init() override;

    void frame() override;

    void set_camera(Camera* camera) { camera_ = camera; }

    Camera* camera() { return camera_; }

    ~SimpleShapeApplication() {
        if (camera_) {
            delete camera_;
        }
    }

private:
    Camera* camera_;

    GLuint vao_;
    std::vector<GLushort> indices;

    glm::mat4 M_;
    GLuint PVM;


    void framebuffer_resize_callback(int w, int h) override;

    void scroll_callback(double xoffset, double yoffset) override {
        Application::scroll_callback(xoffset, yoffset);
        camera()->zoom(yoffset / 30.0f);
    }
};