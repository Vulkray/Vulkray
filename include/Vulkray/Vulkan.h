/*
 * Vulkan.h
 * Initializes and manages all the engine's Vulkan instances.
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2023, Max Rodriguez. All rights reserved.
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

#include <memory>
#include <array>
#include <vector>
#include <string>
#include <optional>

// Class/struct prototypes
class Vulkan;
class Window;
class ShowBase;
class VkModuleBase {
public:
    Vulkan *m_vulkan; // every module has a pointer to the core Vulkan class instance
    VkModuleBase(Vulkan *m_vulkan);
};

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        // vertex binding data
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }
    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
        // vertex position data to shader
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);
        // vertex color data to shader
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }
};
struct GraphicsInput {
    std::vector<Vertex> vertexData = {
            {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, // invisible placeholder vertex (buffer cannot be empty)
    };
    std::vector<uint32_t> indexData = {
            0, 0, 0 // invisible placeholder indices (buffer cannot be initialized empty)
    };
    VkClearValue bufferClearColor = (VkClearValue){{{0.05f, 0.05f, 0.05f, 1.0f}}}; // default world background color
};

#include "ShowBase.h"

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
    Window(Vulkan *m_vulkan, char* winTitle);
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
    VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT; // default MSAA
    PhysicalDevice(Vulkan *m_vulkan);
    VkFormat findDepthFormat();
    bool depthFormatHasStencilComponent(VkFormat format);
    VkSampleCountFlagBits getMaxUsableSampleCount();
private:
    VkFormat findSupportedDepthFormat(const std::vector<VkFormat>& candidates,
                                      VkImageTiling tiling, VkFormatFeatureFlags features);
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

// ---------- DescriptorPool.cxx ---------- //
class DescriptorPool: public VkModuleBase {
public:
    VkDescriptorSetLayout descriptorSetLayout;
    std::vector<VkDescriptorSet> descriptorSets;
    DescriptorPool(Vulkan *m_vulkan);
    ~DescriptorPool();
private:
    VkDescriptorPool descriptorPool;
};

// ---------- Buffers.cxx ---------- //
struct AllocatedBuffer {
    VkBuffer _bufferInstance;
    VmaAllocation _bufferMemory;
};
struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
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
struct AllocatedImage {
    VkImage _imageInstance;
    VmaAllocation _imageMemory;
};
class SwapImageViews: public VkModuleBase {
public:
    std::vector<VkImageView> swapChainImageViews;
    SwapImageViews(Vulkan *m_vulkan);
    ~SwapImageViews();
};
// static helper class for creating images using VMA
class ImageViews {
public:
    static void allocateVMAImage(VmaAllocator allocator, AllocatedImage *allocatedImage, uint32_t width,
                                 uint32_t height, VkImageTiling tiling, VkSampleCountFlagBits msaaSamples,
                                 VkImageUsageFlags usageFlags, VkFormat imageFormat);
    static VkImageView createImageView(VkDevice logicalDevice, VkImage image,
                                       VkFormat format, VkImageAspectFlags aspectFlags);
    static void transitionImageLayout(AllocatedImage allocatedImage, VkFormat format,
                                      VkImageLayout oldLayout, VkImageLayout newLayout);
};

// ---------- MultiSampling.cxx ---------- //
class MultiSampling: public VkModuleBase {
public:
    AllocatedImage msaaImage;
    VkImageView msaaImageView;
    MultiSampling(Vulkan *m_vulkan);
    ~MultiSampling();
};

// ---------- DepthBuffering.cxx ---------- //
class DepthTesting: public VkModuleBase {
public:
    AllocatedImage depthImage;
    VkImageView depthImageView;
    DepthTesting(Vulkan *m_vulkan);
    ~DepthTesting();
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
    const char* engineName = "Vulkray";
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
    std::unique_ptr<SwapImageViews> m_imageViews;
    std::unique_ptr<MultiSampling> m_MSAA;
    std::unique_ptr<DepthTesting> m_depthTesting;
    std::unique_ptr<RenderPass> m_renderPass;
    std::unique_ptr<DescriptorPool> m_descriptorPool;
    std::unique_ptr<GraphicsPipeline> m_graphicsPipeline;
    std::unique_ptr<FrameBuffers> m_frameBuffers;
    std::unique_ptr<CommandPool> m_graphicsCommandPool;
    std::unique_ptr<CommandPool> m_transferCommandPool;
    std::unique_ptr<Buffer> m_vertexBuffer;
    std::unique_ptr<Buffer> m_indexBuffer;
    std::vector<std::unique_ptr<Buffer>> m_uniformBuffers;
    std::unique_ptr<Synchronization> m_synchronization;
    ShowBase *base;
    Vulkan(ShowBase *base, GraphicsInput graphicsInput, char* winTitle, void (*initGlfwInput)(Vulkan *m_vulkan));
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
