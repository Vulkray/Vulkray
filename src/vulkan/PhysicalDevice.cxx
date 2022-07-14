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

#include "PhysicalDevice.hxx"

#include <spdlog/spdlog.h>
#include <map>

void PhysicalDevice::selectPhysicalDevice(VkPhysicalDevice *physicalDevice, VkInstance vulkanInstance) {

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, nullptr);
    // if no GPUs with vulkan support are found, exit with error.
    if (deviceCount == 0) {
        spdlog::error("No Vulkan-compatible system GPU devices were found. Exiting.");
        throw std::runtime_error("No Vulkan-compatible GPU device found.");
    }
    // Get all available GPU devices
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, devices.data());

    // ---------- Scan GPU devices features & score suitability --------- //

    // Use an ordered map to automatically sort candidates by increasing score
    std::multimap<int, VkPhysicalDevice> candidates;

    for (auto& device : devices) {
        int score = rateGPUSuitability(&device);
        candidates.insert(std::make_pair(score, device));
    }

    // Check if the best candidate is suitable at all
    if (candidates.rbegin()->first > 0) {
        *physicalDevice = candidates.rbegin()->second;
    } else {
        spdlog::error("Could not find a suitable GPU device! Exiting.");
        throw std::runtime_error("No system GPU device met the minimal device requirements.");
    }

    // Get selected GPU properties
    VkPhysicalDeviceProperties gpuProperties;
    vkGetPhysicalDeviceProperties(*physicalDevice, &gpuProperties);

    spdlog::info("Vulkan GPU Selected: {0}", gpuProperties.deviceName);
}

int PhysicalDevice::rateGPUSuitability(VkPhysicalDevice *gpuDevice) {

    int deviceScore = 0;

    // Get GPU device information
    VkPhysicalDeviceProperties gpuProperties;
    VkPhysicalDeviceFeatures gpuFeatures;

    vkGetPhysicalDeviceProperties(*gpuDevice, &gpuProperties);
    vkGetPhysicalDeviceFeatures(*gpuDevice, &gpuFeatures);

    // Check minimal GPU device requirements
    if (!gpuFeatures.geometryShader) return 0;

    // Rate GPU physical device with score
    if (gpuProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) deviceScore += 1000;
    // Maximum possible size of textures affects graphics quality
    deviceScore += gpuProperties.limits.maxImageDimension2D;

    return deviceScore;
}