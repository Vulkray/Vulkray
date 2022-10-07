/*
 * EngineBase.cxx
 * Defines the base engine class which initializes the Vulkray engine.
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2022, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#include "global_definitions.h"
#include <spdlog/spdlog.h>

#include "vulkan/Vulkan.h"
#include "../include/vulkray.h"

VulkrayEngine::VulkrayEngine() {
    // spdlog debug/release output configuration
#ifndef NDEBUG
    spdlog::set_level(spdlog::level::debug); // enable debug logging for debug builds
#else
    spdlog::set_level(spdlog::level::info); // only print info output on release builds
#endif
    spdlog::set_pattern("[%H:%M:%S] [%n] [%^%l%$] %v");

    // fill in default graphics input struct
    this->graphicsInput.vertexData = {};
    this->graphicsInput.indexData = {};
    this->graphicsInput.bufferClearColor = (VkClearValue){{{0.1f, 0.1f, 0.1f, 1.0f}}};
}

VulkrayEngine::~VulkrayEngine() {
    this->m_vulkan.reset(); // not actually required, just a placeholder for now
}

void VulkrayEngine::initialize() {
    // Initialize the engine vulkan renderer loop
    this->m_vulkan = std::make_unique<Vulkan>(this->graphicsInput);
}
