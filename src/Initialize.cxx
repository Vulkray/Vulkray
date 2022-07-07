#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include "vulkan/VulkanInstance.hxx"

#include <iostream>
#include <vector>

class Initialize {
public:
    void launch() {
        initGlfw(); // Initializes GLFW window
        initVulkan(); // Initializes Vulkan objects
        mainLoop(); // Main program loop
        cleanup(); // Destroys GLFW/Vulkan objects
    }
private:
    // GLFW
    const char* WIN_TITLE = "Vulkan Program";
    const int WIN_WIDTH = 1300;
    const int WIN_HEIGHT = 750;
    GLFWwindow* window{};
    // Vulkan
    VkInstance vulkanInstance{};

    void initGlfw() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // lock window resizing
        window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE, nullptr, nullptr);

        //glm::mat4 matrix;
        //glm::vec4 vec;
        //auto test = matrix * vec;
        spdlog::info("Initialized GLFW window.");
    }

    void initVulkan() {
        spdlog::info("Initializing Vulkan ...");

        VulkanInstance::createInstance(&vulkanInstance, WIN_TITLE);

        spdlog::info("Initialized Vulkan instances.");
    }

    void mainLoop() {
        spdlog::info("Running main program loop ...");
        while(!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }
    void cleanup() {
        spdlog::info("Cleaning up instances ...");
        // Cleanup Vulkan
        vkDestroyInstance(vulkanInstance, nullptr);
        // Cleanup GLFW
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

int main(int argc, char** argv) {
    spdlog::set_pattern("[%H:%M:%S] [%n] [%^-%L-%$] [thread %t] %v");
    Initialize init;

    try {
        init.launch();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
