/*
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2022, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#include "global_definitions.h"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <memory>
#include <vector>

#include "vulkan/Vulkan.h"

class Initialize {
public:
    std::unique_ptr<Vulkan> m_vulkan;
    void launch() {
        // Initialize the Vulkan renderer module
        this->m_vulkan = std::make_unique<Vulkan>(this->graphicsInput);
    }
private:
    // TODO: Vulkan graphics input struct; temporary location!
    GraphicsInput graphicsInput = {
        .vertices = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
            {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
        },
        .indices = {0, 1, 2, 2, 3, 0},
        .bufferClearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}}
    };
};

int main() {
    // ----- Spdlog logging configuration ----- //
    #ifndef NDEBUG
        spdlog::set_level(spdlog::level::debug); // enable debug logging for debug builds
    #else
        spdlog::set_level(spdlog::level::info); // only print info output on release builds
    #endif
    spdlog::set_pattern("[Vulkray] [%n] [%H:%M:%S] [%^%l%$] %v");

    // ----- Initialize the engine ----- //
    Initialize engineBase;
    try {
        engineBase.launch();
    } catch (const std::exception& exception) {
        spdlog::error("Failed to initialize engine base: {0}", exception.what());
        return 1; // exit with error
    }
    return 0;
}
