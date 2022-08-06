/*
 * Buffers.cxx
 * Allocates the engine buffers for feeding the graphics pipeline using the VMA allocator.
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
#include <vk_mem_alloc.h>

void Buffers::createVertexBuffer(AllocatedBuffer *vertexBuffer, VmaAllocator allocator,
                                 QueueFamilyIndices queueIndices, const std::vector <Vertex> vertices,
                                 VkDevice logicalDevice, VkCommandPool transferPool, VkQueue transferQueue) {

    VkDeviceSize vertexBufferSize = sizeof(vertices[0]) * vertices.size();

    // Allocate the staging buffer using the allocateBuffer() helper function
    AllocatedBuffer stagingBuffer;
    Buffers::allocateBuffer(&stagingBuffer, allocator,
                            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
                            vertexBufferSize, queueIndices);

    // Map the vertex data over to the vertex buffer memory
    void* data;
    vmaMapMemory(allocator, stagingBuffer._bufferMemory, &data);
    memcpy(data, vertices.data(), (size_t) vertexBufferSize);
    vmaUnmapMemory(allocator, stagingBuffer._bufferMemory);

    // Allocate the device local (GPU) vertex buffer
    Buffers::allocateBuffer(vertexBuffer, allocator,
                            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                            (VmaAllocationCreateFlagBits) 0, // VMA allocates device local memory on auto
                            vertexBufferSize, queueIndices);

    // Copy the staging buffer on host RAM to the vertex buffer on GPU memory
    copyBuffer(stagingBuffer, *vertexBuffer, vertexBufferSize, logicalDevice, transferPool, transferQueue);
    vmaDestroyBuffer(allocator, stagingBuffer._buffer, stagingBuffer._bufferMemory);
}

void Buffers::createIndexBuffer(AllocatedBuffer *indexBuffer, VmaAllocator allocator,
                                QueueFamilyIndices queueIndices, const std::vector<uint32_t> indices,
                                VkDevice logicalDevice, VkCommandPool transferPool, VkQueue transferQueue) {

    VkDeviceSize indexBufferSize = sizeof(indices[0]) * indices.size();

    // Allocate the staging buffer using the allocateBuffer() helper function
    AllocatedBuffer stagingBuffer;
    Buffers::allocateBuffer(&stagingBuffer, allocator,
                            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
                            indexBufferSize, queueIndices);

    // Map the vertex data over to the vertex buffer memory
    void* data;
    vmaMapMemory(allocator, stagingBuffer._bufferMemory, &data);
    memcpy(data, indices.data(), (size_t) indexBufferSize);
    vmaUnmapMemory(allocator, stagingBuffer._bufferMemory);

    // Allocate the device local (GPU) vertex buffer
    Buffers::allocateBuffer(indexBuffer, allocator,
                            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                            (VmaAllocationCreateFlagBits) 0, // VMA allocates device local memory on auto
                            indexBufferSize, queueIndices);

    // Copy the staging buffer on host RAM to the vertex buffer on GPU memory
    copyBuffer(stagingBuffer, *indexBuffer, indexBufferSize, logicalDevice, transferPool, transferQueue);
    vmaDestroyBuffer(allocator, stagingBuffer._buffer, stagingBuffer._bufferMemory);
}

void Buffers::allocateBuffer(AllocatedBuffer *buffer, VmaAllocator allocator, VkBufferUsageFlags usageTypeBit,
                             VmaAllocationCreateFlags allocationFlags,
                             VkDeviceSize bufferSize, QueueFamilyIndices queueIndices) {

    // Create vertex buffer create info struct
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = usageTypeBit;

    bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT; // using both graphics and transfer queues
    const unsigned int uniqueQueueFamilies[] = {
            queueIndices.graphicsFamily.value(),
            queueIndices.transferFamily.value()
    };
    bufferInfo.pQueueFamilyIndices = uniqueQueueFamilies;
    bufferInfo.queueFamilyIndexCount = 2;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.flags = allocationFlags; // `VMA_ALLOCATION_CREATE_HOST_ACCESS_*` required for vmaMapMemory()
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

    /* Vulkan Memory Allocator makes allocating the vertex buffer so much easier.
     * Note: vmaCreateBuffer() doesn't just create the buffer instance, but also allocates the
     * required memory for the buffer according to its needs and binds the memory to the buffer for you.
     */
    VkResult bufferResult = vmaCreateBuffer(allocator, &bufferInfo, &allocInfo,
                                      &buffer->_buffer, &buffer->_bufferMemory, nullptr);
    if (bufferResult != VK_SUCCESS) {
        spdlog::error("An error occurred after attempting to allocate the Vertex Buffer using VMA.");
        throw std::runtime_error("Failed to create the Vulkan vertex buffer!");
    }
}

void Buffers::copyBuffer(AllocatedBuffer srcBuffer, AllocatedBuffer dstBuffer, VkDeviceSize bufferSize,
                         VkDevice logicalDevice, VkCommandPool commandPool, VkQueue transferQueue) {

    // Allocate a short-lived single command buffer in the transfer command pool
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer);

    // Begin recording to the temporary transfer command buffer
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    // Record the copy buffer command and end the buffer recording
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // optional
    copyRegion.dstOffset = 0; // optional
    copyRegion.size = bufferSize;
    vkCmdCopyBuffer(commandBuffer, srcBuffer._buffer, dstBuffer._buffer, 1, &copyRegion);
    vkEndCommandBuffer(commandBuffer);

    /* Submit the command buffer to the dedicated transfer queue.
     * (transferQueue may be the same index as graphicsQueue if a
     * dedicated transfer queue family was not found on the GPU)
     */
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(transferQueue);
    // free the temporary allocated command buffer
    vkFreeCommandBuffers(logicalDevice, commandPool, 1, &commandBuffer);
}