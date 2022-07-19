/*
 * CommandPool.cxx
 * Creates the Vulkan command pool instances for rendering operations.
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

#ifndef VULKRAY_COMMANDPOOL_HXX
#define VULKRAY_COMMANDPOOL_HXX

#include <vulkan/vulkan.h>
#include <vector>

class CommandBuffer {
public:
    static void createCommandPool(VkCommandPool *commandPool, VkDevice logicalDevice,
                                  VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    static void createCommandBuffer(VkCommandBuffer *commandBuffer,
                                    VkDevice logicalDevice, VkCommandPool commandPool);
    static void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex,
                                    VkPipeline graphicsPipeline, VkRenderPass renderPass,
                                    std::vector<VkFramebuffer> swapFrameBuffers, VkExtent2D swapExtent);
};

#endif //VULKRAY_COMMANDPOOL_HXX
