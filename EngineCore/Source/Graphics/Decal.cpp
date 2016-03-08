//========================================================================
//	Decal.cpp 
//
//========================================================================

#include "CthughaStd.h"

#include "../CthughaEngine/CthughaApp.h"
#include "../ResourceCache/ResCache.h"
#include "..\Entities\RenderComponent.h"
#include "SceneNodes.h"
#include "Decal.h"

//////////////////////////////////////////////
//	Implement DecalNode
//////////////////////////////////////////////

DecalNode::DecalNode(const EntityId entityId,
	WeakBaseRenderComponentPtr renderComponent,
	std::string sShaderResource,
	std::string sMaterialResource,
	UINT uMaskId,
	const Mat4x4 *world)
{
	m_pMaterialNode.reset(CHG_NEW MaterialNode(sMaterialResource, renderComponent));
	m_pMaterialNode->setActivePOM(false);

	// Vertex Format Shader Description //////////////////////////////////////////

	FormatDesc *vertexDesc = CHG_NEW FormatDesc[1];

	vertexDesc[0].stream = 0;
	vertexDesc[0].format = FORMAT_FLOAT;
	vertexDesc[0].type = TYPE_VERTEX;
	vertexDesc[0].size = 3;

	////////////////////////////////////////////////////////////////////////////////

	m_vShaderNode = std::shared_ptr<ShaderNode>(CHG_NEW ShaderNode(sShaderResource, renderComponent));
	m_vShaderNode->setVertexFormatShaderEntry(vertexDesc, 1);
	
	delete[] vertexDesc;

	m_bCastShadow = false;
	m_EntityID = entityId;
	m_IsVisible = true;
	m_pRenderComponent = renderComponent;

	m_Aabb.vcMax = Vec3(0.5f, 0.5f, 0.5f);
	m_Aabb.vcMin = Vec3(-0.5f, -0.5f, -0.5f);
	m_Aabb.vcCenter = Vec3();

	m_InQueueFrustum = false;
	m_InQueueLight = false;
	m_InQueueRender = false;

	m_ToWorld = *world;
	m_FromWorld = m_ToWorld.inverse();

	m_bHasShadowShader = false;
	m_bHasGIShader = false;

	m_bIsDrawable = true;
}

DecalNode::~DecalNode()
{
}

//
// DecalNode::VOnRestore
//
bool DecalNode::VOnRestore(Scene *pScene)
{
	SceneNode::VOnRestore(pScene);

	return true;
}

//
// DecalNode::VRender
//
bool DecalNode::VOnRender(Scene *pScene)
{
	if (pScene->m_RenderPass != 3)
	{
		UINT offset[] = { 0 };
		VertexBufferID vertexBuffer[] = { pScene->GetZeroCenteredUnitCubeVertexBufferID() };

		m_vShaderNode->setActiveShader();

		m_pMaterialNode->setActiveMaterial();

		g_pApp->m_pRenderer->setShaderConstant2f("screenSize", g_pApp->GetScreenSize());
		g_pApp->m_pRenderer->setShaderConstant4f("vCameraPosition", Vec4(pScene->GetCamera()->ToWorld().GetPosition(), 1.0f));

		g_pApp->m_pRenderer->setShaderConstant4x4f("View", pScene->GetCamera()->GetView());
		g_pApp->m_pRenderer->setShaderConstant4x4f("Projection", pScene->GetCamera()->GetProjection());
		g_pApp->m_pRenderer->setShaderConstant4x4f("mInverseView", pScene->GetCamera()->GetInverseView());
		g_pApp->m_pRenderer->setShaderConstant4x4f("mInverseProjection", pScene->GetCamera()->GetInverseProjection());

		g_pApp->m_pRenderer->setSamplerState("BaseFilter", pScene->m_trilinear[m_pMaterialNode->GetWrapMode()]);

		//g_pApp->m_pRenderer->setSamplerState("VoxelFilter", pScene->m_linearClamp);
		g_pApp->m_pRenderer->setSamplerState("TargetSampler", pScene->m_linearWrap);
		g_pApp->m_pRenderer->setTexture("tex_DepthTarget", pScene->GetPositionRT());

		g_pApp->m_pRenderer->setShaderConstant4x4f("World", transpose(m_ToWorld));
		g_pApp->m_pRenderer->setShaderConstant4x4f("mInverseWorld", transpose(!m_ToWorld));
		g_pApp->m_pRenderer->setVertexBuffer(0, 1, vertexBuffer, offset);

		g_pApp->m_pRenderer->apply();

		g_pApp->m_pRenderer->drawArrays(PRIM_TRIANGLE_STRIP, 0, 24);
	}

	return true;
}

//
// DecalNode::OnRenderWithoutShader
//
bool DecalNode::VOnSimpleRender(Scene *pScene)
{
	return true;
}

//
// DecalNode::OnRenderWithExternalShader
//
bool DecalNode::OnRenderWithExternalShader(Scene *pScene)
{
	return true;
}
