/*
 * DepthBuffering.cxx
 * Handles creating the Depth Buffer resources and images for 3D fragment depth.
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2022, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#include "Vulkan.h"
#include <spdlog/spdlog.h>

DepthBuffering::DepthBuffering(Vulkan *m_vulkan): VkModuleBase(m_vulkan) {

    // Find a suitable depth format supported by the GPU
    VkFormat depthFormat = this->m_vulkan->m_physicalDevice->findDepthFormat();
}

DepthBuffering::~DepthBuffering() {

}