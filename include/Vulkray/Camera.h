/*
 * Camera.h
 * Defines the camera class which manipulates the view matrix.
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2022, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#ifndef VULKRAY_API_CAMERA_H
#define VULKRAY_API_CAMERA_H

#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>

class Camera {
private:
    glm::mat4x4 view_matrix;
    glm::vec3 look_at_vector = {1.0f, 0.0f, 0.0f}; // looking at +X by default
    float fov_radians;
    /*
     * Projection matrix is created in the Vulkan module
     * and not here, as it uses the swap chain width / height values.
     * (Only the `fov_radians` variable is used in the proj. matrix)
     */
public:
    float fov = 45.0f; // field of view (degrees)
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float h = 0.0f; // heading (degrees)
    float p = 0.0f; // pitch (degrees)
    float r = 0.0f; // roll (degrees)
    void set_fov(float fov);
    float get_fov_radians();
    void set_x(float x);
    void set_y(float y);
    void set_z(float z);
    void set_xyz(float x, float y, float z);
    void set_h(float h);
    void set_p(float p);
    void set_r(float r);
    void set_hpr(float h, float p, float r);
    glm::mat4x4 get_view_matrix();
    void create_view_matrix();
    void calculate_look_vector();
    Camera();
    ~Camera();
};

#endif //VULKRAY_API_CAMERA_H
