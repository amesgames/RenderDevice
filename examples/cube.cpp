#include <render_device/platform.h>

#include <render_device/render_device.h>

#include <cstdint>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "image888.h"

const char *vertexShaderSource = "#version 410 core\n"
	"uniform mat4 uModel;\n"
	"uniform mat4 uView;\n"
	"uniform mat4 uProjection;\n"
	"layout (location = 0) in vec3 aPos;\n"
	"layout (location = 1) in vec2 aTexCoord;\n"
	"out vec2 FragTexCoord;\n"
	"void main()\n"
	"{\n"
	"   gl_Position = uProjection * uView * uModel * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"   FragTexCoord = aTexCoord;\n"
	"}";
const char *pixelShaderSource = "#version 410 core\n"
	"uniform sampler2D uTextureSampler;\n"
	"in vec2 FragTexCoord;\n"
	"out vec4 FragColor;\n"
	"void main()\n"
	"{\n"
	"   FragColor = vec4(texture(uTextureSampler, FragTexCoord).rgb, 1);\n"
	"}\n";
const char *pixelShaderSource_YellowColor = "#version 410 core\n"
	"out vec4 FragColor;\n"
	"void main()\n"
	"{\n"
	"   FragColor = vec4(1, 0, 1, 1);\n"
	"}\n";

struct Vertex
{
	float x, y, z;
	float u, v;
};

#define COUNT_OF(arr)	(sizeof(arr) / sizeof(*arr))

int main()
{
	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 800;

	platform::InitPlatform();

	// window creation
	// --------------------
	platform::PLATFORM_WINDOW_REF window =
		platform::CreatePlatformWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Textured Cube");
	if(!window)
	{
		platform::TerminatePlatform();
		return -1;
	}

	render::RenderDevice *renderDevice = render::CreateRenderDevice();

	// build and compile our shader program
	// ------------------------------------
	// vertex shader
	render::VertexShader *vertexShader = renderDevice->CreateVertexShader(vertexShaderSource);

	// fragment shaders
	render::PixelShader *pixelShader = renderDevice->CreatePixelShader(pixelShaderSource);

	render::PixelShader *pixelShader_Yellow = renderDevice->CreatePixelShader(pixelShaderSource_YellowColor);

	// link pipeline
	render::Pipeline *pipeline = renderDevice->CreatePipeline(vertexShader, pixelShader);

	render::Pipeline *pipeline_Yellow = renderDevice->CreatePipeline(vertexShader, pixelShader_Yellow);

	// Set shader params
	render::PipelineParam *param = pipeline->GetParam("uTextureSampler");
	if(param)
		param->SetAsInt(0);

	render::PipelineParam *uModelParam = pipeline->GetParam("uModel");
	render::PipelineParam *uModelParam_Yellow = pipeline_Yellow->GetParam("uModel");

	param = pipeline->GetParam("uView");
	if(param)
	{
		glm::mat4 view = glm::translate(glm::mat4(1), glm::vec3(0, 0, -3));
		param->SetAsMat4(glm::value_ptr(view));
	}

	param = pipeline_Yellow->GetParam("uView");
	if(param)
	{
		glm::mat4 view = glm::translate(glm::mat4(1), glm::vec3(0, 0, -3));
		param->SetAsMat4(glm::value_ptr(view));
	}

	render::PipelineParam *uProjectionParam = pipeline->GetParam("uProjection");

	render::PipelineParam *uProjectionParam_Yellow = pipeline_Yellow->GetParam("uProjection");

	renderDevice->DestroyVertexShader(vertexShader);
	renderDevice->DestroyPixelShader(pixelShader);
	renderDevice->DestroyPixelShader(pixelShader_Yellow);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	Vertex vertices[] = {
		// front
		-0.5f, -0.5f,  0.5f, 0, 1,
		 0.5f, -0.5f,  0.5f, 1, 1,
		 0.5f,  0.5f,  0.5f, 1, 0,
		-0.5f,  0.5f,  0.5f, 0, 0,

		// right
		 0.5f, -0.5f,  0.5f, 0, 1,
		 0.5f, -0.5f, -0.5f, 1, 1,
		 0.5f,  0.5f, -0.5f, 1, 0,
		 0.5f,  0.5f,  0.5f, 0, 0,

		// top
		-0.5f,  0.5f,  0.5f, 0, 1,
		 0.5f,  0.5f,  0.5f, 1, 1,
		 0.5f,  0.5f, -0.5f, 1, 0,
		-0.5f,  0.5f, -0.5f, 0, 0,

		// back
		 0.5f, -0.5f, -0.5f, 0, 1,
		-0.5f, -0.5f, -0.5f, 1, 1,
		-0.5f,  0.5f, -0.5f, 1, 0,
		 0.5f,  0.5f, -0.5f, 0, 0,

		// left
		-0.5f, -0.5f, -0.5f, 0, 1,
		-0.5f, -0.5f,  0.5f, 1, 1,
		-0.5f,  0.5f,  0.5f, 1, 0,
		-0.5f,  0.5f, -0.5f, 0, 0,

		// bottom
		-0.5f, -0.5f,  0.5f, 0, 1,
		-0.5f, -0.5f, -0.5f, 1, 1,
		 0.5f, -0.5f, -0.5f, 1, 0,
		 0.5f, -0.5f,  0.5f, 0, 0
	};

	uint32_t indices[] = {
		// front
		0, 1, 2, 0, 2, 3,

		// right
		4, 5, 6, 4, 6, 7,

		// top
		8, 9, 10, 8, 10, 11,

		// back
		12, 13, 14, 12, 14, 15,

		// left
		16, 17, 18, 16, 18, 19,

		// bottom
		20, 21, 22, 20, 22, 23
	};

	render::VertexBuffer *vertexBuffer = renderDevice->CreateVertexBuffer(sizeof(vertices), vertices);

	render::VertexElement vertexElements[] = {
		{ 0, render::VERTEXELEMENTTYPE_FLOAT, 3, sizeof(Vertex), 0 },
		{ 1, render::VERTEXELEMENTTYPE_FLOAT, 2, sizeof(Vertex), 12 }
	};
	render::VertexDescription *vertexDescription = renderDevice->CreateVertexDescription(COUNT_OF(vertexElements), vertexElements);

	render::VertexArray *vertexArray = renderDevice->CreateVertexArray(1, &vertexBuffer, &vertexDescription);

	render::IndexBuffer *indexBuffer = renderDevice->CreateIndexBuffer(sizeof(indices), indices);

	render::Texture2D *texture2D = renderDevice->CreateTexture2D(BMPWIDTH, BMPHEIGHT, image32);

	// render loop
	// -----------
	while(platform::PollPlatformWindow(window))
	{
		if(uModelParam)
		{
			glm::mat4 model, projection;

			platform::GetPlatformViewport(model, projection);

			uModelParam->SetAsMat4(glm::value_ptr(model));
			uProjectionParam->SetAsMat4(glm::value_ptr(projection));
		}

		// render
		// ------
		renderDevice->Clear(0.2f, 0.3f, 0.3f, 1.0f, 1.0f);

		// draw a cube
		renderDevice->SetTexture2D(0, texture2D);
		renderDevice->SetPipeline(pipeline);
		renderDevice->SetVertexArray(vertexArray);
		renderDevice->SetIndexBuffer(indexBuffer);
		renderDevice->DrawTrianglesIndexed32(0, COUNT_OF(indices));

		platform::PresentPlatformWindow(window);
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	renderDevice->DestroyTexture2D(texture2D);
	renderDevice->DestroyIndexBuffer(indexBuffer);
	renderDevice->DestroyVertexArray(vertexArray);
	renderDevice->DestroyVertexDescription(vertexDescription);
	renderDevice->DestroyVertexBuffer(vertexBuffer);
	renderDevice->DestroyPipeline(pipeline);

	platform::TerminatePlatform();

	return 0;
}
