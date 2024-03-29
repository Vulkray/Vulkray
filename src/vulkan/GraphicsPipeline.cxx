/*
 * GraphicsPipeline.cxx
 * Configures and creates the Vulkan pipeline instance using shaders.
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2023, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#include "../../include/Vulkray/Vulkan.h"

#include <spdlog/spdlog.h>
#include <fstream>

GraphicsPipeline::GraphicsPipeline(Vulkan *m_vulkan): VkModuleBase(m_vulkan) {

    // read spir-v shader binary files
    // TODO: Get shader code path from engine's input, instead of hardcoded shader file names.
    auto vertShaderCode = GraphicsPipeline::readSpirVShaderBinary("shaders/engine_basic.vert.spv");
    auto fragShaderCode = GraphicsPipeline::readSpirVShaderBinary("shaders/engine_basic.frag.spv");
    // create shader module instances
    VkShaderModule vertShaderModule = GraphicsPipeline::createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = GraphicsPipeline::createShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};

    // configure the pipeline shader stages
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    // Start configuring pipeline stages
    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
    VkPipelineDynamicStateCreateInfo dynamicState{};
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    VkPipelineViewportStateCreateInfo viewportState{};
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    VkPipelineMultisampleStateCreateInfo multisampling{};
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    VkPipelineDepthStencilStateCreateInfo depthStencil{};

    std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
    };
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    // Get vertex shader binding descriptions from Vertex struct
    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // viewport region should cover the entire framebuffer
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) this->m_vulkan->m_swapChain->swapChainExtent.width;
    viewport.height = (float) this->m_vulkan->m_swapChain->swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    // scissor should cover the entire buffer
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = this->m_vulkan->m_swapChain->swapChainExtent;

    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL; // other modes will require more GPU features
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // counterclockwise to fix GLM y-coords flip
    // other optional rasterizer configuration
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

    // currently multisampling is disabled; temporary!
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_TRUE; // TODO: Add engine API to enable/disable texture MSAA.
    multisampling.rasterizationSamples = this->m_vulkan->m_physicalDevice->msaaSamples;
    multisampling.minSampleShading = .2f; // minimum fraction for sample shading. closer to 1 is smoother.
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;

    // basic color blending configuration
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    // pipeline color blending stage configuration
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    // optional blendConstants values
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    // Enable depth buffer image testing
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS; // Fragments that are closer are rendered on top
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional


    // Create the Pipeline Layout vulkan instance
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &this->m_vulkan->m_descriptorPool->descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    VkResult result = vkCreatePipelineLayout(this->m_vulkan->m_logicalDevice->logicalDevice,
                                             &pipelineLayoutInfo, nullptr, &this->pipelineLayout);
    if (result != VK_SUCCESS) {
        spdlog::error("An error occurred when initializing the graphics pipeline layout instance.");
        throw std::runtime_error("Failed to create the graphics pipeline layout!");
    }

    // Create the Vulkan graphics pipeline instance
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = this->pipelineLayout;
    pipelineInfo.renderPass = this->m_vulkan->m_renderPass->renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // optional
    pipelineInfo.basePipelineIndex = -1; // optional

    result = vkCreateGraphicsPipelines(this->m_vulkan->m_logicalDevice->logicalDevice,
                                       VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &this->graphicsPipeline);
    if (result != VK_SUCCESS) {
        spdlog::error("An error occurred when initializing the Vulkan graphics pipeline instance.");
        throw std::runtime_error("Failed to create the Vulkan graphics pipeline.");
    }

    // modules compiled after pipeline creation, so they can be destroyed
    vkDestroyShaderModule(this->m_vulkan->m_logicalDevice->logicalDevice, fragShaderModule, nullptr);
    vkDestroyShaderModule(this->m_vulkan->m_logicalDevice->logicalDevice, vertShaderModule, nullptr);
}

VkShaderModule GraphicsPipeline::createShaderModule(const std::vector<char> &shaderBinary) {

    VkShaderModule shaderModule;
    VkShaderModuleCreateInfo createInfo{};

    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = shaderBinary.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderBinary.data());

    VkResult result = vkCreateShaderModule(this->m_vulkan->m_logicalDevice->logicalDevice,
                                           &createInfo, nullptr, &shaderModule);
    if (result != VK_SUCCESS) {
        spdlog::error("An issue was encountered when initializing a shader module instance.");
        throw std::runtime_error("Failed to create a shader module!");
    }
    return shaderModule;
}

std::vector<char> GraphicsPipeline::readSpirVShaderBinary(const std::string &filename) {

    // open spir-v shader file (cursor at end of file; read binary)
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) { // file access error
        spdlog::error("The engine could not read a Spir-V shader binary file! Exiting.");
        throw std::runtime_error("Failed to open Spir-V shader binary file!");
    }
    // use cursor position to get file size
    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer; // raw spir-v binary data
}

GraphicsPipeline::~GraphicsPipeline() {
    vkDestroyPipeline(this->m_vulkan->m_logicalDevice->logicalDevice, this->graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(this->m_vulkan->m_logicalDevice->logicalDevice, this->pipelineLayout, nullptr);
}