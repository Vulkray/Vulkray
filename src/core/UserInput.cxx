/*
 * UserInput.cxx
 * Uses the GLFW library to get Window keyboard / mouse input.
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2022, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include "../../include/Vulkray/UserInput.h"

UserInput::UserInput() {
    // placeholder
}

UserInput::~UserInput() {
    // placeholder
}

void UserInput::_non_static_key_callback(int key, int scancode, int action, int mods) {

    size_t keyMapLength = sizeof(this->keyMap) / sizeof(this->keyMap[0]);

    for (size_t i = 0; i < keyMapLength; i++) {
        if (this->keyMap[i][0] != key) continue;
        this->keyMap[i][1] = action; // either GLFW_PRESS, GLFW_REPEAT, or GLFW_RELEASE (0, 1, or 2)
    }
}

void UserInput::static_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {

    auto m_vulkan = reinterpret_cast<Vulkan*>(glfwGetWindowUserPointer(window));
    // pass key callback parameter data to non-static method from UserInput class to process input
    m_vulkan->base->input->_non_static_key_callback(key, scancode, action, mods);
}

void UserInput::_non_static_init_glfw_input(Window *m_window) {
    this->m_window = m_window; // store window module pointer
    glfwSetKeyCallback(this->m_window->window, this->static_key_callback);
}

void UserInput::_static_init_glfw_input(Vulkan *m_vulkan) {
    // call non-static method from UserInput class and pass window module pointer
    m_vulkan->base->input->_non_static_init_glfw_input(m_vulkan->m_window.get());
}