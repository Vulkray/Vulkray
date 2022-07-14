/*
 * VulkanInstance.cxx
 * Creates a new Vulkan object instance
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

#ifndef VULKRAY_VULKANINSTANCE_HXX
#define VULKRAY_VULKANINSTANCE_HXX

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

#endif //VULKRAY_VULKANINSTANCE_HXX
