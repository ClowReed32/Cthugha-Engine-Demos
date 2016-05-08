//========================================================================
// PhysicsDebugDrawer.cpp - implements a physics debug drawer
//
//========================================================================



#include "CthughaStd.h"

#include "PhysicsDebugDrawer.h"
#include "../UserInterface/HumanView.h"
#include "..\ResourceCache\ResCache.h"
#include "..\Graphics\Shader.h"

bool BulletDebugDrawer::Init(UINT MaxVertex)
{
	m_LineVertexBuffer = g_pApp->m_pRenderer->addVertexBuffer(MaxVertex * sizeof(Vec3), DYNAMIC);
	m_LineColorBuffer = g_pApp->m_pRenderer->addVertexBuffer(MaxVertex * sizeof(Color), DYNAMIC);

	//Load Debug Shader
	Resource PhysicDebugResource("Shaders\\PhysicShapeDebugShader.shd");
	shared_ptr<ResHandle> PhysicDebugShaderHandle = g_pApp->m_ResCache->GetHandle(&PhysicDebugResource);

	if (!PhysicDebugShaderHandle)
	{
		CHG_ERROR("Not found Physic Debug Shader");

		return false;
	}

	m_PhysicDebugRenderShader = ((ShaderResourceExtraData*)PhysicDebugShaderHandle->GetExtra().get())->m_ShaderID;

	FormatDesc *vertexDesc = CHG_NEW FormatDesc[2];

	vertexDesc[0].stream = 0;
	vertexDesc[0].format = FORMAT_FLOAT;
	vertexDesc[0].type = TYPE_VERTEX;
	vertexDesc[0].size = 3;

	vertexDesc[1].stream = 1;
	vertexDesc[1].format = FORMAT_FLOAT;
	vertexDesc[1].type = TYPE_COLOR;
	vertexDesc[1].size = 4;

	if (m_PhysicDebugRenderShader != SHADER_NONE)
		m_PhysicDebugRenderShaderVF = g_pApp->m_pRenderer->addVertexFormat(vertexDesc, 2, m_PhysicDebugRenderShader);

	delete[] vertexDesc;

	return true;
}

void BulletDebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	// draw a line to represent the normal.  This only lasts one frame, and is hard to see.
	//   it might help to linger this drawn object onscreen for a while to make it more noticeable
	
	btVector3 const startPoint = PointOnB;
	btVector3 const endPoint = PointOnB + normalOnB * distance;
	
	drawLine( startPoint, endPoint, color );
}

void BulletDebugDrawer::reportErrorWarning(const char* warningString)
{
    CHG_WARNING(warningString);
}

void BulletDebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
	// FUTURE WORK - BulletDebugDrawer::draw3dText needs an implementation
}

void BulletDebugDrawer::setDebugMode(int debugMode)
{
    m_DebugModes = (DebugDrawModes) debugMode;
}

int BulletDebugDrawer::getDebugMode() const
{
    return m_DebugModes;
}

void BulletDebugDrawer::ReadOptions()
{
	TiXmlDocument *optionsDoc = g_pApp->m_Options.m_pDoc;
	TiXmlElement *pRoot = optionsDoc->RootElement();
    if (!pRoot)
		return;

	int debugModes = btIDebugDraw::DBG_NoDebug;
	TiXmlElement *pNode = pRoot->FirstChildElement("PhysicsDebug");
	if (pNode)
	{
		if (pNode->Attribute("DrawWireFrame"))
		{
	        std::string attribute(pNode->Attribute("DrawWireFrame"));
			if (attribute == "yes") debugModes |= btIDebugDraw::DBG_DrawWireframe;
		}

		if (pNode->Attribute("DrawAabb"))
		{
	        std::string attribute(pNode->Attribute("DrawAabb"));
			if (attribute == "yes") debugModes |= btIDebugDraw::DBG_DrawAabb;
		}

		if (pNode->Attribute("DrawFeaturesText"))
		{
	        std::string attribute(pNode->Attribute("DrawFeaturesText"));
			if (attribute == "yes") debugModes |= btIDebugDraw::DBG_DrawFeaturesText;
		}

		if (pNode->Attribute("DrawContactPoints"))
		{
	        std::string attribute(pNode->Attribute("DrawContactPoints"));
			if (attribute == "yes") debugModes |= btIDebugDraw::DBG_DrawContactPoints;
		}

		if (pNode->Attribute("NoDeactivation"))
		{
	        std::string attribute(pNode->Attribute("NoDeactivation"));
			if (attribute == "yes") debugModes |= btIDebugDraw::DBG_NoDeactivation;
		}

		if (pNode->Attribute("NoHelpText"))
		{
	        std::string attribute(pNode->Attribute("NoHelpText"));
			if (attribute == "yes") debugModes |= btIDebugDraw::DBG_NoHelpText;
		}

		if (pNode->Attribute("DrawText"))
		{
	        std::string attribute(pNode->Attribute("DrawText"));
			if (attribute == "yes") debugModes |= btIDebugDraw::DBG_DrawText;
		}

		if (pNode->Attribute("ProfileTimings"))
		{
	        std::string attribute(pNode->Attribute("ProfileTimings"));
			if (attribute == "yes") debugModes |= btIDebugDraw::DBG_ProfileTimings;
		}

		if (pNode->Attribute("EnableSatComparison"))
		{
	        std::string attribute(pNode->Attribute("EnableSatComparison"));
			if (attribute == "yes") debugModes |= btIDebugDraw::DBG_EnableSatComparison;
		}

		if (pNode->Attribute("DisableBulletLCP"))
		{
	        std::string attribute(pNode->Attribute("DisableBulletLCP"));
			if (attribute == "yes") debugModes |= btIDebugDraw::DBG_DisableBulletLCP;
		}

		if (pNode->Attribute("EnableCCD"))
		{
	        std::string attribute(pNode->Attribute("EnableCCD"));
			if (attribute == "yes") debugModes |= btIDebugDraw::DBG_EnableCCD;
		}

		if (pNode->Attribute("DrawConstraints"))
		{
	        std::string attribute(pNode->Attribute("DrawConstraints"));
			if (attribute == "yes") debugModes |= btIDebugDraw::DBG_DrawConstraints;
		}

		if (pNode->Attribute("DrawConstraintLimits"))
		{
	        std::string attribute(pNode->Attribute("DrawConstraintLimits"));
			if (attribute == "yes") debugModes |= btIDebugDraw::DBG_DrawConstraintLimits;
		}

		if (pNode->Attribute("FastWireframe"))
		{
	        std::string attribute(pNode->Attribute("FastWireframe"));
			if (attribute == "yes") debugModes |= btIDebugDraw::DBG_FastWireframe;
		}

		if (debugModes != btIDebugDraw::DBG_NoDebug)
		{
			setDebugMode(debugModes);
		}
	}

}

void BulletDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& lineColor)
{
    if (!m_pCameraNode)
    {
        std::shared_ptr<Scene> pScene = std::static_pointer_cast<HumanView>(g_pApp->m_pGame->VFindView(g_pApp->m_pGame->GetCurrentHumanView()))->m_pScene;

        if (pScene)
            m_pCameraNode = pScene->GetCamera();
    }    

	Vec3 vec3From, vec3To;
	vec3From.x = from.x();
	vec3From.y = from.y();
	vec3From.z = from.z();

	vec3To.x = to.x();
	vec3To.y = to.y();
	vec3To.z = to.z();

	Color color = Color(lineColor.x(), lineColor.y(), lineColor.z(), 1.0f);

    Vec3 vDir = vec3To - vec3From;
    float segmentLength = length(vDir);

    Ray ray;
    ray.Set(vec3From, vDir / segmentLength);

    if (m_pCameraNode->GetFrustum().Intersects(m_pCameraNode->ToWorld(), ray, segmentLength))
    {
        m_VertexPoints.push_back(vec3From);
        m_VertexPoints.push_back(vec3To);
        m_ColorPoints.push_back(color);
    }	
}

void BulletDebugDrawer::RenderDebugPhysic()
{
	std::shared_ptr<Scene> pScene = std::static_pointer_cast<HumanView>(g_pApp->m_pGame->VFindView(g_pApp->m_pGame->GetCurrentHumanView()))->m_pScene;

	if (!pScene)
		return;

	std::shared_ptr<CameraNode> pCameraNode = pScene->GetCamera();
	std::shared_ptr<Renderer> pRenderer = g_pApp->m_pRenderer;

    //Fill Buffers ////////////////////////////////////////////////////////////////
    Vec3* vertices = (Vec3*)pRenderer->mapVertexBuffer(m_LineVertexBuffer, MAP_WRITE_DISCARD);

    memcpy(vertices, m_VertexPoints.data(), m_VertexPoints.size() * sizeof(Vec3));

    pRenderer->unMapVertexBuffer(m_LineVertexBuffer);

    Vec4* colors = (Vec4*)pRenderer->mapVertexBuffer(m_LineColorBuffer, MAP_WRITE_DISCARD);

    memcpy(colors, m_ColorPoints.data(), m_ColorPoints.size() * sizeof(Vec4));

    pRenderer->unMapVertexBuffer(m_LineColorBuffer);
    ///////////////////////////////////////////////////////////////////////////////

	//Render Physic Shapes ////////////////////////////////////////////////////////
	VertexBufferID vertexBuffers[2] = { m_LineVertexBuffer, m_LineColorBuffer };
	UINT offset[2] = { 0, 0 };
	
	pRenderer->reset();
	pRenderer->setShader(m_PhysicDebugRenderShader);
	pRenderer->setShaderConstant4x4f("View", pCameraNode->GetView());
	pRenderer->setShaderConstant4x4f("Proj", pCameraNode->GetProjection());
	pRenderer->setBlendState(pScene->m_blendSrcAlpha);
	pRenderer->setDepthState(pScene->m_noDepthTest);
	pRenderer->setRasterizerState(pScene->m_noneFaceCulling);
	pRenderer->setVertexFormat(m_PhysicDebugRenderShaderVF);
	pRenderer->setVertexBuffer(0, 2, vertexBuffers, offset);
	pRenderer->apply();

	pRenderer->drawArrays(PRIM_LINES, 0, m_VertexPoints.size());

	m_ColorPoints.clear();
	m_VertexPoints.clear();
}



