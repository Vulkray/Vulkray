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

#include "Vulkan.h"

#include <spdlog/spdlog.h>
#include <set>

LogicalDevice::LogicalDevice(Vulkan *m_vulkan): VkModuleBase(m_vulkan) {
    QueueFamilyIndices queueFamilies = this->m_vulkan->m_physicalDevice->queueFamilies;

    // Logical device create info structs
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {
            queueFamilies.graphicsFamily.value(),
            queueFamilies.presentFamily.value(),
            queueFamilies.transferFamily.value()
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
    createInfo.enabledExtensionCount = static_cast<uint32_t>(this->m_vulkan->requiredExtensions.size());
    createInfo.ppEnabledExtensionNames = this->m_vulkan->requiredExtensions.data();

    // Validation layer fields are ignored by newer Vk implementations; set for compatibility
    if (this->m_vulkan->enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(this->m_vulkan->validationLayers.size());
        createInfo.ppEnabledLayerNames = this->m_vulkan->validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    // Create the Vulkan logical device
    if (vkCreateDevice(this->m_vulkan->m_physicalDevice->physicalDevice,
                       &createInfo, nullptr, &this->logicalDevice) != VK_SUCCESS) {
        spdlog::error("Failed to create the Vulkan logical device object!");
        throw std::runtime_error("Failed to create the logical device!");
    }

    // Create handles for Graphics, Present, and Transfer queues using given handle pointers
    vkGetDeviceQueue(this->logicalDevice, queueFamilies.graphicsFamily.value(), 0, &this->graphicsQueue);
    vkGetDeviceQueue(this->logicalDevice, queueFamilies.presentFamily.value(), 0, &this->presentQueue);
    vkGetDeviceQueue(this->logicalDevice, queueFamilies.transferFamily.value(), 0, &this->transferQueue);
}

LogicalDevice::~LogicalDevice() {
    vkDestroyDevice(this->logicalDevice, nullptr);
}

void LogicalDevice::waitForDeviceIdle() {
    vkDeviceWaitIdle(this->logicalDevice);
}