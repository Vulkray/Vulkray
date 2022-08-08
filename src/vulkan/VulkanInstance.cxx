/*
 * VulkanInstance.cxx
 * Creates a new Vulkan object instance
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2022, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#include "Vulkan.hxx"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

VulkanInstance::VulkanInstance(Vulkan *m_vulkan): VkModuleBase(m_vulkan) {

    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = this->m_vulkan->engineName;
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

    // Modify createInfo if vulkan validation layers requested
    if (this->m_vulkan->enableValidationLayers) {
        /*
         * On DEBUG cmake build, a warning may be printed by the driver.
         * This is NORMAL as it's simply highlighting that validation layers are enabled.
         *  E.g. "MESA-INTEL: warning: Performance support disabled, consider sysctl dev.i915.perf_stream_paranoid=0"
         *
         *  Update: hold on, it's happening without validation layers too... I don't know why lol
         */
        spdlog::info("Enabling validation layers..");
        createInfo.enabledLayerCount = static_cast<uint32_t>(this->m_vulkan->validationLayers.size());
        createInfo.ppEnabledLayerNames = this->m_vulkan->validationLayers.data();
    }

    /*
     * Check supported vulkan validation layers, if requested.
     *  Ensures vkCreateInstance() never returns 'VK_ERROR_LAYER_NOT_PRESENT' enum.
     */
    if (this->m_vulkan->enableValidationLayers && !checkValidationLayerSupport(this->m_vulkan->validationLayers)) {
        spdlog::error("Vulkan Validation layers requested, but not available!");
        throw std::runtime_error("Failed required validation layers check.");
    }

    // Create the Vulkan instance
    VkResult result = vkCreateInstance(&createInfo, nullptr, &this->vulkanInstance);

    if (result != VK_SUCCESS) {
        spdlog::error("An error occurred while creating the Vulkan instance:");
        switch (result) {
            case VK_ERROR_INCOMPATIBLE_DRIVER:
                spdlog::error("The system Vulkan ICD (Installable Client Driver) is not compatible.");
                break;
            case VK_ERROR_LAYER_NOT_PRESENT:
                spdlog::error("The requested Vulkan instance validation layers were not found.");
                break;
            case VK_ERROR_EXTENSION_NOT_PRESENT:
                spdlog::error("The requested Vulkan instance device extensions were not found.");
                break;
            case VK_ERROR_OUT_OF_HOST_MEMORY:
                spdlog::error("The host system ran out of memory while initializing the Vulkan instance.");
                break;
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                spdlog::error("The device ran out of memory while initializing the Vulkan instance.");
                break;
            default:
                spdlog::error("An unknown issue was encountered when creating the Vulkan instance.");
                break;
        }
        throw std::runtime_error("Fatal! Failed to create the Vulkan instance!");
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
VulkanInstance::~VulkanInstance() {
    vkDestroyInstance(this->vulkanInstance, nullptr);
}

int VulkanInstance::checkRequiredExtensions(const char** glfwExtensions, uint32_t glfwCount,
                                            std::vector<VkExtensionProperties> extensions) {

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

bool VulkanInstance::checkValidationLayerSupport(const std::vector<const char*> vkLayers) {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : vkLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        if (!layerFound) {
            return false;
        }
    }
    return true;
}