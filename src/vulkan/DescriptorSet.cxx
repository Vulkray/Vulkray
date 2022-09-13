/*
 * DescriptorSet.cxx
 * Sets up the descriptor set for passing UBO memory to the graphics pipeline.
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

DescriptorSet::DescriptorSet(Vulkan *m_vulkan): VkModuleBase(m_vulkan) {

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

    VkResult result = vkCreateDescriptorSetLayout(this->m_vulkan->m_logicalDevice->logicalDevice,
                                                  &layoutInfo, nullptr, &this->descriptorSetLayout);
    if (result != VK_SUCCESS) {
        spdlog::error("An error occurred while trying to create the descriptor set layout.");
        throw std::runtime_error("Failed to create descriptor set layout!");
    }
}

DescriptorSet::~DescriptorSet() {
    vkDestroyDescriptorSetLayout(this->m_vulkan->m_logicalDevice->logicalDevice,
                                 this->descriptorSetLayout, nullptr);
}