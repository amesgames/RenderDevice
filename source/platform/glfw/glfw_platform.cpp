#include "render_device/platform.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <iostream>

namespace platform
{

static float s_Width;

static float s_Height;

static glm::mat4 s_Projection;

static glm::mat4 s_Model(1);

static glm::mat3 s_LastRot(1);

static glm::mat3 s_ThisRot(1);

static glm::vec3 s_BegVec(0);
static glm::vec3 s_EndVec(0);
static float s_AdjustWidth;
static float s_AdjustHeight;

static glm::vec2 s_MousePt;
static bool s_isDragging = false;

static void SetBounds(float width, float height)
{
	// Set adjustment factor for width/height
    s_AdjustWidth  = 1.0f / ((width  - 1.0f) * 0.5f);
    s_AdjustHeight = 1.0f / ((height - 1.0f) * 0.5f);
}

static void MapToSphere(const glm::vec2 &pt, glm::vec3 &vec)
{
    glm::vec2 tempPt;
    float length;

    // Copy paramter into temp point
    tempPt = pt;

    // Adjust point coords and scale down to range of [-1 ... 1]
    tempPt.x =        (tempPt.x * s_AdjustWidth) - 1.0f;
    tempPt.y = 1.0f - (tempPt.y * s_AdjustHeight);

    // Compute the square of the length of the vector to the point from the center
    length = (tempPt.x * tempPt.x) + (tempPt.y * tempPt.y);

    // If the point is mapped outside of the sphere... (length > radius squared)
    if(length > 1.0f)
    {
        // Compute a normalizing factor (radius / sqrt(length))
        float norm = 1.0f / std::sqrt(length);

        // Return the "normalized" vector, a point on the sphere
        vec.x = tempPt.x * norm;
        vec.y = tempPt.y * norm;
        vec.z = 0.0f;
    }
    else // Else it's on the inside
    {
        // Return a vector to a point mapped inside the sphere sqrt(radius squared - length)
        vec.x = tempPt.x;
        vec.y = tempPt.y;
        vec.z = std::sqrt(1.0f - length);
    }
}

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

static void set_viewport_size(int width, int height)
{
	s_Width = static_cast<float>(width);
	s_Height = static_cast<float>(height);

	SetBounds(s_Width, s_Height);

	s_Projection = glm::perspective(glm::radians(45.0f), static_cast<float>(s_Width) / static_cast<float>(s_Height), 0.1f, 100.f);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	set_viewport_size(width, height);

    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
	if(action == GLFW_PRESS)
	{
		if(button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			s_Model = glm::mat4(1);

			s_LastRot = glm::mat3(1);

			s_ThisRot = glm::mat3(1);
		}
		else if(button == GLFW_MOUSE_BUTTON_LEFT)
		{
			s_isDragging = true;
			s_LastRot = s_ThisRot;
			MapToSphere(s_MousePt, s_BegVec);
		}
	}
	else if(action == GLFW_RELEASE)
	{
		if(button == GLFW_MOUSE_BUTTON_LEFT)
			s_isDragging = false;
	}
}

static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
	s_MousePt.x = static_cast<float>(xpos);
	s_MousePt.y = static_cast<float>(ypos);

	if(s_isDragging)
	{
		// Map the point to the sphere
		MapToSphere(s_MousePt, s_EndVec);

		// Compute the vector perpendicular to the begin and end vectors
		glm::vec3 perp = glm::cross(s_BegVec, s_EndVec);

		glm::quat quat;
		if(glm::length(perp) > 1.0e-5) // if the length of the perpendicular vector non-zero
		{
			quat.x = perp.x;
			quat.y = perp.y;
			quat.z = perp.z;

			// In the quaternion values, w is cosine (theta / 2), where theta is rotation angle
			quat.w = glm::dot(s_BegVec, s_EndVec);
		}
		else // if its zero
		{
			// The begin and end vectors coincide, so use an identity transform
			quat.x = quat.y = quat.z = quat.w = 0.0f;
		}

		s_ThisRot = glm::mat3_cast(quat) * s_LastRot;

		/*float scale = std::sqrt(
			(glm::length2(glm::row(s_Model, 0)) + 
             glm::length2(glm::row(s_Model, 1)) +
			 glm::length2(glm::row(s_Model, 2))) / 3.0f);
		glm::vec3 translate = glm::row(s_Model, 3);*/

		// only using model rotation anyway.
		// Commented out code above and below would allow us to preserve any uniform scale and translation of the object.
		// #TODO
		s_Model = s_ThisRot;

        //s_Model[0][0] *= scale; s_Model[1][0] *= scale; s_Model[2][0] *= scale;
        //s_Model[0][1] *= scale; s_Model[1][1] *= scale; s_Model[2][1] *= scale;
        //s_Model[0][2] *= scale; s_Model[1][2] *= scale; s_Model[2][2] *= scale;
		//s_Model[0][3] = translate[0]; s_Model[1][3] = translate[1]; s_Model[2][3] = translate[2];
	}
}

PLATFORM_WINDOW_REF CreatePlatformWindow(int width, int height, const char *title)
{
	set_viewport_size(width, height);

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
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

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

void GetPlatformViewport(glm::mat4 &model, glm::mat4 &projection)
{
	model = s_Model;
	projection = s_Projection;
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
