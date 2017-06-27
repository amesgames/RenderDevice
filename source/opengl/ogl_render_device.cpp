#include "ogl_render_device.h"

#include <glad/glad.h>

#include <iostream>

namespace render
{

class OpenGLVertexShader : public VertexShader
{
public:

	OpenGLVertexShader(const char *code)
	{
		// ------------------------------------
		// vertex shader
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &code, NULL);
		glCompileShader(vertexShader);

		// check for shader compile errors
		int success;
		char infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if(!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
	}

	~OpenGLVertexShader() override
	{
		glDeleteShader(vertexShader);
	}

	int vertexShader = 0;
};

class OpenGLPixelShader : public PixelShader
{
public:

	OpenGLPixelShader(const char *code)
	{
		// fragment shader
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &code, NULL);
		glCompileShader(fragmentShader);

		// check for shader compile errors
		int success;
		char infoLog[512];
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if(!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
	}

	~OpenGLPixelShader() override
	{
		glDeleteShader(fragmentShader);
	}

	int fragmentShader = 0;
};

class OpenGLPipeline : public Pipeline
{
public:

	OpenGLPipeline(OpenGLVertexShader *vertexShader, OpenGLPixelShader *pixelShader)
	{
		// link shaders
		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader->vertexShader);
		glAttachShader(shaderProgram, pixelShader->fragmentShader);
		glLinkProgram(shaderProgram);

		// check for linking errors
		int success;
		char infoLog[512];
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if(!success)
		{
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
	}

	~OpenGLPipeline() override
	{
		glDeleteProgram(shaderProgram);
	}

	int shaderProgram = 0;
};

class OpenGLVertexBuffer : public VertexBuffer
{
public:

	OpenGLVertexBuffer() {}
};

class OpenGLVertexDescription : public VertexDescription
{
};

class OpenGLVertexArray : public VertexArray
{
};

VertexShader *OpenGLRenderDevice::CreateVertexShader(const char *code)
{
	return new OpenGLVertexShader(code);
}

void OpenGLRenderDevice::DestroyVertexShader(VertexShader *vertexShader)
{
	delete vertexShader;
}

PixelShader *OpenGLRenderDevice::CreatePixelShader(const char *code)
{
	return new OpenGLPixelShader(code);
}

void OpenGLRenderDevice::DestroyPixelShader(PixelShader *pixelShader)
{
	delete pixelShader;
}

Pipeline *OpenGLRenderDevice::CreatePipeline(VertexShader *vertexShader, PixelShader *pixelShader)
{
	return new OpenGLPipeline(reinterpret_cast<OpenGLVertexShader *>(vertexShader), reinterpret_cast<OpenGLPixelShader *>(pixelShader));
}

void OpenGLRenderDevice::DestroyPipeline(Pipeline *pipeline)
{
	delete pipeline;
}

void OpenGLRenderDevice::SetPipeline(Pipeline *pipeline)
{
	glUseProgram(reinterpret_cast<OpenGLPipeline *>(pipeline)->shaderProgram);
}

VertexBuffer *OpenGLRenderDevice::CreateVertexBuffer(long long size, void *data)
{
	return nullptr;
}

void OpenGLRenderDevice::DestroyVertexBuffer(VertexBuffer *vertexBuffer)
{
}

VertexDescription *OpenGLRenderDevice::CreateVertexDescription(unsigned int numVertexElements, const VertexElement *vertexElements)
{
	return nullptr;
}

void OpenGLRenderDevice::DestroyVertexDescription(VertexDescription *vertexDescription)
{
}

VertexArray *OpenGLRenderDevice::CreateVertexArray(unsigned int numVertexBuffers, VertexBuffer **vertexBuffers, VertexDescription **vertexDescriptions)
{
	return nullptr;
}

void OpenGLRenderDevice::DestroyVertexArray(VertexArray *vertexArray)
{
}

void OpenGLRenderDevice::SetVertexArray(VertexArray *vertexArray)
{
}

void OpenGLRenderDevice::ClearColor(float red, float green, float blue, float alpha)
{
	glClearColor(red, green, blue, alpha);
}

void OpenGLRenderDevice::DrawTriangles(int offset, int count)
{
}

} // end namespace render
