/*
 * VulkanInstance class
 * Creates a new Vulkan object instance
 */
#ifndef LEARNING_VULKAN_VULKANINSTANCE_HXX
#define LEARNING_VULKAN_VULKANINSTANCE_HXX

#include <vulkan/vulkan.h>
#include <vector>

class VulkanInstance {
public:
    static void createInstance(VkInstance* vulkanInstance, const char* appName);
private:
    static int checkRequiredExtensions(
            const char** glfwExts, uint32_t glfwCount, std::vector<VkExtensionProperties> exts);
};

#endif //LEARNING_VULKAN_VULKANINSTANCE_HXX
