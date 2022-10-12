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

#include "Vulkan.h"
#include <chrono>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Vulkan::Vulkan(GraphicsInput graphicsInput, char* winTitle) {
    // store as class attribute for modules to access
    this->graphicsInput = graphicsInput;

    // initialize modules using smart pointers and store as class properties
    spdlog::debug("Initializing Vulkan ...");
    this->m_vulkanInstance = std::make_unique<VulkanInstance>(this);
    this->m_window = std::make_unique<Window>(this, winTitle);
    this->m_physicalDevice = std::make_unique<PhysicalDevice>(this);
    this->m_logicalDevice = std::make_unique<LogicalDevice>(this);
    this->m_VMA = std::make_unique<VulkanMemoryAllocator>(this);
    this->m_swapChain = std::make_unique<SwapChain>(this);
    this->m_imageViews = std::make_unique<SwapImageViews>(this);
    this->m_MSAA = std::make_unique<MultiSampling>(this);
    this->m_depthTesting = std::make_unique<DepthTesting>(this);
    this->m_renderPass = std::make_unique<RenderPass>(this);
    this->m_graphicsCommandPool = std::make_unique<CommandPool>(
            this, (VkCommandPoolCreateFlags) 0, this->m_physicalDevice->queueFamilies.graphicsFamily.value());
    this->m_transferCommandPool = std::make_unique<CommandPool>(
            this, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, this->m_physicalDevice->queueFamilies.transferFamily.value());
    this->m_vertexBuffer = std::make_unique<Buffer>(this, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                    &this->graphicsInput.vertexData, nullptr);
    this->m_indexBuffer = std::make_unique<Buffer>(this, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                                   nullptr, &this->graphicsInput.indexData);
    this->m_uniformBuffers.resize(this->MAX_FRAMES_IN_FLIGHT); // have as many UBs as frames in flight
    for (size_t i = 0; i < this->MAX_FRAMES_IN_FLIGHT; i++) {
        this->m_uniformBuffers[i] = std::make_unique<Buffer>(this, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, nullptr, nullptr);
    }
    this->m_descriptorPool = std::make_unique<DescriptorPool>(this);
    this->m_graphicsPipeline = std::make_unique<GraphicsPipeline>(this);
    this->m_frameBuffers = std::make_unique<FrameBuffers>(this);
    this->m_synchronization = std::make_unique<Synchronization>(this);

    spdlog::debug("Running engine renderer ...");
    while(!glfwWindowShouldClose(this->m_window->window)) {
        glfwPollEvents(); // Respond to window events (exit, resize, etc.)
        renderFrame();
    }
}

void Vulkan::renderFrame() {
    uint32_t imageIndex;
    this->waitForPreviousFrame(); // TODO: Measure FPS at this point in the engine renderer
    this->getNextSwapChainImage(&imageIndex); // <-- swap chain recreation called here (via Vulkan OUT_OF_DATE_KHR)
    this->m_graphicsCommandPool->resetGraphicsCmdBuffer(imageIndex);
    this->updateUniformBuffer(imageIndex);
    this->m_graphicsCommandPool->submitNextCommandBuffer();
    this->presentImageBuffer(&imageIndex); // <-- swap chain recreation called here (via GLFW framebuffer callback)
    this->frameIndex = (this->frameIndex + 1) % this->MAX_FRAMES_IN_FLIGHT;
}

void Vulkan::waitForPreviousFrame() {
    vkWaitForFences(this->m_logicalDevice->logicalDevice, 1,
                    &this->m_synchronization->inFlightFences[this->frameIndex], VK_TRUE, UINT64_MAX);
}

void Vulkan::updateUniformBuffer(uint32_t imageIndex) {
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    uint32_t swapImageWidth = this->m_swapChain->swapChainExtent.width;
    uint32_t swapImageHeight = this->m_swapChain->swapChainExtent.height;

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), swapImageWidth / (float) swapImageHeight, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1; // GLM was designed for OpenGL, where Y coordinates are flipped. Corrected for vulkan here.

    // map new UBO information to current uniform buffer memory
    void* data;
    vmaMapMemory(this->m_VMA->memoryAllocator, this->m_uniformBuffers.at(this->frameIndex)->buffer._bufferMemory, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vmaUnmapMemory(this->m_VMA->memoryAllocator, this->m_uniformBuffers.at(this->frameIndex)->buffer._bufferMemory);
}

void Vulkan::getNextSwapChainImage(uint32_t *imageIndex) {

    // acquire next image view, also get swap chain status
    VkResult result = vkAcquireNextImageKHR(this->m_logicalDevice->logicalDevice, this->m_swapChain->swapChain,
                                            10, // timeout in nanoseconds (kept low for best latency)
                                            this->m_synchronization->imageAvailableSemaphores[frameIndex],
                                            VK_NULL_HANDLE, imageIndex);

    /* check if vkAcquireNextImageKHR returned an out of date framebuffer flag
     * Note: this is not a feature on all Vulkan compatible drivers! also checking via GLFW resize callback!
     */
    if (result == VK_ERROR_OUT_OF_DATE_KHR || this->framebufferResized) {
        this->framebufferResized = false; // reset GLFW triggered framebuffer resized flag
        this->recreateSwapChain();
        // resetting fences here fixed something. already forgot what lol
        vkResetFences(this->m_logicalDevice->logicalDevice, 1,
                      &this->m_synchronization->inFlightFences[this->frameIndex]);
        return;

    // TODO: Handle VK_SUBOPTIMAL_KHR status code
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        spdlog::error("An error occurred when acquiring the next swap chain image view; Exiting.");
        throw std::runtime_error("Failed to acquire swap chain image!");
    }
    // reset fence only if we know we're submitting work
    vkResetFences(this->m_logicalDevice->logicalDevice, 1, &this->m_synchronization->inFlightFences[this->frameIndex]);
}

void Vulkan::presentImageBuffer(uint32_t *imageIndex) {
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = this->m_synchronization->signalSemaphores;

    VkSwapchainKHR swapChains[] = {this->m_swapChain->swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = imageIndex;
    presentInfo.pResults = nullptr; // optional

    VkResult result = vkQueuePresentKHR(this->m_logicalDevice->presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        this->recreateSwapChain();

    } else if (result != VK_SUCCESS) {
        spdlog::error("An error occurred while submitting a swap chain image for presentation.");
        throw std::runtime_error("Failed to present the current swap chain image!");
    }
}

// Swap chain recreation (m_swapChain, m_imageViews, m_depthTesting, m_frameBuffers)
void Vulkan::recreateSwapChain() {
    this->m_window->waitForWindowFocus();
    this->m_logicalDevice->waitForDeviceIdle();
    // Move current swap to old swap smart pointer
    this->m_oldSwapChain = std::move(this->m_swapChain);
    // recreate the swap chain & its dependent modules
    this->m_imageViews.reset();
    this->m_MSAA.reset();
    this->m_depthTesting.reset();
    this->m_frameBuffers.reset();
    this->m_swapChain = std::make_unique<SwapChain>(this);
    this->m_imageViews = std::make_unique<SwapImageViews>(this);
    this->m_MSAA = std::make_unique<MultiSampling>(this);
    this->m_depthTesting = std::make_unique<DepthTesting>(this);
    this->m_frameBuffers = std::make_unique<FrameBuffers>(this);
    // destroy old swap chain module after recreation
    this->m_oldSwapChain.reset();
    spdlog::debug("Recreated the swap chain!");
}

Vulkan::~Vulkan() {
    // Sleeps thread until GPU is idle before cleaning up engine memory
    this->m_logicalDevice->waitForDeviceIdle();
}

// Module base class constructor
VkModuleBase::VkModuleBase(Vulkan *m_vulkan) {
    this->m_vulkan = m_vulkan; // store pointer to core Vulkan class instance in every module
}