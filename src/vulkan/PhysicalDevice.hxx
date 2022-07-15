/*
 * PhysicalDevice.cxx
 * Scans system GPU devices and selects a suitable device.
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

#ifndef VULKRAY_PHYSICALDEVICE_HXX
#define VULKRAY_PHYSICALDEVICE_HXX

#include <vulkan/vulkan.h>
#include <optional>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    bool isComplete() {
        return graphicsFamily.has_value()
        && presentFamily.has_value();
    }
};

class PhysicalDevice {
public:
    static void selectPhysicalDevice(VkPhysicalDevice *physicalDevice,
                                     VkInstance vulkanInstance, VkSurfaceKHR surface);
    static QueueFamilyIndices findDeviceQueueFamilies(VkPhysicalDevice gpuDevice, VkSurfaceKHR surface);
private:
    static int rateGPUSuitability(VkPhysicalDevice gpuDevice, VkSurfaceKHR surface);
};

#endif //VULKRAY_PHYSICALDEVICE_HXX
