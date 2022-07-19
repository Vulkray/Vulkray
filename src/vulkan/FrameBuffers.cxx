/*
 * FrameBuffers.cxx
 * Creates the Frame Buffers instances for drawing to the swap chain.
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

#include "FrameBuffers.hxx"

#include <spdlog/spdlog.h>

void FrameBuffers::createFrameBuffers(std::vector<VkFramebuffer> swapChainFrameBuffers,
                                      std::vector<VkImageView> swapChainImageViews,
                                      VkDevice logicalDevice, VkRenderPass renderPass, VkExtent2D swapChainExtent) {

    // resize frame buffer vector to image views count
    swapChainFrameBuffers.resize(swapChainImageViews.size());

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

        VkResult result = vkCreateFramebuffer(logicalDevice, &framebufferInfo, nullptr, &swapChainFrameBuffers[i]);
        if (result != VK_SUCCESS) {
            spdlog::error("Failed to create the Vulkan framebuffer instances; Exiting.");
            throw std::runtime_error("Failed to create the framebuffer instances!");
        }
    }
}