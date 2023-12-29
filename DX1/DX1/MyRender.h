#pragma once

#include "D3D11_Framework.h"
#include <xnamath.h>

using namespace D3D11Framework;

class StaticMesh;

class MyRender : public Render
{
public:
	MyRender();
	bool Init(HWND hwnd);
	bool Draw();
	void Close();

	void* operator new(size_t i)
	{
		return _aligned_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		_aligned_free(p);
	}

private:
	friend StaticMesh;

	StaticMesh* m_mesh;

	XMMATRIX m_View;
	XMMATRIX m_Projection;
};