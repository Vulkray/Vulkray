/*
 * Synchronization.cxx
 * Creates the Vulkan semaphores / fences instances for GPU/CPU sync.
 *
 * Copyright 2022 Max Rodriguez

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 */

#include "Vulkan.hxx"
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