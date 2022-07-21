/*
 * LogicalDevice.cxx
 * Creates the Vulkan logical device instance with the selected GPU physical device.
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2022, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#include "Vulkan.hxx"

#include <spdlog/spdlog.h>
#include <set>

void LogicalDevice::createLogicalDevice(VkDevice *logicalDevice, VkQueue *graphicsQueue, VkQueue *presentQueue,
                                        VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                                        const std::vector<const char*> gpuExtensions,
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
    createInfo.enabledExtensionCount = static_cast<uint32_t>(gpuExtensions.size());
    createInfo.ppEnabledExtensionNames = gpuExtensions.data();

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