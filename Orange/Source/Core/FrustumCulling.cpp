#include "../Misc/pch.h"
#include "FrustumCulling.h"
#include "../Utility/DebugRenderer.h"

using namespace DirectX;

namespace Orange
{
	Frustum FrustumCulling::m_frustum = Frustum();


	void FrustumCulling::SetFrustum(const Frustum& frustum) { m_frustum = frustum; }
	const Frustum FrustumCulling::GetFrustum() { return m_frustum; }

	bool FrustumCulling::CalculateChunkPosAgainstFrustum(const XMFLOAT3 chunkPosWS)
	{
		// Convert Chunk pos to AABB to test against frustum
		AABB aabb = ConvertChunkPosToAABB(chunkPosWS);

		// Test the AABB against the frustum and return the result
	#if defined(PASS_STRADDLING_CHUNKS)
		if (TestAABBAgainstPlane(aabb, m_frustum.planes[Frustum_Planes::BACK])		< 0) return false;
		if (TestAABBAgainstPlane(aabb, m_frustum.planes[Frustum_Planes::BOTTOM])	< 0) return false;
		if (TestAABBAgainstPlane(aabb, m_frustum.planes[Frustum_Planes::FRONT])		< 0) return false;
		if (TestAABBAgainstPlane(aabb, m_frustum.planes[Frustum_Planes::LEFT])		< 0) return false;
		if (TestAABBAgainstPlane(aabb, m_frustum.planes[Frustum_Planes::RIGHT])		< 0) return false;
		if (TestAABBAgainstPlane(aabb, m_frustum.planes[Frustum_Planes::TOP])		< 0) return false;
		return true;
	#elif
		if (TestAABBAgainstPlane(aabb, m_frustum.planes[Frustum_Planes::BACK]) > 0) return false;
		if (TestAABBAgainstPlane(aabb, m_frustum.planes[Frustum_Planes::BOTTOM]) > 0) return false;
		if (TestAABBAgainstPlane(aabb, m_frustum.planes[Frustum_Planes::FRONT]) > 0) return false;
		if (TestAABBAgainstPlane(aabb, m_frustum.planes[Frustum_Planes::LEFT]) > 0) return false;
		if (TestAABBAgainstPlane(aabb, m_frustum.planes[Frustum_Planes::RIGHT]) > 0) return false;
		if (TestAABBAgainstPlane(aabb, m_frustum.planes[Frustum_Planes::TOP]) > 0) return false;
		return true;
	#endif
	}

	void FrustumCulling::CalculateFrustum(float FOV, float aspectRatio, float nearPlane, float farPlane, const XMMATRIX& viewMatrix, const XMFLOAT3& camPos)
	{
		XMVECTOR cameraPos = XMLoadFloat3(&camPos);

		//Project the centers outward
		XMVECTOR nearCenter = cameraPos + viewMatrix.r[2] * nearPlane;
		XMVECTOR farCenter = cameraPos + viewMatrix.r[2] * farPlane;

		//Variables needed to calculate the values
		float halfHeightNear = tan(FOV / 2.0f) * nearPlane;
		float halfHeightFar = tan(FOV / 2.0f) * farPlane;
		float halfWidthNear = halfHeightNear * aspectRatio;
		float halfWidthFar = halfHeightFar * aspectRatio;

		//All possible points of frustum
		XMVECTOR zeroVector = { 0.0f, 0.0f, 0.0f, 0.0f};
		XMVECTOR topLeftNear = nearCenter + (viewMatrix.r[1] * halfHeightNear - viewMatrix.r[0] * halfWidthNear);
		XMVECTOR topRightNear = nearCenter + (viewMatrix.r[1] * halfHeightNear + viewMatrix.r[0] * halfWidthNear);
		XMVECTOR botRightNear = nearCenter + (zeroVector - (viewMatrix.r[1] * halfHeightNear) + viewMatrix.r[0] * halfWidthNear);
		XMVECTOR botLeftNear = nearCenter + (zeroVector - (viewMatrix.r[1] * halfHeightNear) - viewMatrix.r[0] * halfWidthNear);
		XMVECTOR topLeftFar = farCenter + (viewMatrix.r[1] * halfHeightFar - viewMatrix.r[0] * halfWidthFar);
		XMVECTOR topRightFar = farCenter + (viewMatrix.r[1] * halfHeightFar + viewMatrix.r[0] * halfWidthFar);
		XMVECTOR botRightFar = farCenter + (zeroVector - (viewMatrix.r[1] * halfHeightFar) + viewMatrix.r[0] * halfWidthFar);
		XMVECTOR botLeftFar = farCenter + (zeroVector - (viewMatrix.r[1] * halfHeightFar) - viewMatrix.r[0] * halfWidthFar);

		m_frustum.planes[Frustum_Planes::FRONT] = CalculatePlaneFromPoints(botRightNear, topLeftNear, topRightNear);
		m_frustum.planes[Frustum_Planes::BACK] = CalculatePlaneFromPoints(botRightFar, topRightFar, topLeftFar);
		m_frustum.planes[Frustum_Planes::LEFT] = CalculatePlaneFromPoints(botLeftFar, topLeftFar, topLeftNear);
		m_frustum.planes[Frustum_Planes::RIGHT] = CalculatePlaneFromPoints(topRightNear, topRightFar, botRightFar);
		m_frustum.planes[Frustum_Planes::TOP] = CalculatePlaneFromPoints(topLeftFar, topRightFar, topRightNear);
		m_frustum.planes[Frustum_Planes::BOTTOM] = CalculatePlaneFromPoints(botRightFar, botLeftFar, botLeftNear);

		m_frustum.vertices[Frustum_Vertices::BLF] = botLeftFar;
		m_frustum.vertices[Frustum_Vertices::BLN] = botLeftNear;
		m_frustum.vertices[Frustum_Vertices::BRF] = botRightFar;
		m_frustum.vertices[Frustum_Vertices::BRN] = botRightNear;
		m_frustum.vertices[Frustum_Vertices::TLF] = topLeftFar;
		m_frustum.vertices[Frustum_Vertices::TLN] = topLeftNear;
		m_frustum.vertices[Frustum_Vertices::TRF] = topRightFar;
		m_frustum.vertices[Frustum_Vertices::TRN] = topRightNear;
	}


	const AABB FrustumCulling::ConvertChunkPosToAABB(const XMFLOAT3 chunkPosWS)
	{
		AABB aabb;
		aabb.center = { chunkPosWS.x + CHUNK_SIZE / 2.0f, chunkPosWS.y + CHUNK_SIZE / 2.0f, chunkPosWS.z + CHUNK_SIZE / 2.0f };
		aabb.extent = { 8.0f, 8.0f, 8.0f };
		return aabb;
	}

	int FrustumCulling::TestAABBAgainstPlane(const AABB& aabb, const Plane& plane)
	{
		float projectedRadius = aabb.extent.x * abs(plane.normal.x) + aabb.extent.y * abs(plane.normal.y) + aabb.extent.z * abs(plane.normal.z);
		XMFLOAT3 projSpherePos = aabb.center;
		Sphere testSphere = { projSpherePos, projectedRadius };
		return TestSphereAgainstPlane(testSphere, plane);
	}

	int FrustumCulling::TestSphereAgainstPlane(const Sphere& sphere, const Plane& plane)
	{
		float sphereDist = XMVector3Dot(XMLoadFloat3(&sphere.center), XMLoadFloat3(&plane.normal)).m128_f32[0] - plane.point;

		if (sphereDist > sphere.radius) return 1;
		else if (sphereDist < -sphere.radius) return -1;
		else return 0;
	}

	Plane FrustumCulling::CalculatePlaneFromPoints(const XMVECTOR& a, const XMVECTOR& b, const XMVECTOR& c)
	{
		Plane plane;

		//Considering B is the "center" of the triangle
		XMVECTOR vecOne = XMVector3Normalize(a - b);
		XMVECTOR vecTwo = XMVector3Normalize(c - b);
		XMVECTOR normal = XMVector3Cross(vecOne, vecTwo);

		XMStoreFloat3(&plane.normal, normal);

		//The offset is the dot product between a triangle vertex and the normal
		plane.point = XMVector3Dot(normal, b).m128_f32[0];

		return plane;
	}

	void FrustumCulling::Debug_DrawFrustum()
	{
		XMFLOAT4 lineColor = { 0.0f, 1.0f, 0.0f, 1.0f };

		XMFLOAT3 tln, trn, bln, brn, tlf, trf, blf, brf;
		XMStoreFloat3(&tln, m_frustum.vertices[Frustum_Vertices::TLN]);
		XMStoreFloat3(&trn, m_frustum.vertices[Frustum_Vertices::TRN]);
		XMStoreFloat3(&bln, m_frustum.vertices[Frustum_Vertices::BLN]);
		XMStoreFloat3(&brn, m_frustum.vertices[Frustum_Vertices::BRN]);
		XMStoreFloat3(&tlf, m_frustum.vertices[Frustum_Vertices::TLF]);
		XMStoreFloat3(&trf, m_frustum.vertices[Frustum_Vertices::TRF]);
		XMStoreFloat3(&blf, m_frustum.vertices[Frustum_Vertices::BLF]);
		XMStoreFloat3(&brf, m_frustum.vertices[Frustum_Vertices::BRF]);

		DebugRenderer::DrawLine(tln, tlf, lineColor);
		DebugRenderer::DrawLine(trn, trf, lineColor);
		DebugRenderer::DrawLine(bln, blf, lineColor);
		DebugRenderer::DrawLine(brn, brf, lineColor);

		DebugRenderer::DrawLine(tln, trn, lineColor);
		DebugRenderer::DrawLine(trn, brn, lineColor);
		DebugRenderer::DrawLine(brn, bln, lineColor);
		DebugRenderer::DrawLine(bln, tln, lineColor);

		DebugRenderer::DrawLine(tlf, trf, lineColor);
		DebugRenderer::DrawLine(trf, brf, lineColor);
		DebugRenderer::DrawLine(brf, blf, lineColor);
		DebugRenderer::DrawLine(blf, tlf, lineColor);

		// Frustum normals
		XMFLOAT4 nrmStartCol = { 1.0f, 0.0f, 0.0f, 1.0f };
		XMFLOAT4 nrmEndCol = { 0.0f, 0.0f, 1.0f, 1.0f };

		Plane currPlane = m_frustum.planes[Frustum_Planes::TOP];
		XMFLOAT3 planeMidpoint;
		XMFLOAT3 endPoint;
		XMStoreFloat3(&planeMidpoint, {
			(m_frustum.vertices[Frustum_Vertices::TLN] +
			m_frustum.vertices[Frustum_Vertices::TRN] +
			((m_frustum.vertices[Frustum_Vertices::TLF] - m_frustum.vertices[Frustum_Vertices::TLN]) / 100.0f + m_frustum.vertices[Frustum_Vertices::TLN]) +
			((m_frustum.vertices[Frustum_Vertices::TRF] - m_frustum.vertices[Frustum_Vertices::TRN]) / 100.0f + m_frustum.vertices[Frustum_Vertices::TRN]))
						 / 4.0f });
		XMStoreFloat3(&endPoint, XMLoadFloat3(&planeMidpoint) + XMLoadFloat3(&currPlane.normal));
		DebugRenderer::DrawLine(planeMidpoint, endPoint, nrmStartCol, nrmEndCol);

		currPlane = m_frustum.planes[Frustum_Planes::BOTTOM];
		XMStoreFloat3(&planeMidpoint, {
			(m_frustum.vertices[Frustum_Vertices::BLN] +
			m_frustum.vertices[Frustum_Vertices::BRN] +
			((m_frustum.vertices[Frustum_Vertices::BLF] - m_frustum.vertices[Frustum_Vertices::BLN]) / 100.0f + m_frustum.vertices[Frustum_Vertices::BLN]) +
			((m_frustum.vertices[Frustum_Vertices::BRF] - m_frustum.vertices[Frustum_Vertices::BRN]) / 100.0f + m_frustum.vertices[Frustum_Vertices::BRN]))
						 / 4.0f });
		XMStoreFloat3(&endPoint, XMLoadFloat3(&planeMidpoint) + XMLoadFloat3(&currPlane.normal));
		DebugRenderer::DrawLine(planeMidpoint, endPoint, nrmStartCol, nrmEndCol);

		currPlane = m_frustum.planes[Frustum_Planes::LEFT];
		XMStoreFloat3(&planeMidpoint, {
			(m_frustum.vertices[Frustum_Vertices::BLN] +
			m_frustum.vertices[Frustum_Vertices::TLN] +
			((m_frustum.vertices[Frustum_Vertices::BLF] - m_frustum.vertices[Frustum_Vertices::BLN]) / 100.0f + m_frustum.vertices[Frustum_Vertices::BLN]) +
			((m_frustum.vertices[Frustum_Vertices::TLF] - m_frustum.vertices[Frustum_Vertices::TLN]) / 100.0f + m_frustum.vertices[Frustum_Vertices::TLN]))
						 / 4.0f });
		XMStoreFloat3(&endPoint, XMLoadFloat3(&planeMidpoint) + XMLoadFloat3(&currPlane.normal));
		DebugRenderer::DrawLine(planeMidpoint, endPoint, nrmStartCol, nrmEndCol);

		currPlane = m_frustum.planes[Frustum_Planes::RIGHT];
		XMStoreFloat3(&planeMidpoint, {
			(m_frustum.vertices[Frustum_Vertices::BRN] +
			m_frustum.vertices[Frustum_Vertices::TRN] +
			((m_frustum.vertices[Frustum_Vertices::BRF] - m_frustum.vertices[Frustum_Vertices::BRN]) / 100.0f + m_frustum.vertices[Frustum_Vertices::BRN]) +
			((m_frustum.vertices[Frustum_Vertices::TRF] - m_frustum.vertices[Frustum_Vertices::TRN]) / 100.0f + m_frustum.vertices[Frustum_Vertices::TRN]))
						 / 4.0f });
		XMStoreFloat3(&endPoint, XMLoadFloat3(&planeMidpoint) + XMLoadFloat3(&currPlane.normal));
		DebugRenderer::DrawLine(planeMidpoint, endPoint, nrmStartCol, nrmEndCol);

		currPlane = m_frustum.planes[Frustum_Planes::FRONT];
		XMStoreFloat3(&planeMidpoint, {
			(m_frustum.vertices[Frustum_Vertices::TLN] +
			m_frustum.vertices[Frustum_Vertices::TRN] +
			m_frustum.vertices[Frustum_Vertices::BRN] +
			m_frustum.vertices[Frustum_Vertices::BLN])
						 / 4.0f });
		XMStoreFloat3(&endPoint, XMLoadFloat3(&planeMidpoint) + XMLoadFloat3(&currPlane.normal));
		DebugRenderer::DrawLine(planeMidpoint, endPoint, nrmStartCol, nrmEndCol);

		currPlane = m_frustum.planes[Frustum_Planes::BACK];
		XMStoreFloat3(&planeMidpoint, {
			(m_frustum.vertices[Frustum_Vertices::TLF] +
			m_frustum.vertices[Frustum_Vertices::TRF] +
			m_frustum.vertices[Frustum_Vertices::BRF] +
			m_frustum.vertices[Frustum_Vertices::BLF])
						 / 4.0f });
		XMStoreFloat3(&endPoint, XMLoadFloat3(&planeMidpoint) + XMLoadFloat3(&currPlane.normal));
		DebugRenderer::DrawLine(planeMidpoint, endPoint, nrmStartCol, nrmEndCol);
	}

	void FrustumCulling::Debug_DrawAABB(const AABB& aabb)
	{
		XMFLOAT4 lineCol = { 1.0f, 0.0f, 0.0f, 1.0f };
		XMFLOAT3 center = aabb.center;
		XMFLOAT3 extent = aabb.extent;
		XMFLOAT3 tln = { center.x - extent.x, center.y + extent.y, center.z - extent.z };
		XMFLOAT3 trn = { center.x + extent.x, center.y + extent.y, center.z - extent.z };
		XMFLOAT3 bln = { center.x - extent.x, center.y - extent.y, center.z - extent.z };
		XMFLOAT3 brn = { center.x + extent.x, center.y - extent.y, center.z - extent.z };
		XMFLOAT3 tlf = { center.x - extent.x, center.y + extent.y, center.z + extent.z };
		XMFLOAT3 trf = { center.x + extent.x, center.y + extent.y, center.z + extent.z };
		XMFLOAT3 blf = { center.x - extent.x, center.y - extent.y, center.z + extent.z };
		XMFLOAT3 brf = { center.x + extent.x, center.y - extent.y, center.z + extent.z };

		// FRONT FACE
		DebugRenderer::DrawLine(tln, trn, lineCol);
		DebugRenderer::DrawLine(trn, brn, lineCol);
		DebugRenderer::DrawLine(brn, bln, lineCol);
		DebugRenderer::DrawLine(bln, tln, lineCol);

		// BACK FACE
		DebugRenderer::DrawLine(tlf, trf, lineCol);
		DebugRenderer::DrawLine(trf, brf, lineCol);
		DebugRenderer::DrawLine(brf, blf, lineCol);
		DebugRenderer::DrawLine(blf, tlf, lineCol);

		// TOP FACE
		DebugRenderer::DrawLine(tln, trn, lineCol);
		DebugRenderer::DrawLine(trn, trf, lineCol);
		DebugRenderer::DrawLine(trf, tlf, lineCol);
		DebugRenderer::DrawLine(tlf, tln, lineCol);

		// BOTTOM FACE
		DebugRenderer::DrawLine(bln, brn, lineCol);
		DebugRenderer::DrawLine(brn, brf, lineCol);
		DebugRenderer::DrawLine(brf, blf, lineCol);
		DebugRenderer::DrawLine(blf, bln, lineCol);
	}
}