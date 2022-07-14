#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include "vulkan/VulkanInstance.hxx"

#include <iostream>

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
    GLFWwindow* window{};
    // Vulkan instances
    VkInstance vulkanInstance{};

    // Vulkan validation layers
    const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };
    #ifdef NDEBUG
        const bool enableValidationLayers = false;
    #else
        const bool enableValidationLayers = true;
    #endif

    void initGlfw() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // lock window resizing
        window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE, nullptr, nullptr);

        //glm::mat4 matrix;
        //glm::vec4 vec;
        //auto test = matrix * vec;
        spdlog::debug("Initialized GLFW window.");
    }

    void initVulkan() {
        spdlog::debug("Initializing Vulkan ...");

        VulkanInstance::createInstance(&vulkanInstance, WIN_TITLE, enableValidationLayers, validationLayers);

        spdlog::debug("Initialized Vulkan instances.");
    }

    void mainLoop() {
        spdlog::debug("Running main program loop ...");
        while(!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }
    void cleanup() {
        spdlog::debug("Cleaning up instances ...");
        // Cleanup Vulkan
        vkDestroyInstance(vulkanInstance, nullptr);
        // Cleanup GLFW
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

int main() {
    #ifndef NDEBUG
        spdlog::set_level(spdlog::level::debug); // enable debug logging for debug builds
    #endif
    spdlog::set_pattern("[Vulkray] [%H:%M:%S] [%^%l%$] [thread %t] %v");
    Initialize init;

    try {
        init.launch();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
