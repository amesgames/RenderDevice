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

	OpenGLVertexBuffer(long long size, void *data)
	{
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW); // always assuming static, for now
	}

	~OpenGLVertexBuffer() override
	{
		glDeleteBuffers(1, &VBO);
	}

	unsigned int VBO = 0;
};

class OpenGLVertexDescription : public VertexDescription
{
public:

	struct OpenGLVertexElement
	{
		GLuint index;
		GLint size;
		GLenum type;
		GLboolean normalized;
		GLsizei stride;
		const GLvoid *pointer;
	};

	OpenGLVertexDescription(unsigned int _numVertexElements, const VertexElement *vertexElements) : numVertexElements(_numVertexElements)
	{
		static GLenum toOpenGLType[] = { GL_BYTE, GL_SHORT, GL_INT, GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT,
			GL_BYTE, GL_SHORT, GL_INT, GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT, GL_HALF_FLOAT, GL_FLOAT, GL_DOUBLE };
		static GLboolean toOpenGLNormalized[] = { GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE,
			GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE };

		openGLVertexElements = new OpenGLVertexElement[numVertexElements];
		for(unsigned int i = 0; i < numVertexElements; i++)
		{
			openGLVertexElements[i].index = vertexElements[i].index;
			openGLVertexElements[i].size = vertexElements[i].size;
			openGLVertexElements[i].type = toOpenGLType[vertexElements[i].type];
			openGLVertexElements[i].normalized = toOpenGLNormalized[vertexElements[i].type];
			openGLVertexElements[i].stride = vertexElements[i].stride;
			openGLVertexElements[i].pointer = (char *)nullptr + vertexElements[i].offset;
		}
	}

	~OpenGLVertexDescription() override
	{
		delete[] openGLVertexElements;
	}

	unsigned int numVertexElements = 0;
	OpenGLVertexElement *openGLVertexElements = nullptr;
};

class OpenGLVertexArray : public VertexArray
{
public:

	OpenGLVertexArray(unsigned int numVertexBuffers, VertexBuffer **vertexBuffers, VertexDescription **vertexDescriptions)
	{
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		for(unsigned int i = 0; i < numVertexBuffers; i++)
		{
			OpenGLVertexBuffer *vertexBuffer = reinterpret_cast<OpenGLVertexBuffer *>(vertexBuffers[i]);
			OpenGLVertexDescription *vertexDescription = reinterpret_cast<OpenGLVertexDescription *>(vertexDescriptions[i]);

			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->VBO);

			for(unsigned int j = 0; j < vertexDescription->numVertexElements; j++)
			{
				glEnableVertexAttribArray(vertexDescription->openGLVertexElements[j].index);
				glVertexAttribPointer(vertexDescription->openGLVertexElements[j].index, vertexDescription->openGLVertexElements[j].size, vertexDescription->openGLVertexElements[j].type,
					vertexDescription->openGLVertexElements[j].normalized, vertexDescription->openGLVertexElements[j].stride, vertexDescription->openGLVertexElements[j].pointer);
			}
		}
	}

	~OpenGLVertexArray() override
	{
		glDeleteVertexArrays(1, &VAO);
	}

	unsigned int VAO = 0;
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
	return new OpenGLVertexBuffer(size, data);
}

void OpenGLRenderDevice::DestroyVertexBuffer(VertexBuffer *vertexBuffer)
{
	delete vertexBuffer;
}

VertexDescription *OpenGLRenderDevice::CreateVertexDescription(unsigned int numVertexElements, const VertexElement *vertexElements)
{
	return new OpenGLVertexDescription(numVertexElements, vertexElements);
}

void OpenGLRenderDevice::DestroyVertexDescription(VertexDescription *vertexDescription)
{
	delete vertexDescription;
}

VertexArray *OpenGLRenderDevice::CreateVertexArray(unsigned int numVertexBuffers, VertexBuffer **vertexBuffers, VertexDescription **vertexDescriptions)
{
	return new OpenGLVertexArray(numVertexBuffers, vertexBuffers, vertexDescriptions);
}

void OpenGLRenderDevice::DestroyVertexArray(VertexArray *vertexArray)
{
	delete vertexArray;
}

void OpenGLRenderDevice::SetVertexArray(VertexArray *vertexArray)
{
	glBindVertexArray(reinterpret_cast<OpenGLVertexArray *>(vertexArray)->VAO);
}

void OpenGLRenderDevice::ClearColor(float red, float green, float blue, float alpha)
{
	glClearColor(red, green, blue, alpha);
	glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLRenderDevice::DrawTriangles(int offset, int count)
{
	glDrawArrays(GL_TRIANGLES, offset, count);
}

} // end namespace render
