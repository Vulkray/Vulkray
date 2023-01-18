/*
 * FrameBuffers.cxx
 * Creates the Frame Buffers instances for drawing to the swap chain.
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

FrameBuffers::FrameBuffers(Vulkan *m_vulkan): VkModuleBase(m_vulkan) {

    // resize frame buffer vector to image views count
    this->swapChainFrameBuffers.resize(this->m_vulkan->m_imageViews->swapChainImageViews.size());

    for (size_t i = 0; i < this->m_vulkan->m_imageViews->swapChainImageViews.size(); i++) {

        std::array<VkImageView, 3> attachments = {
                this->m_vulkan->m_MSAA->msaaImageView, // color attachment resolve (multi-sampling/MSAA)
                this->m_vulkan->m_depthTesting->depthImageView, // depth attachment (depth testing)
                this->m_vulkan->m_imageViews->swapChainImageViews[i] // color attachment (fragment render)
        };
        VkFramebufferCreateInfo framebufferInfo{};

        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = this->m_vulkan->m_renderPass->renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = this->m_vulkan->m_swapChain->swapChainExtent.width;
        framebufferInfo.height = this->m_vulkan->m_swapChain->swapChainExtent.height;
        framebufferInfo.layers = 1;

        VkResult result = vkCreateFramebuffer(this->m_vulkan->m_logicalDevice->logicalDevice,
                                              &framebufferInfo, nullptr, &this->swapChainFrameBuffers.at(i));
        if (result != VK_SUCCESS) {
            spdlog::error("Failed to create the Vulkan framebuffer instances; Exiting.");
            throw std::runtime_error("Failed to create the framebuffer instances!");
        }
    }
}

FrameBuffers::~FrameBuffers() {
    for (size_t i = 0; i < this->swapChainFrameBuffers.size(); i++) {
        vkDestroyFramebuffer(this->m_vulkan->m_logicalDevice->logicalDevice,
                             this->swapChainFrameBuffers[i], nullptr);
        this->swapChainFrameBuffers[i] = VK_NULL_HANDLE; // less validation layer errors on clean up
    }
}