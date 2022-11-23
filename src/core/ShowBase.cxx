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
    this->input = std::make_unique<UserInput>();
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
    // Initialize the engine vulkan renderer loop
    this->vulkanRenderer = std::make_unique<Vulkan>(this, this->config.graphicsInput,
                                                    (char*) this->config.windowTitle,
                                                    this->input->_static_init_glfw_input);
}
