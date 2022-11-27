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

#include "InputManager.h"
#include "JobManager.h"
#include "Camera.h"
#include "Vulkan.h"
#include <memory>

// class prototypes
class InputManager;

struct EngineConfig {
    const char* windowTitle = nullptr; // default set at vulkan/Window.cxx module
    GraphicsInput graphicsInput; // temporary
};

class ShowBase {
public:
    EngineConfig config;
    std::unique_ptr<InputManager> input;
    std::unique_ptr<JobManager> jobManager;
    std::unique_ptr<Camera> camera;
    ShowBase(EngineConfig config);
    ~ShowBase();
    void launch();
    void enable_cam_controls();
    void disable_cam_controls();
private:
    std::unique_ptr<Vulkan> vulkanRenderer;
    // default cam control callbacks
    static void cam_control_forward(ShowBase *base);
    static void cam_control_backward(ShowBase *base);
    static void cam_control_left(ShowBase *base);
    static void cam_control_right(ShowBase *base);
};

#endif //VULKRAY_API_SHOWBASE_H
