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

#include "../include/vulkray.h"
#include <iostream>

int main() {
    // Instantiate the engine base class using a smart pointer
    std::unique_ptr<VulkrayEngine> engine = std::make_unique<VulkrayEngine>();

    // Feed graphics information to the engine
    engine->graphicsInput.vertexData = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
            {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };
    engine->graphicsInput.indexData = {0, 1, 2, 2, 3, 0};
    engine->graphicsInput.bufferClearColor = {{{0.1f, 0.1f, 0.1f, 1.0f}}};

    // Initialize the engine vulkan renderer
    try {
        engine->initialize();
    } catch (const std::exception& exception) {
        std::cout << "An exception was thrown by the engine:\n" << exception.what();
        return 1;
    }
    return 0;
}
