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

#include "VulkanInstance.hxx"
#include "WindowSurface.hxx"
#include "PhysicalDevice.hxx"
#include "LogicalDevice.hxx"
#include "SwapChain.hxx"
#include "ImageViews.hxx"
#include "RenderPass.hxx"
#include "GraphicsPipeline.hxx"
#include "FrameBuffers.hxx"
#include "CommandBuffer.hxx"

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
    ImageViews::createImageViews(this->swapChainImageViews, this->logicalDevice,
                                 this->swapChainImages, this->swapChainImageFormat);
    RenderPass::createRenderPass(&this->renderPass, this->logicalDevice, this->swapChainImageFormat);
    GraphicsPipeline::createGraphicsPipeline(&this->graphicsPipeline, &this->pipelineLayout, this->renderPass,
                                             this->logicalDevice, this->swapChainExtent);
    FrameBuffers::createFrameBuffers(this->swapChainFrameBuffers, this->swapChainImageViews,
                                     this->logicalDevice, this->renderPass, this->swapChainExtent);
    CommandBuffer::createCommandPool(&this->commandPool, this->logicalDevice, this->physicalDevice, this->surface);
    CommandBuffer::createCommandBuffer(&this->commandBuffer, this->logicalDevice, this->commandPool);
    spdlog::debug("Initialized Vulkan instances.");
}

void Vulkan::shutdown() {
    vkDestroyCommandPool(this->logicalDevice, this->commandPool, nullptr);
    for (auto framebuffer : swapChainFrameBuffers) {
        vkDestroyFramebuffer(this->logicalDevice, framebuffer, nullptr);
    }
    vkDestroyPipeline(this->logicalDevice, this->graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(this->logicalDevice, this->pipelineLayout, nullptr);
    vkDestroyRenderPass(this->logicalDevice, this->renderPass, nullptr);
    for (auto imageView : this->swapChainImageViews) {
        vkDestroyImageView(this->logicalDevice, imageView, nullptr);
    }
    vkDestroySwapchainKHR(this->logicalDevice, this->swapChain, nullptr);
    vkDestroyDevice(this->logicalDevice, nullptr);
    vkDestroySurfaceKHR(this->vulkanInstance, this->surface, nullptr);
    vkDestroyInstance(this->vulkanInstance, nullptr);
}