/*
 * Vulkan.cxx
 * Initializes and manages all the engine's Vulkan instances.
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2022, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
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
    CommandBuffer::createCommandBuffer(&this->commandBuffers, this->MAX_FRAMES_IN_FLIGHT,
                                       this->logicalDevice, this->commandPool);
    Synchronization::createSyncObjects(&this->imageAvailableSemaphores, &this->renderFinishedSemaphores,
                                       &this->inFlightFences, this->logicalDevice, this->MAX_FRAMES_IN_FLIGHT);
    spdlog::debug("Initialized Vulkan instances.");
}

// Synchronization / Command Buffer wrappers
void Vulkan::waitForDeviceIdle() {
    vkDeviceWaitIdle(this->logicalDevice);
}

void Vulkan::waitForPreviousFrame(uint32_t frameIndex) {
    vkWaitForFences(this->logicalDevice, 1, &this->inFlightFences[frameIndex], VK_TRUE, UINT64_MAX);
}

void Vulkan::getNextSwapChainImage(uint32_t *imageIndex, uint32_t frameIndex,
                                   bool *windowResized, GLFWwindow *window) {

    // acquire next image view, also get swap chain status
    VkResult result = vkAcquireNextImageKHR(this->logicalDevice, this->swapChain, UINT64_MAX,
                          this->imageAvailableSemaphores[frameIndex], VK_NULL_HANDLE, imageIndex);
    /* check if vkAcquireNextImageKHR returned an out of date framebuffer flag
     * Note: this is not a feature on all Vulkan compatible drivers! also checking via GLFW resize callback!
     */
    if (result == VK_ERROR_OUT_OF_DATE_KHR || *windowResized) {
        this->recreateSwapChain(window);
        *windowResized = false; // reset GLFW triggered framebuffer resized flag
        return;
    // TODO: Handle VK_SUBOPTIMAL_KHR status code
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        spdlog::error("An error occurred when acquiring the next swap chain image view; Exiting.");
        throw std::runtime_error("Failed to acquire swap chain image!");
    }
    // reset fence only if we know we're submitting work
    vkResetFences(this->logicalDevice, 1, &this->inFlightFences[frameIndex]);
}

void Vulkan::resetCommandBuffer(uint32_t imageIndex, uint32_t frameIndex) {
    vkResetCommandBuffer(this->commandBuffers[frameIndex], 0);
    CommandBuffer::recordCommandBuffer(this->commandBuffers[frameIndex], imageIndex, this->graphicsPipeline,
                                       this->renderPass, this->swapChainFrameBuffers, this->swapChainExtent);
}

void Vulkan::submitCommandBuffer(uint32_t frameIndex) {
    CommandBuffer::submitCommandBuffer(&this->commandBuffers[frameIndex], this->graphicsQueue,
                                       this->inFlightFences[frameIndex], this->imageAvailableSemaphores[frameIndex],
                                       this->renderFinishedSemaphores[frameIndex],
                                       this->waitSemaphores, this->signalSemaphores);
}

void Vulkan::presentImageBuffer(uint32_t *imageIndex, GLFWwindow *window) {
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = this->signalSemaphores;

    VkSwapchainKHR swapChains[] = {this->swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = imageIndex;
    presentInfo.pResults = nullptr; // optional

    VkResult result = vkQueuePresentKHR(this->presentQueue, &presentInfo);

    // recreate swap chain if out of date or suboptimal flags returned
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        this->recreateSwapChain(window);
    } else if (result != VK_SUCCESS) {
        spdlog::error("An error occurred while submitting a swap chain image for presentation.");
        throw std::runtime_error("Failed to present the current swap chain image!");
    }
}

// Swap chain recreation methods
void Vulkan::recreateSwapChain(GLFWwindow *engineWindow) {

    int width = 0, height = 0;
    glfwGetFramebufferSize(engineWindow, &width, &height);
    while (width == 0 || height == 0) { // pause graphics until window is un-minimized
        glfwGetFramebufferSize(engineWindow, &width, &height);
        glfwWaitEvents();
    }

    this->waitForDeviceIdle(); // finish last GPU render
    this->destroySwapChain(); // destroy the previous swap chain

    SwapChain::createSwapChain(&this->swapChain, &this->swapChainImages, &this->swapChainImageFormat,
                               &this->swapChainExtent, this->logicalDevice, this->physicalDevice,
                               this->surface, engineWindow);
    ImageViews::createImageViews(&this->swapChainImageViews, this->logicalDevice,
                                 this->swapChainImages, this->swapChainImageFormat);
    FrameBuffers::createFrameBuffers(&this->swapChainFrameBuffers, this->swapChainImageViews,
                                     this->logicalDevice, this->renderPass, this->swapChainExtent);
}
void Vulkan::destroySwapChain() {
    // Destroy the frame buffer instances
    for (size_t i = 0; i < this->swapChainFrameBuffers.size(); i++) {
        vkDestroyFramebuffer(this->logicalDevice, this->swapChainFrameBuffers[i], nullptr);
    }
    // Destroy the swap chain image view instances
    for (size_t i = 0; i < this->swapChainImageViews.size(); i++) {
        vkDestroyImageView(this->logicalDevice, this->swapChainImageViews[i], nullptr);
    }
    vkDestroySwapchainKHR(this->logicalDevice, this->swapChain, nullptr);
}

void Vulkan::shutdown() {
    // Destroy the swap chain instances
    this->destroySwapChain();
    // Clean up Pipeline instances
    vkDestroyPipeline(this->logicalDevice, this->graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(this->logicalDevice, this->pipelineLayout, nullptr);
    vkDestroyRenderPass(this->logicalDevice, this->renderPass, nullptr);
    // Clean up synchronization objects
    for (int i = 0; i < this->MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(this->logicalDevice, this->imageAvailableSemaphores.at(i), nullptr);
        vkDestroySemaphore(this->logicalDevice, this->renderFinishedSemaphores.at(i), nullptr);
        vkDestroyFence(this->logicalDevice, this->inFlightFences.at(i), nullptr);
    }
    // Clean up Command Buffers, Logical & Physical devices, & VK instance
    vkDestroyCommandPool(this->logicalDevice, this->commandPool, nullptr);
    vkDestroyDevice(this->logicalDevice, nullptr);
    vkDestroySurfaceKHR(this->vulkanInstance, this->surface, nullptr);
    vkDestroyInstance(this->vulkanInstance, nullptr);
}