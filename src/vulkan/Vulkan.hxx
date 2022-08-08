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
    static bool checkValidationLayerSupport(const std::vector<const char*> vkLayers);
};

// ---------- WindowSurface.cxx ---------- //
class WindowSurface: public VkModuleBase {
public:
    VkSurfaceKHR surface;
    WindowSurface(Vulkan *m_vulkan);
    ~WindowSurface();
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
class LogicalDevice {
public:
    static void createLogicalDevice(VkDevice *logicalDevice,
                                    VkQueue *graphicsQueue, VkQueue *presentQueue, VkQueue *transferQueue,
                                    VkPhysicalDevice physicalDevice, QueueFamilyIndices gpuQueueIndices,
                                    const std::vector<const char*> gpuExtensions,
                                    const bool enableVkLayers, const std::vector<const char*> vkLayers);
};

// ---------- VulkanMemoryAllocator.cxx ---------- //
struct AllocatedBuffer {
    VkBuffer _buffer;
    VmaAllocation _bufferMemory;
};
class VulkanMemoryAllocator {
public:
    static void initializeMemoryAllocator(VmaAllocator *memoryAllocator, VkPhysicalDevice physicalDevice,
                                   VkDevice logicalDevice, VkInstance vulkanInstance);
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

class SwapChain {
public:
    static void createSwapChain(VkSwapchainKHR *swapChain, std::vector<VkImage> *swapImages,
                                VkFormat *format, VkExtent2D *extent, VkDevice logicalDevice,
                                VkPhysicalDevice gpuDevice, VkSurfaceKHR surface,
                                QueueFamilyIndices queueIndices, GLFWwindow *window);

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
class ImageViews {
public:
    static void createImageViews(std::vector<VkImageView> *swapChainImageViews, VkDevice logicalDevice,
                                 std::vector<VkImage> swapChainImages, VkFormat swapImageFormat);
};

// ---------- RenderPass.cxx ---------- //
class RenderPass {
public:
    static void createRenderPass(VkRenderPass *renderPass, VkDevice logicalDevice, VkFormat swapImageFormat);
};

// ---------- GraphicsPipeline.cxx ---------- //
class GraphicsPipeline {
public:
    static void createGraphicsPipeline(VkPipeline *graphicsPipeline, VkPipelineLayout *pipelineLayout,
                                       VkRenderPass renderPass, VkDevice logicalDevice, VkExtent2D swapExtent);
private:
    static VkShaderModule createShaderModule(const std::vector<char> &shaderBinary, VkDevice logicalDevice);
    static std::vector<char> readSpirVShaderBinary(const std::string &filename);
};

// ---------- FrameBuffers.cxx ---------- //
class FrameBuffers {
public:
    static void createFrameBuffers(std::vector<VkFramebuffer> *swapChainFrameBuffers,
                                   std::vector<VkImageView> swapChainImageViews,
                                   VkDevice logicalDevice, VkRenderPass renderPass, VkExtent2D swapChainExtent);
};

// ---------- Buffers.cxx ---------- //
struct Vertex {
    glm::vec2 pos;
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
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);
        // vertex color data to shader
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }
};

class Buffers {
public:
    glm::mat4 matrix;
    glm::vec4 vec;
    static void createVertexBuffer(AllocatedBuffer *vertexBuffer, VmaAllocator allocator,
                                   QueueFamilyIndices queueIndices, const std::vector<Vertex> vertices,
                                   VkDevice logicalDevice, VkCommandPool transferPool, VkQueue transferQueue);
    static void createIndexBuffer(AllocatedBuffer *indexBuffer, VmaAllocator allocator,
                                  QueueFamilyIndices queueIndices, const std::vector<uint32_t> indices,
                                  VkDevice logicalDevice, VkCommandPool transferPool, VkQueue transferQueue);
private:
    static void allocateBuffer(AllocatedBuffer *buffer, VmaAllocator allocator, VkBufferUsageFlags usageTypeBit,
                               VmaAllocationCreateFlags allocationFlags,
                               VkDeviceSize bufferSize, QueueFamilyIndices queueIndices);
    static void copyBuffer(AllocatedBuffer srcBuffer, AllocatedBuffer dstBuffer, VkDeviceSize bufferSize,
                           VkDevice logicalDevice, VkCommandPool commandPool, VkQueue transferQueue);
};

// ---------- CommandBuffer.cxx ---------- //
struct GraphicsInput {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    VkClearValue bufferClearColor;
};

class CommandBuffer {
public:
    static void createCommandPool(VkCommandPool *commandPool, VkCommandPoolCreateFlags additionalFlags,
                                  VkDevice logicalDevice, uint32_t queueIndex);
    static void createCommandBuffer(std::vector<VkCommandBuffer> *commandBuffers, const int MAX_FRAMES_IN_FLIGHT,
                                    VkDevice logicalDevice, VkCommandPool commandPool);
    static void recordGraphicsCommands(VkCommandBuffer commandBuffer, uint32_t imageIndex,
                                    VkPipeline graphicsPipeline, VkRenderPass renderPass,
                                    std::vector<VkFramebuffer> swapFrameBuffers,
                                    AllocatedBuffer vertexBuffer, AllocatedBuffer indexBuffer,
                                    GraphicsInput graphicsInput, VkExtent2D swapExtent);
    static void submitCommandBuffer(VkCommandBuffer *commandBuffer, VkQueue graphicsQueue, VkFence inFlightFence,
                                    VkSemaphore imageAvailableSemaphore, VkSemaphore renderFinishedSemaphore,
                                    VkSemaphore waitSemaphores[], VkSemaphore signalSemaphores[]);
};

// ---------- Synchronization.cxx ---------- //
class Synchronization {
public:
    static void createSyncObjects(std::vector<VkSemaphore> *imageAvailableSemaphores,
                                  std::vector<VkSemaphore> *renderFinishedSemaphores,
                                  std::vector<VkFence> *inFlightFences, VkDevice logicalDevice,
                                  const int MAX_FRAMES_IN_FLIGHT);
};

// ---------- Vulkan.cxx ---------- //
class Vulkan {
public:
    // Vulkan configuration
    const char *engineName;
    GLFWwindow *engineWindow;
    GraphicsInput graphicsInput;

    const int MAX_FRAMES_IN_FLIGHT = 2;
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
    std::unique_ptr<WindowSurface> m_windowSurface;
    std::unique_ptr<PhysicalDevice> m_physicalDevice;
    VkDevice logicalDevice = VK_NULL_HANDLE;
    VmaAllocator memoryAllocator; // VMA allocator
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    std::vector<VkFramebuffer> swapChainFrameBuffers;
    VkCommandPool graphicsCommandPool;
    VkCommandPool transferCommandPool;
    std::vector<VkCommandBuffer> graphicsCommandBuffers;
    std::vector<VkCommandBuffer> transferCommandBuffers;
    // GPU queue handles
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkQueue transferQueue;
    // Synchronization Objects (semaphores / fences)
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    VkSemaphore waitSemaphores[1];
    VkSemaphore signalSemaphores[1];
    // Buffer allocations
    AllocatedBuffer vertexBuffer;
    AllocatedBuffer indexBuffer;

    Vulkan(const char* engineName, GLFWwindow *engineWindow, GraphicsInput graphicsInput);
    void waitForDeviceIdle(); // Wrapper for vkDeviceWaitIdle()
    void waitForPreviousFrame(uint32_t frameIndex); // Wrapper for vkWaitForFences()
    void getNextSwapChainImage(uint32_t *imageIndex, uint32_t frameIndex, GLFWwindow *window);
    void resetGraphicsCmdBuffer(uint32_t imageIndex, uint32_t frameIndex, GraphicsInput graphicsInput);
    void submitGraphicsCmdBuffer(uint32_t frameIndex); // Wrapper for vkQueueSubmit() via CommandBuffer class
    void presentImageBuffer(uint32_t *imageIndex, GLFWwindow *window, bool *windowResized);
    ~Vulkan();
private:
    void recreateSwapChain(GLFWwindow *engineWindow);
    void destroySwapChain();
};

#endif //VULKRAY_VULKAN_HXX
