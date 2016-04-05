#include "CthughaStd.h"

#include "..\CthughaEngine\CthughaApp.h"
#include "..\Entities\RenderComponent.h"
#include "Light.h"

////////////////////////////////////////////////////
// LightManager Implementation
////////////////////////////////////////////////////

LightManager::LightManager()
{ 
	m_NumActiveSpotPointLights = 0;
	m_NumActiveDirectionalLights = 0;

	m_uNumWidthTiles = (UINT)(g_pApp->GetScreenSize().x + COMPUTE_SHADER_TILE_GROUP_DIM - 1) / COMPUTE_SHADER_TILE_GROUP_DIM;
	m_uNumHeightTiles = (UINT)(g_pApp->GetScreenSize().y + COMPUTE_SHADER_TILE_GROUP_DIM - 1) / COMPUTE_SHADER_TILE_GROUP_DIM;

	m_sbSpotPointLightBuffer = g_pApp->m_pRenderer->createStructuredBuffer(sizeof(PointSpotLightShaderData), MAX_POINT_LIGHTS, true, false);
	m_sbTiledLightIndices = g_pApp->m_pRenderer->createStructuredBuffer(sizeof(UINT), (MAX_POINT_LIGHTS + MAX_DIRECTIONAL_LIGHTS)*m_uNumWidthTiles*m_uNumHeightTiles, false, true);
}

void LightManager::UpdateLightBuffers(Scene* pScene, std::list<std::shared_ptr<SceneNode>> LightList)
{
	shared_ptr<Renderer> pRenderer = g_pApp->m_pRenderer;

	m_NumActiveSpotPointLights = 0;
	m_NumActiveDirectionalLights = 0;

	Mat4x4 mTransposeView = transpose(pScene->GetCamera()->GetView());

	while (!LightList.empty())
	{
		std::shared_ptr<SceneNode> pSceneNode = LightList.front();

		std::shared_ptr<LightNode> pLightNode = std::static_pointer_cast<LightNode>(pSceneNode);
		std::shared_ptr<Light> pLight = pLightNode->GetLightData();

		if (pLight->getType() == DIRECTIONAL_LIGHT)
		{
			std::shared_ptr<DirectionalLight> pDirectionalLight = std::static_pointer_cast<DirectionalLight>(pLight);

			m_DirectionalLights[m_NumActiveDirectionalLights].fIntensitive = pDirectionalLight->getLightIntensitive();
			m_DirectionalLights[m_NumActiveDirectionalLights].vColor = pDirectionalLight->getLightColor().xyz();
			m_DirectionalLights[m_NumActiveDirectionalLights].iShadows = pDirectionalLight->getGenerateShadow();
			m_DirectionalLights[m_NumActiveDirectionalLights].vLightDirection = (mTransposeView * pDirectionalLight->getLightDirection()).xyz();
			m_DirectionalLights[m_NumActiveDirectionalLights].fMaxRadius = pDirectionalLight->GetMaxRadius();
			m_DirectionalLights[m_NumActiveDirectionalLights].fLength = pDirectionalLight->GetMaxLength();
			m_DirectionalLights[m_NumActiveDirectionalLights].vViewInitPosition = (mTransposeView * Vec4(pLightNode->ToWorld().GetPosition(), 1.0f)).xyz();

			m_NumActiveDirectionalLights++;
		}
		else if (pLight->getType() == POINT_LIGHT)
		{
			std::shared_ptr<PointLight> pPointLight = std::static_pointer_cast<PointLight>(pLight);

			m_SpotPointLights[m_NumActiveSpotPointLights].fIntensitive = pPointLight->getLightIntensitive();
			m_SpotPointLights[m_NumActiveSpotPointLights].fRadius = pPointLight->getLightRadius();
			m_SpotPointLights[m_NumActiveSpotPointLights].iShadows = pPointLight->getGenerateShadow();
			m_SpotPointLights[m_NumActiveSpotPointLights].iType = 1;
			m_SpotPointLights[m_NumActiveSpotPointLights].vColor = pPointLight->getLightColor().xyz();
			m_SpotPointLights[m_NumActiveSpotPointLights].vViewPosition = (mTransposeView * Vec4(pLightNode->ToWorld().GetPosition(), 1.0f)).xyz();

			m_NumActiveSpotPointLights++;
		}
		else if (pLight->getType() == SPOT_LIGHT)
		{
			std::shared_ptr<SpotLight> pSpotLight = std::static_pointer_cast<SpotLight>(pLight);

			m_SpotPointLights[m_NumActiveSpotPointLights].fIntensitive = pSpotLight->getLightIntensitive();
			m_SpotPointLights[m_NumActiveSpotPointLights].fRadius = pSpotLight->getLightRadius();
			m_SpotPointLights[m_NumActiveSpotPointLights].iShadows = pSpotLight->getGenerateShadow();
			m_SpotPointLights[m_NumActiveSpotPointLights].iType = 2;
			m_SpotPointLights[m_NumActiveSpotPointLights].vColor = pSpotLight->getLightColor().xyz();
			m_SpotPointLights[m_NumActiveSpotPointLights].vViewPosition = (mTransposeView * Vec4(pLightNode->ToWorld().GetPosition(), 1.0f)).xyz();
			m_SpotPointLights[m_NumActiveSpotPointLights].fPhi = pSpotLight->getPhiAngle();
			m_SpotPointLights[m_NumActiveSpotPointLights].fTheta = pSpotLight->getThetaAngle();
			m_SpotPointLights[m_NumActiveSpotPointLights].vLightDirection = (mTransposeView * pSpotLight->getLightDirection()).xyz();

			m_NumActiveSpotPointLights++;
		}

		LightList.pop_front();
	}

	PointSpotLightShaderData* pLights = (PointSpotLightShaderData*)pRenderer->mapBuffer(m_sbSpotPointLightBuffer, MAP_WRITE_DISCARD);

	for (UINT i = 0; i < (UINT)m_NumActiveSpotPointLights; i++)
		pLights[i] = m_SpotPointLights[i];

	pRenderer->unMapBuffer(m_sbSpotPointLightBuffer);
}

void LightManager::UpdateLightBuffers(Scene* pScene)
{
	shared_ptr<Renderer> pRenderer = g_pApp->m_pRenderer;

	m_NumActiveSpotPointLights = 0;
	m_NumActiveDirectionalLights = 0;

	Mat4x4 mTransposeView = transpose(pScene->GetCamera()->GetView());

	while (!pScene->GetLightQueue().empty())
	{
		std::shared_ptr<SceneNode> pSceneNode = pScene->GetLightQueue().front();

		std::shared_ptr<LightNode> pLightNode = std::static_pointer_cast<LightNode>(pSceneNode);
		std::shared_ptr<Light> pLight = pLightNode->GetLightData();

		if (pLight->getType() == DIRECTIONAL_LIGHT)
		{
			std::shared_ptr<DirectionalLight> pDirectionalLight = std::static_pointer_cast<DirectionalLight>(pLight);

			m_DirectionalLights[m_NumActiveDirectionalLights].fIntensitive = pDirectionalLight->getLightIntensitive();
			m_DirectionalLights[m_NumActiveDirectionalLights].vColor = pDirectionalLight->getLightColor().xyz();
			m_DirectionalLights[m_NumActiveDirectionalLights].iShadows = pDirectionalLight->getGenerateShadow();
			m_DirectionalLights[m_NumActiveDirectionalLights].vLightDirection = (mTransposeView * pDirectionalLight->getLightDirection()).xyz();
			m_DirectionalLights[m_NumActiveDirectionalLights].fMaxRadius = pDirectionalLight->GetMaxRadius();
			m_DirectionalLights[m_NumActiveDirectionalLights].fLength = pDirectionalLight->GetMaxLength();
			m_DirectionalLights[m_NumActiveDirectionalLights].vViewInitPosition = (mTransposeView * Vec4(pLightNode->ToWorld().GetPosition(), 1.0f)).xyz();

			m_NumActiveDirectionalLights++;
		}
		else if (pLight->getType() == POINT_LIGHT)
		{
			std::shared_ptr<PointLight> pPointLight = std::static_pointer_cast<PointLight>(pLight);

			m_SpotPointLights[m_NumActiveSpotPointLights].fIntensitive = pPointLight->getLightIntensitive();
			m_SpotPointLights[m_NumActiveSpotPointLights].fRadius = pPointLight->getLightRadius();
			m_SpotPointLights[m_NumActiveSpotPointLights].iShadows = pPointLight->getGenerateShadow();
			m_SpotPointLights[m_NumActiveSpotPointLights].iType = 1;
			m_SpotPointLights[m_NumActiveSpotPointLights].vColor = pPointLight->getLightColor().xyz();
			m_SpotPointLights[m_NumActiveSpotPointLights].vViewPosition = (mTransposeView * Vec4(pLightNode->ToWorld().GetPosition(), 1.0f)).xyz();

			m_NumActiveSpotPointLights++;
		}
		else if (pLight->getType() == SPOT_LIGHT)
		{
			std::shared_ptr<SpotLight> pSpotLight = std::static_pointer_cast<SpotLight>(pLight);

			m_SpotPointLights[m_NumActiveSpotPointLights].fIntensitive = pSpotLight->getLightIntensitive();
			m_SpotPointLights[m_NumActiveSpotPointLights].fRadius = pSpotLight->getLightRadius();
			m_SpotPointLights[m_NumActiveSpotPointLights].iShadows = pSpotLight->getGenerateShadow();
			m_SpotPointLights[m_NumActiveSpotPointLights].iType = 2;
			m_SpotPointLights[m_NumActiveSpotPointLights].vColor = pSpotLight->getLightColor().xyz();
			m_SpotPointLights[m_NumActiveSpotPointLights].vViewPosition = (mTransposeView * Vec4(pLightNode->ToWorld().GetPosition(), 1.0f)).xyz();
			m_SpotPointLights[m_NumActiveSpotPointLights].fPhi = pSpotLight->getPhiAngle();
			m_SpotPointLights[m_NumActiveSpotPointLights].fTheta = pSpotLight->getThetaAngle();
			m_SpotPointLights[m_NumActiveSpotPointLights].vLightDirection = (mTransposeView * pSpotLight->getLightDirection()).xyz();

			m_NumActiveSpotPointLights++;
		}

		pScene->RemoveFrontLightNodeToLightQueue();
	}

	PointSpotLightShaderData* pLights = (PointSpotLightShaderData*)pRenderer->mapBuffer(m_sbSpotPointLightBuffer, MAP_WRITE_DISCARD);

	for (UINT i = 0; i < (UINT)m_NumActiveSpotPointLights; i++)
		pLights[i] = m_SpotPointLights[i];

	pRenderer->unMapBuffer(m_sbSpotPointLightBuffer);
}

void LightManager::SetLightBuffers(Scene* pScene)
{
	shared_ptr<Renderer> pRenderer = g_pApp->m_pRenderer;

	pRenderer->setStructuredBuffer("sbPointLights", m_sbSpotPointLightBuffer);
	pRenderer->setShaderConstantArrayStruct("cbDirectionalLights", m_DirectionalLights, sizeof(DirectionalLightShaderData), m_NumActiveDirectionalLights);
	pRenderer->setStructuredBuffer("sbLightIndices", m_sbTiledLightIndices);

	// Lights Count
	pRenderer->setShaderConstant1i("iNumDirectionalLights", m_NumActiveDirectionalLights);
	pRenderer->setShaderConstant1i("iNumPointLights", m_NumActiveSpotPointLights);
}

////////////////////////////////////////////////////
// LightNode Implementation
////////////////////////////////////////////////////

LightNode::LightNode(const EntityId entityId, 
		WeakBaseRenderComponentPtr renderComponent,
		shared_ptr<Light> light, const Mat4x4 *world)
{
	m_pLight = light;
	m_ToWorld = *world;

	//Calculate Aabb
	if(m_pLight->getType() == DIRECTIONAL_LIGHT)
	{
		shared_ptr<DirectionalLight> directionalLight = static_pointer_cast<DirectionalLight>(m_pLight);
		Vec3 LightPos = m_ToWorld.GetPosition();
		float fMaxRadius = directionalLight->GetMaxRadius();
		float fMaxLength = directionalLight->GetMaxLength();

		if (directionalLight->GetMaxRadius() > 0.0f)
		{
			m_Aabb.vcMin = Vec3(-fMaxRadius, -fMaxLength, -fMaxRadius);
			m_Aabb.vcMax = Vec3(fMaxRadius, 0.0f, fMaxRadius);
			m_Aabb.isInfinite = false;
		}
		else
		{
			m_Aabb.isInfinite = true;
		}

		m_ToWorld = m_ToWorld * directionalLight->GetLightDirectionMatrix();

		/*Old Shadow Map
		float diagonalLength = distance(m_Aabb.vcMax, m_Aabb.vcMin);

		m_pLight->m_LightFrustum.Init(LightVolume.x, LightVolume.y, 1.0f, -LightVolume.z, LightVolume.z);
		m_pLight->m_ViewToProjLightMatrix = orthoMatrixX(LightVolume.x, LightVolume.y, -LightVolume.z, LightVolume.z);

		Vec3 look = directionalLight->getLightDirection().xyz();
		Vec3 right = normalize((abs(dot(Vec3(0.0f, 1.0f, 0.0f), look)) != 1.0f) ? normalize(cross(look, Vec3(0.0f, 1.0f, 0.0f))) : normalize(cross(look, Vec3(1.0f, 0.0f, 0.0f))));
		right = normalize(cross(right, look));
		Vec3 up = cross(look, right);

		directionalLight->m_ViewLightMat = Mat4x4(right.x,									      up.x,									         look.x,								         0.0f,
												  right.y,									      up.y,									         look.y,								         0.0f,
												  right.z,									      up.z,									         look.z,								         0.0f,
												  -dot(LightPos, right),                          -dot(LightPos, up),                            -dot(LightPos, look),                            1.0f);

		//directionalLight->m_ViewLightMat = viewMatrix(LightPos, look, right);

		directionalLight->m_NumFrustumPartition = 4;
		directionalLight->m_FrustumPartitionBuffer = g_pApp->m_pRenderer->createObjectBuffer(SHADER_STORAGE_BUFFER, sizeof(FrustumPartition), directionalLight->m_NumFrustumPartition, STATIC, NULL); */
	}
	else if(m_pLight->getType() == POINT_LIGHT)
	{
		shared_ptr<PointLight> pointLight = static_pointer_cast<PointLight>(m_pLight);
		
        m_Aabb.vcMin = Vec3(-pointLight->m_LightRadius, -pointLight->m_LightRadius, -pointLight->m_LightRadius);
        m_Aabb.vcMax = Vec3(pointLight->m_LightRadius, pointLight->m_LightRadius, pointLight->m_LightRadius);
		m_Aabb.vcCenter = (m_Aabb.vcMin + m_Aabb.vcMax)*0.5f;

		m_pLight->m_LightFrustum.Init(0.5f*CHG_PI, 1.0f, 0.01f, (m_Aabb.vcMax.x - m_Aabb.vcMin.x)*0.5f);
        m_pLight->m_ViewToProjLightMatrix = perspectiveMatrix(0.5f*CHG_PI, 1.0f, 0.01f, (m_Aabb.vcMax.x - m_Aabb.vcMin.x)*0.5f);
	}
	else if(m_pLight->getType() == SPOT_LIGHT)
	{
        shared_ptr<SpotLight> spotLight = static_pointer_cast<SpotLight>(m_pLight);

        m_Aabb.vcMin = Vec3(-spotLight->m_LightRadius, -spotLight->m_LightRadius, -spotLight->m_LightRadius);
        m_Aabb.vcMax = Vec3(spotLight->m_LightRadius, spotLight->m_LightRadius, spotLight->m_LightRadius);
		m_Aabb.vcCenter = (m_Aabb.vcMin + m_Aabb.vcMax)*0.5f;

		m_pLight->m_LightFrustum.Init(spotLight->getPhiAngle(), 1.0f, 0.01f, (m_Aabb.vcMax.x - m_Aabb.vcMin.x)*0.5f);
		m_pLight->m_ViewToProjLightMatrix = perspectiveMatrix(spotLight->getPhiAngle(), 1.0f, 0.01f, (m_Aabb.vcMax.x - m_Aabb.vcMin.x)*0.5f);

        Vec3 LightPos = world->GetPosition();
		Vec3 look = spotLight->getLightDirection().xyz();
		Vec3 right = normalize((abs(dot(Vec3(0.0f, 1.0f, 0.0f), look)) != 1.0f) ? normalize(cross(look, Vec3(0.0f, 1.0f, 0.0f))) : normalize(cross(look, Vec3(1.0f, 0.0f, 0.0f))));
		right = normalize(cross(right, look));
		Vec3 up = cross(look, right);

		spotLight->m_ViewLightMat = Mat4x4(right.x, up.x, look.x, 0.0f,
			right.y, up.y, look.y, 0.0f,
			right.z, up.z, look.z, 0.0f,
			-dot(LightPos, right), -dot(LightPos, up), -dot(LightPos, look), 1.0f);

		//directionalLight->m_ViewLightMat = viewMatrix(LightPos, look, right);
	}

	m_EntityID = entityId;
	m_IsVisible = true;
	m_pRenderComponent = renderComponent;
	
	m_InQueueFrustum = false;
	m_InQueueLight = false;
	m_InQueueRender = false;

	m_FromWorld = m_ToWorld.inverse();

	m_bIsDrawable = false;
}

LightNode::~LightNode()
{
}

//
// MeshNode::VOnRestore
//
bool LightNode::VOnRestore(Scene *pScene)
{
	SceneNode::VOnRestore(pScene);

	return true;
}

//
// MeshNode::VRender
//
bool LightNode::VOnRender(Scene *pScene)
{
	return true;
}

//
// MeshNode::ActiveLight
//
bool LightNode::ActiveLight()
{
	shared_ptr<Renderer> pRenderer = g_pApp->m_pRenderer;

	//General properties
	pRenderer->setShaderConstant1f("lightIntensitive", m_pLight->getLightIntensitive());
	pRenderer->setShaderConstant4f("lightColor", m_pLight->getLightColor());
	pRenderer->setShaderConstant1i("lightType", m_pLight->getType());

	if(m_pLight->getType() == DIRECTIONAL_LIGHT)
	{
		shared_ptr<DirectionalLight> directionalLight = static_pointer_cast<DirectionalLight>(m_pLight);

		Vec4 worldDirection = m_ToWorld * directionalLight->getLightDirection();

		//Directional Light properties
		pRenderer->setShaderConstant4f("lightDirection", worldDirection);
		pRenderer->setShaderConstant4x4f("lightViewMat", directionalLight->getViewLightMatrix());
	}
	else if(m_pLight->getType() == POINT_LIGHT)
	{
		shared_ptr<PointLight> pointLight = static_pointer_cast<PointLight>(m_pLight);

		Vec4 worldPosition = m_ToWorld * pointLight->getLightPosition();

		//Point Light properties
		pRenderer->setShaderConstant4f("positionLight", worldPosition);
		pRenderer->setShaderConstant1f("lightRadius", pointLight->getLightRadius());
	}
	else if(m_pLight->getType() == SPOT_LIGHT)
	{
		shared_ptr<SpotLight> spotLight = static_pointer_cast<SpotLight>(m_pLight);

		Vec4 worldDirection = m_ToWorld * spotLight->getLightDirection();
		Vec4 worldPosition = m_ToWorld * spotLight->getLightPosition();

		//Spot Light properties
		pRenderer->setShaderConstant4f("positionLight", worldPosition);
        pRenderer->setShaderConstant1f("lightRadius", spotLight->getLightRadius());
		pRenderer->setShaderConstant4f("lightDirection", worldDirection);
		pRenderer->setShaderConstant1f("thetaAngle", spotLight->getThetaAngle());
		pRenderer->setShaderConstant1f("phiAngle", spotLight->getPhiAngle());
	}

	return true;
}