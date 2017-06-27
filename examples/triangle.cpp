#include "render_device/platform.h"

#include "render_device/render_device.h"

#include <glad/glad.h>

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

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 

    // render loop
    // -----------
    while(platform::PollPlatformWindow(window))
    {
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
		renderDevice->SetPipeline(pipeline);

        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glBindVertexArray(0); // no need to unbind it every time 
 
        platform::PresentPlatformWindow(window);
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    platform::TerminatePlatform();

    return 0;
}
