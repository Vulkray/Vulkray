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

#ifndef VULKRAY_SURFACEKHR_HXX
#define VULKRAY_SURFACEKHR_HXX

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

class SurfaceKHR {
public:
    static void createSurfaceKHR(VkSurfaceKHR *surface, VkInstance vulkanInstance, GLFWwindow *window);
};

#endif //VULKRAY_SURFACEKHR_HXX
