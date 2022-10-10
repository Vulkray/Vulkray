/*
 * PhysicalDevice.cxx
 * Scans system GPU devices and selects a suitable device.
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
#include <map>
#include <set>

PhysicalDevice::PhysicalDevice(Vulkan *m_vulkan): VkModuleBase(m_vulkan) {

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(this->m_vulkan->m_vulkanInstance->vulkanInstance, &deviceCount, nullptr);
    // if no GPUs with vulkan support are found, exit with error.
    if (deviceCount == 0) {
        spdlog::error("No Vulkan-compatible system GPU devices were found. Exiting.");
        throw std::runtime_error("No Vulkan-compatible GPU device found.");
    }
    // Get all available GPU devices
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(this->m_vulkan->m_vulkanInstance->vulkanInstance, &deviceCount, devices.data());

    // ---------- Scan GPU devices features & score suitability --------- //

    // Use an ordered map to automatically sort candidates by increasing score
    std::multimap<int, VkPhysicalDevice> candidates;

    for (auto& device : devices) {
        // Initialize `queueFamilies` struct with GPU device queue family indices
        this->physicalDevice = device;
        this->queueFamilies = this->findDeviceQueueFamilies();
        int score = this->rateGPUSuitability();
        candidates.insert(std::make_pair(score, device));
    }

    // Check if the best candidate is suitable at all
    if (candidates.rbegin()->first > 0) {
        this->physicalDevice = candidates.rbegin()->second;
    } else {
        spdlog::error("Could not find a suitable GPU device! Exiting.");
        throw std::runtime_error("No system GPU device met the minimal device requirements.");
    }

    // Get selected GPU properties
    VkPhysicalDeviceProperties gpuProperties;
    vkGetPhysicalDeviceProperties(this->physicalDevice, &gpuProperties);

    spdlog::info("Vulkan GPU Selected: {0}", gpuProperties.deviceName);

    // Store final selected GPU device information
    this->queueFamilies = this->findDeviceQueueFamilies();
    this->msaaSamples = this->getMaxUsableSampleCount();
}

int PhysicalDevice::rateGPUSuitability() {
    int deviceScore = 0;

    // Get GPU device information
    VkPhysicalDeviceProperties gpuProperties;
    VkPhysicalDeviceFeatures gpuFeatures;
    bool hasRequiredExtensions = this->checkGPUExtensionSupport();
    VkSampleCountFlagBits msaaSupported = this->getMaxUsableSampleCount();

    vkGetPhysicalDeviceProperties(this->physicalDevice, &gpuProperties);
    vkGetPhysicalDeviceFeatures(this->physicalDevice, &gpuFeatures);

    // Check minimal GPU device requirements
    if (!hasRequiredExtensions) return 0; // required GPU extensions
    else {
        // Check GPU swap chain support
        SwapChainSupportDetails swapChainSupport = SwapChain::querySwapChainSupport(
                this->physicalDevice, this->m_vulkan->m_window->surface);

        if (swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty()) return 0;
    }
    if (!this->queueFamilies.isComplete()) return 0; // required GPU queues
    if (!gpuFeatures.geometryShader) return 0; // required geometry shader

    // Rate GPU physical device with score
    if (gpuProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) deviceScore += 1000;
    if (msaaSupported != VK_SAMPLE_COUNT_1_BIT) deviceScore += 500; // prefer MSAA support
    // Maximum possible size of textures affects graphics quality
    deviceScore += gpuProperties.limits.maxImageDimension2D;

    return deviceScore;
}

QueueFamilyIndices PhysicalDevice::findDeviceQueueFamilies() {
    QueueFamilyIndices queueIndices; // values initialized with std::optional, so no init required

    // Get device queue family information
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(this->physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(this->physicalDevice, &queueFamilyCount, queueFamilies.data());

    // For each queue family in GPU, find the index of each queue type we need.
    int index = 0;
    for (const auto &queueFamily : queueFamilies) {
        /* break the loop if the queue indices struct is complete (WITH a dedicated transfer family,
         * or else it will exit early as the graphics family will also pre-fill the transfer family index
         * before a dedicated one is possibly found. this check prevents that bug.
         */
        if (queueIndices.isComplete() && queueIndices.dedicatedTransferFamily) break;

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(this->physicalDevice, index,
                                             this->m_vulkan->m_window->surface, &presentSupport);

        VkQueueFlags queueFlags = queueFamily.queueFlags;

        if (presentSupport) {
            queueIndices.presentFamily = index; // found present queue index
        } else {
            // most likely a dedicated VK_QUEUE_TRANSFER_BIT capable queue family, check!
            if ((queueFlags & VK_QUEUE_TRANSFER_BIT) && !(queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
                queueIndices.transferFamily = index;
                queueIndices.dedicatedTransferFamily = true;
            }
        }
        if (queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queueIndices.graphicsFamily = index; // found graphics queue index

            /* If a dedicated `VK_QUEUE_TRANSFER_BIT` capable queue family is not found on the GPU,
             * the transferFamily index will be set to the graphics family, as any queue family with
             * `VK_QUEUE_GRAPHICS_BIT` or `VK_QUEUE_COMPUTE_BIT` bit flags implicitly supports transfer operations.
             *
             * "if (!queueIndices.transferFamily.has_value())" statement makes sure it does not overwrite the
             * transferFamily property if its index has already been found, because if it has, it was most likely
             * a dedicated transfer queue family that was found, and we want to keep that separate queue family!
             */
            if (!queueIndices.dedicatedTransferFamily) {
                queueIndices.transferFamily = index;
            }
        }
        index++;
    }
    return queueIndices;
}

bool PhysicalDevice::checkGPUExtensionSupport() {
    // Get GPU extensions information
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(this->physicalDevice, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(this->physicalDevice, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(this->m_vulkan->requiredExtensions.begin(),
                                             this->m_vulkan->requiredExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }
    return requiredExtensions.empty();
}

VkFormat PhysicalDevice::findSupportedDepthFormat(const std::vector<VkFormat>& candidates,
                                             VkImageTiling tiling, VkFormatFeatureFlags features) {

    // Get GPU format properties per format candidate & compare requirements
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(this->physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }
    spdlog::error("Could not find a suitable depth format supported by your GPU!");
    throw std::runtime_error("Failed to find a supported depth format.");
}

// wrapper for private method findSupportedDepthFormat() with preset depth format requirements
VkFormat PhysicalDevice::findDepthFormat() {
    return this->findSupportedDepthFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

bool PhysicalDevice::depthFormatHasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

// Gets best compatible MSAA configuration for GPU device
VkSampleCountFlagBits PhysicalDevice::getMaxUsableSampleCount() {

    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(this->physicalDevice, &physicalDeviceProperties);

    VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts &
                                physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
    if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
    if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
    if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

    return VK_SAMPLE_COUNT_1_BIT; // if no MSAA supported, keep disabled default.
}