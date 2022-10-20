/*
 * Example program using the early Vulkray engine API.
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2022, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#include "../include/Vulkray/ShowBase.h"
#include <iostream>

class Application {
public:
    EngineConfig configuration;
    std::unique_ptr<ShowBase> base;
    Application();
};

Application::Application() {
    // Prepare Vulkray engine configuration
    this->configuration.windowTitle = (char*) "Vulkray Test";
    this->configuration.graphicsInput.vertexData = {
            {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}}, // 0
            {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}}, // 1
            {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}, // 2
            {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}}, // 3
            {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}}, // 4
            {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}}, // 5
            {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}, // 6
            {{-0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}} // 7
    };
    this->configuration.graphicsInput.indexData = {
            0, 1, 2, 2, 3, 0, // top face
            4, 7, 6, 6, 5, 4, // bottom face
            0, 4, 5, 5, 1, 0, // back face
            1, 5, 6, 6, 2, 1, // right face
            4, 0, 3, 3, 7, 4, // left face
            3, 2, 6, 6, 7, 3 // front face
    };

    // Instantiate the engine base class using a smart pointer
    this->base = std::make_unique<ShowBase>(configuration);

    // Set initial camera field of view in degrees
    this->base->camera->set_fov(50);

    // Move camera 4 units back to see the cube
    this->base->camera->set_xyz(-4, 0, 0);
    this->base->camera->set_hpr(0, 0, 0);

    // Initialize the engine vulkan renderer
    try {
        this->base->launch();
    } catch (const std::exception& exception) {
        std::cout << "An exception was thrown by the engine:\n" << exception.what();
        exit(1);
    }
}

int main() {
    Application *myGame = new Application();
    return 0;
}
