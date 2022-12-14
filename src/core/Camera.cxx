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

#include "../../include/Vulkray/Camera.h"
#include "../../include/Vulkray/ObjectNode.h"
#include "../../include/Vulkray/JobManager.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(ShowBase *base): ObjectNode() {
    this->update(); // initial camera update
    this->base = base;
    base->jobManager->new_job("_global_cam_update", this, &this->_per_frame_update);
}

Camera::~Camera() {
    // placeholder
}

void Camera::update() {
    this->create_view_matrix();
    this->fov_radians = glm::radians(this->fov);
    this->calculate_look_vector();
}

void Camera::_per_frame_update(void *caller, ShowBase *base) {
    Camera* self = (Camera*)caller;
    self->update(); // update camera
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

glm::vec3 Camera::get_look_at_vector() {
    this->calculate_look_vector(); // make sure look_at vector is updated!
    return this->look_at_vector;
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
