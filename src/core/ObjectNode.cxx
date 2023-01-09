/*
 * ObjectNode.cxx
 * Defines the fundamental base class for all objects in 3D space.
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2022, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#include "../../include/Vulkray/ObjectNode.h"

ObjectNode::ObjectNode() {
    // placeholder
}

ObjectNode::~ObjectNode() {
    // placeholder
}

void ObjectNode::set_x(float x) {
    this->x = x;
}

void ObjectNode::set_y(float y) {
    this->y = y;
}

void ObjectNode::set_z(float z) {
    this->z = z;
}

void ObjectNode::set_xyz(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

void ObjectNode::set_xyz(Vector3 xyz) {
    this->x = xyz.x;
    this->y = xyz.y;
    this->z = xyz.z;
}

Vector3 ObjectNode::get_xyz() {
    Vector3 vec3;
    vec3.x = this->x;
    vec3.y = this->y;
    vec3.z = this->z;
    return vec3;
}

void ObjectNode::set_h(float h) {
    this->h = h;
}

void ObjectNode::set_p(float p) {
    this->p = p;
}

void ObjectNode::set_r(float r) {
    this->r = r;
}

void ObjectNode::set_hpr(float h, float p, float r) {
    this->h = h;
    this->p = p;
    this->r = r;
}

void ObjectNode::set_hpr(Vector3 hpr) {
    this->h = hpr.x;
    this->p = hpr.y;
    this->r = hpr.z;
}

Vector3 ObjectNode::get_hpr() {
    Vector3 vec3;
    vec3.x = this->h;
    vec3.y = this->p;
    vec3.z = this->r;
    return vec3;
}