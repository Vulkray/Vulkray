/*
 * ImageViews.cxx
 * Creates the Vulkan image view instances for the swap chain buffers.
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

#include "ImageViews.hxx"

#include <spdlog/spdlog.h>

void ImageViews::createImageViews(std::vector <VkImageView> swapChainImageViews, VkDevice logicalDevice,
                                  std::vector <VkImage> swapChainImages, VkFormat swapImageFormat) {

    // Resize image views vector to available swap chain images
    swapChainImageViews.resize(swapChainImages.size());

    for (size_t i = 0; i < swapChainImages.size(); i++) {
        // Create image view for every swap image
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapImageFormat;
        // color mapping on images (set default, no special engine feature needs this)
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

        // Create the image view instance
        VkResult result = vkCreateImageView(logicalDevice, &createInfo, nullptr, &swapChainImageViews[i]);
        if (result != VK_SUCCESS) {
            spdlog::error("Failed to create Vulkan swap chain image view instance!");
            throw std::runtime_error("Failed to create Vulkan image views!");
        }
    }
}