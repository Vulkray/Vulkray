/*
 * PhysicalDevice.cxx
 * Scans system GPU devices and selects a suitable device.
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

#include "Vulkan.hxx"

#include <spdlog/spdlog.h>
#include <map>
#include <set>

void PhysicalDevice::selectPhysicalDevice(VkPhysicalDevice *physicalDevice, VkInstance vulkanInstance,
                                          VkSurfaceKHR surface, const std::vector<const char*> extensions) {

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, nullptr);
    // if no GPUs with vulkan support are found, exit with error.
    if (deviceCount == 0) {
        spdlog::error("No Vulkan-compatible system GPU devices were found. Exiting.");
        throw std::runtime_error("No Vulkan-compatible GPU device found.");
    }
    // Get all available GPU devices
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, devices.data());

    // ---------- Scan GPU devices features & score suitability --------- //

    // Use an ordered map to automatically sort candidates by increasing score
    std::multimap<int, VkPhysicalDevice> candidates;

    for (auto& device : devices) {
        int score = PhysicalDevice::rateGPUSuitability(device, surface, extensions);
        candidates.insert(std::make_pair(score, device));
    }

    // Check if the best candidate is suitable at all
    if (candidates.rbegin()->first > 0) {
        *physicalDevice = candidates.rbegin()->second;
    } else {
        spdlog::error("Could not find a suitable GPU device! Exiting.");
        throw std::runtime_error("No system GPU device met the minimal device requirements.");
    }

    // Get selected GPU properties
    VkPhysicalDeviceProperties gpuProperties;
    vkGetPhysicalDeviceProperties(*physicalDevice, &gpuProperties);

    spdlog::info("Vulkan GPU Selected: {0}", gpuProperties.deviceName);
}

int PhysicalDevice::rateGPUSuitability(VkPhysicalDevice gpuDevice, VkSurfaceKHR surface,
                                       const std::vector<const char*> extensions) {
    int deviceScore = 0;

    // Get GPU device information
    VkPhysicalDeviceProperties gpuProperties;
    VkPhysicalDeviceFeatures gpuFeatures;
    QueueFamilyIndices gpuQueueIndices = PhysicalDevice::findDeviceQueueFamilies(gpuDevice, surface);
    bool hasRequiredExtensions = PhysicalDevice::checkGPUExtensionSupport(gpuDevice, extensions);

    vkGetPhysicalDeviceProperties(gpuDevice, &gpuProperties);
    vkGetPhysicalDeviceFeatures(gpuDevice, &gpuFeatures);

    // Check minimal GPU device requirements
    if (!hasRequiredExtensions) return 0; // required GPU extensions
    else {
        // Check GPU swap chain support
        SwapChainSupportDetails swapChainSupport = SwapChain::querySwapChainSupport(gpuDevice, surface);
        if (swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty()) return 0;
    }
    if (!gpuQueueIndices.isComplete()) return 0; // required GPU queues
    if (!gpuFeatures.geometryShader) return 0; // required geometry shader

    // Rate GPU physical device with score
    if (gpuProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) deviceScore += 1000;
    // Maximum possible size of textures affects graphics quality
    deviceScore += gpuProperties.limits.maxImageDimension2D;

    return deviceScore;
}

QueueFamilyIndices PhysicalDevice::findDeviceQueueFamilies(VkPhysicalDevice gpuDevice, VkSurfaceKHR surface) {
    QueueFamilyIndices queueIndices; // values initialized with std::optional, so no init required

    // Get device queue family information
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(gpuDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(gpuDevice, &queueFamilyCount, queueFamilies.data());

    // For each queue family in GPU, find the index of each queue type we need.
    int index = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueIndices.isComplete()) break; // break loop if queue indices struct is complete

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(gpuDevice, index, surface, &presentSupport);

        if (presentSupport) queueIndices.presentFamily = index; // found present queue index

        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queueIndices.graphicsFamily = index; // found graphics queue index
        }
        index++;
    }
    return queueIndices;
}

bool PhysicalDevice::checkGPUExtensionSupport(VkPhysicalDevice gpuDevice, const std::vector<const char*> extensions) {

    // Get GPU extensions information
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(gpuDevice, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(gpuDevice, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(extensions.begin(), extensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }
    return requiredExtensions.empty();
}