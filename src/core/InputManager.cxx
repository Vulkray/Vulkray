/*
 * InputManager.cxx
 * Uses the GLFW library to get Window keyboard / mouse input.
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2023, Max Rodriguez. All rights reserved.
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
            if (callback.key.compare(alias.keyAlias) != 0) continue; // skip if callback key doesn't match

            if (callback.action != KEY_ANY) {
                // skip if callback action doesn't match
                if (callback.action != action) {
                    // could be that callback.action == 3, which is our own feature that represents either 1 or 2.
                    if ((callback.action == KEY_EITHER) & (action != KEY_RELEASED)) {
                        callback.pFunction(callback.caller, this->m_window->m_vulkan->base, action);
                    } else continue;
                }
            }
            callback.pFunction(callback.caller, this->m_window->m_vulkan->base, action);
        }
        return;
    }
    spdlog::error("The UserInput module key callback received an invalid key!");
    throw std::runtime_error("An invalid key was received by the input module.");
}

void InputManager::_non_static_cursor_callback(double x_pos, double y_pos) {
    // call every cursor callback allocated by the developer
    for (CursorCallback callback : this->cursorCallbacks) {
        callback.pFunction(callback.caller, this->m_window->m_vulkan->base, x_pos, y_pos);
    }
}

void InputManager::static_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {

    auto m_vulkan = reinterpret_cast<Vulkan*>(glfwGetWindowUserPointer(window));
    // pass key callback parameter data to non-static method from UserInput class to process input
    m_vulkan->base->input->_non_static_key_callback(key, scancode, action, mods);
}

void InputManager::static_cursor_callback(GLFWwindow *window, double x_pos, double y_pos) {

    auto m_vulkan = reinterpret_cast<Vulkan*>(glfwGetWindowUserPointer(window));
    m_vulkan->base->input->_non_static_cursor_callback(x_pos, y_pos);
}

void InputManager::_non_static_init_glfw_input(Window *m_window) {
    this->m_window = m_window; // store window module pointer
    glfwSetKeyCallback(this->m_window->window, this->static_key_callback);
    glfwSetCursorPosCallback(this->m_window->window, this->static_cursor_callback);
}

void InputManager::_static_init_glfw_input(Vulkan *m_vulkan) {
    // call non-static method from UserInput class and pass window module pointer
    m_vulkan->base->input->_non_static_init_glfw_input(m_vulkan->m_window.get());
}

void InputManager::new_accept_key(const char *key, int action, void *caller,
                              void (*pFunction)(void *caller, ShowBase *, int action)) {

    size_t keyMapLength = sizeof(this->keyAliases) / sizeof(GLFWKeyAlias);

    for (size_t i = 0; i < keyMapLength; i++) {
        if (this->keyAliases[i].keyAlias.compare(key) == 0) continue;
        if (action > 4 || action < 0) {
            spdlog::error("new_accept(): Key callbacks can only accept from an action between 0-4!");
            throw std::runtime_error("An invalid action was given to the input module to accept.");
        }
        // once params are validated, append new callback struct to vector
        KeyCallback newCallback;
        newCallback.key.assign(key);
        newCallback.action = action;
        newCallback.caller = caller;
        newCallback.pFunction = pFunction;
        this->keyCallbacks.push_back(newCallback);
        return;
    }
    spdlog::error("new_accept(): Received an invalid key string!");
    throw std::runtime_error("An invalid key was given to the input module to accept.");
}

void InputManager::new_accept_key(const char *key, void *caller,
                              void (*pFunction)(void *caller, ShowBase *, int action)) {

    size_t keyMapLength = sizeof(this->keyAliases) / sizeof(GLFWKeyAlias);

    for (size_t i = 0; i < keyMapLength; i++) {
        if (this->keyAliases[i].keyAlias.compare(key) == 0) continue;

        // once params are validated, append new callback struct to vector
        KeyCallback newCallback;
        newCallback.key.assign(key);
        newCallback.action = KEY_ANY; // this method overload takes no action param, listen to all actions.
        newCallback.caller = caller;
        newCallback.pFunction = pFunction;
        this->keyCallbacks.push_back(newCallback);
        return;
    }
    spdlog::error("new_accept(): Received an invalid key string!");
    throw std::runtime_error("An invalid key was given to the input module to accept.");
}

void InputManager::remove_accept_key(const char *key, int action) {
    size_t keyMapLength = sizeof(this->keyAliases) / sizeof(GLFWKeyAlias);

    for (size_t i = 0; i < keyMapLength; i++) {
        GLFWKeyAlias alias = this->keyAliases[i];
        if (alias.keyAlias != key) continue;

        int j = 0; // search through callbacks vector and find index of matching callback to delete
        for (KeyCallback callback : this->keyCallbacks) {
            if (callback.key.compare(alias.keyAlias) != 0) {
                j++; continue;
            }
            if (callback.action != action) {
                if ((callback.action == KEY_EITHER) & (action != KEY_RELEASED)) {
                    this->keyCallbacks.erase(this->keyCallbacks.begin() + j);
                    return; // callback found, return
                } else {
                    j++; continue;
                    /* I know this is a mess and would be cleaner if the for loop incremented j for me,
                     * but it works and does not affect performance at all. and it's late.
                     */
                }
            }
            this->keyCallbacks.erase(this->keyCallbacks.begin() + j);
            return; // callback found, return
        }
        return;
    }
    spdlog::error("remove_accept(): Received an invalid key string!");
    throw std::runtime_error("An invalid key was received by the input module.");
}

void InputManager::remove_accept_key(const char *key) {
    size_t keyMapLength = sizeof(this->keyAliases) / sizeof(GLFWKeyAlias);

    for (size_t i = 0; i < keyMapLength; i++) {
        GLFWKeyAlias alias = this->keyAliases[i];
        if (alias.keyAlias != key) continue;

        int j = 0; // search through callbacks vector and find index of matching callback to delete
        for (KeyCallback callback : this->keyCallbacks) {
            if (callback.key.compare(alias.keyAlias) != 0) {
                j++; continue;
            }
            this->keyCallbacks.erase(this->keyCallbacks.begin() + j);
            return; // callback found, return
        }
        return;
    }
    spdlog::error("remove_accept(): Received an invalid key string!");
    throw std::runtime_error("An invalid key was received by the input module.");
}

void InputManager::new_accept_cursor(void *caller, const char* id,
                                     void (*pFunction)(void *, ShowBase *, double, double)) {
    CursorCallback newCallback;
    newCallback.id.assign(id);
    newCallback.caller = caller;
    newCallback.pFunction = pFunction;
    this->cursorCallbacks.push_back(newCallback);
}

void InputManager::remove_accept_cursor(const char *id) {
    int j = 0;
    for (CursorCallback callback : this->cursorCallbacks) {
        if (callback.id.compare(id) != 0) {
            j++; continue;
        }
        this->cursorCallbacks.erase(this->cursorCallbacks.begin() + j);
        return;
    }
}