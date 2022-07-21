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

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <string>
#include <optional>

// ---------- Vulkan.cxx ---------- //
class Vulkan {
public:
    // Vulkan configuration
    const int MAX_FRAMES_IN_FLIGHT = 2;
    const std::vector<const char*> requiredExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };
    void initialize(const char* engineName, GLFWwindow *engineWindow); // Initializes the class and its instances.
    void waitForDeviceIdle(); // Wrapper for vkDeviceWaitIdle()
    void waitForPreviousFrame(uint32_t frameIndex); // Wrapper for vkWaitForFences()
    void getNextSwapChainImage(uint32_t *imageIndex, uint32_t frameIndex,
                               bool *windowResized, GLFWwindow *window); // Wrapper for vkAcquireNextImageKHR()
    void resetCommandBuffer(uint32_t imageIndex, uint32_t frameIndex); // Wrapper for vkResetCommandBuffer()
    void submitCommandBuffer(uint32_t frameIndex); // Wrapper for vkQueueSubmit() via CommandBuffer class
    void presentImageBuffer(uint32_t *imageIndex, GLFWwindow *window); // Wrapper for vkQueuePresentKHR()
    void shutdown(); // Cleans up & terminates all Vulkan instances.
private:
    void recreateSwapChain(GLFWwindow *engineWindow);
    void destroySwapChain();
    // Vulkan validation layers
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
    std::vector<VkCommandBuffer> commandBuffers;
    // GPU queue handles
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    // Synchronization Objects (semaphores / fences)
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    VkSemaphore waitSemaphores[1];
    VkSemaphore signalSemaphores[1];
};

// ---------- VulkanInstance.cxx ---------- //
class VulkanInstance {
public:
    static void createInstance(VkInstance* vkInstance, const char* appName,
                               const bool enableVkLayers, const std::vector<const char*> vkLayers);
private:
    static int checkRequiredExtensions(const char** glfwExtensions, uint32_t glfwCount,
                                       std::vector<VkExtensionProperties> extensions);
    static bool checkValidationLayerSupport(const std::vector<const char*> vkLayers);
};

// ---------- WindowSurface.cxx ---------- //
class WindowSurface {
public:
    static void createSurfaceKHR(VkSurfaceKHR *surface, VkInstance vulkanInstance, GLFWwindow *window);
};

// ---------- PhysicalDevice.cxx ---------- //
struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

class PhysicalDevice {
public:
    static void selectPhysicalDevice(VkPhysicalDevice *physicalDevice, VkInstance vulkanInstance,
                                     VkSurfaceKHR surface, const std::vector<const char*> extensions);
    static QueueFamilyIndices findDeviceQueueFamilies(VkPhysicalDevice gpuDevice, VkSurfaceKHR surface);
private:
    static int rateGPUSuitability(VkPhysicalDevice gpuDevice, VkSurfaceKHR surface,
                                  const std::vector<const char*> extensions);
    static bool checkGPUExtensionSupport(VkPhysicalDevice gpuDevice, const std::vector<const char*> extensions);
};

// ---------- LogicalDevice.cxx ---------- //
class LogicalDevice {
public:
    static void createLogicalDevice(VkDevice *logicalDevice, VkQueue *graphicsQueue, VkQueue *presentQueue,
                                    VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                                    const std::vector<const char*> gpuExtensions,
                                    const bool enableVkLayers, const std::vector<const char*> vkLayers);
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
                                VkPhysicalDevice gpuDevice, VkSurfaceKHR surface, GLFWwindow *window);

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

// ---------- CommandBuffer.cxx ---------- //
class CommandBuffer {
public:
    static void createCommandPool(VkCommandPool *commandPool, VkDevice logicalDevice,
                                  VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    static void createCommandBuffer(std::vector<VkCommandBuffer> *commandBuffers, const int MAX_FRAMES_IN_FLIGHT,
                                    VkDevice logicalDevice, VkCommandPool commandPool);
    static void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex,
                                    VkPipeline graphicsPipeline, VkRenderPass renderPass,
                                    std::vector<VkFramebuffer> swapFrameBuffers, VkExtent2D swapExtent);
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

#endif //VULKRAY_VULKAN_HXX