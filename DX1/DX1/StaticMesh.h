#pragma once

#include "MyRender.h"

class StaticMesh
{
public:
	StaticMesh();

	bool Init(MyRender* render, const wchar_t* name);
	void Render();
	void Close();

private:
	bool m_loadMS3DFile(const wchar_t* name);
	bool m_LoadTextures(const LPCSTR name);
	bool m_InitShader(const wchar_t* namevs, const wchar_t* nameps);

	void m_RenderBuffers();
	void m_SetShaderParameters();
	void m_RenderShader();

	MyRender* m_render;

	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_pConstantBuffer;
	ID3D11SamplerState* m_sampleState;
	ID3D11ShaderResourceView* m_texture;

	XMMATRIX m_objMatrix;
	unsigned short m_indexCount;
	float m_rot;
};