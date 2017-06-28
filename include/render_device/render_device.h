#pragma once

namespace render
{

// Encapsulates a vertex shader
class VertexShader
{
public:

	// virtual destructor to ensure subclasses have a virtual destructor
	virtual ~VertexShader() {}

protected:

	// protected default constructor to ensure these are never created directly
	VertexShader() {}
};

// Encapsulates a pixel shader
class PixelShader
{
public:

	// virtual destructor to ensure subclasses have a virtual destructor
	virtual ~PixelShader() {}

protected:

	// protected default constructor to ensure these are never created directly
	PixelShader() {}
};

// Encapsulates a shader pipeline uniform parameter
class PipelineParam
{
public:

	// virtual destructor to ensure subclasses have a virtual destructor
	virtual ~PipelineParam() {}

	virtual void SetAsInt(int value) = 0;

	virtual void SetAsFloat(float value) = 0;

	virtual void SetAsMat4(const float *value) = 0;

	virtual void SetAsIntArray(int count, const int *values) = 0;

	virtual void SetAsFloatArray(int count, const float *values) = 0;

	virtual void SetAsMat4Array(int count, const float *values) = 0;

protected:

	// protected default constructor to ensure these are never created directly
	PipelineParam() {}
};

// Encapsulates a shader pipeline
class Pipeline
{
public:

	// virtual destructor to ensure subclasses have a virtual destructor
	virtual ~Pipeline() {}

	virtual PipelineParam *GetParam(const char *name) = 0;

protected:

	// protected default constructor to ensure these are never created directly
	Pipeline() {}
};

// Encapsulates a vertex buffer
class VertexBuffer
{
public:

	// virtual destructor to ensure subclasses have a virtual destructor
	virtual ~VertexBuffer() {}

protected:

	// protected default constructor to ensure these are never created directly
	VertexBuffer() {}
};

// Encapsulates a vertex buffer semantic description
class VertexDescription
{
public:

	// virtual destructor to ensure subclasses have a virtual destructor
	virtual ~VertexDescription() {}

protected:

	// protected default constructor to ensure these are never created directly
	VertexDescription() {}
};

// Encapsulates a collection of vertex buffers and their semantic descriptions
class VertexArray
{
public:

	// virtual destructor to ensure subclasses have a virtual destructor
	virtual ~VertexArray() {}

protected:

	// protected default constructor to ensure these are never created directly
	VertexArray() {}
};
    
// Encapsulates an index buffer
class IndexBuffer
{
public:

    // virtual destructor to ensure subclasses have a virtual destructor
    virtual ~IndexBuffer() {}
        
protected:
        
    // protected default constructor to ensure these are never created directly
    IndexBuffer() {}
};

// Encapsulates a 2D texture
class Texture2D
{
public:

    // virtual destructor to ensure subclasses have a virtual destructor
    virtual ~Texture2D() {}
        
protected:
        
    // protected default constructor to ensure these are never created directly
    Texture2D() {}
};

// Describes a vertex element's type
enum VertexElementType
{
	VERTEXELEMENTTYPE_BYTE = 0,
	VERTEXELEMENTTYPE_SHORT,
	VERTEXELEMENTTYPE_INT,

	VERTEXELEMENTTYPE_UNSIGNED_BYTE,	
	VERTEXELEMENTTYPE_UNSIGNED_SHORT,
	VERTEXELEMENTTYPE_UNSIGNED_INT,

	VERTEXELEMENTTYPE_BYTE_NORMALIZE,
	VERTEXELEMENTTYPE_SHORT_NORMALIZE,
	VERTEXELEMENTTYPE_INT_NORMALIZE,

	VERTEXELEMENTTYPE_UNSIGNED_BYTE_NORMALIZE,	
	VERTEXELEMENTTYPE_UNSIGNED_SHORT_NORMALIZE,
	VERTEXELEMENTTYPE_UNSIGNED_INT_NORMALIZE,

	VERTEXELEMENTTYPE_HALF_FLOAT,
	VERTEXELEMENTTYPE_FLOAT,
	VERTEXELEMENTTYPE_DOUBLE
};

// Describes a vertex element within a vertex buffer
struct VertexElement
{
	unsigned int index; // location binding for vertex element
	VertexElementType type; // type of vertex element
	int size; // number of components
	int stride; // number of bytes between each successive element (leave zero for this to be assumed to be size times size of type)
	long long offset; // offset where first occurrence of this vertex element resides in the buffer
};

// Encapsulates the render device API.
class RenderDevice
{
public:

	// virtual destructor to ensure subclasses have a virtual destructor
	virtual ~RenderDevice() {}

	// Create a vertex shader from the supplied code; code is assumed to be GLSL for now.
	virtual VertexShader *CreateVertexShader(const char *code) = 0;

	// Destroy a vertex shader
	virtual void DestroyVertexShader(VertexShader *vertexShader) = 0;

	// Create a pixel shader from the supplied code; code is assumed to be GLSL for now.
	virtual PixelShader *CreatePixelShader(const char *code) = 0;

	// Destroy a pixel shader
	virtual void DestroyPixelShader(PixelShader *pixelShader) = 0;

	// Create a linked shader pipeline given a vertex and pixel shader
	virtual Pipeline *CreatePipeline(VertexShader *vertexShader, PixelShader *pixelShader) = 0;

	// Destroy a shader pipeline
	virtual void DestroyPipeline(Pipeline *pipeline) = 0;

	// Set a shader pipeline as active for subsequent draw commands
	virtual void SetPipeline(Pipeline *pipeline) = 0;

	// Create a vertex buffer
	virtual VertexBuffer *CreateVertexBuffer(long long size, const void *data = nullptr) = 0;

	// Destroy a vertex buffer
	virtual void DestroyVertexBuffer(VertexBuffer *vertexBuffer) = 0;

	// Create a vertex description given an array of VertexElement structures
	virtual VertexDescription *CreateVertexDescription(unsigned int numVertexElements, const VertexElement *vertexElements) = 0;

	// Destroy a vertex description
	virtual void DestroyVertexDescription(VertexDescription *vertexDescription) = 0;

	// Create a vertex array given an array of vertex buffers and associated vertex descriptions; the arrays must be the same size.
	virtual VertexArray *CreateVertexArray(unsigned int numVertexBuffers, VertexBuffer **vertexBuffers, VertexDescription **vertexDescriptions) = 0;

	// Destroy a vertex array
	virtual void DestroyVertexArray(VertexArray *vertexArray) = 0;

	// Set a vertex array as active for subsequent draw commands
	virtual void SetVertexArray(VertexArray *vertexArray) = 0;

    // Create an index buffer
    virtual IndexBuffer *CreateIndexBuffer(long long size, const void *data = nullptr) = 0;

    // Destroy an index buffer
    virtual void DestroyIndexBuffer(IndexBuffer *indexBuffer) = 0;
    
    // Set an index buffer as active for subsequent draw commands
    virtual void SetIndexBuffer(IndexBuffer *indexBuffer) = 0;

	// Create a 2D texture
    virtual Texture2D *CreateTexture2D(int width, int height, const void *data = nullptr) = 0;

    // Destroy a 2D texture
    virtual void DestroyTexture2D(Texture2D *texture2D) = 0;
    
    // Set a 2D texture as active on a slot for subsequent draw commands
    virtual void SetTexture2D(unsigned int slot, Texture2D *texture2D) = 0;

    // Clear the default render target's color buffer to the specified RGBA values
	virtual void Clear(float red, float green, float blue, float alpha, float depth) = 0;

	// Draw a collection of triangles using the currently active shader pipeline and vertex array data
	virtual void DrawTriangles(int offset, int count) = 0;

    // Draw a collection of triangles using the currently active shader pipeline, vertex array data,
    // and index buffer
    virtual void DrawTrianglesIndexed32(long long offset, int count) = 0;
};

// Creates a RenderDevice
RenderDevice *CreateRenderDevice();

// Destroys a RenderDevice
void DestroyRenderDevice(RenderDevice *renderDevice);

} // end namespace render
