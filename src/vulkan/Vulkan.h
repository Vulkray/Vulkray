/*
 * Vulkan.h
 * Initializes and manages all the engine's Vulkan instances.
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2022, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#ifndef VULKRAY_VULKAN_HXX
#define VULKRAY_VULKAN_HXX

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vk_mem_alloc.h>
// exposing some definitions to the API
#include "../../include/vulkan.h"

#include <memory>
#include <array>
#include <vector>
#include <string>
#include <optional>

// Class prototypes
class Vulkan;
class VkModuleBase {
public:
    Vulkan *m_vulkan; // every module has a pointer to the core Vulkan class instance
    VkModuleBase(Vulkan *m_vulkan);
};

// ---------- VulkanInstance.cxx ---------- //
class VulkanInstance: public VkModuleBase {
public:
    VkInstance vulkanInstance;
    VulkanInstance(Vulkan *m_vulkan);
    ~VulkanInstance();
private:
    static int checkRequiredExtensions(const char** glfwExtensions, uint32_t glfwCount,
                                std::vector<VkExtensionProperties> extensions);
    bool checkValidationLayerSupport();
};

// ---------- Window.cxx ---------- //
class Window: public VkModuleBase {
public:
    char* title;
    int width = 900;
    int height = 600;
    GLFWwindow *window{};
    VkSurfaceKHR surface;
    Window(Vulkan *m_vulkan);
    ~Window();
    void waitForWindowFocus();
private:
    static void framebufferResizeCallback(GLFWwindow* engineWindow, int width, int height);
};

// ---------- PhysicalDevice.cxx ---------- //
struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    std::optional<uint32_t> transferFamily;
    std::optional<bool> dedicatedTransferFamily;
    /* Note: Queue families with either `VK_QUEUE_GRAPHICS_BIT` or `VK_QUEUE_COMPUTE_BIT` capabilities already
     * implicitly support `VK_QUEUE_TRANSFER_BIT` operations. Just for the challenge, I've added a dedicated
     * `transferFamily` property to the indices in case there is a GPU queue family dedicated to transfer operations.
     * These queue families are queried by PhysicalDevice::findDeviceQueueFamilies() at PhysicalDevice.cxx.
     */
    bool isComplete() {
        return graphicsFamily.has_value() &&
                presentFamily.has_value() &&
                transferFamily.has_value();
    }
};

class PhysicalDevice: public VkModuleBase {
public:
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    QueueFamilyIndices queueFamilies;
    PhysicalDevice(Vulkan *m_vulkan);
private:
    QueueFamilyIndices findDeviceQueueFamilies();
    int rateGPUSuitability();
    bool checkGPUExtensionSupport();
};

// ---------- LogicalDevice.cxx ---------- //
class LogicalDevice: public VkModuleBase {
public:
    VkDevice logicalDevice = VK_NULL_HANDLE;
    // GPU queue handles
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkQueue transferQueue;
    LogicalDevice(Vulkan *m_vulkan);
    ~LogicalDevice();
    void waitForDeviceIdle();
};

// ---------- DescriptorSet.cxx ---------- //
class DescriptorSet: public VkModuleBase {
public:
    VkDescriptorSetLayout descriptorSetLayout;
    DescriptorSet(Vulkan *m_vulkan);
    ~DescriptorSet();
private:
};

// ---------- Buffers.cxx ---------- //
struct AllocatedBuffer {
    VkBuffer _bufferInstance;
    VmaAllocation _bufferMemory;
};
struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

class Buffer: public VkModuleBase {
public:
    AllocatedBuffer buffer;
    // When vertex/index buffer init, only one data parameter is used (after BufferUsageFlagBit), never both.
    Buffer(Vulkan *m_vulkan, VkBufferUsageFlagBits bufferType,
           const std::vector<Vertex> *vertexData, const std::vector<uint32_t> *indexData);
    ~Buffer();
private:
    void createVertexBuffer(const std::vector<Vertex> vertices);
    void createIndexBuffer(const std::vector<uint32_t> indices);
    void createUniformBuffer();
    void allocateBuffer(AllocatedBuffer *buffer, VkBufferUsageFlags usageTypeBit,
                        VmaAllocationCreateFlags allocationFlags, VkDeviceSize bufferSize);
    void copyBuffer(AllocatedBuffer srcBuffer, AllocatedBuffer dstBuffer, VkDeviceSize bufferSize);
};

// ---------- VulkanMemoryAllocator.cxx ---------- //
class VulkanMemoryAllocator: public VkModuleBase {
public:
    VmaAllocator memoryAllocator;
    VulkanMemoryAllocator(Vulkan *m_vulkan);
    ~VulkanMemoryAllocator();
};

// ---------- SwapChain.cxx ---------- //

// Prefer standard 32-bit color formats (SRGB)
const VkFormat PREFERRED_COLOR_FORMAT = VK_FORMAT_B8G8R8A8_SRGB;
const VkColorSpaceKHR PREFERRED_COLOR_SPACE = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
// Preferred swap chain presentation modes
const VkPresentModeKHR DEFAULT_PRESENTATION = VK_PRESENT_MODE_FIFO_KHR; // guaranteed & optimal, higher latency
const VkPresentModeKHR PREFERRED_PRESENTATION = VK_PRESENT_MODE_MAILBOX_KHR; // more expensive, but lowest latency

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class SwapChain: public VkModuleBase {
public:
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    SwapChain(Vulkan *m_vulkan);
    ~SwapChain();
    static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
private:
    // Surface format (presentation color depth)
    static VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    // Presentation mode (image buffer swapping methods)
    static VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    // Swap Extent (resolution of swap chain images)
    static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, GLFWwindow *window);
};

// ---------- ImageViews.cxx ---------- //
class ImageViews: public VkModuleBase {
public:
    std::vector<VkImageView> swapChainImageViews;
    ImageViews(Vulkan *m_vulkan);
    ~ImageViews();
};

// ---------- RenderPass.cxx ---------- //
class RenderPass: public VkModuleBase {
public:
    VkRenderPass renderPass;
    RenderPass(Vulkan *m_vulkan);
    ~RenderPass();
};

// ---------- GraphicsPipeline.cxx ---------- //
class GraphicsPipeline: public VkModuleBase {
public:
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    GraphicsPipeline(Vulkan *m_vulkan);
    ~GraphicsPipeline();
private:
    VkShaderModule createShaderModule(const std::vector<char> &shaderBinary);
    static std::vector<char> readSpirVShaderBinary(const std::string &filename);
};

// ---------- FrameBuffers.cxx ---------- //
class FrameBuffers: public VkModuleBase {
public:
    std::vector<VkFramebuffer> swapChainFrameBuffers;
    FrameBuffers(Vulkan *m_vulkan);
    ~FrameBuffers();
};

// ---------- CommandBuffer.cxx ---------- //
class CommandPool: public VkModuleBase {
public:
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
    CommandPool(Vulkan *m_vulkan, VkCommandPoolCreateFlags additionalFlags, uint32_t queueIndex);
    ~CommandPool();
    void recordGraphicsCommands(uint32_t imageIndex);
    void submitNextCommandBuffer();
    void resetGraphicsCmdBuffer(uint32_t imageIndex);
private:
    void createCommandPool(VkCommandPoolCreateFlags additionalFlags, uint32_t queueIndex);
    void createCommandBuffer();
};

// ---------- Synchronization.cxx ---------- //
class Synchronization: public VkModuleBase {
public:
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    VkSemaphore waitSemaphores[1];
    VkSemaphore signalSemaphores[1];
    Synchronization(Vulkan *m_vulkan);
    ~Synchronization();
};

// ---------- Vulkan.cxx ---------- //
class Vulkan {
public:
    const char* engineName = "Vulkray Engine";
    GraphicsInput graphicsInput;
    // Render variables
    const unsigned int MAX_FRAMES_IN_FLIGHT = 2;
    uint32_t frameIndex = 0;
    bool framebufferResized = false;
    const std::vector<const char*> requiredExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };
    // Vulkan validation layers (debug build only)
    #ifdef NDEBUG
        const bool enableValidationLayers = false;
    #else
        const bool enableValidationLayers = true;
    #endif
    // Vulkan instance modules (RAII)
    std::unique_ptr<VulkanInstance> m_vulkanInstance;
    std::unique_ptr<Window> m_window;
    std::unique_ptr<PhysicalDevice> m_physicalDevice;
    std::unique_ptr<LogicalDevice> m_logicalDevice;
    std::unique_ptr<VulkanMemoryAllocator> m_VMA;
    std::unique_ptr<SwapChain> m_swapChain;
    std::unique_ptr<SwapChain> m_oldSwapChain = nullptr; // used for swap recreation
    std::unique_ptr<ImageViews> m_imageViews;
    std::unique_ptr<RenderPass> m_renderPass;
    std::unique_ptr<DescriptorSet> m_descriptorSet;
    std::unique_ptr<GraphicsPipeline> m_graphicsPipeline;
    std::unique_ptr<FrameBuffers> m_frameBuffers;
    std::unique_ptr<CommandPool> m_graphicsCommandPool;
    std::unique_ptr<CommandPool> m_transferCommandPool;
    std::unique_ptr<Buffer> m_vertexBuffer;
    std::unique_ptr<Buffer> m_indexBuffer;
    std::vector<std::unique_ptr<Buffer>> m_uniformBuffers;
    std::unique_ptr<Synchronization> m_synchronization;
    Vulkan(GraphicsInput graphicsInput);
    ~Vulkan();
private:
    void renderFrame();
    void waitForPreviousFrame(); // Wrapper for vkWaitForFences()
    void updateUniformBuffer(uint32_t imageIndex);
    void getNextSwapChainImage(uint32_t *imageIndex);
    void resetGraphicsCmdBuffer(uint32_t imageIndex);
    void presentImageBuffer(uint32_t *imageIndex);
    void recreateSwapChain();
};

#endif //VULKRAY_VULKAN_HXX
