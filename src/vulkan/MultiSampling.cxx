/*
 * MultiSampling.cxx
 * Allocates the multi-sampling image buffer for smoothing 3D fragments.
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2023, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#include "../../include/Vulkray/Vulkan.h"

MultiSampling::MultiSampling(Vulkan *m_vulkan): VkModuleBase(m_vulkan) {

    VkFormat colorImageFormat = this->m_vulkan->m_swapChain->swapChainImageFormat;
    // Allocate MSAA color image buffer
    ImageViews::allocateVMAImage(this->m_vulkan->m_VMA->memoryAllocator, &this->msaaImage,
                                 this->m_vulkan->m_swapChain->swapChainExtent.width,
                                 this->m_vulkan->m_swapChain->swapChainExtent.height,
                                 VK_IMAGE_TILING_OPTIMAL, this->m_vulkan->m_physicalDevice->msaaSamples,
                                 VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                 colorImageFormat);
    this->msaaImageView = ImageViews::createImageView(this->m_vulkan->m_logicalDevice->logicalDevice,
                                                      this->msaaImage._imageInstance, colorImageFormat,
                                                      VK_IMAGE_ASPECT_COLOR_BIT);
}

MultiSampling::~MultiSampling() {
    vmaDestroyImage(this->m_vulkan->m_VMA->memoryAllocator,
                    this->msaaImage._imageInstance, this->msaaImage._imageMemory);
    vkDestroyImageView(this->m_vulkan->m_logicalDevice->logicalDevice, this->msaaImageView, nullptr);
}