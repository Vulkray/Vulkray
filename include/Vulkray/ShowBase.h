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
    bool defaultCamEnabled = false;
    std::unique_ptr<InputManager> input;
    std::unique_ptr<JobManager> jobManager;
    std::unique_ptr<Camera> camera;
    ShowBase(EngineConfig config);
    ~ShowBase();
    void launch();
    void enable_cam_controls();
    void disable_cam_controls();
    // below has to be public, used by the builtin camera (accessed via key callback static method)
    int _cam_controls_key_map[6] = {0, 0, 0, 0, 0, 0};
private:
    std::unique_ptr<Vulkan> vulkanRenderer;
    // default cam control callbacks
    static void camera_task(void *caller, ShowBase *base);
    static void cam_control_forward(void *caller, ShowBase *base, int action);
    static void cam_control_backward(void *caller, ShowBase *base, int action);
    static void cam_control_left(void *caller, ShowBase *base, int action);
    static void cam_control_right(void *caller, ShowBase *base, int action);
    static void cam_fov_increase(void *caller, ShowBase *base, int action);
    static void cam_fov_decrease(void *caller, ShowBase *base, int action);
    static void cam_mouse_look(void *caller, ShowBase *base, double x, double y);
};

#endif //VULKRAY_API_SHOWBASE_H
