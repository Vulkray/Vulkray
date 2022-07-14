/*
 * VulkanInstance class
 * Creates a new Vulkan object instance
 *
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

#include "VulkanInstance.hxx"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

void VulkanInstance::createInstance(
        VkInstance* vkInstance, const char* appName,
        const bool enableVkLayers, const std::vector<const char*> vkLayers) {

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

    // Modify createInfo if vulkan validation layers requested
    if (enableVkLayers) {
        /*
         * On DEBUG cmake build, a warning may be printed by the driver.
         * This is NORMAL as it's simply highlighting that validation layers are enabled.
         *  E.g. "MESA-INTEL: warning: Performance support disabled, consider sysctl dev.i915.perf_stream_paranoid=0"
         *
         *  Update: hold on, it's happening without validation layers too... I don't know why lol
         */
        spdlog::info("Enabling validation layers..");
        createInfo.enabledLayerCount = static_cast<uint32_t>(vkLayers.size());
        createInfo.ppEnabledLayerNames = vkLayers.data();
    }

    /*
     * Check supported vulkan validation layers, if requested.
     *  Ensures vkCreateInstance() never returns 'VK_ERROR_LAYER_NOT_PRESENT' enum.
     */
    if (enableVkLayers && !checkValidationLayerSupport(vkLayers)) {
        spdlog::error("Vulkan Validation layers requested, but not available!");
        throw std::runtime_error("Failed required validation layers check.");
    }
    // Create vulkan instance
    if (vkCreateInstance(&createInfo, nullptr, vkInstance) != VK_SUCCESS) {
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