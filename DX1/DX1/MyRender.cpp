#include "MyRender.h"
#include "StaticMesh.h"

MyRender::MyRender()
{
	m_mesh = nullptr;
}

bool MyRender::Init(HWND hwnd)
{
	XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, -2.8f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_View = XMMatrixLookAtLH(Eye, At, Up);

	m_Projection = XMMatrixPerspectiveFovLH(0.4f * 3.14f, (float)640 / 480, 0.1f, 1000.0f);

	m_mesh = new StaticMesh();
	if (!m_mesh->Init(this, L"mesh.ms3d"))
		return false;

	return true;
}

bool MyRender::Draw()
{
	m_mesh->Render();
	return true;
}

void MyRender::Close()
{
	_CLOSE(m_mesh);
}