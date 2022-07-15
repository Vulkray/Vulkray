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

#include "vulkan/VulkanInstance.hxx"
#include "vulkan/WindowSurface.hxx"
#include "vulkan/PhysicalDevice.hxx"
#include "vulkan/LogicalDevice.hxx"
#include "vulkan/SwapChain.hxx"

#include <chrono>
#include <thread>

void set_spdlog_debug() {
    spdlog::set_level(spdlog::level::debug);
}

class Initialize {
public:
    void launch() {
        initGlfw(); // Initializes GLFW window
        initVulkan(); // Initializes Vulkan objects
        mainLoop(); // Main program loop
        cleanup(); // Destroys GLFW/Vulkan objects
    }
private:
    const char* WIN_TITLE = "Vulkray";
    const int WIN_WIDTH = 1300;
    const int WIN_HEIGHT = 750;

    // GLFW instances
    GLFWwindow *glfwWindow{};
    // GLM variables
    glm::mat4 matrix;
    glm::vec4 vec;
    //auto test = matrix * vec;

    // Vulkan validation layers
    const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };
    #ifdef NDEBUG
        const bool enableValidationLayers = false;
    #else
        const bool enableValidationLayers = true;
    #endif
    // Vulkan instances
    VkInstance vulkanInstance{};
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice logicalDevice = VK_NULL_HANDLE;
    VkSurfaceKHR surface;
    const std::vector<const char*> requiredExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    // GPU queue handles
    VkQueue graphicsQueue;
    VkQueue presentQueue;

    void initGlfw() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // lock window resizing; temporary!
        glfwWindow = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE, nullptr, nullptr);
        spdlog::debug("Initialized GLFW window.");
    }

    void initVulkan() {
        spdlog::debug("Initializing Vulkan ...");
        VulkanInstance::createInstance(&vulkanInstance, WIN_TITLE, enableValidationLayers, validationLayers);
        WindowSurface::createSurfaceKHR(&surface, vulkanInstance, glfwWindow);
        PhysicalDevice::selectPhysicalDevice(&physicalDevice, vulkanInstance, surface, requiredExtensions);
        LogicalDevice::createLogicalDevice(&logicalDevice, &graphicsQueue, &presentQueue, physicalDevice,
                                           surface, requiredExtensions, enableValidationLayers, validationLayers);
        SwapChain::createSwapChain();
        spdlog::debug("Initialized Vulkan instances.");
    }

    void mainLoop() {
        spdlog::debug("Running main program loop ...");
        while(!glfwWindowShouldClose(glfwWindow)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(16)); // check ~60 times/second; temporary!
            glfwPollEvents();
        }
    }
    void cleanup() {
        spdlog::debug("Cleaning up engine ...");
        // Cleanup Vulkan
        vkDestroyDevice(logicalDevice, nullptr);
        vkDestroySurfaceKHR(vulkanInstance, surface, nullptr);
        vkDestroyInstance(vulkanInstance, nullptr);
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
