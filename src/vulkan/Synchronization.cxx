/*
 * Synchronization.cxx
 * Creates the Vulkan semaphores / fences instances for GPU/CPU sync.
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

void Synchronization::createSyncObjects(std::vector<VkSemaphore> *imageAvailableSemaphores,
                                        std::vector<VkSemaphore> *renderFinishedSemaphores,
                                        std::vector<VkFence> *inFlightFences, VkDevice logicalDevice,
                                        const int MAX_FRAMES_IN_FLIGHT) {

    // resize semaphore and fence vectors to max frames in flight value
    imageAvailableSemaphores->resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores->resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences->resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // start fence signaled (to render first frame)

    // create each semaphore & fence for each frame that can be drawn in flight
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphores->at(i)) != VK_SUCCESS ||
            vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores->at(i)) != VK_SUCCESS ||
            vkCreateFence(logicalDevice, &fenceInfo, nullptr, &inFlightFences->at(i)) != VK_SUCCESS) {
            spdlog::error("An error occurred while initializing the Vulkan semaphores and fence instances.");
            throw std::runtime_error("Failed to create the synchronization objects!");
        }
    }
}