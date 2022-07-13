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
    static void createInstance(VkInstance* vkInstance, const char* appName,
                               const bool enableVkLayers, const std::vector<const char*> vkLayers);
private:
    static int checkRequiredExtensions(
            const char** glfwExts, uint32_t glfwCount, std::vector<VkExtensionProperties> exts);
    static bool checkValidationLayerSupport(const std::vector<const char*> vkLayers);
};

#endif //LEARNING_VULKAN_VULKANINSTANCE_HXX
