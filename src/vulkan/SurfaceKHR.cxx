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

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

#ifdef _WIN32 // Windows platform-specific (I haven't even tested this tbh)
    #define VK_USE_PLATFORM_WIN32_KHR
    #define GLFW_EXPOSE_NATIVE_WIN32
    #include <GLFW/glfw3native.h>
#endif

void SurfaceKHR::createSurfaceKHR(VkSurfaceKHR *surface, VkInstance vulkanInstance, GLFWwindow *window) {

    // Windows platform-specific window
    #ifdef _WIN32
        VkWin32SurfaceCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hwnd = glfwGetWin32Window(window);
        createInfo.hinstance = GetModuleHandle(nullptr);

        if (vkCreateWin32SurfaceKHR(vulkanInstance, &createInfo, nullptr, surface) != VK_SUCCESS) {
            spdlog::error("Failed to create Vulkan window surface instance.");
            throw std::runtime_error("Failed to create window surface!");
        }
    // GLFW window surface
    #elifdef __unix__
        if (glfwCreateWindowSurface(vulkanInstance, window, nullptr, surface) != VK_SUCCESS) {
            spdlog::error("Failed to create Vulkan window surface instance.");
            throw std::runtime_error("Failed to create window surface!");
        }
    #endif
}