#include "ogl_render_device.h"

#include <glad/glad.h>

#include <iostream>
#include <string>
#include <map>

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

class OpenGLPipelineParam;

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

	PipelineParam *GetParam(const char *name) override;

	int shaderProgram = 0;

	std::map<std::string, OpenGLPipelineParam *> paramsByName;
};

class OpenGLPipelineParam : public PipelineParam
{
public:

	OpenGLPipelineParam(OpenGLPipeline *_pipeline, int _location) : pipeline(_pipeline), location(_location) {}

	void SetAsInt(int value) override
	{
		glUseProgram(pipeline->shaderProgram);
		glUniform1i(location, value);
	}

	void SetAsFloat(float value) override
	{
		glUseProgram(pipeline->shaderProgram);
		glUniform1f(location, value);
	}

	void SetAsMat4(const float *value) override
	{
		glUseProgram(pipeline->shaderProgram);
		glUniformMatrix4fv(location, 1, /*transpose=*/GL_FALSE, value);
	}

	void SetAsIntArray(int count, const int *values) override
	{
		glUseProgram(pipeline->shaderProgram);
		glUniform1iv(location, count, values);
	}

	void SetAsFloatArray(int count, const float *values) override
	{
		glUseProgram(pipeline->shaderProgram);
		glUniform1fv(location, count, values);
	}

	void SetAsMat4Array(int count, const float *values) override
	{
		glUseProgram(pipeline->shaderProgram);
		glUniformMatrix4fv(location, count, /*transpose=*/GL_FALSE, values);
	}

	OpenGLPipeline *pipeline;
	int location;
};

PipelineParam *OpenGLPipeline::GetParam(const char *name)
{
	auto &iter = paramsByName.find(name);
	if(iter == paramsByName.end())
	{
		int location = glGetUniformLocation(shaderProgram, name);
		if(location < 0) return nullptr;
		OpenGLPipelineParam *param = new OpenGLPipelineParam(this, location);
		paramsByName.insert(iter, std::make_pair(name, param));
		return param;
	}
	return iter->second;
}

class OpenGLVertexBuffer : public VertexBuffer
{
public:

	OpenGLVertexBuffer(long long size, const void *data)
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

class OpenGLIndexBuffer : public IndexBuffer
{
public:

	OpenGLIndexBuffer(long long size, const void *data)
	{
		glGenBuffers(1, &IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW); // always assuming static, for now
	}

	~OpenGLIndexBuffer() override
	{
		glDeleteBuffers(1, &IBO);
	}

	unsigned int IBO = 0;
};

class OpenGLTexture2D : public Texture2D
{
public:

	OpenGLTexture2D(int width, int height, const void *data = nullptr)
	{
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	~OpenGLTexture2D() override
	{
		glDeleteTextures(1, &texture);
	}

	unsigned int texture = 0;
};

OpenGLRenderDevice::OpenGLRenderDevice()
{
	glEnable(GL_DEPTH_TEST);
}

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

VertexBuffer *OpenGLRenderDevice::CreateVertexBuffer(long long size, const void *data)
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

IndexBuffer *OpenGLRenderDevice::CreateIndexBuffer(long long size, const void *data)
{
	return new OpenGLIndexBuffer(size, data);
}

void OpenGLRenderDevice::DestroyIndexBuffer(IndexBuffer *indexBuffer)
{
	delete indexBuffer;
}
    
void OpenGLRenderDevice::SetIndexBuffer(IndexBuffer *indexBuffer)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, reinterpret_cast<OpenGLIndexBuffer *>(indexBuffer)->IBO);
}

Texture2D *OpenGLRenderDevice::CreateTexture2D(int width, int height, const void *data)
{
	return new OpenGLTexture2D(width, height, data);
}

void OpenGLRenderDevice::DestroyTexture2D(Texture2D *texture2D)
{
	delete texture2D;
}
   
void OpenGLRenderDevice::SetTexture2D(unsigned int slot, Texture2D *texture2D)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, texture2D ? reinterpret_cast<OpenGLTexture2D *>(texture2D)->texture : 0);
}

void OpenGLRenderDevice::Clear(float red, float green, float blue, float alpha, float depth)
{
	glClearColor(red, green, blue, alpha);
	glClearDepth(depth);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderDevice::DrawTriangles(int offset, int count)
{
	glDrawArrays(GL_TRIANGLES, offset, count);
}

void OpenGLRenderDevice::DrawTrianglesIndexed32(long long offset, int count)
{
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, reinterpret_cast<const void *>(offset));
}

} // end namespace render
