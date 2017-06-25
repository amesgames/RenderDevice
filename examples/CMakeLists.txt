link_libraries(RenderDeviceLib glfw)

include_directories(${glfw_INCLUDE_DIRS} "${GLFW_SOURCE_DIR}/deps")

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

set(GLAD "${GLFW_SOURCE_DIR}/deps/glad/glad.h"
         "${GLFW_SOURCE_DIR}/deps/glad.c")

add_executable(triangle WIN32 MACOSX_BUNDLE triangle.cpp ${GLAD})

set(WINDOWS_BINARIES triangle)

set_target_properties(${WINDOWS_BINARIES} PROPERTIES
                      FOLDER "RenderDevice-Examples")

if(MSVC)
    # Tell MSVC to use main instead of WinMain for Windows subsystem executables
    set_target_properties(${WINDOWS_BINARIES} PROPERTIES
                          LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()

if (APPLE)
    set_target_properties(triangle PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Triangle")

    #set_target_properties(${WINDOWS_BINARIES} PROPERTIES
    #                      RESOURCE glfw.icns
    #                      MACOSX_BUNDLE_SHORT_VERSION_STRING ${GLFW_VERSION}
    #                      MACOSX_BUNDLE_LONG_VERSION_STRING ${GLFW_VERSION_FULL}
    #                      MACOSX_BUNDLE_ICON_FILE glfw.icns
    #                      MACOSX_BUNDLE_INFO_PLIST "${GLFW_SOURCE_DIR}/CMake/MacOSXBundleInfo.plist.in")
endif()
