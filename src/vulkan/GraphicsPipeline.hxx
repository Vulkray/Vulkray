/*
 * GraphicsPipeline.cxx
 * Configures and creates the Vulkan pipeline instance using shaders.
 *
 * Copyright 2022 Max Rodriguez

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 */

#ifndef VULKRAY_GRAPHICSPIPELINE_HXX
#define VULKRAY_GRAPHICSPIPELINE_HXX

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

class GraphicsPipeline {
public:
    static void createGraphicsPipeline(VkPipeline *graphicsPipeline, VkPipelineLayout *pipelineLayout,
                                       VkRenderPass renderPass, VkDevice logicalDevice, VkExtent2D swapExtent);
private:
    static VkShaderModule createShaderModule(const std::vector<char> &shaderBinary, VkDevice logicalDevice);
    static std::vector<char> readSpirVShaderBinary(const std::string &filename);
};

#endif //VULKRAY_GRAPHICSPIPELINE_HXX
