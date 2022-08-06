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

#include "Vulkan.hxx"

#include <spdlog/spdlog.h>

void CommandBuffer::createCommandPool(VkCommandPool *commandPool, VkCommandPoolCreateFlags additionalFlags,
                                      VkDevice logicalDevice, uint32_t queueIndex) {

    // Configure & create the command pool instance
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | additionalFlags;
    poolInfo.queueFamilyIndex = queueIndex;

    VkResult result = vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, commandPool);
    if (result != VK_SUCCESS) {
        spdlog::error("An error occurred while initializing the Vulkan command pool instance.");
        throw std::runtime_error("Failed to create the command pool instance!");
    }
}

void CommandBuffer::createCommandBuffer(std::vector<VkCommandBuffer> *commandBuffers, const int MAX_FRAMES_IN_FLIGHT,
                                        VkDevice logicalDevice, VkCommandPool commandPool) {

    // Resize command buffer vector to max frames in flight value
    commandBuffers->resize(MAX_FRAMES_IN_FLIGHT);

    // Configure & create the command buffer instance
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // primary buffer (will be executed directly)
    allocInfo.commandBufferCount = (uint32_t) commandBuffers->size();

    VkResult result = vkAllocateCommandBuffers(logicalDevice, &allocInfo, commandBuffers->data());
    if (result != VK_SUCCESS) {
        spdlog::error("An error occurred while allocating the Vulkan command buffers.");
        throw std::runtime_error("Failed to allocate the command buffers!");
    }
}

void CommandBuffer::recordGraphicsCommands(VkCommandBuffer commandBuffer, uint32_t imageIndex,
                                        VkPipeline graphicsPipeline, VkRenderPass renderPass,
                                        std::vector<VkFramebuffer> swapFrameBuffers,
                                        AllocatedBuffer vertexBuffer, AllocatedBuffer indexBuffer,
                                        GraphicsInput graphicsInput, VkExtent2D swapExtent) {

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // optional
    beginInfo.pInheritanceInfo = nullptr; // optional

    // Start recording to the command buffer
    VkResult result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    if (result != VK_SUCCESS) {
        spdlog::error("An error occurred while trying to start recording to a command buffer.");
        throw std::runtime_error("Failed to begin recording the command buffer!");
    }

    // Start configuring the render pass
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapFrameBuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapExtent;
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &graphicsInput.bufferClearColor;

    // Submit (record) command to begin render pass
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    // Record binding the graphics pipeline to the command buffer
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    // Bind the geometry buffers to the command buffer
    VkBuffer vertexBuffers[] = { vertexBuffer._buffer };
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer._buffer, 0, VK_INDEX_TYPE_UINT32);

    // Record setting the viewport
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapExtent.width);
    viewport.height = static_cast<float>(swapExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    // Record scissor configuration
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    // Submit (record) the draw command and end the render pass
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(graphicsInput.indices.size()), 1, 0, 0, 0);
    vkCmdEndRenderPass(commandBuffer);

    // Finish recording to the command buffer
    result = vkEndCommandBuffer(commandBuffer);
    if (result != VK_SUCCESS) {
        spdlog::error("An error occurred while trying to stop recording to a command buffer.");
        throw std::runtime_error("Failed to stop recording the command buffer!");
    }
}

void CommandBuffer::submitCommandBuffer(VkCommandBuffer *commandBuffer, VkQueue graphicsQueue, VkFence inFlightFence,
                                        VkSemaphore imageAvailableSemaphore, VkSemaphore renderFinishedSemaphore,
                                        VkSemaphore waitSemaphores[], VkSemaphore signalSemaphores[]) {

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    waitSemaphores[0] = imageAvailableSemaphore;
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = commandBuffer;
    signalSemaphores[0] = renderFinishedSemaphore;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    VkResult result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence);
    if (result != VK_SUCCESS) {
        spdlog::error("An error occurred while submitting a command buffer to the graphics queue.");
        throw std::runtime_error("Failed to submit the draw command buffer!");
    }
}