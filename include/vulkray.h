/*
 * Vulkray.h
 * API Header - The first header file in early development of the Vulkray engine API.
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2022, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#ifndef VULKRAY_API_VULKRAY_H
#define VULKRAY_API_VULKRAY_H

#include "vulkan.h"
#include <memory>

class VulkrayEngine {
public:
    GraphicsInput graphicsInput;
    VulkrayEngine();
    ~VulkrayEngine();
    void initialize();
private:
    std::unique_ptr<Vulkan> m_vulkan;
};

#endif //VULKRAY_API_VULKRAY_H