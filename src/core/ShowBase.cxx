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
    this->input->new_accept("w", KEY_EITHER, this->cam_control_forward);
    this->input->new_accept("s", KEY_EITHER, this->cam_control_backward);
    this->input->new_accept("a", KEY_EITHER, this->cam_control_left);
    this->input->new_accept("d", KEY_EITHER, this->cam_control_right);
}

void ShowBase::disable_cam_controls() {
    this->input->remove_accept("w", KEY_EITHER);
    this->input->remove_accept("s", KEY_EITHER);
    this->input->remove_accept("a", KEY_EITHER);
    this->input->remove_accept("d", KEY_EITHER);
}

void ShowBase::cam_control_forward(ShowBase *base) {
    base->camera->set_x(base->camera->x + 0.05);
}

void ShowBase::cam_control_backward(ShowBase *base) {
    base->camera->set_x(base->camera->x - 0.05);
}

void ShowBase::cam_control_left(ShowBase *base) {
    base->camera->set_y(base->camera->y + 0.05);
}

void ShowBase::cam_control_right(ShowBase *base) {
    base->camera->set_y(base->camera->y - 0.05);
}