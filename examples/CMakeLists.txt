link_libraries(RenderDeviceLib glfw)

include_directories(${glfw_INCLUDE_DIRS} "${GLFW_SOURCE_DIR}/deps")

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

if (WIN32)
    set(ICON example.rc)
elseif (APPLE)
    set(ICON example.icns)
    set_source_files_properties(example.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

set(GLAD "${GLFW_SOURCE_DIR}/deps/glad/glad.h"
         "${GLFW_SOURCE_DIR}/deps/glad.c")

add_executable(triangle WIN32 MACOSX_BUNDLE triangle.cpp ${ICON} ${GLAD})
add_executable(cube WIN32 MACOSX_BUNDLE cube.cpp image888.c ${ICON} ${GLAD})

set(WINDOWS_BINARIES triangle cube)

set_target_properties(${WINDOWS_BINARIES} PROPERTIES
                      FOLDER "RenderDevice-Examples")

if(MSVC)
    # Tell MSVC to use main instead of WinMain for Windows subsystem executables
    set_target_properties(${WINDOWS_BINARIES} PROPERTIES
                          LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()

if (APPLE)
    set_target_properties(triangle PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Triangle")
		set_target_properties(cube PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Cube")

    set_target_properties(${WINDOWS_BINARIES} PROPERTIES
                          RESOURCE example.icns
                          MACOSX_BUNDLE_SHORT_VERSION_STRING 1.0
                          MACOSX_BUNDLE_LONG_VERSION_STRING 1.0.0.0
                          MACOSX_BUNDLE_ICON_FILE example.icns
                          MACOSX_BUNDLE_INFO_PLIST "${RenderDeviceLib_SOURCE_DIR}/CMake/MacOSXBundleInfo.plist.in")
endif()
