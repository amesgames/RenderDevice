#pragma once

#include "render_device/render_device.h"

namespace render
{

class OpenGLRenderDevice : public RenderDevice
{
public:

	VertexShader *CreateVertexShader(const char *code) override;

	void DestroyVertexShader(VertexShader *vertexShader) override;

	PixelShader *CreatePixelShader(const char *code) override;

	void DestroyPixelShader(PixelShader *pixelShader) override;

	Pipeline *CreatePipeline(VertexShader *vertexShader, PixelShader *pixelShader) override;

	void DestroyPipeline(Pipeline *pipeline) override;

	void SetPipeline(Pipeline *pipeline) override;

	VertexBuffer *CreateVertexBuffer(long long size, void *data = nullptr) override;

	void DestroyVertexBuffer(VertexBuffer *vertexBuffer) override;

	VertexDescription *CreateVertexDescription(unsigned int numVertexElements, const VertexElement *vertexElements) override;

	void DestroyVertexDescription(VertexDescription *vertexDescription) override;

	VertexArray *CreateVertexArray(unsigned int numVertexBuffers, VertexBuffer **vertexBuffers, VertexDescription **vertexDescriptions) override;

	void DestroyVertexArray(VertexArray *vertexArray) override;

	void SetVertexArray(VertexArray *vertexArray) override;

	void ClearColor(float red, float green, float blue, float alpha) override;

	void DrawTriangles(int offset, int count) override;
};

} // end namespace render
