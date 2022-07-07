#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <spdlog/spdlog.h>

#include <iostream>
#include <vector>
#include <format>

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
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE, nullptr, nullptr);

        glm::mat4 matrix;
        glm::vec4 vec;
        auto test = matrix * vec;
        spdlog::info("Initialized GLFW window.");
    }

    void initVulkan() {
        createVulkanInstance();
        spdlog::info("Initialized Vulkan instances.");
    }

    void createVulkanInstance() {
        VkApplicationInfo applicationInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = WIN_TITLE;
        applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
        applicationInfo.pEngineName = "No Engine";
        applicationInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
        applicationInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &applicationInfo;

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        createInfo.enabledLayerCount = 0;

        if (vkCreateInstance(&createInfo, nullptr, &vulkanInstance) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create Vulkan instance!");
        }

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        spdlog::info("Available extensions: ");

        for (const auto& extension : extensions) {
            spdlog::info(std::format("\t{}", extension.extensionName));
        }
    }

    void mainLoop() {
        spdlog::info("Running main program loop ...");
        while(!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }
    void cleanup() {
        spdlog::info("Cleaning up GLFW & Vulkan instances ...");
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

int main(int argc, char** argv) {
    Initialize init;
    spdlog::set_pattern("[%H:%M:%S] [%n] [%^--%L--%$] [thread %t] %v");

    try {
        init.launch();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
