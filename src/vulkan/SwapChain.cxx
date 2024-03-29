/*
 * SwapChain.cxx
 * Creates the Vulkan swap chain instance and swap chain buffers.
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2023, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#include "../../include/Vulkray/Vulkan.h"

#include <spdlog/spdlog.h>
#include <cstdint>
#include <limits>
#include <algorithm>

SwapChain::SwapChain(Vulkan *m_vulkan): VkModuleBase(m_vulkan) {
    // Get device swap chain support info
    SwapChainSupportDetails supportDetails = SwapChain::querySwapChainSupport(
            this->m_vulkan->m_physicalDevice->physicalDevice, this->m_vulkan->m_window->surface);

    // Choose swap chain configuration
    VkSurfaceFormatKHR surfaceFormat = SwapChain::chooseSurfaceFormat(supportDetails.formats);
    VkPresentModeKHR presentMode = SwapChain::choosePresentMode(supportDetails.presentModes);
    VkExtent2D swapExtent = SwapChain::chooseSwapExtent(supportDetails.capabilities, this->m_vulkan->m_window->window);
    uint32_t imageCount = supportDetails.capabilities.minImageCount + 1;

    // fallback to maximum image count if variable above exceeds it
    if (supportDetails.capabilities.maxImageCount > 0 && imageCount > supportDetails.capabilities.maxImageCount) {
        imageCount = supportDetails.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapCreateInfo{};
    swapCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapCreateInfo.surface = this->m_vulkan->m_window->surface;
    swapCreateInfo.minImageCount = imageCount;
    swapCreateInfo.imageFormat = surfaceFormat.format;
    swapCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapCreateInfo.imageExtent = swapExtent;
    swapCreateInfo.imageArrayLayers = 1; // default: 1 (unless this engine renders stereoscopic 3D)
    /*
     * VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT is the current default for basic direct rendering.
     * If we want to implement post-processing rendering, switch to VK_IMAGE_USAGE_TRANSFER_DST_BIT instead.
     */
    swapCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices queueIndices = this->m_vulkan->m_physicalDevice->queueFamilies;

    // Get GPU device queue indices
    uint32_t queueFamilyIndices[] = {
            queueIndices.graphicsFamily.value(),
            queueIndices.presentFamily.value()
    };
    /*
     * Configure the GPU device queues to be used.
     * If queues are in different queue families, use VK_SHARING_MODE_CONCURRENT, else use exclusive.
     */
    if (queueIndices.graphicsFamily != queueIndices.presentFamily) {
        swapCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapCreateInfo.queueFamilyIndexCount = 2;
        swapCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        swapCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapCreateInfo.queueFamilyIndexCount = 0;
        swapCreateInfo.pQueueFamilyIndices = nullptr;
    }

    swapCreateInfo.preTransform = supportDetails.capabilities.currentTransform;
    swapCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // ignore alpha channel
    swapCreateInfo.presentMode = presentMode;
    swapCreateInfo.clipped = VK_TRUE; // ignore pixels covered by another window (no use in this engine)

    // pass old swap chain instance if this is not the first swap chain
    if (this->m_vulkan->m_oldSwapChain == nullptr) {
        swapCreateInfo.oldSwapchain = VK_NULL_HANDLE;
    } else {
        swapCreateInfo.oldSwapchain = this->m_vulkan->m_oldSwapChain->swapChain;
    }

    // Initialize the Vulkan swap chain instance
    VkResult result = vkCreateSwapchainKHR(this->m_vulkan->m_logicalDevice->logicalDevice,
                                           &swapCreateInfo, nullptr, &this->swapChain);
    if (result != VK_SUCCESS) {
        switch (result) {
            case VK_ERROR_INITIALIZATION_FAILED:
                spdlog::error("Swap chain initialization could not be completed for implementation-specific reasons.");
                break;
            case VK_ERROR_SURFACE_LOST_KHR:
                spdlog::error("The GLFW vulkan surface instance is no longer available.");
                break;
            case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
                spdlog::error("The requested window is already in use by Vulkan or another API.");
                break;
            default:
                spdlog::error("vkCreateSwapchainKHR() returned error code: {0}", result);
                break;
        }
        this->swapChain = VK_NULL_HANDLE; // less validation layer errors on clean up after error
        throw std::runtime_error("Failed to recreate the swap chain!");
    }

    // Get the swap chain images handles
    vkGetSwapchainImagesKHR(this->m_vulkan->m_logicalDevice->logicalDevice,
                            this->swapChain, &imageCount, nullptr);
    this->swapChainImages.resize(imageCount); // resize swap chain images vector to expected image count
    vkGetSwapchainImagesKHR(this->m_vulkan->m_logicalDevice->logicalDevice,
                            this->swapChain, &imageCount, this->swapChainImages.data());
    // Store swap format & extent in variable pointers for global use
    this->swapChainImageFormat = surfaceFormat.format;
    this->swapChainExtent = swapExtent;
}

SwapChain::~SwapChain() {
    vkDestroySwapchainKHR(this->m_vulkan->m_logicalDevice->logicalDevice, this->swapChain, nullptr);
}

SwapChainSupportDetails SwapChain::querySwapChainSupport(VkPhysicalDevice gpuDevice, VkSurfaceKHR surface) {

    // Get GPU device surface capabilities
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpuDevice, surface, &details.capabilities);

    // Get GPU device surface formats
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpuDevice, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpuDevice, surface, &formatCount, details.formats.data());
    }

    // Get GPU device present modes
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(gpuDevice, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(gpuDevice, surface, &presentModeCount, details.presentModes.data());
    }
    return details; // return filled out GPU support details
}

VkSurfaceFormatKHR SwapChain::chooseSurfaceFormat(const std::vector <VkSurfaceFormatKHR> &availableFormats) {

    // Go through all supported swap chain color formats
    for (const auto &availableFormat : availableFormats) {
        if (availableFormat.format == PREFERRED_COLOR_FORMAT
            && availableFormat.colorSpace == PREFERRED_COLOR_SPACE) {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

VkPresentModeKHR SwapChain::choosePresentMode(const std::vector <VkPresentModeKHR> &availablePresentModes) {

    // Look for the preferred swap presentation mode
    for (const auto &availablePresentMode : availablePresentModes) {
        if (availablePresentMode == PREFERRED_PRESENTATION) return availablePresentMode;
    }
    return DEFAULT_PRESENTATION;
}

VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, GLFWwindow *window) {

    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        int pxWidth, pxHeight;
        glfwGetFramebufferSize(window, &pxWidth, &pxHeight); // get latest window framebuffer pixel resolution

        VkExtent2D actualPixelExtent = {
                static_cast<uint32_t>(pxWidth),
                static_cast<uint32_t>(pxHeight)
        };
        actualPixelExtent.width = std::clamp(actualPixelExtent.width,
                                             capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualPixelExtent.height = std::clamp(actualPixelExtent.height,
                                              capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        return actualPixelExtent;
    }
}