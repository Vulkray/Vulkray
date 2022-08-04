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
                                 QueueFamilyIndices queueIndices, const std::vector <Vertex> vertices) {
    // Allocate the vertex buffer using the allocateBuffer() helper function
    Buffers::allocateBuffer(vertexBuffer, allocator, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                            sizeof(vertices[0]) * vertices.size(), queueIndices);

    // Map the vertex data over to the vertex buffer memory
    void* data;
    vmaMapMemory(allocator, vertexBuffer->_bufferMemory, &data);
    memcpy(data, vertices.data(), (size_t) sizeof(vertices[0]) * vertices.size());
    vmaUnmapMemory(allocator, vertexBuffer->_bufferMemory);
}

void Buffers::allocateBuffer(AllocatedBuffer *buffer, VmaAllocator allocator, VkBufferUsageFlagBits usageTypeBit,
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
    allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT; // required for vmaMapMemory()
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