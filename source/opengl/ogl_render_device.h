#pragma once

#include "render_device/render_device.h"

namespace render
{

class OpenGLRasterState;
class OpenGLDepthStencilState;

class OpenGLRenderDevice : public RenderDevice
{
public:

	OpenGLRenderDevice();

	VertexShader *CreateVertexShader(const char *code) override;

	void DestroyVertexShader(VertexShader *vertexShader) override;

	PixelShader *CreatePixelShader(const char *code) override;

	void DestroyPixelShader(PixelShader *pixelShader) override;

	Pipeline *CreatePipeline(VertexShader *vertexShader, PixelShader *pixelShader) override;

	void DestroyPipeline(Pipeline *pipeline) override;

	void SetPipeline(Pipeline *pipeline) override;

	VertexBuffer *CreateVertexBuffer(long long size, const void *data = nullptr) override;

	void DestroyVertexBuffer(VertexBuffer *vertexBuffer) override;

	VertexDescription *CreateVertexDescription(unsigned int numVertexElements, const VertexElement *vertexElements) override;

	void DestroyVertexDescription(VertexDescription *vertexDescription) override;

	VertexArray *CreateVertexArray(unsigned int numVertexBuffers, VertexBuffer **vertexBuffers, VertexDescription **vertexDescriptions) override;

	void DestroyVertexArray(VertexArray *vertexArray) override;

	void SetVertexArray(VertexArray *vertexArray) override;

	IndexBuffer *CreateIndexBuffer(long long size, const void *data = nullptr) override;

    void DestroyIndexBuffer(IndexBuffer *indexBuffer) override;
    
    void SetIndexBuffer(IndexBuffer *indexBuffer) override;

    Texture2D *CreateTexture2D(int width, int height, const void *data = nullptr) override;

    void DestroyTexture2D(Texture2D *texture2D) override;
    
	void SetTexture2D(unsigned int slot, Texture2D *texture2D) override;

	RasterState *CreateRasterState(bool cullEnabled = true, Winding frontFace = WINDING_CCW, Face cullFace = FACE_BACK, RasterMode rasterMode = RASTERMODE_FILL) override;

	void DestroyRasterState(RasterState *rasterState) override;

	void SetRasterState(RasterState *rasterState) override;

	DepthStencilState *CreateDepthStencilState(bool depthEnabled = true,
		bool			depthWriteEnabled = true,
		float			depthNear = 0,
		float			depthFar = 1,
		Compare			depthCompare = COMPARE_LESS,

		bool			frontFaceStencilEnabled = false,
		Compare			frontFaceStencilCompare = COMPARE_ALWAYS,
		StencilAction	frontFaceStencilFail = STENCIL_KEEP,
		StencilAction	frontFaceStencilPass = STENCIL_KEEP,
		StencilAction	frontFaceDepthFail = STENCIL_KEEP,
		int				frontFaceRef = 0,
		unsigned int	frontFaceReadMask = 0xFFFFFFFF,
		unsigned int	frontFaceWriteMask = 0xFFFFFFFF,

		bool			backFaceStencilEnabled = false,
		Compare			backFaceStencilCompare = COMPARE_ALWAYS,
		StencilAction	backFaceStencilFail = STENCIL_KEEP,
		StencilAction	backFaceStencilPass = STENCIL_KEEP,
		StencilAction	backFaceDepthFail = STENCIL_KEEP,
		int				backFaceRef = 0,
		unsigned int	backFaceReadMask = 0xFFFFFFFF,
		unsigned int	backFaceWriteMask = 0xFFFFFFFF) override;

	void DestroyDepthStencilState(DepthStencilState *depthStencilState) override;

	void SetDepthStencilState(DepthStencilState *depthStencilState) override;

	void Clear(float red = 0.0f, float green = 0.0f, float blue = 0.0f, float alpha = 1.0f, float depth = 1.0f, int stencil = 0) override;

	void DrawTriangles(int offset, int count) override;

	void DrawTrianglesIndexed32(long long offset, int count) override;

private:

	OpenGLRasterState *m_RasterState = nullptr;
	OpenGLRasterState *m_DefaultRasterState = nullptr;

	OpenGLDepthStencilState *m_DepthStencilState = nullptr;
	OpenGLDepthStencilState *m_DefaultDepthStencilState = nullptr;

};

} // end namespace render
