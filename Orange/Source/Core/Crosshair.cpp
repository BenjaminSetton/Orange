#include "../Misc/pch.h"

#include <DirectXMath.h>

#include "Application.h"
#include "Crosshair.h"
#include "ShaderBufferManagers/QuadNDCBufferManager.h"
#include "../Utility/Utility.h"

using namespace DirectX;

namespace Orange
{

	float Crosshair::m_scale = 0.5f;

	void Crosshair::Update(const float dt)
	{
		UNUSED(dt);
		PushQuadsToManager();
	}

	void Crosshair::PushQuadsToManager()
	{
		float aspectRatio = Application::Handle->GetMainWindow()->GetAspectRatio();
		QuadNDCVertexData vertexData[6] =
		{
			{ { -0.1f, 0.1f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { aspectRatio, m_scale } }, // TL
			{ { 0.1f, 0.1f }, { 1.0f, 0.0f }, { 0.0f, 0.0f }, { aspectRatio, m_scale } }, // TR
			{ { -0.1f, -0.1f }, { 0.0f, 1.0f }, { 0.0f, 0.0f }, { aspectRatio, m_scale } }, // BL

			{ { 0.1f, 0.1f }, { 1.0f, 0.0f }, { 0.0f, 0.0f }, { aspectRatio, m_scale } }, // TR
			{ { 0.1f, -0.1f }, { 1.0f, 1.0f }, { 0.0f, 0.0f }, { aspectRatio, m_scale } }, // BR
			{ { -0.1f, -0.1f }, { 0.0f, 1.0f }, { 0.0f, 0.0f }, { aspectRatio, m_scale } }, // BL
		};

		for (uint32_t i = 0; i < 6; i++) { QuadNDCBufferManager::PushVertex(vertexData[i]); }
	}

	const float Crosshair::GetScale() { return m_scale; }
	void Crosshair::SetScale(const float scale) { m_scale = scale; }

}
