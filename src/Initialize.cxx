/*
 * Copyright 2022 Max Rodriguez

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
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
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // lock window resizing; temporary!
        glfwWindow = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE, nullptr, nullptr);
        spdlog::debug("Initialized GLFW window.");
    }

    void renderFrame() {
        VulkanCore.waitForPreviousFrame();
        // Get the next image from the swap chain & reset cmd buffer
        uint32_t imageIndex;
        VulkanCore.getNextSwapChainImage(&imageIndex);
        VulkanCore.resetCommandBuffer(imageIndex);
        VulkanCore.submitCommandBuffer();
        VulkanCore.presentImageBuffer(&imageIndex);
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
    Initialize base;

    try {
        base.launch();
    } catch (const std::exception& exception) {
        spdlog::error("Failed to initialize Vulkray engine base: {0}", exception.what());
        return 1; // exit with error
    }
    return 0;
}
