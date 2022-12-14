/*
 * ObjectNode.cxx
 * API Header - Defines the fundamental base class for all objects in 3D space.
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2022, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#ifndef VULKRAY_OBJECTNODE_H
#define VULKRAY_OBJECTNODE_H

class ObjectNode {
public:
    ObjectNode();
    ~ObjectNode();
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float h = 0.0f; // heading (degrees)
    float p = 0.0f; // pitch (degrees)
    float r = 0.0f; // roll (degrees)
    void set_x(float x);
    void set_y(float y);
    void set_z(float z);
    void set_xyz(float x, float y, float z);
    void set_h(float h);
    void set_p(float p);
    void set_r(float r);
    void set_hpr(float h, float p, float r);
};

#endif //VULKRAY_OBJECTNODE_H
