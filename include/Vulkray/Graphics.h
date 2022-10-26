/*
 * Graphics.h
 * API Header - Exposes some definitions of the Vulkan classes / instances.
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2022, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#ifndef VULKRAY_API_GRAPHICS_H
#define VULKRAY_API_GRAPHICS_H

#include <vulkan/vulkan.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <vector>
#include <array>

// module class prototypes
class Vulkan;

// module struct definitions
struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        // vertex binding data
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }
    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
        // vertex position data to shader
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);
        // vertex color data to shader
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }
};

struct GraphicsInput {
    std::vector<Vertex> vertexData = {
            {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, // invisible placeholder vertex (buffer cannot be empty)
    };
    std::vector<uint32_t> indexData = {
            0, 0, 0 // invisible placeholder indices (buffer cannot be initialized empty)
    };
    VkClearValue bufferClearColor = (VkClearValue){{{0.05f, 0.05f, 0.05f, 1.0f}}}; // default world background color
};

#endif //VULKRAY_API_GRAPHICS_H
