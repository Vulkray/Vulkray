# Compile the GLSL shader code to Spir-V byte code
# (using the 'glslc' compiler included in the Vulkan SDK)

# GLSL shader code sources
set(SHADERS
        triangle.vert
        triangle.frag
)

# Target Spir-V version
set(SPIRV_TARGET spv1.6) # (spv1.6 required by Vulkan 1.3 API)

# glsl compiler flags
set(GLSLC_FLAGS -Werror --target-spv=${SPIRV_TARGET})

foreach(SHADER ${SHADERS})
    get_filename_component(FILE_NAME ${SHADER} NAME)
    set(SPIRV ${FILE_NAME}.spv)
    add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND glslc ${GLSLC_FLAGS} ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER} -o ${SPIRV}
            DEPENDS ${SHADER}
    )
    list(APPEND SPIRV_FILES ${SPIRV})
endforeach()

source_group("Shader Files" FILES ${SHADERS})
add_custom_target(vulkray-shaders DEPENDS ${SPIRV_FILES} SOURCES ${SHADERS})