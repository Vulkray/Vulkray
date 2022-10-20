/*
 * Camera.cxx
 * Defines the camera class which manipulates the view matrix.
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2022, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#include "../include/Vulkray/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera() {
    // Create default view matrix
    this->create_view_matrix();
    // Store default FOV value in radians
    this->fov_radians = glm::radians(this->fov);
}

Camera::~Camera() {
    // placeholder
}

void Camera::create_view_matrix() {
    glm::vec3 eye = glm::vec3(this->x, this->y, this->z); // 'eye' coordinate (cam pos)
    glm::vec3 center = glm::vec3(this->x + this->look_at_vector.x,
                                 this->y + this->look_at_vector.y, // 'center' coordinate (cam look at)
                                 this->z + this->look_at_vector.z);
    glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f); // 'up' vector (Z axis is up in this engine)

    this->view_matrix = glm::lookAt(eye, center, up);
}

glm::mat4x4 Camera::get_view_matrix() {
    // used by the Vulkan module for the uniform buffer
    return this->view_matrix;
}

void Camera::calculate_look_vector() {
    /*
     * Using formulas 'sin(rads) * r = y' & 'cos(rads) * r = x`
     * (radius doesn't matter, so it's not used)
     *
     * Note: y value of heading vector is inverted for clockwise degrees. (45 degrees turns 45 degrees right)
     *       Pretty sure (if I'm correct) this is because OpenGL / Vulkan has flipped y coords.
     */
    this->look_at_vector = glm::vec3(0.0f, 0.0f, 0.0f);
    // calculate heading
    this->look_at_vector.x += glm::cos(glm::radians(this->h)); // x
    this->look_at_vector.y += glm::sin(glm::radians(this->h)) * -1; // y
    // calculate pitch
    this->look_at_vector.x += glm::cos(glm::radians(this->p)); // x
    this->look_at_vector.z += glm::sin(glm::radians(this->p)); // z
}

void Camera::set_near(float near) {
    this->near = near;
}

void Camera::set_far(float far) {
    this->far = far;
}

void Camera::set_fov(float fov) {
    this->fov = fov;
    this->fov_radians = glm::radians(fov);
}

float Camera::get_fov_radians() {
    return this->fov_radians;
}

void Camera::set_x(float x) {
    this->x = x;
    this->create_view_matrix();
}

void Camera::set_y(float y) {
    this->y = y;
    this->create_view_matrix();
}

void Camera::set_z(float z) {
    this->z = z;
    this->create_view_matrix();
}

void Camera::set_xyz(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->create_view_matrix();
}

void Camera::set_h(float h) {
    this->h = h;
    this->calculate_look_vector();
    this->create_view_matrix();
}

void Camera::set_p(float p) {
    this->p = p;
    this->calculate_look_vector();
    this->create_view_matrix();
}

void Camera::set_r(float r) {
    this->r = r;
    this->calculate_look_vector();
    this->create_view_matrix();
}

void Camera::set_hpr(float h, float p, float r) {
    this->h = h;
    this->p = p;
    this->r = r;
    this->calculate_look_vector();
    this->create_view_matrix();
}