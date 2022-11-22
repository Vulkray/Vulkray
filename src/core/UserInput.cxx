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
#include "../../include/Vulkray/UserInput.h"

UserInput::UserInput() {
    // placeholder
}

UserInput::~UserInput() {
    // placeholder
}

void UserInput::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    // placeholder
}

void UserInput::_init_glfw_callback(Window *m_window) {
    this->m_window = m_window; // store window module pointer
    glfwSetKeyCallback(this->m_window->window, UserInput::key_callback);
}