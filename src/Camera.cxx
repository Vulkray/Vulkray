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
    this->view_matrix = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
                                    glm::vec3(0.0f, 0.0f, 0.0f),
                                    glm::vec3(0.0f, 0.0f, 1.0f));
    this->fov_radians = glm::radians(this->fov);
}

Camera::~Camera() {
    // placeholder
}

void Camera::set_fov(double fov) {
    this->fov = fov;
}

void Camera::set_x(double x) {
    this->x = x;
}

void Camera::set_y(double y) {
    this->y = y;
}

void Camera::set_z(double z) {
    this->z = z;
}

void Camera::set_xyz(double x, double y, double z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

void Camera::set_h(double h) {
    this->h = h;
}

void Camera::set_p(double p) {
    this->p = p;
}

void Camera::set_r(double r) {
    this->r = r;
}

void Camera::set_hpr(double h, double p, double r) {
    this->h = h;
    this->p = p;
    this->r = r;
}