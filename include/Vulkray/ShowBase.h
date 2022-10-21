/*
 * ShowBase.h
 * API Header - The Vulkray API entry point. Defines the ShowBase class.
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2022, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#ifndef VULKRAY_API_SHOWBASE_H
#define VULKRAY_API_SHOWBASE_H

#include "Graphics.h"
#include "JobManager.h"
#include "Camera.h"
#include <memory>

struct EngineConfig {
    const char* windowTitle = nullptr; // default set at vulkan/Window.cxx module
    GraphicsInput graphicsInput; // temporary
};

class ShowBase {
public:
    EngineConfig config;
    std::unique_ptr<JobManager> jobManager;
    std::unique_ptr<Camera> camera;
    ShowBase(EngineConfig config);
    ~ShowBase();
    void launch();
private:
    std::unique_ptr<Vulkan> vulkanRenderer;
};

#endif //VULKRAY_API_SHOWBASE_H
