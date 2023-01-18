/*
 * Camera.h
 * Defines the camera class which manipulates the view matrix.
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2023, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#ifndef VULKRAY_API_CAMERA_H
#define VULKRAY_API_CAMERA_H

#include "ObjectNode.h"
#include "ShowBase.h"
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>

class Camera : public ObjectNode {
public:
    Camera(ShowBase *base);
    ~Camera();
    ShowBase *base;
    float near = 0.1f; // projection near plane
    float far = 5000.0f; // projection far plane
    float fov = 45.0f; // field of view (degrees)
    void update(); // updates camera by its properties
    void set_near(float near);
    void set_far(float far);
    void set_fov(float fov);
    float get_fov_radians();
    glm::mat4x4 get_view_matrix();
    void create_view_matrix();
    void calculate_look_vector();
    glm::vec3 get_look_at_vector();
private:
    glm::mat4x4 view_matrix;
    glm::vec3 look_at_vector = {1.0f, 0.0f, 0.0f}; // looking at +X by default
    float fov_radians;
    /*
     * Projection matrix is created in the Vulkan module
     * and not here, as it uses the swap chain width / height values.
     * (Only the `fov_radians` variable is used in the proj. matrix)
     */
    static void _per_frame_update(void *caller, ShowBase *base);
};

#endif //VULKRAY_API_CAMERA_H
