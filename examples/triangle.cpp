#include "render_device/platform.h"

#include "render_device/render_device.h"

#include <iostream>

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char *vertexShaderSource = "#version 410 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *pixelShaderSource = "#version 410 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

int main()
{
    platform::InitPlatform();

    // window creation
    // --------------------
    platform::PLATFORM_WINDOW_REF window = platform::CreatePlatformWindow(SCR_WIDTH, SCR_HEIGHT, "Triangle");
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

    // fragment shader
    render::PixelShader *pixelShader = renderDevice->CreatePixelShader(pixelShaderSource);

    // link shaders
    render::Pipeline *pipeline = renderDevice->CreatePipeline(vertexShader, pixelShader);

    renderDevice->DestroyVertexShader(vertexShader);
	renderDevice->DestroyPixelShader(pixelShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f,  0.5f, 0.0f  // top   
    }; 

	render::VertexBuffer *vertexBuffer = renderDevice->CreateVertexBuffer(sizeof(vertices), vertices);

	render::VertexElement vertexElement = { 0, render::VERTEXELEMENTTYPE_FLOAT, 3, 0, 0, };
	render::VertexDescription *vertexDescription = renderDevice->CreateVertexDescription(1, &vertexElement);

	render::VertexArray *vertexArray = renderDevice->CreateVertexArray(1, &vertexBuffer, &vertexDescription);

    // render loop
    // -----------
    while(platform::PollPlatformWindow(window))
    {
        // render
        // ------
        renderDevice->ClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        // draw our first triangle
		renderDevice->SetPipeline(pipeline);
		renderDevice->SetVertexArray(vertexArray);
		renderDevice->DrawTriangles(0, 3);

        platform::PresentPlatformWindow(window);
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    renderDevice->DestroyVertexArray(vertexArray);
    renderDevice->DestroyVertexBuffer(vertexBuffer);

    platform::TerminatePlatform();

    return 0;
}
