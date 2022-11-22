/*
 * DepthTesting.cxx
 * Handles creating the Depth image buffer for 3D fragment depth testing.
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2022, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#include "../../include/Vulkray/Vulkan.h"
#include <spdlog/spdlog.h>

DepthTesting::DepthTesting(Vulkan *m_vulkan): VkModuleBase(m_vulkan) {

    // Find a suitable depth format supported by the GPU
    VkFormat depthFormat = this->m_vulkan->m_physicalDevice->findDepthFormat();

    ImageViews::allocateVMAImage(this->m_vulkan->m_VMA->memoryAllocator, &this->depthImage,
                                 this->m_vulkan->m_swapChain->swapChainExtent.width,
                                 this->m_vulkan->m_swapChain->swapChainExtent.height,
                                 VK_IMAGE_TILING_OPTIMAL, this->m_vulkan->m_physicalDevice->msaaSamples,
                                 VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, depthFormat);

    this->depthImageView = ImageViews::createImageView(this->m_vulkan->m_logicalDevice->logicalDevice,
                                                       this->depthImage._imageInstance,
                                                       depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
    // Currently does nothing; Will implement logic once textures are implemented.
    ImageViews::transitionImageLayout(this->depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED,
                                      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

DepthTesting::~DepthTesting() {
    vmaDestroyImage(this->m_vulkan->m_VMA->memoryAllocator,
                    this->depthImage._imageInstance, this->depthImage._imageMemory);
    vkDestroyImageView(this->m_vulkan->m_logicalDevice->logicalDevice, this->depthImageView, nullptr);
}