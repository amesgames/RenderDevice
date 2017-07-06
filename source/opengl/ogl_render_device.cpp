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
	auto const &iter = paramsByName.find(name);
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

class OpenGLRasterState : public RasterState
{
public:

	OpenGLRasterState(bool _cullEnabled = true, Winding _frontFace = WINDING_CCW, Face _cullFace = FACE_BACK, RasterMode _rasterMode = RASTERMODE_FILL)
	{
		static const GLenum front_face_map[] = { GL_CW, GL_CCW };
		static const GLenum cull_face_map[] = { GL_FRONT, GL_BACK, GL_FRONT_AND_BACK };
		static const GLenum raster_mode_map[] = { GL_POINT, GL_LINE, GL_FILL };

		cullEnabled = _cullEnabled;
		frontFace = front_face_map[_frontFace];
		cullFace = cull_face_map[_cullFace];
		polygonMode = raster_mode_map[_rasterMode];
	}

	bool cullEnabled;
	GLenum frontFace;
	GLenum cullFace;
	GLenum polygonMode;
};

class OpenGLDepthStencilState : public DepthStencilState
{
public:

	OpenGLDepthStencilState(
		bool			_depthEnabled				= true,
		bool			_depthWriteEnabled			= true,
		float			_depthNear					= 0,
		float			_depthFar					= 1,
		Compare			_depthCompare				= COMPARE_LESS,

		bool			_frontFaceStencilEnabled	= false,
		Compare			_frontFaceStencilCompare	= COMPARE_ALWAYS,
		StencilAction	_frontFaceStencilFail		= STENCIL_KEEP,
		StencilAction	_frontFaceStencilPass		= STENCIL_KEEP,
		StencilAction	_frontFaceDepthFail			= STENCIL_KEEP,
		int				_frontFaceRef				= 0,
		unsigned int	_frontFaceReadMask			= 0xFFFFFFFF,
		unsigned int	_frontFaceWriteMask			= 0xFFFFFFFF,

		bool			_backFaceStencilEnabled		= false,
		Compare			_backFaceStencilCompare		= COMPARE_ALWAYS,
		StencilAction	_backFaceStencilFail		= STENCIL_KEEP,
		StencilAction	_backFaceStencilPass		= STENCIL_KEEP,
		StencilAction	_backFaceDepthFail			= STENCIL_KEEP,
		int				_backFaceRef				= 0,
		unsigned int	_backFaceReadMask			= 0xFFFFFFFF,
		unsigned int	_backFaceWriteMask			= 0xFFFFFFFF)
		
	{
		static const GLenum compare_map[] = { GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, GL_ALWAYS };
		static const GLenum stencil_map[] = { GL_KEEP, GL_ZERO, GL_REPLACE, GL_INCR, GL_INCR_WRAP, GL_DECR, GL_DECR_WRAP, GL_INVERT };

		depthEnabled = _depthEnabled;
		depthWriteEnabled = _depthWriteEnabled;
		depthNear = _depthNear;
		depthFar = _depthFar;
		depthFunc = compare_map[_depthCompare];

		frontFaceStencilEnabled = _frontFaceStencilEnabled;
		frontStencilFunc = compare_map[_frontFaceStencilCompare];
		frontFaceStencilFail = stencil_map[_frontFaceStencilFail];
		frontFaceStencilPass = stencil_map[_frontFaceStencilPass];
		frontFaceDepthFail = stencil_map[_frontFaceDepthFail];
		frontFaceRef = _frontFaceRef;
		frontFaceReadMask = _frontFaceReadMask;
		frontFaceWriteMask = _frontFaceWriteMask;

		backFaceStencilEnabled = _backFaceStencilEnabled;
		backStencilFunc = compare_map[_backFaceStencilCompare];
		backFaceStencilFail = stencil_map[_backFaceStencilFail];
		backFaceStencilPass = stencil_map[_backFaceStencilPass];
		backFaceDepthFail = stencil_map[_backFaceDepthFail];
		backFaceRef = _backFaceRef;
		backFaceReadMask = _backFaceReadMask;
		backFaceWriteMask = _backFaceWriteMask;
	}


	bool depthEnabled;
	bool depthWriteEnabled;
	float depthNear;
	float depthFar;
	GLenum depthFunc;
	
	bool frontFaceStencilEnabled;
	GLenum	frontStencilFunc;
	GLenum frontFaceStencilFail;
	GLenum frontFaceStencilPass;
	GLenum frontFaceDepthFail;
	GLint frontFaceRef;
	GLuint frontFaceReadMask;
	GLuint frontFaceWriteMask;

	bool backFaceStencilEnabled;
	GLenum backStencilFunc;
	GLenum backFaceStencilFail;
	GLenum backFaceStencilPass;
	GLenum backFaceDepthFail;
	GLint backFaceRef;
	GLuint backFaceReadMask;
	GLuint backFaceWriteMask;
};

OpenGLRenderDevice::OpenGLRenderDevice()
{
	m_DefaultRasterState = dynamic_cast<OpenGLRasterState *>(CreateRasterState());
	SetRasterState(m_DefaultRasterState);

	m_DefaultDepthStencilState = dynamic_cast<OpenGLDepthStencilState *>(CreateDepthStencilState());
	SetDepthStencilState(m_DefaultDepthStencilState);
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

RasterState *OpenGLRenderDevice::CreateRasterState(bool cullEnabled, Winding frontFace, Face cullFace, RasterMode rasterMode)
{
	return new OpenGLRasterState(cullEnabled, frontFace, cullFace, rasterMode);
}

void OpenGLRenderDevice::DestroyRasterState(RasterState *rasterState)
{
	delete rasterState;
}

void OpenGLRenderDevice::SetRasterState(RasterState *rasterState)
{
	RasterState *oldRasterState = m_RasterState;

	if(rasterState)
		m_RasterState = dynamic_cast<OpenGLRasterState *>(rasterState);
	else
		m_RasterState = m_DefaultRasterState;

	if(m_RasterState != oldRasterState)
	{
		if(m_RasterState->cullEnabled)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);
		glFrontFace(m_RasterState->frontFace);
		glCullFace(m_RasterState->cullFace);
		glPolygonMode(GL_FRONT_AND_BACK, m_RasterState->polygonMode);
	}
}

DepthStencilState *OpenGLRenderDevice::CreateDepthStencilState(bool depthEnabled, bool depthWriteEnabled, float depthNear, float depthFar, Compare depthCompare,
		bool frontFaceStencilEnabled, Compare frontFaceStencilCompare, StencilAction frontFaceStencilFail, StencilAction frontFaceStencilPass,
		StencilAction frontFaceDepthFail, int frontFaceRef, unsigned int frontFaceReadMask, unsigned int frontFaceWriteMask, bool backFaceStencilEnabled,
		Compare backFaceStencilCompare, StencilAction backFaceStencilFail, StencilAction backFaceStencilPass, StencilAction backFaceDepthFail,
		int backFaceRef, unsigned int backFaceReadMask, unsigned int backFaceWriteMask)
{
	return new OpenGLDepthStencilState(depthEnabled, depthWriteEnabled, depthNear, depthFar, depthCompare, frontFaceStencilEnabled, frontFaceStencilCompare,
		frontFaceStencilFail, frontFaceStencilPass, frontFaceDepthFail, frontFaceRef, frontFaceReadMask, frontFaceWriteMask, backFaceStencilEnabled,
		backFaceStencilCompare, backFaceStencilFail, backFaceStencilPass, backFaceDepthFail, backFaceRef, backFaceReadMask, backFaceWriteMask);
}

void OpenGLRenderDevice::DestroyDepthStencilState(DepthStencilState *depthStencilState)
{
	delete depthStencilState;
}

void OpenGLRenderDevice::SetDepthStencilState(DepthStencilState *depthStencilState)
{
	DepthStencilState *oldDepthStencilState = m_DepthStencilState;

	if (depthStencilState)
		m_DepthStencilState = dynamic_cast<OpenGLDepthStencilState *>(depthStencilState);
	else
		m_DepthStencilState = m_DefaultDepthStencilState;

	if(m_DepthStencilState != oldDepthStencilState)
	{
		if(m_DepthStencilState->depthEnabled)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
		glDepthFunc(m_DepthStencilState->depthFunc);
		glDepthMask(m_DepthStencilState->depthWriteEnabled ? GL_TRUE : GL_FALSE);
		glDepthRange(m_DepthStencilState->depthNear, m_DepthStencilState->depthFar);

		if(m_DepthStencilState->frontFaceStencilEnabled || m_DepthStencilState->backFaceStencilEnabled)
			glEnable(GL_STENCIL_TEST);
		else
			glDisable(GL_STENCIL_TEST);

		// front face
		glStencilFuncSeparate(GL_FRONT, m_DepthStencilState->frontStencilFunc, m_DepthStencilState->frontFaceRef, m_DepthStencilState->frontFaceReadMask);
		glStencilMaskSeparate(GL_FRONT, m_DepthStencilState->frontFaceWriteMask);
		glStencilOpSeparate(GL_FRONT, m_DepthStencilState->frontFaceStencilFail, m_DepthStencilState->frontFaceDepthFail, m_DepthStencilState->frontFaceStencilPass);

		// back face
		glStencilFuncSeparate(GL_BACK, m_DepthStencilState->backStencilFunc, m_DepthStencilState->backFaceRef, m_DepthStencilState->backFaceReadMask);
		glStencilMaskSeparate(GL_BACK, m_DepthStencilState->backFaceWriteMask);
		glStencilOpSeparate(GL_BACK, m_DepthStencilState->backFaceStencilFail, m_DepthStencilState->backFaceDepthFail, m_DepthStencilState->backFaceStencilPass);
	}
}

void OpenGLRenderDevice::Clear(float red, float green, float blue, float alpha, float depth, int stencil)
{
	glClearColor(red, green, blue, alpha);
	glClearDepth(depth);
	glClearStencil(stencil);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT| GL_STENCIL_BUFFER_BIT);
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
