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

#include "Vulkan.hxx"

#include <spdlog/spdlog.h>

void Vulkan::initialize(const char* engineName, GLFWwindow *engineWindow) {

    spdlog::debug("Initializing Vulkan ...");
    VulkanInstance::createInstance(&this->vulkanInstance, engineName,
                                   this->enableValidationLayers, this->validationLayers);
    WindowSurface::createSurfaceKHR(&this->surface, this->vulkanInstance, engineWindow);
    PhysicalDevice::selectPhysicalDevice(&this->physicalDevice, this->vulkanInstance,
                                         this->surface, this->requiredExtensions);
    LogicalDevice::createLogicalDevice(&this->logicalDevice, &this->graphicsQueue, &this->presentQueue,
                                       this->physicalDevice, this->surface, this->requiredExtensions,
                                       this->enableValidationLayers, this->validationLayers);
    SwapChain::createSwapChain(&this->swapChain, &this->swapChainImages, &this->swapChainImageFormat,
                               &this->swapChainExtent, this->logicalDevice, this->physicalDevice,
                               this->surface, engineWindow);
    ImageViews::createImageViews(&this->swapChainImageViews, this->logicalDevice,
                                 this->swapChainImages, this->swapChainImageFormat);
    RenderPass::createRenderPass(&this->renderPass, this->logicalDevice, this->swapChainImageFormat);
    GraphicsPipeline::createGraphicsPipeline(&this->graphicsPipeline, &this->pipelineLayout, this->renderPass,
                                             this->logicalDevice, this->swapChainExtent);
    FrameBuffers::createFrameBuffers(&this->swapChainFrameBuffers, this->swapChainImageViews,
                                     this->logicalDevice, this->renderPass, this->swapChainExtent);
    CommandBuffer::createCommandPool(&this->commandPool, this->logicalDevice, this->physicalDevice, this->surface);
    CommandBuffer::createCommandBuffer(&this->commandBuffer, this->logicalDevice, this->commandPool);
    Synchronization::createSyncObjects(&this->imageAvailableSemaphore, &this->renderFinishedSemaphore,
                                       &this->inFlightFence, this->logicalDevice);
    spdlog::debug("Initialized Vulkan instances.");
}

// Synchronization / Command Buffer wrappers
void Vulkan::waitForDeviceIdle() {
    vkDeviceWaitIdle(this->logicalDevice);
}
void Vulkan::waitForPreviousFrame() {
    vkWaitForFences(this->logicalDevice, 1, &this->inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(this->logicalDevice, 1, &this->inFlightFence);
}
void Vulkan::getNextSwapChainImage(uint32_t *imageIndex) {
    vkAcquireNextImageKHR(this->logicalDevice, this->swapChain, UINT64_MAX,
                          this->imageAvailableSemaphore, VK_NULL_HANDLE, imageIndex);
}
void Vulkan::resetCommandBuffer(uint32_t imageIndex) {
    vkResetCommandBuffer(this->commandBuffer, 0);
    CommandBuffer::recordCommandBuffer(this->commandBuffer, imageIndex, this->graphicsPipeline,
                                       this->renderPass, this->swapChainFrameBuffers, this->swapChainExtent);
}
void Vulkan::submitCommandBuffer() {
    CommandBuffer::submitCommandBuffer(&this->commandBuffer, this->graphicsQueue, this->inFlightFence,
                                       this->imageAvailableSemaphore, this->renderFinishedSemaphore,
                                       this->waitSemaphores, this->signalSemaphores);
}
void Vulkan::presentImageBuffer(uint32_t *imageIndex) {
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = this->signalSemaphores;

    VkSwapchainKHR swapChains[] = {this->swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = imageIndex;
    presentInfo.pResults = nullptr; // optional

    vkQueuePresentKHR(this->presentQueue, &presentInfo);
}

void Vulkan::shutdown() {
    // Clean up synchronization objects
    vkDestroySemaphore(this->logicalDevice, this->imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(this->logicalDevice, this->renderFinishedSemaphore, nullptr);
    vkDestroyFence(this->logicalDevice, this->inFlightFence, nullptr);
    // Clean up Command Buffers
    vkDestroyCommandPool(this->logicalDevice, this->commandPool, nullptr);
    // Clean up Frame Buffers
    for (auto framebuffer : swapChainFrameBuffers) {
        vkDestroyFramebuffer(this->logicalDevice, framebuffer, nullptr);
    }
    // Clean up Pipeline instances
    vkDestroyPipeline(this->logicalDevice, this->graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(this->logicalDevice, this->pipelineLayout, nullptr);
    vkDestroyRenderPass(this->logicalDevice, this->renderPass, nullptr);
    // Clean up swap chainb Image Views
    for (auto imageView : this->swapChainImageViews) {
        vkDestroyImageView(this->logicalDevice, imageView, nullptr);
    }
    // Clean up Swap Chain, Vulkan logical & physical devices
    vkDestroySwapchainKHR(this->logicalDevice, this->swapChain, nullptr);
    vkDestroyDevice(this->logicalDevice, nullptr);
    vkDestroySurfaceKHR(this->vulkanInstance, this->surface, nullptr);
    vkDestroyInstance(this->vulkanInstance, nullptr);
}