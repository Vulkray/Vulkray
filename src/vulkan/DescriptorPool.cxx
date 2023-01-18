/*
 * DescriptorPool.cxx
 * Sets up the descriptor pool for passing UBO data to the graphics pipeline.
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

DescriptorPool::DescriptorPool(Vulkan *m_vulkan): VkModuleBase(m_vulkan) {

    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(this->m_vulkan->MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(this->m_vulkan->MAX_FRAMES_IN_FLIGHT);

    VkResult result = vkCreateDescriptorPool(this->m_vulkan->m_logicalDevice->logicalDevice,
                                             &poolInfo, nullptr, &this->descriptorPool);
    if (result != VK_SUCCESS) {
        spdlog::error("An error occurred while trying to initialize the descriptor pool!");
        throw std::runtime_error("Failed to create the descriptor pool instance.");
    }

    // Create the descriptor set layout to use for the pool
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; // Use descriptor only at the vertex stage
    uboLayoutBinding.pImmutableSamplers = nullptr; // optional

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    result = vkCreateDescriptorSetLayout(this->m_vulkan->m_logicalDevice->logicalDevice,
                                         &layoutInfo, nullptr, &this->descriptorSetLayout);
    if (result != VK_SUCCESS) {
        spdlog::error("An error occurred while trying to create the descriptor set layout.");
        throw std::runtime_error("Failed to create descriptor set layout!");
    }

    this->descriptorSets.resize(this->m_vulkan->MAX_FRAMES_IN_FLIGHT);
    std::vector<VkDescriptorSetLayout> layouts(this->m_vulkan->MAX_FRAMES_IN_FLIGHT, this->descriptorSetLayout);

    // Allocate descriptor sets to the descriptor pool
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = this->descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(this->m_vulkan->MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    result = vkAllocateDescriptorSets(this->m_vulkan->m_logicalDevice->logicalDevice,
                                      &allocInfo, this->descriptorSets.data());
    if (result != VK_SUCCESS) {
        spdlog::error("An error occurred attempting to allocate descriptor sets to the descriptor pool!");
        throw std::runtime_error("Failed to allocate descriptor sets.");
    }

    // Configure all descriptor buffers in the pool
    for (size_t i = 0; i < this->m_vulkan->MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = this->m_vulkan->m_uniformBuffers.at(i)->buffer._bufferInstance;
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);
        // Update the descriptor set
        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = this->descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = nullptr; // optional
        descriptorWrite.pTexelBufferView = nullptr; // optional
        vkUpdateDescriptorSets(this->m_vulkan->m_logicalDevice->logicalDevice, 1, &descriptorWrite, 0, nullptr);
    }
}

DescriptorPool::~DescriptorPool() {
    vkDestroyDescriptorPool(this->m_vulkan->m_logicalDevice->logicalDevice, this->descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(this->m_vulkan->m_logicalDevice->logicalDevice, this->descriptorSetLayout, nullptr);
}