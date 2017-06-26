#include "render_device/platform.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

namespace platform
{

void InitPlatform()
{
	// glfw: initialize and configure
    // ------------------------------
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // required on OS X
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

PLATFORM_WINDOW_REF CreatePlatformWindow(int width, int height, const char *title)
{
	// glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(width, height, title, NULL, NULL);
    if(!window)
    {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return 0;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
    // ---------------------------------------
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return 0;
    }

	return (PLATFORM_WINDOW_REF)window;
}

bool PollPlatformWindow(PLATFORM_WINDOW_REF window)
{
	// glfw: poll IO events (keys pressed/released, mouse moved etc.)
	glfwPollEvents();

	if(glfwGetKey((GLFWwindow *)window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose((GLFWwindow *)window, true);

	// glfw: return whether app should continue polling
	return !glfwWindowShouldClose((GLFWwindow *)window);
}

void PresentPlatformWindow(PLATFORM_WINDOW_REF window)
{
	// glfw: swap buffers
    // -------------------------------------------------------------------------------
    glfwSwapBuffers((GLFWwindow *)window);
}

void TerminatePlatform()
{
	// glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
}

} // end namespace platform
