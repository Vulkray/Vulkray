/*
 * VulkanInstance class
 * Creates a new Vulkan object instance
 */
#include "VulkanInstance.h"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <iostream>
#include <vector>

void VulkanInstance::createInstance(VkInstance* vulkanInstance, const char* appName) {

    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = appName;
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

    if (vkCreateInstance(&createInfo, nullptr, vulkanInstance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan instance!");
    }

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    int res = VulkanInstance::checkRequiredExtensions(glfwExtensions, glfwExtensionCount, extensions);
    if (!res) {
        spdlog::error("System Vulkan API version does not provide required GLFW extensions.");
        throw std::runtime_error("Failed required extensions check.");
    }
}

int VulkanInstance::checkRequiredExtensions(
        const char** glfwExtensions, uint32_t glfwCount, std::vector<VkExtensionProperties> extensions) {

    for (unsigned int i = 0; i < glfwCount; i++) {
        const std::string glfwRequirement = glfwExtensions[i];

        int extensionFound = 0;
        for (const auto &extension : extensions) {
            const std::string availableExtension = extension.extensionName;
            if (glfwRequirement == availableExtension) extensionFound = 1;
        }
        if (!extensionFound) return 0;
    }
    return 1;
}