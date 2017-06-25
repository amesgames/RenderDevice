include_directories("${GLFW_SOURCE_DIR}/deps")

add_library(RenderDeviceLib STATIC ../include/render_device/render_device.hpp render_device.cpp)

target_link_libraries(RenderDeviceLib glfw glm)

target_include_directories(RenderDeviceLib PUBLIC ../include)
