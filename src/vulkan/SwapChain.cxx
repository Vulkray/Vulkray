/*
 * SwapChain.cxx
 * Creates the Vulkan swap chain instance and swap chain buffers.
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

#include "SwapChain.hxx"

void SwapChain::createSwapChain() {
    return; // placeholder; temporary!
}

SwapChainSupportDetails SwapChain::querySwapChainSupport(VkPhysicalDevice gpuDevice, VkSurfaceKHR surface) {
    // Get GPU device surface capabilities
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpuDevice, surface, &details.capabilities);

    // Get GPU device surface formats
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpuDevice, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpuDevice, surface, &formatCount, details.formats.data());
    }

    // Get GPU device present modes
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(gpuDevice, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(gpuDevice, surface, &presentModeCount, details.presentModes.data());
    }
    return details; // return filled out GPU support details
}