/*
 * LogicalDevice.cxx
 * Creates the Vulkan logical device instance with the selected GPU physical device.
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

#include "LogicalDevice.hxx"
#include "PhysicalDevice.hxx"

#include <spdlog/spdlog.h>
#include <set>

void LogicalDevice::createLogicalDevice(VkDevice *logicalDevice, VkQueue *graphicsQueue, VkQueue *presentQueue,
                                        VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                                        const bool enableVkLayers, const std::vector<const char*> vkLayers) {

    // get GPU queue family indices
    QueueFamilyIndices gpuQueueIndices = PhysicalDevice::findDeviceQueueFamilies(physicalDevice, surface);

    // Logical device create info structs
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {
            gpuQueueIndices.graphicsFamily.value(),
            gpuQueueIndices.presentFamily.value()
    };
    VkPhysicalDeviceFeatures deviceFeatures{}; // device features are not configured for now
    VkDeviceCreateInfo createInfo{};

    // Create logical device queue create info struct for each queue
    for (uint32_t queueFamily : uniqueQueueFamilies) {

        VkDeviceQueueCreateInfo queueCreateInfo{};
        float graphicsQueuePriority = 1.0f;

        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &graphicsQueuePriority;
        // Push new queue create info to vector
        queueCreateInfos.push_back(queueCreateInfo);
    }

    // Create Vulkan logical device
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = 0;

    // Validation layer fields are ignored by newer Vk implementations; set for compatibility
    if (enableVkLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(vkLayers.size());
        createInfo.ppEnabledLayerNames = vkLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    // Create the Vulkan logical device
    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, logicalDevice) != VK_SUCCESS) {
        spdlog::error("Failed to create the Vulkan logical device object!");
        throw std::runtime_error("Failed to create the logical device!");
    }

    // Create handles for Graphics and Present queues using given handle pointers
    vkGetDeviceQueue(*logicalDevice, gpuQueueIndices.graphicsFamily.value(), 0, graphicsQueue);
    vkGetDeviceQueue(*logicalDevice, gpuQueueIndices.presentFamily.value(), 0, presentQueue);
}