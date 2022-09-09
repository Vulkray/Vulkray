/*
 * CommandPool.cxx
 * Creates the Vulkan command pool instances for rendering operations.
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

CommandPool::CommandPool(Vulkan *m_vulkan, VkCommandPoolCreateFlags additionalFlags,
                         uint32_t queueIndex): VkModuleBase(m_vulkan) {

    this->createCommandPool(additionalFlags, queueIndex);
    this->createCommandBuffer();
}

CommandPool::~CommandPool() {
    vkDestroyCommandPool(this->m_vulkan->m_logicalDevice->logicalDevice, this->commandPool, nullptr);
}

void CommandPool::createCommandPool(VkCommandPoolCreateFlags additionalFlags, uint32_t queueIndex) {

    // Configure & create the command pool instance
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | additionalFlags;
    poolInfo.queueFamilyIndex = queueIndex;

    VkResult result = vkCreateCommandPool(this->m_vulkan->m_logicalDevice->logicalDevice,
                                          &poolInfo, nullptr, &this->commandPool);
    if (result != VK_SUCCESS) {
        spdlog::error("An error occurred while initializing the Vulkan command pool instance.");
        throw std::runtime_error("Failed to create the command pool instance!");
    }
}

void CommandPool::createCommandBuffer() {

    // Resize command buffer vector to max frames in flight value
    this->commandBuffers.resize(this->m_vulkan->MAX_FRAMES_IN_FLIGHT);

    // Configure & create the command buffer instance
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = this->commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // primary buffer (will be executed directly)
    allocInfo.commandBufferCount = (uint32_t) this->commandBuffers.size();

    VkResult result = vkAllocateCommandBuffers(this->m_vulkan->m_logicalDevice->logicalDevice,
                                               &allocInfo, this->commandBuffers.data());
    if (result != VK_SUCCESS) {
        spdlog::error("An error occurred while allocating the Vulkan command buffers.");
        throw std::runtime_error("Failed to allocate the command buffers!");
    }
}

// Only applies to the graphics command pool instance
void CommandPool::recordGraphicsCommands(uint32_t imageIndex) {

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // optional
    beginInfo.pInheritanceInfo = nullptr; // optional

    // Start recording to the command buffer
    VkResult result = vkBeginCommandBuffer(this->commandBuffers[this->m_vulkan->frameIndex], &beginInfo);
    if (result != VK_SUCCESS) {
        spdlog::error("An error occurred while trying to start recording to a command buffer.");
        throw std::runtime_error("Failed to begin recording the command buffer!");
    }

    // Start configuring the render pass
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = this->m_vulkan->m_renderPass->renderPass;
    renderPassInfo.framebuffer = this->m_vulkan->m_frameBuffers->swapChainFrameBuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = this->m_vulkan->m_swapChain->swapChainExtent;
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &this->m_vulkan->graphicsInput.bufferClearColor;

    // Submit (record) command to begin render pass
    vkCmdBeginRenderPass(this->commandBuffers[this->m_vulkan->frameIndex],
                         &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    // Record binding the graphics pipeline to the command buffer
    vkCmdBindPipeline(this->commandBuffers[this->m_vulkan->frameIndex],
                      VK_PIPELINE_BIND_POINT_GRAPHICS, this->m_vulkan->m_graphicsPipeline->graphicsPipeline);

    // Bind the geometry buffers to the command buffer
    VkBuffer vertexBuffers[] = { this->m_vulkan->m_vertexBuffer->buffer._bufferInstance };
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(this->commandBuffers[this->m_vulkan->frameIndex],
                           0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(this->commandBuffers[this->m_vulkan->frameIndex],
                         this->m_vulkan->m_indexBuffer->buffer._bufferInstance, 0, VK_INDEX_TYPE_UINT32);

    // Record setting the viewport
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(this->m_vulkan->m_swapChain->swapChainExtent.width);
    viewport.height = static_cast<float>(this->m_vulkan->m_swapChain->swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(this->commandBuffers[this->m_vulkan->frameIndex], 0, 1, &viewport);

    // Record scissor configuration
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = this->m_vulkan->m_swapChain->swapChainExtent;
    vkCmdSetScissor(this->commandBuffers[this->m_vulkan->frameIndex], 0, 1, &scissor);

    // Submit (record) the draw command and end the render pass
    vkCmdDrawIndexed(this->commandBuffers[this->m_vulkan->frameIndex],
                     static_cast<uint32_t>(this->m_vulkan->graphicsInput.indexData.size()), 1, 0, 0, 0);
    vkCmdEndRenderPass(this->commandBuffers[this->m_vulkan->frameIndex]);

    // Finish recording to the command buffer
    result = vkEndCommandBuffer(this->commandBuffers[this->m_vulkan->frameIndex]);
    if (result != VK_SUCCESS) {
        spdlog::error("An error occurred while trying to stop recording to a command buffer.");
        throw std::runtime_error("Failed to stop recording the command buffer!");
    }
}

void CommandPool::submitNextCommandBuffer() {

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // Synchronization module pointer reference to shorten code
    Synchronization *m_synchronization = this->m_vulkan->m_synchronization.get();

    m_synchronization->waitSemaphores[0] = m_synchronization->imageAvailableSemaphores[this->m_vulkan->frameIndex];
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = m_synchronization->waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &this->commandBuffers[this->m_vulkan->frameIndex];
    m_synchronization->signalSemaphores[0] = m_synchronization->renderFinishedSemaphores[this->m_vulkan->frameIndex];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = m_synchronization->signalSemaphores;

    VkResult result = vkQueueSubmit(this->m_vulkan->m_logicalDevice->graphicsQueue, 1, &submitInfo,
                                    m_synchronization->inFlightFences[this->m_vulkan->frameIndex]);
    if (result != VK_SUCCESS) {
        spdlog::error("An error occurred while submitting a command buffer to the graphics queue.");
        throw std::runtime_error("Failed to submit the draw command buffer!");
    }
}

void CommandPool::resetGraphicsCmdBuffer(uint32_t imageIndex) {
    vkResetCommandBuffer(this->commandBuffers[this->m_vulkan->frameIndex], 0);
    this->recordGraphicsCommands(imageIndex);
}