/*
 * SurfaceKHR.cxx
 * Creates the Vulkan Surface instance for presenting to the GLFW window.
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

#include "SurfaceKHR.hxx"

#include <spdlog/spdlog.h>

void SurfaceKHR::createSurfaceKHR(VkSurfaceKHR *surface, VkInstance vulkanInstance, GLFWwindow *window) {
    if (glfwCreateWindowSurface(vulkanInstance, window, nullptr, surface) != VK_SUCCESS) {
        spdlog::error("Failed to create Vulkan window surface instance.");
        throw std::runtime_error("Failed to create window surface!");
    }
}