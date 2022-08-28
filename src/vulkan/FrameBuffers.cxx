/*
 * FrameBuffers.cxx
 * Creates the Frame Buffers instances for drawing to the swap chain.
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

void FrameBuffers::createFrameBuffers(std::vector<VkFramebuffer> *swapChainFrameBuffers,
                                      std::vector<VkImageView> swapChainImageViews,
                                      VkDevice logicalDevice, VkRenderPass renderPass, VkExtent2D swapChainExtent) {

    // resize frame buffer vector to image views count
    swapChainFrameBuffers->resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {

        VkImageView attachments[] = {swapChainImageViews[i]};
        VkFramebufferCreateInfo framebufferInfo{};

        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        VkResult result = vkCreateFramebuffer(logicalDevice, &framebufferInfo, nullptr, &swapChainFrameBuffers->at(i));
        if (result != VK_SUCCESS) {
            spdlog::error("Failed to create the Vulkan framebuffer instances; Exiting.");
            throw std::runtime_error("Failed to create the framebuffer instances!");
        }
    }
}