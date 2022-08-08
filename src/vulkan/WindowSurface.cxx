/*
 * SurfaceKHR.cxx
 * Creates the Vulkan Surface instance for presenting to the GLFW window.
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2022, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#include "Vulkan.hxx"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#ifdef _WIN32 // Windows platform-specific (I haven't even tested this tbh)
    #define VK_USE_PLATFORM_WIN32_KHR
    #define GLFW_EXPOSE_NATIVE_WIN32
    #include <GLFW/glfw3native.h>
#endif

WindowSurface::WindowSurface(Vulkan *m_vulkan): VkModuleBase(m_vulkan) {

    // Windows platform-specific window
    #ifdef _WIN32
        VkWin32SurfaceCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hwnd = glfwGetWin32Window(this->m_vulkan->engineWindow);
        createInfo.hinstance = GetModuleHandle(nullptr);

        VkResult result = vkCreateWin32SurfaceKHR(this->m_vulkan->m_vulkanInstance->vulkanInstance,
                                                  &createInfo, nullptr, this->surface);
        if (result != VK_SUCCESS) {
            spdlog::error("Failed to create Vulkan window surface instance.");
            throw std::runtime_error("Failed to create window surface!");
        }
    // GLFW window surface
    #elifdef __unix__
        VkResult result = glfwCreateWindowSurface(this->m_vulkan->m_vulkanInstance->vulkanInstance,
                                                  this->m_vulkan->engineWindow, nullptr, &this->surface);

        if (result != VK_SUCCESS) {
            // Check if it's a GLFW error or a Vulkan error
            const char *errorMsg;
            int errorCode = glfwGetError(&errorMsg);

            if (errorCode != GLFW_NO_ERROR) {
                spdlog::error("A GLFW error occurred while trying to create the window surface:");
                spdlog::error("Error: {0}", errorMsg); // print error description
            } else {
                spdlog::error("A Vulkan error occurred while trying to create the window surface:");
                switch (result) {
                    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
                        spdlog::error("Vulkan error returned: VK_ERROR_INCOMPATIBLE_DISPLAY_KHR");
                        break;
                    default:
                        spdlog::error("An unkown Vulkan error was returned by glfwCreateWindowSurface.");
                        break;
                }
            }
            throw std::runtime_error("Failed to create the Vulkan window surface!");
        }
    #endif
}

WindowSurface::~WindowSurface() {
    vkDestroySurfaceKHR(this->m_vulkan->m_vulkanInstance->vulkanInstance, this->surface, nullptr);
}