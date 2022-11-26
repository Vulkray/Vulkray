/*
 * InputManager.cxx
 * Uses the GLFW library to get Window keyboard / mouse input.
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2022, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#include <string>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include "../../include/Vulkray/InputManager.h"

InputManager::InputManager() {
    // placeholder
}

InputManager::~InputManager() {
    // placeholder
}

void InputManager::_non_static_key_callback(int key, int scancode, int action, int mods) {
    size_t keyMapLength = sizeof(this->keyAliases) / sizeof(GLFWKeyAlias);

    for (size_t i = 0; i < keyMapLength; i++) {
        GLFWKeyAlias alias = this->keyAliases[i];
        if (alias.glfwKeyID != key) continue;

        for (KeyCallback callback : this->keyCallbacks) {
            if (callback.key.compare(alias.keyAlias) == 0) continue; // skip if callback key doesn't match
            if (callback.action != action) continue; // skip if callback action doesn't match
            callback.pFunction(this->m_window->m_vulkan->base);
        }
        return;
    }
    spdlog::error("The UserInput module key callback received an invalid key!");
    throw std::runtime_error("An invalid key was received by the input module.");
}

void InputManager::static_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {

    auto m_vulkan = reinterpret_cast<Vulkan*>(glfwGetWindowUserPointer(window));
    // pass key callback parameter data to non-static method from UserInput class to process input
    m_vulkan->base->input->_non_static_key_callback(key, scancode, action, mods);
}

void InputManager::_non_static_init_glfw_input(Window *m_window) {
    this->m_window = m_window; // store window module pointer
    glfwSetKeyCallback(this->m_window->window, this->static_key_callback);
}

void InputManager::_static_init_glfw_input(Vulkan *m_vulkan) {
    // call non-static method from UserInput class and pass window module pointer
    m_vulkan->base->input->_non_static_init_glfw_input(m_vulkan->m_window.get());
}

void InputManager::accept_key(const char *key, int action, void (*pFunction)(ShowBase *)) {
    size_t keyMapLength = sizeof(this->keyAliases) / sizeof(GLFWKeyAlias);

    for (size_t i = 0; i < keyMapLength; i++) {
        if (this->keyAliases[i].keyAlias.compare(key) == 0) continue;
        if (action > 2 || action < 0) {
            spdlog::error("Key callbacks can only accept from an action between 0-2!");
            throw std::runtime_error("An invalid action was given to the input module to accept.");
        }
        // once params are validated, append new callback struct to vector
        KeyCallback newCallback;
        newCallback.key.assign(key);
        newCallback.action = action;
        newCallback.pFunction = pFunction;
        this->keyCallbacks.push_back(newCallback);
        return;
    }
    spdlog::error("An invalid key was given to the input module to accept!");
    throw std::runtime_error("An invalid key was given to the input module to accept.");
}