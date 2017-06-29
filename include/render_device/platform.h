#pragma once

#include "render_device/render_device.h"

#include <glm/glm.hpp>

namespace platform
{

typedef void *PLATFORM_WINDOW_REF;

void InitPlatform();

PLATFORM_WINDOW_REF CreatePlatformWindow(int width, int height, const char *title);

bool PollPlatformWindow(PLATFORM_WINDOW_REF window);

void GetPlatformWindowTrackball(glm::mat4 &trackball);

void PresentPlatformWindow(PLATFORM_WINDOW_REF window);

void TerminatePlatform();

} // end namespace platform
