/*
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2022, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include "vulkan/Vulkan.hxx"

class Initialize {
public:
    void launch() {
        initGlfw(); // Initializes GLFW window
        VulkanCore.initialize(WIN_TITLE, glfwWindow); // Initializes Vulkan objects
        mainLoop(); // Main program loop
        cleanup(); // Destroys GLFW/Vulkan objects
    }
private:
    const char* WIN_TITLE = "Vulkray Engine - Alpha";
    const int WIN_WIDTH = 800;
    const int WIN_HEIGHT = 700;

    // GLFW instances
    GLFWwindow *glfwWindow{};
    // GLM variables
    glm::mat4 matrix;
    glm::vec4 vec;
    //auto test = matrix * vec;

    // Engine's Vulkan class
    Vulkan VulkanCore;

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

    void renderFrame() {
        uint32_t imageIndex;
        this->VulkanCore.waitForPreviousFrame(this->frameIndex);
        // Get the next image from the swap chain & reset cmd buffer
        this->VulkanCore.getNextSwapChainImage(&imageIndex, this->frameIndex, this->glfwWindow);
        this->VulkanCore.resetCommandBuffer(imageIndex, this->frameIndex);
        this->VulkanCore.submitCommandBuffer(this->frameIndex);
        this->VulkanCore.presentImageBuffer(&imageIndex, this->glfwWindow, &this->framebufferResized);
        // Advance index to the next frame
        this->frameIndex = (this->frameIndex + 1) % VulkanCore.MAX_FRAMES_IN_FLIGHT;
    }

    void mainLoop() {
        spdlog::debug("Running engine main loop ...");
        while(!glfwWindowShouldClose(glfwWindow)) {
            glfwPollEvents(); // Respond to window events (exit, resize, etc.)
            renderFrame(); // Render frame using Vulkan
        }
        VulkanCore.waitForDeviceIdle(); // finish GPUs last process
    }
    void cleanup() {
        spdlog::debug("Cleaning up engine ...");
        // Cleanup Vulkan
        VulkanCore.shutdown();
        // Cleanup GLFW
        glfwDestroyWindow(glfwWindow);
        glfwTerminate();
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
