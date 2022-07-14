/*
 * PhysicalDevice class
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

class PhysicalDevice {
public:
    static void selectPhysicalDevice(VkPhysicalDevice *physicalDevice, VkInstance vulkanInstance);
private:
    static int rateGPUSuitability(VkPhysicalDevice *gpuDevice);
};


#endif //VULKRAY_PHYSICALDEVICE_HXX