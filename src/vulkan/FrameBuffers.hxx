/*
 * FrameBuffers.cxx
 * Creates the Frame Buffers instances for drawing to the swap chain.
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

#ifndef VULKRAY_FRAMEBUFFERS_HXX
#define VULKRAY_FRAMEBUFFERS_HXX

#include <vulkan/vulkan.h>
#include <vector>

class FrameBuffers {
public:
    static void createFrameBuffers(std::vector<VkFramebuffer> swapChainFrameBuffers,
                                   std::vector<VkImageView> swapChainImageViews,
                                   VkDevice logicalDevice, VkRenderPass renderPass, VkExtent2D swapChainExtent);
};

#endif //VULKRAY_FRAMEBUFFERS_HXX
