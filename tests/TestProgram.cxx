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

#include "../include/Vulkray.h"

int main() {
    // Instantiate the engine base class using a smart pointer
    std::unique_ptr<VulkrayEngine> vulkray = std::make_unique<VulkrayEngine>();

    // Feed graphics information to the engine
    vulkray->graphicsInput.vertexData = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
            {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };
    vulkray->graphicsInput.indexData = {0, 1, 2, 2, 3, 0};
    vulkray->graphicsInput.bufferClearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

    // Initialize the engine vulkan renderer
    try {
        vulkray->initialize();
    } catch (const std::exception& exception) {
        printf("An exception was thrown by the engine: %s", exception.what());
        return 1;
    }
    return 0;
}