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

#include "Vulkan.h"
#include <spdlog/spdlog.h>
#include <vk_mem_alloc.h>

Buffer::Buffer(Vulkan *m_vulkan, VkBufferUsageFlagBits bufferType,
               const std::vector<Vertex> *vertexData, const std::vector<uint32_t> *indexData): VkModuleBase(m_vulkan) {

    switch (bufferType) {
        case VK_BUFFER_USAGE_VERTEX_BUFFER_BIT:
            this->createVertexBuffer(*vertexData);
            break;
        case VK_BUFFER_USAGE_INDEX_BUFFER_BIT:
            this->createIndexBuffer(*indexData);
            break;
        default:
            spdlog::error("Unsupported VkBufferUsageFlagBit type given to Buffer constructor. Exiting..");
            throw std::runtime_error("Invalid buffer usage flag bit given to buffer constructor.");
    }
}

Buffer::~Buffer() {
    vmaDestroyBuffer(this->m_vulkan->m_VMA->memoryAllocator,
                     this->buffer._bufferInstance, this->buffer._bufferMemory);
}

void Buffer::createVertexBuffer(const std::vector<Vertex> vertices) {
    VkDeviceSize vertexBufferSize = sizeof(vertices[0]) * vertices.size();

    // Allocate the staging buffer using the allocateBuffer() helper function
    AllocatedBuffer stagingBuffer;
    this->allocateBuffer(&stagingBuffer, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
                         vertexBufferSize);

    // Map the vertex data over to the vertex buffer memory
    void* data;
    vmaMapMemory(this->m_vulkan->m_VMA->memoryAllocator, stagingBuffer._bufferMemory, &data);
    memcpy(data, vertices.data(), (size_t) vertexBufferSize);
    vmaUnmapMemory(this->m_vulkan->m_VMA->memoryAllocator, stagingBuffer._bufferMemory);

    // Allocate the device local (GPU) vertex buffer
    this->allocateBuffer(&this->buffer, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                            (VmaAllocationCreateFlagBits) 0, vertexBufferSize); // VMA defaults to device local memory

    // Copy the staging buffer on host RAM to the vertex buffer on GPU memory
    copyBuffer(stagingBuffer, this->buffer, vertexBufferSize);
    vmaDestroyBuffer(this->m_vulkan->m_VMA->memoryAllocator,
                     stagingBuffer._bufferInstance, stagingBuffer._bufferMemory);
}

void Buffer::createIndexBuffer(const std::vector<uint32_t> indices) {

    VkDeviceSize indexBufferSize = sizeof(indices[0]) * indices.size();

    // Allocate the staging buffer using the allocateBuffer() helper function
    AllocatedBuffer stagingBuffer;
    this->allocateBuffer(&stagingBuffer, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT, indexBufferSize);

    // Map the vertex data over to the vertex buffer memory
    void* data;
    vmaMapMemory(this->m_vulkan->m_VMA->memoryAllocator, stagingBuffer._bufferMemory, &data);
    memcpy(data, indices.data(), (size_t) indexBufferSize);
    vmaUnmapMemory(this->m_vulkan->m_VMA->memoryAllocator, stagingBuffer._bufferMemory);

    // Allocate the device local (GPU) vertex buffer
    this->allocateBuffer(&this->buffer, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                         (VmaAllocationCreateFlagBits) 0, indexBufferSize); // VMA defaults to device local memory

    // Copy the staging buffer on host RAM to the vertex buffer on GPU memory
    copyBuffer(stagingBuffer, this->buffer, indexBufferSize);
    vmaDestroyBuffer(this->m_vulkan->m_VMA->memoryAllocator,
                     stagingBuffer._bufferInstance, stagingBuffer._bufferMemory);
}

void Buffer::allocateBuffer(AllocatedBuffer *buffer, VkBufferUsageFlags usageTypeBit,
                            VmaAllocationCreateFlags allocationFlags, VkDeviceSize bufferSize) {

    // Create vertex buffer create info struct
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = usageTypeBit;

    bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT; // using both graphics and transfer queues
    const unsigned int uniqueQueueFamilies[] = {
            this->m_vulkan->m_physicalDevice->queueFamilies.graphicsFamily.value(),
            this->m_vulkan->m_physicalDevice->queueFamilies.transferFamily.value()
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
    VkResult bufferResult = vmaCreateBuffer(this->m_vulkan->m_VMA->memoryAllocator, &bufferInfo, &allocInfo,
                                      &buffer->_bufferInstance, &buffer->_bufferMemory, nullptr);
    if (bufferResult != VK_SUCCESS) {
        spdlog::error("An error occurred after attempting to allocate the Vertex Buffer using VMA.");
        throw std::runtime_error("Failed to create the Vulkan vertex buffer!");
    }
}

void Buffer::copyBuffer(AllocatedBuffer srcBuffer, AllocatedBuffer dstBuffer, VkDeviceSize bufferSize) {

    // Allocate a short-lived single command buffer in the transfer command pool
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = this->m_vulkan->m_transferCommandPool->commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(this->m_vulkan->m_logicalDevice->logicalDevice, &allocInfo, &commandBuffer);

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
    vkCmdCopyBuffer(commandBuffer, srcBuffer._bufferInstance, dstBuffer._bufferInstance, 1, &copyRegion);
    vkEndCommandBuffer(commandBuffer);

    /* Submit the command buffer to the dedicated transfer queue.
     * (transferQueue may be the same index as graphicsQueue if a
     * dedicated transfer queue family was not found on the GPU)
     */
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(this->m_vulkan->m_logicalDevice->transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(this->m_vulkan->m_logicalDevice->transferQueue);
    // free the temporary allocated command buffer
    vkFreeCommandBuffers(this->m_vulkan->m_logicalDevice->logicalDevice,
                         this->m_vulkan->m_transferCommandPool->commandPool, 1, &commandBuffer);
}