/*
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2022, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#include "global_definitions.hxx"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <memory>
#include <vector>

#include "vulkan/Vulkan.hxx"

class Initialize {
public:
    void launch() {
        initGlfw(); // Initializes GLFW window
        this->m_Vulkan = std::make_unique<Vulkan>(this->WIN_TITLE, this->glfwWindow, this->graphicsInput);
        mainLoop(); // Main program loop
    }
    ~Initialize() {
        spdlog::debug("Cleaning up engine ...");
        // Cleanup GLFW
        glfwDestroyWindow(this->glfwWindow);
        glfwTerminate();
    }
private:
    const char* WIN_TITLE = "Vulkray Engine";
    const int WIN_WIDTH = 900;
    const int WIN_HEIGHT = 600;
    GLFWwindow *glfwWindow{};
    std::unique_ptr<Vulkan> m_Vulkan;

    void initGlfw() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        this->glfwWindow = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE, nullptr, nullptr);
        glfwSetWindowUserPointer(this->glfwWindow, this);
        glfwSetFramebufferSizeCallback(this->glfwWindow, Initialize::framebufferResizeCallback);
        spdlog::debug("Initialized GLFW window.");
    }

    uint32_t frameIndex = 0;
    bool framebufferResized = false;
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

    void renderFrame() {
        uint32_t imageIndex;
        this->m_Vulkan->waitForPreviousFrame(this->frameIndex);
        // Get the next image from the swap chain & reset cmd buffer
        this->m_Vulkan->getNextSwapChainImage(&imageIndex, this->frameIndex, this->glfwWindow);
        this->m_Vulkan->resetGraphicsCmdBuffer(imageIndex, this->frameIndex, this->graphicsInput);
        this->m_Vulkan->submitGraphicsCmdBuffer(this->frameIndex);
        this->m_Vulkan->presentImageBuffer(&imageIndex, this->glfwWindow, &this->framebufferResized);
        // Advance index to the next frame
        this->frameIndex = (this->frameIndex + 1) % m_Vulkan->MAX_FRAMES_IN_FLIGHT;
    }

    void mainLoop() {
        spdlog::debug("Running engine main loop ...");
        while(!glfwWindowShouldClose(glfwWindow)) {
            glfwPollEvents(); // Respond to window events (exit, resize, etc.)
            renderFrame(); // Render frame using Vulkan
        }
    }

    static void framebufferResizeCallback(GLFWwindow* engineWindow, int width, int height) {
        auto engine = reinterpret_cast<Initialize*>(glfwGetWindowUserPointer(engineWindow));
        engine->framebufferResized = true;
    }
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
