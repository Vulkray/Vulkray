/*
 * Geometry.cxx
 * I honestly don't know yet im just learning computer graphics
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2022, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#ifndef VULKRAY_GEOMETRY_HXX
#define VULKRAY_GEOMETRY_HXX

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>

class Geometry {
    struct Vertex {
        glm::vec2 pos;
        glm::vec3 color;
    };
    glm::mat4 matrix;
    glm::vec4 vec;
};

#endif //VULKRAY_GEOMETRY_HXX
