/*
 * LogicalDevice.cxx
 * Creates the Vulkan logical device instance with the selected GPU physical device.
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

#ifndef VULKRAY_LOGICALDEVICE_HXX
#define VULKRAY_LOGICALDEVICE_HXX

#include <vulkan/vulkan.h>
#include <vector>

class LogicalDevice {
public:
    static void createLogicalDevice(VkDevice *logicalDevice, VkQueue *graphicsQueue, VkQueue *presentQueue,
                                    VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                                    const std::vector<const char*> gpuExtensions,
                                    const bool enableVkLayers, const std::vector<const char*> vkLayers);
};

#endif //VULKRAY_LOGICALDEVICE_HXX
