/*
 * Vulkan.cxx
 * Initializes and manages all the engine's Vulkan instances.
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

#ifndef VULKRAY_VULKAN_HXX
#define VULKRAY_VULKAN_HXX

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <vector>

class Vulkan {
public:
    void initialize(const char* engineName, GLFWwindow *engineWindow); // Initializes the class and its instances.
    void shutdown(); // Cleans up & terminates all Vulkan instances.
private:
    // Vulkan validation layers
    const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif
    // Vulkan instances
    VkInstance vulkanInstance{};
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice logicalDevice = VK_NULL_HANDLE;
    VkSurfaceKHR surface;
    const std::vector<const char*> requiredExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    std::vector<VkFramebuffer> swapChainFrameBuffers;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    // GPU queue handles
    VkQueue graphicsQueue;
    VkQueue presentQueue;
};

#endif //VULKRAY_VULKAN_HXX
