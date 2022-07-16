/*
 * SwapChain.cxx
 * Creates the Vulkan swap chain instance and swap chain buffers.
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

#include "SwapChain.hxx"
#include "PhysicalDevice.hxx"

#include <spdlog/spdlog.h>
#include <cstdint>
#include <limits>
#include <algorithm>

void SwapChain::createSwapChain(VkSwapchainKHR *swapChain, std::vector<VkImage> *swapImages,
                                VkFormat *format, VkExtent2D *extent, VkDevice logicalDevice,
                                VkPhysicalDevice gpuDevice, VkSurfaceKHR surface, GLFWwindow *window) {

    // Get device swap chain support info
    SwapChainSupportDetails supportDetails = SwapChain::querySwapChainSupport(gpuDevice, surface);

    // Choose swap chain configuration
    VkSurfaceFormatKHR surfaceFormat = SwapChain::chooseSurfaceFormat(supportDetails.formats);
    VkPresentModeKHR presentMode = SwapChain::choosePresentMode(supportDetails.presentModes);
    VkExtent2D swapExtent = SwapChain::chooseSwapExtent(supportDetails.capabilities, window);
    uint32_t imageCount = supportDetails.capabilities.minImageCount + 1;

    // fallback to maximum image count if variable above exceeds it
    if (supportDetails.capabilities.maxImageCount > 0 && imageCount > supportDetails.capabilities.maxImageCount) {
        imageCount = supportDetails.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapCreateInfo{};
    swapCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapCreateInfo.surface = surface;
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

    // Get GPU device queue indices
    QueueFamilyIndices queueIndices = PhysicalDevice::findDeviceQueueFamilies(gpuDevice, surface);
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
    swapCreateInfo.oldSwapchain = VK_NULL_HANDLE; // swap recreation not yet implemented; temporary!

    // Initialize the Vulkan swap chain instance
    VkResult result = vkCreateSwapchainKHR(logicalDevice, &swapCreateInfo, nullptr, swapChain);
    if (result != VK_SUCCESS) {
        spdlog::error("An issue was encountered while trying to create the Vulkan swap chain.");
        throw std::runtime_error("Failed to initialize the swap chain!");
    }

    // Get the swap chain images handles
    vkGetSwapchainImagesKHR(logicalDevice, *swapChain, &imageCount, nullptr);
    vkGetSwapchainImagesKHR(logicalDevice, *swapChain, &imageCount, swapImages->data());
    // Store swap format & extent in variable pointers for global use
    *format = surfaceFormat.format;
    *extent = swapExtent;
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
    spdlog::warn("Preferred surface format not found; Using the first format available.");
    return availableFormats[0];
}

VkPresentModeKHR SwapChain::choosePresentMode(const std::vector <VkPresentModeKHR> &availablePresentModes) {

    // Look for the preferred swap presentation mode
    for (const auto &availablePresentMode : availablePresentModes) {
        if (availablePresentMode == PREFERRED_PRESENTATION) return availablePresentMode;
    }
    spdlog::warn("Preferred presentation mode not found, using the default FIFO mode.");
    return DEFAULT_PRESENTATION;
}

VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, GLFWwindow *window) {

    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        int pxWidth, pxHeight;
        glfwGetFramebufferSize(window, &pxWidth, &pxHeight); // get actual window framebuffer pixel resolution

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