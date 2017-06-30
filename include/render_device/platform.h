#pragma once

#include "render_device/render_device.h"

#include <glm/glm.hpp>

namespace platform
{

typedef void *PLATFORM_WINDOW_REF;

void InitPlatform();

PLATFORM_WINDOW_REF CreatePlatformWindow(int width, int height, const char *title);

bool PollPlatformWindow(PLATFORM_WINDOW_REF window);

void GetPlatformViewport(glm::mat4 &modelview, glm::mat4 &projection);

void PresentPlatformWindow(PLATFORM_WINDOW_REF window);

void TerminatePlatform();

} // end namespace platform
