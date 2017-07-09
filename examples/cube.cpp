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

struct Vertex
{
	float x, y, z;
	float u, v;
};

#define COUNT_OF(arr)	(sizeof(arr) / sizeof(*arr))

int main()
{
	platform::InitPlatform();

	platform::PLATFORM_WINDOW_REF window =
		platform::CreatePlatformWindow(800, 800, "Cube");
	if(!window)
	{
		platform::TerminatePlatform();
		return -1;
	}

	render::RenderDevice *renderDevice = render::CreateRenderDevice();

	render::VertexShader *vertexShader = renderDevice->CreateVertexShader(vertexShaderSource);

	render::PixelShader *pixelShader = renderDevice->CreatePixelShader(pixelShaderSource);

	render::Pipeline *pipeline = renderDevice->CreatePipeline(vertexShader, pixelShader);

	// For Sampler2D objects, we bind integers representing the texture
	// slot number to use
	render::PipelineParam *param = pipeline->GetParam("uTextureSampler");
	if(param)
		param->SetAsInt(0);

	// Get shader parameter for model matrix; we will set it every frame
	render::PipelineParam *uModelParam =
		pipeline->GetParam("uModel");

	// Get shader parameter for view matrix; we will set it every frame
	render::PipelineParam *uViewParam =
		pipeline->GetParam("uView");

	// Get shader parameter for projection matrix; we will set it every frame
	render::PipelineParam *uProjectionParam =
		pipeline->GetParam("uProjection");

	renderDevice->DestroyVertexShader(vertexShader);
	renderDevice->DestroyPixelShader(pixelShader);
	
	// Our vertices now have 2D texture coordinates
	Vertex vertices[] = {
		// front
		{-0.5f, -0.5f,  0.5f, 0, 1},
		{ 0.5f, -0.5f,  0.5f, 1, 1},
		{ 0.5f,  0.5f,  0.5f, 1, 0},
		{-0.5f,  0.5f,  0.5f, 0, 0},

		// right
		{ 0.5f, -0.5f,  0.5f, 0, 1},
		{ 0.5f, -0.5f, -0.5f, 1, 1},
		{ 0.5f,  0.5f, -0.5f, 1, 0},
		{ 0.5f,  0.5f,  0.5f, 0, 0},

		// top
		{-0.5f,  0.5f,  0.5f, 0, 1},
		{ 0.5f,  0.5f,  0.5f, 1, 1},
		{ 0.5f,  0.5f, -0.5f, 1, 0},
		{-0.5f,  0.5f, -0.5f, 0, 0},

		// back
		{ 0.5f, -0.5f, -0.5f, 0, 1},
		{-0.5f, -0.5f, -0.5f, 1, 1},
		{-0.5f,  0.5f, -0.5f, 1, 0},
		{ 0.5f,  0.5f, -0.5f, 0, 0},

		// left
		{-0.5f, -0.5f, -0.5f, 0, 1},
		{-0.5f, -0.5f,  0.5f, 1, 1},
		{-0.5f,  0.5f,  0.5f, 1, 0},
		{-0.5f,  0.5f, -0.5f, 0, 0},

		// bottom
		{-0.5f, -0.5f,  0.5f, 0, 1},
		{-0.5f, -0.5f, -0.5f, 1, 1},
		{ 0.5f, -0.5f, -0.5f, 1, 0},
		{ 0.5f, -0.5f,  0.5f, 0, 0}
	};

	render::VertexBuffer *vertexBuffer = renderDevice->CreateVertexBuffer(sizeof(vertices), vertices);

	render::VertexElement vertexElements[] = {
		{ 0, render::VERTEXELEMENTTYPE_FLOAT, 3, sizeof(Vertex), 0 },
		{ 1, render::VERTEXELEMENTTYPE_FLOAT, 2, sizeof(Vertex), 12 }
	};
	render::VertexDescription *vertexDescription = renderDevice->CreateVertexDescription(COUNT_OF(vertexElements), vertexElements);

	render::VertexArray *vertexArray = renderDevice->CreateVertexArray(1, &vertexBuffer, &vertexDescription);

	// Setup indices and create index buffer
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

	render::IndexBuffer *indexBuffer = renderDevice->CreateIndexBuffer(sizeof(indices), indices);

	// create texture
	render::Texture2D *texture2D = renderDevice->CreateTexture2D(BMPWIDTH, BMPHEIGHT, image32);

	while(platform::PollPlatformWindow(window))
	{
		glm::mat4 model(glm::uninitialize), view(glm::uninitialize), projection(glm::uninitialize);
		platform::GetPlatformViewport(model, view, projection);

		uModelParam->SetAsMat4(glm::value_ptr(model));
		uViewParam->SetAsMat4(glm::value_ptr(view));
		uProjectionParam->SetAsMat4(glm::value_ptr(projection));

		renderDevice->Clear(0.2f, 0.3f, 0.3f);

		// Set the texture for slot 0
		renderDevice->SetTexture2D(0, texture2D);

		renderDevice->SetPipeline(pipeline);
		renderDevice->SetVertexArray(vertexArray);

		// Set the index buffer
		renderDevice->SetIndexBuffer(indexBuffer);

		// Draw assuming index buffer consists of 32-bit, unsigned integers
		renderDevice->DrawTrianglesIndexed32(0, COUNT_OF(indices));

		platform::PresentPlatformWindow(window);
	}

	renderDevice->DestroyTexture2D(texture2D);
	renderDevice->DestroyIndexBuffer(indexBuffer);
	renderDevice->DestroyVertexArray(vertexArray);
	renderDevice->DestroyVertexDescription(vertexDescription);
	renderDevice->DestroyVertexBuffer(vertexBuffer);
	renderDevice->DestroyPipeline(pipeline);

	platform::TerminatePlatform();

	return 0;
}
