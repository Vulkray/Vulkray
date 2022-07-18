/*
 * ImageViews.cxx
 * Creates the Vulkan image view instances for the swap chain buffers.
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

#ifndef VULKRAY_IMAGEVIEWS_HXX
#define VULKRAY_IMAGEVIEWS_HXX

#include <vulkan/vulkan.h>
#include <vector>

class ImageViews {
public:
    static void createImageViews(std::vector<VkImageView> swapChainImageViews, VkDevice logicalDevice,
                                 std::vector<VkImage> swapChainImages, VkFormat swapImageFormat);
};

#endif //VULKRAY_IMAGEVIEWS_HXX
