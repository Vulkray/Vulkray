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
#define KEY_EITHER 3
#define KEY_ANY 4

struct GLFWKeyAlias {
    int glfwKeyID;
    std::string keyAlias;
};
struct KeyCallback {
    std::string key;
    int action; // integer in 0-4 range
    void *caller; // pointer to class that created callback
    void(*pFunction)(void *caller, ShowBase *base, int action); // all callbacks must return void
};
struct CursorCallback {
    void *caller; // pointer to class that created callback
    void(*pFunction)(void *caller, ShowBase *base, double x, double y); // all callbacks must return void
};

class InputManager {
private:
    Window *m_window;
    std::vector<KeyCallback> keyCallbacks;
    std::vector<CursorCallback> cursorCallbacks;
    static void static_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void static_cursor_callback(GLFWwindow* window, double x_pos, double y_pos);

    GLFWKeyAlias keyAliases[128] = { // 2^7 (7-bit; USB keyboard standard) = 128 possible keys
            // Integer
            {GLFW_KEY_0, "0"}, {GLFW_KEY_1, "1"}, {GLFW_KEY_2, "2"},
            {GLFW_KEY_3, "3"}, {GLFW_KEY_4, "4"}, {GLFW_KEY_5, "5"},
            {GLFW_KEY_6, "6"}, {GLFW_KEY_7, "7"}, {GLFW_KEY_8, "8"}, {GLFW_KEY_9, "9"},
            // Character
            {GLFW_KEY_A, "a"}, {GLFW_KEY_B, "b"}, {GLFW_KEY_C, "c"}, {GLFW_KEY_D, "d"},
            {GLFW_KEY_E, "e"}, {GLFW_KEY_F, "f"}, {GLFW_KEY_G, "g"}, {GLFW_KEY_H, "h"},
            {GLFW_KEY_I, "i"}, {GLFW_KEY_J, "j"}, {GLFW_KEY_K, "k"}, {GLFW_KEY_L, "l"},
            {GLFW_KEY_M, "m"}, {GLFW_KEY_N, "n"}, {GLFW_KEY_O, "o"}, {GLFW_KEY_P, "p"},
            {GLFW_KEY_Q, "q"}, {GLFW_KEY_R, "r"}, {GLFW_KEY_S, "s"}, {GLFW_KEY_T, "t"}, {GLFW_KEY_U, "u"},
            {GLFW_KEY_V, "v"}, {GLFW_KEY_W, "w"}, {GLFW_KEY_X, "x"}, {GLFW_KEY_Y, "y"}, {GLFW_KEY_Z, "z"},
            // Function
            {GLFW_KEY_F1, "F1"}, {GLFW_KEY_F2, "F2"}, {GLFW_KEY_F3, "F3"}, {GLFW_KEY_F4, "F4"},
            {GLFW_KEY_F5, "F5"}, {GLFW_KEY_F6, "F6"}, {GLFW_KEY_F7, "F7"}, {GLFW_KEY_F8, "F8"},
            {GLFW_KEY_F9, "F9"}, {GLFW_KEY_F10, "F10"}, {GLFW_KEY_F11, "F11"}, {GLFW_KEY_F12, "F12"},
            {GLFW_KEY_F13, "F13"}, {GLFW_KEY_F14, "F14"}, {GLFW_KEY_F15, "F15"}, {GLFW_KEY_F16, "F16"},
            {GLFW_KEY_F17, "F17"}, {GLFW_KEY_F18, "F18"}, {GLFW_KEY_F19, "F19"}, {GLFW_KEY_F20, "F20"},
            {GLFW_KEY_F21, "F21"}, {GLFW_KEY_F22, "F22"}, {GLFW_KEY_F23, "F23"},
            {GLFW_KEY_F24, "F24"}, {GLFW_KEY_F25, "F25"},
            // Punctuation
            {GLFW_KEY_APOSTROPHE, "'"}, {GLFW_KEY_COMMA, ","}, {GLFW_KEY_PERIOD, "."},
            {GLFW_KEY_SLASH, "/"}, {GLFW_KEY_SEMICOLON, ";"}, {GLFW_KEY_BACKSLASH, "\\"},
            {GLFW_KEY_MINUS, "-"}, {GLFW_KEY_EQUAL, "="}, {GLFW_KEY_LEFT_BRACKET, "["},
            {GLFW_KEY_RIGHT_BRACKET, "]"}, {GLFW_KEY_GRAVE_ACCENT, "`"}, {GLFW_KEY_SPACE, " "},
            // Modifiers
            {GLFW_KEY_ESCAPE, "escape"}, {GLFW_KEY_TAB, "tab"}, {GLFW_KEY_CAPS_LOCK, "capslock"},
            {GLFW_KEY_LEFT_SHIFT, "left_shift"}, {GLFW_KEY_RIGHT_SHIFT, "right_shift"},
            {GLFW_KEY_ENTER, "enter"}, {GLFW_KEY_BACKSPACE, "backspace"},
            {GLFW_KEY_LEFT_CONTROL, "left_control"}, {GLFW_KEY_RIGHT_CONTROL, "right_control"},
            {GLFW_KEY_LEFT_ALT, "left_alt"}, {GLFW_KEY_RIGHT_ALT, "right_alt"},
            {GLFW_KEY_LEFT_SUPER, "left_super"}, {GLFW_KEY_RIGHT_SUPER, "right_super"}, // windows/command key(s)
            // Navigation
            {GLFW_KEY_UP, "up"}, {GLFW_KEY_DOWN, "down"}, {GLFW_KEY_LEFT, "left"}, {GLFW_KEY_RIGHT, "right"},
            {GLFW_KEY_DELETE, "delete"}, {GLFW_KEY_INSERT, "insert"}, {GLFW_KEY_HOME, "home"},
            {GLFW_KEY_PAGE_UP, "page_up"}, {GLFW_KEY_PAGE_DOWN, "page_down"}, {GLFW_KEY_END, "end"},
            {GLFW_KEY_PRINT_SCREEN, "print_screen"}, {GLFW_KEY_SCROLL_LOCK, "scroll_lock"}, {GLFW_KEY_PAUSE, "pause"},
            // Number Pad (Key Pad)
            {GLFW_KEY_KP_SUBTRACT, "np_subtract"}, {GLFW_KEY_KP_DIVIDE, "np_divide"}, {GLFW_KEY_KP_ADD, "np_add"},
            {GLFW_KEY_KP_MULTIPLY, "np_multiply"}, {GLFW_KEY_KP_DECIMAL, "np_decimal"}, {GLFW_KEY_KP_ENTER, "np_enter"},
            {GLFW_KEY_KP_EQUAL, "np_equal"}, {GLFW_KEY_KP_0, "np_0"}, {GLFW_KEY_KP_1, "np_1"}, {GLFW_KEY_KP_2, "np_2"},
            {GLFW_KEY_KP_3, "np_3"}, {GLFW_KEY_KP_4, "np_4"}, {GLFW_KEY_KP_5, "np_5"}, {GLFW_KEY_KP_6, "np_6"},
            {GLFW_KEY_KP_7, "np_7"}, {GLFW_KEY_KP_8, "np_8"}, {GLFW_KEY_KP_9, "np_9"}
    };
public:
    InputManager();
    ~InputManager();
    void new_accept_key(const char *key, int action, void *caller,
                    void (*pFunction)(void *caller, ShowBase *base, int action));
    void new_accept_key(const char *key, void *caller,
                    void (*pFunction)(void *caller, ShowBase *base, int action));
    void new_accept_cursor(void *caller, void(*pFunction)(void *caller, ShowBase *base, double x, double y));
    void remove_accept_key(const char *key, int action);
    void remove_accept_key(const char *key);
    static void _static_init_glfw_input(Vulkan *m_vulkan);
    void _non_static_init_glfw_input(Window *m_window);
    void _non_static_key_callback(int key, int scancode, int action, int mods);
    void _non_static_cursor_callback(double x_pos, double y_pos);
};

#endif //VULKRAY_API_INPUTMANAGER_H
