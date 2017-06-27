include_directories("${GLFW_SOURCE_DIR}/deps")

add_library(RenderDeviceLib STATIC ../include/render_device/platform.h ../include/render_device/render_device.h platform/glfw/glfw_platform.cpp render_device.cpp opengl/ogl_render_device.h opengl/ogl_render_device.cpp)

target_link_libraries(RenderDeviceLib glfw glm)

target_include_directories(RenderDeviceLib PUBLIC ../include)
