/*
 * VertexBuffer.cxx
 * Allocates the Vertex Buffer for the vertex shader using the VMA allocator.
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

void VertexBuffer::createVertexBuffer(VkBuffer *vertexBuffer, VmaAllocation *allocation,
                                      VmaAllocator allocator, const std::vector<Vertex> vertices) {
    // Create vertex buffer create info struct
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(vertices[0]) * vertices.size();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

    if (vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, vertexBuffer, allocation, nullptr) != VK_SUCCESS) {
        spdlog::error("An error occurred after attempting to allocate the Vertex Buffer using VMA.");
        throw std::runtime_error("Failed to create the Vulkan vertex buffer!");
    }
}