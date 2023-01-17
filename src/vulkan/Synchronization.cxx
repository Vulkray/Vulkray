/*
 * Synchronization.cxx
 * Creates the Vulkan semaphores/fences instances for GPU/CPU synchronization.
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

Synchronization::Synchronization(Vulkan *m_vulkan): VkModuleBase(m_vulkan) {

    // resize semaphore and fence vectors to max frames in flight value
    this->imageAvailableSemaphores.resize(this->m_vulkan->MAX_FRAMES_IN_FLIGHT);
    this->renderFinishedSemaphores.resize(this->m_vulkan->MAX_FRAMES_IN_FLIGHT);
    this->inFlightFences.resize(this->m_vulkan->MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // start fence signaled (to render first frame)

    // create each semaphore & fence for each frame that can be drawn in flight
    for (unsigned int i = 0; i < this->m_vulkan->MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(this->m_vulkan->m_logicalDevice->logicalDevice, &semaphoreInfo, nullptr,
                              &this->imageAvailableSemaphores.at(i)) != VK_SUCCESS ||
            vkCreateSemaphore(this->m_vulkan->m_logicalDevice->logicalDevice, &semaphoreInfo, nullptr,
                              &this->renderFinishedSemaphores.at(i)) != VK_SUCCESS ||
            vkCreateFence(this->m_vulkan->m_logicalDevice->logicalDevice, &fenceInfo, nullptr,
                          &this->inFlightFences.at(i)) != VK_SUCCESS) {

            spdlog::error("An error occurred while initializing the Vulkan semaphores and fence instances.");
            throw std::runtime_error("Failed to create the synchronization objects!");
        }
    }
}

Synchronization::~Synchronization() {
    for (unsigned int i = 0; i < this->m_vulkan->MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(this->m_vulkan->m_logicalDevice->logicalDevice,
                           this->imageAvailableSemaphores.at(i), nullptr);
        vkDestroySemaphore(this->m_vulkan->m_logicalDevice->logicalDevice,
                           this->renderFinishedSemaphores.at(i), nullptr);
        vkDestroyFence(this->m_vulkan->m_logicalDevice->logicalDevice,
                       this->inFlightFences.at(i), nullptr);
    }
}