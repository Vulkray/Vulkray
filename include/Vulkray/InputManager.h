/*
 * InputManager.h
 * API Header - Uses the GLFW library to get Window keyboard / mouse input.
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2022, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#ifndef VULKRAY_API_INPUTMANAGER_H
#define VULKRAY_API_INPUTMANAGER_H

#include "Vulkan.h"
#include <GLFW/glfw3.h>
#include <string>

#define KEY_RELEASED 0
#define KEY_PRESSED 1
#define KEY_HELD 2

struct KeyCallback {
    std::string key;
    void(*pFunction)(ShowBase *base); // all callbacks must return void
};

class UserInput {
private:
    Window *m_window;
    std::vector<KeyCallback> keyCallbacks;
    static void static_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    int keyMap[128][2] = { // 2^7 (7-bit) = 128 possible keys. nested array is GLFW key macro & 0-2 value (key status)
            // Integer
            {GLFW_KEY_0, 0}, {GLFW_KEY_1, 0}, {GLFW_KEY_2, 0}, {GLFW_KEY_3, 0}, {GLFW_KEY_4, 0},
            {GLFW_KEY_5, 0}, {GLFW_KEY_6, 0}, {GLFW_KEY_7, 0}, {GLFW_KEY_8, 0}, {GLFW_KEY_9, 0},
            // Character
            {GLFW_KEY_A, 0}, {GLFW_KEY_B, 0}, {GLFW_KEY_C, 0}, {GLFW_KEY_D, 0}, {GLFW_KEY_E, 0},
            {GLFW_KEY_F, 0}, {GLFW_KEY_G, 0}, {GLFW_KEY_H, 0}, {GLFW_KEY_I, 0}, {GLFW_KEY_J, 0},
            {GLFW_KEY_K, 0}, {GLFW_KEY_L, 0}, {GLFW_KEY_M, 0}, {GLFW_KEY_N, 0}, {GLFW_KEY_O, 0},
            {GLFW_KEY_P, 0}, {GLFW_KEY_Q, 0}, {GLFW_KEY_R, 0}, {GLFW_KEY_S, 0}, {GLFW_KEY_T, 0},
            {GLFW_KEY_U, 0}, {GLFW_KEY_V, 0}, {GLFW_KEY_W, 0}, {GLFW_KEY_X, 0}, {GLFW_KEY_Y, 0}, {GLFW_KEY_Z, 0},
            // Function
            {GLFW_KEY_F1, 0}, {GLFW_KEY_F2, 0}, {GLFW_KEY_F3, 0}, {GLFW_KEY_F4, 0}, {GLFW_KEY_F5, 0},
            {GLFW_KEY_F6, 0}, {GLFW_KEY_F7, 0}, {GLFW_KEY_F8, 0}, {GLFW_KEY_F9, 0}, {GLFW_KEY_F10, 0},
            {GLFW_KEY_F11, 0}, {GLFW_KEY_F12, 0}, {GLFW_KEY_F13, 0}, {GLFW_KEY_F14, 0}, {GLFW_KEY_F15, 0},
            {GLFW_KEY_F16, 0}, {GLFW_KEY_F17, 0}, {GLFW_KEY_F18, 0}, {GLFW_KEY_F19, 0}, {GLFW_KEY_F20, 0},
            {GLFW_KEY_F21, 0}, {GLFW_KEY_F22, 0}, {GLFW_KEY_F23, 0}, {GLFW_KEY_F24, 0}, {GLFW_KEY_F25, 0},
            // Punctuation
            {GLFW_KEY_APOSTROPHE, 0}, {GLFW_KEY_COMMA, 0}, {GLFW_KEY_PERIOD, 0}, {GLFW_KEY_SLASH, 0},
            {GLFW_KEY_SEMICOLON, 0}, {GLFW_KEY_BACKSLASH, 0}, {GLFW_KEY_MINUS, 0}, {GLFW_KEY_EQUAL, 0},
            {GLFW_KEY_LEFT_BRACKET, 0}, {GLFW_KEY_RIGHT_BRACKET, 0}, {GLFW_KEY_GRAVE_ACCENT, 0}, {GLFW_KEY_SPACE, 0},
            // Modifiers
            {GLFW_KEY_ESCAPE, 0}, {GLFW_KEY_TAB, 0}, {GLFW_KEY_CAPS_LOCK, 0}, {GLFW_KEY_LEFT_SHIFT, 0},
            {GLFW_KEY_RIGHT_SHIFT, 0}, {GLFW_KEY_ENTER, 0}, {GLFW_KEY_BACKSPACE, 0}, {GLFW_KEY_LEFT_CONTROL, 0},
            {GLFW_KEY_RIGHT_CONTROL, 0}, {GLFW_KEY_LEFT_ALT, 0}, {GLFW_KEY_RIGHT_ALT, 0},
            {GLFW_KEY_LEFT_SUPER, 0}, {GLFW_KEY_RIGHT_SUPER, 0}, // windows/command key(s)
            // Navigation
            {GLFW_KEY_UP, 0}, {GLFW_KEY_DOWN, 0}, {GLFW_KEY_LEFT, 0}, {GLFW_KEY_RIGHT, 0},
            {GLFW_KEY_DELETE, 0}, {GLFW_KEY_INSERT, 0}, {GLFW_KEY_HOME, 0},
            {GLFW_KEY_PAGE_UP, 0}, {GLFW_KEY_PAGE_DOWN, 0}, {GLFW_KEY_END, 0},
            {GLFW_KEY_PRINT_SCREEN, 0}, {GLFW_KEY_SCROLL_LOCK, 0}, {GLFW_KEY_PAUSE, 0},
            // Number Pad (Key Pad)
            {GLFW_KEY_KP_SUBTRACT, 0}, {GLFW_KEY_KP_DIVIDE, 0}, {GLFW_KEY_KP_ADD, 0}, {GLFW_KEY_KP_MULTIPLY, 0},
            {GLFW_KEY_KP_DECIMAL, 0}, {GLFW_KEY_KP_ENTER, 0}, {GLFW_KEY_KP_EQUAL, 0}, {GLFW_KEY_KP_0, 0},
            {GLFW_KEY_KP_1, 0}, {GLFW_KEY_KP_2, 0}, {GLFW_KEY_KP_3, 0}, {GLFW_KEY_KP_4, 0}, {GLFW_KEY_KP_5, 0},
            {GLFW_KEY_KP_6, 0}, {GLFW_KEY_KP_7, 0}, {GLFW_KEY_KP_8, 0}, {GLFW_KEY_KP_9, 0}
    };
    std::string keyAliases[128] = {
            // Integer
            "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
            // Character
            "a", "b", "c", "d", "e", "f", "g", "h", "i", "j",
            "k", "l", "m", "n", "o", "p", "q", "r", "s", "t",
            "u", "v", "w", "x", "y", "z",
            // Function
            "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10",
            "F11", "F12", "F13", "F14", "F15", "F16", "F17", "F18", "F19", "F20",
            "F21", "F22", "F23", "F24", "F25",
            // Punctuation
            "'", ",", ".", "/", ";", "\\", "-", "=", "[", "]", "`", " ",
            // Modifiers
            "ESC", "TAB", "CAPS_LOCK", "LEFT_SHIFT", "RIGHT_SHIFT", "ENTER", "BACKSPACE",
            "LEFT_CONTROL", "RIGHT_CONTROL", "LEFT_ALT", "RIGHT_ALT", "LEFT_SUPER", "RIGHT_SUPER",
            // Navigation
            "UP", "DOWN", "LEFT", "RIGHT", "DELETE", "INSERT", "HOME", "PAGE_UP",
            "PAGE_DOWN", "END", "PRINT_SCREEN", "SCROLL_LOCK", "PAUSE",
            // Number Pad (Key Pad)
            "NP_SUBTRACT", "NP_DIVIDE", "NP_ADD", "NP_MULTIPLY", "NP_DECIMAL",
            "NP_ENTER", "NP_EQUAL", "NP_0", "NP_1", "NP_2", "NP_3",
            "NP_4", "NP_5", "NP_6", "NP_7", "NP_8", "NP_9"
    };
public:
    UserInput();
    ~UserInput();
    void accept(const char *key, int action, void (*pFunction)(ShowBase *base));
    static void _static_init_glfw_input(Vulkan *m_vulkan);
    void _non_static_init_glfw_input(Window *m_window);
    void _non_static_key_callback(int key, int scancode, int action, int mods);
};

#endif //VULKRAY_API_INPUTMANAGER_H
