/*
 * VulkanMemoryAllocator.cxx
 * Creates the VMA Allocator instance using the VMA header-only library.
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2022, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#include <vk_mem_alloc.h>
#include "Vulkan.h"

VulkanMemoryAllocator::VulkanMemoryAllocator(Vulkan *m_vulkan): VkModuleBase(m_vulkan) {
    VmaVulkanFunctions vulkanFunctions = {};
    vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
    vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

    VmaAllocatorCreateInfo allocatorCreateInfo = {};
    allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
    allocatorCreateInfo.physicalDevice = this->m_vulkan->m_physicalDevice->physicalDevice;
    allocatorCreateInfo.device = this->m_vulkan->m_logicalDevice->logicalDevice;
    allocatorCreateInfo.instance = this->m_vulkan->m_vulkanInstance->vulkanInstance;
    allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;

    vmaCreateAllocator(&allocatorCreateInfo, &this->memoryAllocator);
}
VulkanMemoryAllocator::~VulkanMemoryAllocator() {
    vmaDestroyBuffer(this->memoryAllocator, this->m_vulkan->vertexBuffer._buffer,
                     this->m_vulkan->vertexBuffer._bufferMemory);
    vmaDestroyBuffer(this->memoryAllocator, this->m_vulkan->indexBuffer._buffer,
                     this->m_vulkan->indexBuffer._bufferMemory);
    vmaDestroyAllocator(this->memoryAllocator);
}