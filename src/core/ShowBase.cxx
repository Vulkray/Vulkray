/*
 * ShowBase.cxx
 * Defines the ShowBase class which initializes the Vulkray engine.
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2022, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#include "../global_definitions.h"
#include <spdlog/spdlog.h>

#include "../../include/Vulkray/Vulkan.h"
#include "../../include/Vulkray/ShowBase.h"

ShowBase::ShowBase(EngineConfig config) {
    this->config = config;

    // spdlog debug/release output configuration
#ifndef NDEBUG
    spdlog::set_level(spdlog::level::debug); // enable debug logging for debug builds
#else
    spdlog::set_level(spdlog::level::info); // only print info output on release builds
#endif
    spdlog::set_pattern("[%H:%M:%S] [%n] [%^%l%$] %v");

    // Initialize top level show base instances
    this->input = std::make_unique<InputManager>();
    this->jobManager = std::make_unique<JobManager>();
    this->camera = std::make_unique<Camera>();
}

ShowBase::~ShowBase() {
    // not actually required, just a placeholder for now
    this->input.reset();
    this->jobManager.reset();
    this->camera.reset();
    this->vulkanRenderer.reset();
}

void ShowBase::launch() {
    // Enable built-in default camera controls
    this->enable_cam_controls();
    // Initialize the engine vulkan renderer loop
    this->vulkanRenderer = std::make_unique<Vulkan>(this, this->config.graphicsInput,
                                                    (char*) this->config.windowTitle,
                                                    this->input->_static_init_glfw_input);
    // NOTE: Vulkan should be initialized last ALWAYS, because that's where the render loop starts.
}

// ----- Default Camera Controls ----- //

void ShowBase::enable_cam_controls() {
    if (this->defaultCamEnabled) return; // can't enable if already enabled!
    this->defaultCamEnabled = true;
    this->jobManager->new_job("_builtin_camera", this, &this->camera_task);
    this->input->new_accept_cursor(this, "_builtin_cam_look", this->cam_mouse_look);
    this->input->new_accept_key("w", this, this->cam_control_forward);
    this->input->new_accept_key("s", this, this->cam_control_backward);
    this->input->new_accept_key("a", this, this->cam_control_left);
    this->input->new_accept_key("d", this, this->cam_control_right);
    this->input->new_accept_key("q", this, this->cam_fov_increase);
    this->input->new_accept_key("e", this, this->cam_fov_decrease);
}

void ShowBase::disable_cam_controls() {
    if (!this->defaultCamEnabled) return; // can't disable if already disabled!
    this->defaultCamEnabled = false;
    this->jobManager->remove_job("_builtin_camera");
    this->input->remove_accept_cursor("_builtin_cam_look");
    this->input->remove_accept_key("w");
    this->input->remove_accept_key("s");
    this->input->remove_accept_key("a");
    this->input->remove_accept_key("d");
    this->input->remove_accept_key("q");
    this->input->remove_accept_key("e");
}

void ShowBase::camera_task(void *caller, ShowBase *base) {
    ShowBase* self = (ShowBase*)caller; // cast void pointer to defined class pointer
    for (int i = 0; i < 6; i++) {
        base->camera->set_x(base->camera->x + (0.03 * self->_cam_controls_key_map[0]));
        base->camera->set_x(base->camera->x - (0.03 * self->_cam_controls_key_map[1]));
        base->camera->set_y(base->camera->y + (0.03 * self->_cam_controls_key_map[2]));
        base->camera->set_y(base->camera->y - (0.03 * self->_cam_controls_key_map[3]));
        base->camera->set_fov(base->camera->fov + (0.1 * self->_cam_controls_key_map[4]));
        base->camera->set_fov(base->camera->fov - (0.1 * self->_cam_controls_key_map[5]));
        // fov limiter
        if (base->camera->fov > 120) base->camera->set_fov(120);
        if (base->camera->fov < 30) base->camera->set_fov(30);
    }
}

void ShowBase::cam_mouse_look(void *caller, ShowBase *base, double x, double y) {
    base->camera->set_hpr(x, y, 0);
}

void ShowBase::cam_control_forward(void *caller, ShowBase *base, int action) {
    ShowBase* self = (ShowBase*)caller;
    switch (action) {
        case KEY_PRESSED:
            self->_cam_controls_key_map[0] = 1;
            break;
        case KEY_RELEASED:
            self->_cam_controls_key_map[0] = 0;
            break;
    }
}

void ShowBase::cam_control_backward(void *caller, ShowBase *base, int action) {
    ShowBase* self = (ShowBase*)caller;
    switch (action) {
        case KEY_PRESSED:
            self->_cam_controls_key_map[1] = 1;
            break;
        case KEY_RELEASED:
            self->_cam_controls_key_map[1] = 0;
            break;
    }
}

void ShowBase::cam_control_left(void *caller, ShowBase *base, int action) {
    ShowBase* self = (ShowBase*)caller;
    switch (action) {
        case KEY_PRESSED:
            self->_cam_controls_key_map[2] = 1;
            break;
        case KEY_RELEASED:
            self->_cam_controls_key_map[2] = 0;
            break;
    }
}

void ShowBase::cam_control_right(void *caller, ShowBase *base, int action) {
    ShowBase* self = (ShowBase*)caller;
    switch (action) {
        case KEY_PRESSED:
            self->_cam_controls_key_map[3] = 1;
            break;
        case KEY_RELEASED:
            self->_cam_controls_key_map[3] = 0;
            break;
    }
}

void ShowBase::cam_fov_increase(void *caller, ShowBase *base, int action) {
    ShowBase* self = (ShowBase*)caller;
    switch (action) {
        case KEY_PRESSED:
            self->_cam_controls_key_map[4] = 1;
            break;
        case KEY_RELEASED:
            self->_cam_controls_key_map[4] = 0;
            break;
    }
}

void ShowBase::cam_fov_decrease(void *caller, ShowBase *base, int action) {
    ShowBase* self = (ShowBase*)caller;
    switch (action) {
        case KEY_PRESSED:
            self->_cam_controls_key_map[5] = 1;
            break;
        case KEY_RELEASED:
            self->_cam_controls_key_map[5] = 0;
            break;
    }
}