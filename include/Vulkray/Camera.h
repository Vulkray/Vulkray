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
    double fov_radians;
public:
    double fov = 45.0f; // field of view (degrees)
    double x = 0.0f;
    double y = 0.0f;
    double z = 0.0f;
    double h = 0.0f; // heading (degrees)
    double p = 0.0f; // pitch (degrees)
    double r = 0.0f; // roll (degrees)
    void set_fov(double fov);
    void set_x(double x);
    void set_y(double y);
    void set_z(double z);
    void set_xyz(double x, double y, double z);
    void set_h(double h);
    void set_p(double p);
    void set_r(double r);
    void set_hpr(double h, double p, double r);
    Camera();
    ~Camera();
};

#endif //VULKRAY_API_CAMERA_H
