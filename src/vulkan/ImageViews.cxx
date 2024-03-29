/*
 * ImageViews.cxx
 * Allocates image view instances for swap chain images & 3D depth images.
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

SwapImageViews::SwapImageViews(Vulkan *m_vulkan): VkModuleBase(m_vulkan) {

    // Resize image views vector to available swap chain images
    this->swapChainImageViews.resize(this->m_vulkan->m_swapChain->swapChainImages.size());

    for (size_t i = 0; i < this->m_vulkan->m_swapChain->swapChainImages.size(); i++) {
        // Create image view for every swap image
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = this->m_vulkan->m_swapChain->swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = this->m_vulkan->m_swapChain->swapChainImageFormat;
        // color mapping on images (set default, no engine feature needs this)
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        // no mip-mapping levels or multiple layers used. (not needed)
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        /*
         * Create the image view instance
         * NOTE: Usually images should be allocated using VMA, but swap chain images are an exception.
         *       This is because the swap chain has ownership over the images, not the developer.
         */
        VkResult result = vkCreateImageView(this->m_vulkan->m_logicalDevice->logicalDevice,
                                            &createInfo, nullptr, &this->swapChainImageViews.at(i));
        if (result != VK_SUCCESS) {
            spdlog::error("Failed to create Vulkan swap chain image view instance!");
            throw std::runtime_error("Failed to create Vulkan image views!");
        }
    }
}

SwapImageViews::~SwapImageViews() {
    for (size_t i = 0; i < this->swapChainImageViews.size(); i++) {
        vkDestroyImageView(this->m_vulkan->m_logicalDevice->logicalDevice,
                           this->swapChainImageViews[i], nullptr);
        this->swapChainImageViews[i] = VK_NULL_HANDLE; // less validation layer errors on clean up
    }
}

// --------- Static Helper Class for allocating images using VMA ---------- //

void ImageViews::allocateVMAImage(VmaAllocator allocator, AllocatedImage *allocatedImage, uint32_t width,
                                  uint32_t height, VkImageTiling tiling, VkSampleCountFlagBits msaaSamples,
                                  VkImageUsageFlags usageFlags, VkFormat imageFormat) {
    VkExtent3D imageExtent = {
        .width = width,
        .height = height,
        .depth = 1
    };
    const VkImageCreateInfo imageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = imageFormat,
        .extent = imageExtent,
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = msaaSamples,
        .tiling = tiling,
        .usage = usageFlags,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };
    const VmaAllocationCreateInfo allocCreateInfo = {
        .usage = VMA_MEMORY_USAGE_AUTO
    };

    VkResult result = vmaCreateImage(allocator, &imageCreateInfo, &allocCreateInfo,
                                     &allocatedImage->_imageInstance, &allocatedImage->_imageMemory, nullptr);
    if (result != VK_SUCCESS) {
        spdlog::error("An error occurred while allocating an image using VMA. Error Code: {}", result);
        throw std::runtime_error("Failed to allocate a new VMA image.\n");
    }
}

VkImageView ImageViews::createImageView(VkDevice logicalDevice, VkImage image,
                                        VkFormat format, VkImageAspectFlags aspectFlags) {

    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = image;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = format;

    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    createInfo.subresourceRange.aspectMask = aspectFlags;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    // Create the image view instance
    VkImageView imageView = VK_NULL_HANDLE;
    VkResult result = vkCreateImageView(logicalDevice, &createInfo, nullptr, &imageView);

    if (result != VK_SUCCESS) {
        spdlog::error("Failed to create an image view instance!");
        throw std::runtime_error("Failed to create image views!");
    }
    return imageView;
}

void ImageViews::transitionImageLayout(AllocatedImage allocatedImage, VkFormat format,
                                       VkImageLayout oldLayout, VkImageLayout newLayout) {
    return;
    // TODO: Will work on this once textures are implemented :) (not required for depth image buffer)
    /*
     * if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

            if (hasStencilComponent(format)) {
                barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        } else {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }
         if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        } else {
            throw std::invalid_argument("unsupported layout transition!");
        }
     */
}