#include "CthughaStd.h"
#include "Light.h"
#include "Mesh.h"

#include "ShadowMap.h"

////////////////////////////////////////////////////
// AtlasTreeNode Implementation
////////////////////////////////////////////////////

AtlasTreeNode::AtlasTreeNode(int maxSize, int minSize, stackBuffer* data)
{
	m_pTreeData = data;

	m_MaxSize = maxSize;
	m_MinSize = minSize;

	//memset(m_pTreeData, 0, m_DataSize);
}

AtlasTreeNode::~AtlasTreeNode()
{
	//memset(m_pTreeData, 0, m_DataSize);
}

void AtlasTreeNode::initChilds()
{
	for (int i = 0; i<4; i++)
	{
		m_vpChildren[i] = (AtlasTreeNode*)(m_pTreeData->data + m_pTreeData->top);
		m_pTreeData->top += sizeof(AtlasTreeNode);

		m_vpChildren[i]->m_Empty = true;
		m_vpChildren[i]->m_Full = false;
		m_vpChildren[i]->m_Size = m_Size / 2;
		m_vpChildren[i]->m_level = m_level + 1;
		m_vpChildren[i]->m_pRoot = m_pRoot;
		m_vpChildren[i]->m_pParent = this;
		m_vpChildren[i]->m_MaxSize = m_MaxSize;
		m_vpChildren[i]->m_MinSize = m_MinSize;
		m_vpChildren[i]->m_NumFrees = 4;
		m_vpChildren[i]->m_pTreeData = m_pTreeData;

		memset(&m_vpChildren[i]->m_vpChildren[0], 0, 4 * sizeof(AtlasTreeNode*));

		switch (i)
		{
		case 0:
			m_vpChildren[i]->m_AtlasPos = m_AtlasPos;
			break;
		case 1:
			m_vpChildren[i]->m_AtlasPos.y = m_AtlasPos.y;
			m_vpChildren[i]->m_AtlasPos.x = m_AtlasPos.x + m_vpChildren[i]->m_Size;
			break;
		case 2:
			m_vpChildren[i]->m_AtlasPos.x = m_AtlasPos.x;
			m_vpChildren[i]->m_AtlasPos.y = m_AtlasPos.y + m_vpChildren[i]->m_Size;
			break;
		case 3:
			m_vpChildren[i]->m_AtlasPos.y = m_AtlasPos.y + m_vpChildren[i]->m_Size;
			m_vpChildren[i]->m_AtlasPos.x = m_AtlasPos.x + m_vpChildren[i]->m_Size;
			break;

		default: break;
		}
	}
}

void AtlasTreeNode::Reset()
{
	memset(m_pRoot, 0, sizeof(AtlasTreeNode));
}

bool AtlasTreeNode::addTextureToAtlas(int tam, Vec2 &pos)
{
	if (m_Full)
	{
		return false;
	}

	if (tam == m_Size)
	{
		if (m_Empty)
		{
			m_Empty = false;
			m_Full = true;
			m_NumFrees = 0;

			pos = m_AtlasPos;

			if (m_pParent != NULL)
			{
				m_pParent->m_NumFrees--;
				m_pParent->m_Empty = false;
			}

			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (m_vpChildren[0] == NULL)
			initChilds();

		for (int i = 0; i<4; i++)
		{
			if (m_vpChildren[i]->addTextureToAtlas(tam, pos))
			{
				if (m_NumFrees == 0)
				{
					m_Full = true;
					m_pParent->m_NumFrees--;
				}

				if (m_pParent != NULL)
				{
					m_pParent->m_Empty = false;
				}

				return true;
			}
		}
	}

	return false;
}


////////////////////////////////////////////////////
// ShadowMapManager Implementation
////////////////////////////////////////////////////

ShadowMapManager::ShadowMapManager(int maxLevel, int minLevel, int directionalAtlasRes)
{
	m_MaxLevel = maxLevel;
	m_MinLevel = minLevel;

	m_DirectionalAtlasRes = directionalAtlasRes;

	m_InverseMaxLevel = 1.0f / (float)m_MaxLevel;

	m_pAtlasTreeData = CHG_NEW stackBuffer;

	m_pAtlasTreeData->data = CHG_NEW char[1000 * sizeof(AtlasTreeNode)];
	m_pAtlasTreeData->size = 1000 * sizeof(AtlasTreeNode);
	m_pAtlasTreeData->top = 0;

	m_pRoot = (AtlasTreeNode*)m_pAtlasTreeData->data;
	m_pAtlasTreeData->top += sizeof(AtlasTreeNode);
	m_pRoot->m_pTreeData = m_pAtlasTreeData;

	m_pRoot->m_Empty = true;
	m_pRoot->m_Full = false;
	m_pRoot->m_Size = maxLevel;
	m_pRoot->m_level = 0;
	m_pRoot->m_pRoot = m_pRoot;
	m_pRoot->m_pParent = NULL;
	m_pRoot->m_MaxSize = maxLevel;
	m_pRoot->m_MinSize = minLevel;
	m_pRoot->m_NumFrees = 4;
	m_pRoot->m_AtlasPos.x = 0.0f;
	m_pRoot->m_AtlasPos.y = 0.0f;

	memset(&m_pRoot->m_vpChildren[0], 0, 4 * sizeof(AtlasTreeNode*));

	m_UsedMemory = 0;
	m_TotalMemory = (maxLevel*maxLevel * 2) / 1024;

	m_DirectionalAtlasID = TEXTURE_NONE;
	m_OmniDirectionalAtlasID = g_pApp->m_pRenderer->addRenderDepth(maxLevel, maxLevel, 1, FORMAT_D16, 1, SS_NONE, SAMPLE_DEPTH);
	//m_DirectionalAtlasID = g_pApp->m_pRenderer->addRenderTarget(m_DirectionalAtlasRes, m_DirectionalAtlasRes, 1, 1, 1, FORMAT_D16, 1, SS_NONE, SAMPLE_DEPTH);

	//Test Initialization//////////////////////////////////////////////////////////////////////////
	m_ShadowTestCoverageRT = g_pApp->m_pRenderer->addRenderTarget((int)g_pApp->GetScreenSize().x / 4, (int)g_pApp->GetScreenSize().y / 4, 1, 1, 1, FORMAT_R8, 1, SS_NONE);

	Resource ShadowProjectionCoverageShaderResource("Shaders\\ShadowProjectionCoverageShader.shd");
	shared_ptr<ResHandle> ShadowProjectionCoverageShaderHandle = g_pApp->m_ResCache->GetHandle(&ShadowProjectionCoverageShaderResource);

	if (ShadowProjectionCoverageShaderHandle.get() == NULL)
	{
		CHG_ERROR("Not found ShadowProjectionCoverage Shader");
	}

	m_ShadowTestCoverageShaderID = ((ShaderResourceExtraData*)ShadowProjectionCoverageShaderHandle->GetExtra().get())->m_ShaderID;
	///////////////////////////////////////////////////////////////////////////////////////////////

	//QueryQueue Init
	m_pQueryQueue.reset(new QueryQueue(40));

	//Shadow Shader load
	Resource resource("Shaders\\ShadowMapShader.shd");
	shared_ptr<ResHandle> handle = g_pApp->m_ResCache->GetHandle(&resource);

	if (handle.get() == NULL)
	{
		CHG_ERROR("Not found ShadowMap Shader");
	}

	m_ShadowShaderID = ((ShaderResourceExtraData*)handle->GetExtra().get())->m_ShaderID;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Load Frustum Partition Shaders

	/*Resource resourceClearZBoundsShader("Shaders\\ClearZBoundsComputerShader.shd");
	shared_ptr<ResHandle> resourceClearZBoundsShaderHandle = g_pApp->m_ResCache->GetHandle(&resourceClearZBoundsShader);

	if (resourceClearZBoundsShaderHandle.get() == NULL)
	{
		CHG_ERROR("Not found ClearZBounds Shader");
	}

	m_ClearZBoundsShaderID = ((GLSLShaderResourceExtraData*)resourceClearZBoundsShaderHandle->GetExtra().get())->m_ShaderID;

	//-------------------------------------------------------------------------------------------------------------------

	Resource resourceReduceZBoundsShader("Shaders\\ReduceZBoundsComputerShader.shd");
	shared_ptr<ResHandle> resourceReduceZBoundsShaderHandle = g_pApp->m_ResCache->GetHandle(&resourceReduceZBoundsShader);

	if (resourceReduceZBoundsShaderHandle.get() == NULL)
	{
		CHG_ERROR("Not found ClearZBounds Shader");
	}

	m_ReduceZBoundsShaderID = ((GLSLShaderResourceExtraData*)resourceReduceZBoundsShaderHandle->GetExtra().get())->m_ShaderID;

	//--------------------------------------------------------------------------------------------------------------------

	Resource resourceLogPartitionShader("Shaders\\LogarithmicZPartitionComputeShader.shd");
	shared_ptr<ResHandle> resourceLogPartitionShaderHandle = g_pApp->m_ResCache->GetHandle(&resourceLogPartitionShader);

	if (resourceLogPartitionShaderHandle.get() == NULL)
	{
		CHG_ERROR("Not found ClearZBounds Shader");
	}

	m_LogPartitionShaderID = ((GLSLShaderResourceExtraData*)resourceLogPartitionShaderHandle->GetExtra().get())->m_ShaderID;*/

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Shadow Vertex Format
    FormatDesc *vertexDesc = CHG_NEW FormatDesc[1];

	vertexDesc[0].stream = 0;
	vertexDesc[0].format = FORMAT_FLOAT;
	vertexDesc[0].type = TYPE_VERTEX;
	vertexDesc[0].size = 3;

	if (m_ShadowShaderID != SHADER_NONE)
		m_ShadowVertexFormatID = g_pApp->m_pRenderer->addVertexFormat(vertexDesc, 1, m_ShadowShaderID);

	if (m_ShadowShaderID != SHADER_NONE)
		m_ShadowTestCoverageVFID = g_pApp->m_pRenderer->addVertexFormat(vertexDesc, 1, m_ShadowTestCoverageShaderID);

    delete[] vertexDesc;
}

ShadowMapManager::~ShadowMapManager()
{
	SAFE_DELETE(m_pAtlasTreeData->data);
	SAFE_DELETE(m_pAtlasTreeData);
}

void ShadowMapManager::reset()
{
	memset(&m_pRoot->m_vpChildren[0], 0, 4 * sizeof(AtlasTreeNode*));

	m_pRoot->m_Empty = true;
	m_pRoot->m_Full = false;
	m_pRoot->m_NumFrees = 4;

	m_pAtlasTreeData->top = sizeof(AtlasTreeNode);

	m_UsedMemory = 0;
}

bool ShadowMapManager::UpdateShadows(Scene *pScene)
{
	reset();

	for (auto lightNode : pScene->m_LightQueue)
	{
		shared_ptr<LightNode> light = static_pointer_cast<LightNode>(lightNode);

		if (light->m_pLight->getType() == POINT_LIGHT)
		{
			if (light->m_pLight->getGenerateShadow())
			{
				//Set target for shadow
				g_pApp->m_pRenderer->changeRenderTargets(NULL, 0, m_OmniDirectionalAtlasID);

				shared_ptr<PointLight> pointLight = static_pointer_cast<PointLight>(light->m_pLight);
				pointLight->m_ShadowAtlas = m_OmniDirectionalAtlasID;

				Frustum viewFrustum = pScene->GetCamera()->GetFrustum();
				Mat4x4 cameraMat = pScene->GetCamera()->ToWorld();
				Vec3 lightPos = light->ToWorld().GetPosition();
				bool intersectProjection[6] = { true, true, true, true, true, true };

				//Frustum culling to point light frustum projection
				if (!viewFrustum.Inside(lightPos, cameraMat))
				{
					for (int j = 0; j<6; j++)
					{
						Mat4x4 cubeView = cubeViewMatrix(j);
						cubeView.SetPosition(lightPos);

						if (!viewFrustum.Inside(cameraMat, light->m_pLight->getLightFrustum(), cubeView))
							intersectProjection[j] = false;

						printf("%s, ", intersectProjection[j] ? "true" : "false");
					}

					printf("\n");
				}

				for (int j = 0; j<6; j++)
				{
					if (intersectProjection[j])
					{
						//Obtain object in Projector
						Mat4x4 cubeView = cubeViewMatrix(j);
						cubeView.SetPosition(lightPos);

						//Obtain max edge in light aabb for texture resolution
						Aabb lightAabb = light->getNodeAabb();

						/*Vec3 aabbEdges[3];
						aabbEdges[0] = lightAabb.vcMin - Vec3(lightAabb.vcMax.x, lightAabb.vcMin.y, lightAabb.vcMin.z);
						aabbEdges[1] = lightAabb.vcMin - Vec3(lightAabb.vcMin.x, lightAabb.vcMin.y, lightAabb.vcMax.z);
						aabbEdges[2] = lightAabb.vcMin - Vec3(lightAabb.vcMin.x, lightAabb.vcMax.y, lightAabb.vcMin.z);

						float maxLength = std::max(squaredLength(aabbEdges[0]) ,std::max(squaredLength(aabbEdges[1]), squaredLength(aabbEdges[2])));
						maxLength = sqrt(maxLength);*/
						float maxLength = 1024;

						int shadowMapSize = min(pow2roundup((int)maxLength), 1024);	//1024 is the maximum of shadow texture

						/*if(!m_pRoot->addTextureToAtlas(shadowMapSize, pointLight->m_ShadowMapPos[j]))
						{
						CHG_ERROR("not enough memory in atlas");
						}*/

						g_pApp->m_pRenderer->setViewport(shadowMapSize, shadowMapSize);

						std::vector<std::shared_ptr<SceneNode>> objectInProjector;

						pScene->m_Root->TraverseOctreeFromSpecificFrustum(objectInProjector, pointLight->getLightFrustum(), cubeView, pScene);
						pScene->TraverseDynamicObjectFromSpecificFrustum(objectInProjector, pointLight->getLightFrustum(), cubeView);

						while (!objectInProjector.empty())
						{
							Aabb objectAabb = objectInProjector.back()->getNodeAabb();
							std::shared_ptr<MeshNode> meshNode = static_pointer_cast<MeshNode>(objectInProjector.back());

							if (meshNode->getCastShadows())
							{
								//Frustum Object
								float radioConeBase = length(objectAabb.vcMin - objectAabb.vcMax)*0.5f;
								float lightToObjectDistance = length(pointLight->m_LightPosition.xyz() - meshNode->FromWorld().GetPosition());
								float frustumAngle = atan(radioConeBase / lightToObjectDistance);

								Frustum objectFrustum;
								objectFrustum.Init(frustumAngle, 1.0f, lightToObjectDistance - radioConeBase, pointLight->getLightFrustum().m_Far);

								Vec3 look = normalize(pointLight->m_LightPosition.xyz() - meshNode->FromWorld().GetPosition());
								Vec3 right = cross(Vec3(0.0f, 1.0f, 0.0f), look);
								Vec3 up = cross(right, look);

								Mat4x4 objectFrustumMat = Mat4x4(right.x, up.x, look.x, pointLight->m_LightPosition.x,
									right.y, up.y, look.y, pointLight->m_LightPosition.y,
									right.z, up.z, look.z, pointLight->m_LightPosition.z,
									0.0f, 0.0f, 0.0f, 1.0f);

								if (viewFrustum.Inside(cameraMat, objectFrustum, objectFrustumMat))
								{
									float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
									g_pApp->m_pRenderer->clear(false, true, false, ClearColor, 1.0f);

									//Draw Object
									g_pApp->m_pRenderer->reset();
									//m_pRenderer->setRasterizerState(backFaceCulling);
									g_pApp->m_pRenderer->setShader(m_ShadowShaderID);
									g_pApp->m_pRenderer->setVertexFormat(m_ShadowVertexFormatID);

									g_pApp->m_pRenderer->setShaderConstant4x4f("LightViewMatrix", *((Mat4x4*)&cubeView));
									g_pApp->m_pRenderer->setShaderConstant4x4f("LightProjMatrix", *((Mat4x4*)&pointLight->getViewToProjLightMatrix()));

									//meshNode->OnSimpleRender(pScene);
								}
							}

							objectInProjector.back()->SetInFrustumQueue(false);
							objectInProjector.pop_back();

						}
					}
				}
			}
		}
	}

	return true;
}

bool ShadowMapManager::UpdateShadowsV_2(Scene *pScene)
{
	shared_ptr<Renderer> pRenderer = g_pApp->m_pRenderer;

	//Reset tree texture manager and clear depthTexture
	reset();

	pRenderer->changeRenderTarget(TEXTURE_NONE, m_OmniDirectionalAtlasID);
	pRenderer->clear(false, true, false, Color(0.0f, 0.0f, 0.0f, 0.0f), 1.0f);

	//Get previous frame result Queries////////////////////////////////
	//m_pQueryQueue->WaitQueries();
	///////////////////////////////////////////////////////////////////

	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	UINT offset[] = { 0 };

	for (auto lightNode : pScene->m_LightQueue)
	{
		//printf("Numero Luces: %d\n", pScene->m_LightQueue.size());

		pRenderer->setViewport((int)(g_pApp->GetScreenSize().x*0.25f), (int)(g_pApp->GetScreenSize().y*0.25f));
		pRenderer->changeRenderTarget(m_ShadowTestCoverageRT, TEXTURE_NONE, -1, VIEWPORT_SIZE);

		shared_ptr<LightNode> light = static_pointer_cast<LightNode>(lightNode);

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		//	DIRECTIONAL LIGHT
		//
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (light->m_pLight->getType() == DIRECTIONAL_LIGHT)
		{
			shared_ptr<DirectionalLight> directionalLight = static_pointer_cast<DirectionalLight>(light->m_pLight);
			directionalLight->m_ShadowAtlas = m_DirectionalAtlasID;

			Mat4x4 lightView = directionalLight->getViewLightMatrix();

			if (light->m_pLight->getGenerateShadow())
			{
				Frustum viewFrustum = pScene->GetCamera()->GetFrustum();
				Mat4x4 cameraMat = pScene->GetCamera()->ToWorld();
				Vec3 lightPos = (true) ? pScene->GetCamera()->ToWorld().GetPosition() : light->ToWorld().GetPosition();
				Vec3 lightDirection = directionalLight->getLightDirection().xyz();

				lightView.rows[3].data[0] = -dot(lightPos, Vec3(lightView.rows[0].x, lightView.rows[1].x, lightView.rows[2].x));
				lightView.rows[3].data[1] = -dot(lightPos, Vec3(lightView.rows[0].y, lightView.rows[1].y, lightView.rows[2].y));
				lightView.rows[3].data[2] = -dot(lightPos, Vec3(lightView.rows[0].z, lightView.rows[1].z, lightView.rows[2].z));

				//Calculate Pixel Extension
				unsigned long long lightProjectResult = directionalLight->m_QueryResult;

				/*pRenderer->clear(true, true, false, ClearColor, 1.0f);

				pRenderer->reset();

				pRenderer->setShader(m_ShadowTestCoverageShaderID);
				pRenderer->setVertexFormat(m_ShadowTestCoverageVFID);
				pRenderer->setVertexBuffer(0, 1, &pScene->m_ScreenAlignedQuadVBID, offset);

				pRenderer->setDepthState(pScene->m_noDepthTest);
				pRenderer->setRasterizerState(pScene->m_noneFaceCulling);

				pRenderer->setTexture("depthTarget", pScene->m_PositionRT);
				pRenderer->setSamplerState("targetSampler", pScene->m_pointClamp);

				pRenderer->setShaderConstant4x4f("projectorView", *((Mat4x4*)&lightView));
				pRenderer->setShaderConstant4x4f("projectorProj", *((Mat4x4*)&directionalLight->getViewToProjLightMatrix()));
				pRenderer->setShaderConstant4x4f("View", *((Mat4x4*)&pScene->m_Camera->GetView()));
				pRenderer->setShaderConstant4x4f("Projection", *((Mat4x4*)&pScene->m_Camera->GetProjection()));
				pRenderer->setShaderConstant4x4f("InverseView", *((Mat4x4*)&(!pScene->m_Camera->GetView())));
				pRenderer->setShaderConstant4x4f("InverseProjection", *((Mat4x4*)&(!pScene->m_Camera->GetProjection())));
				pRenderer->setShaderConstant4f("cameraPosition", Vec4(pScene->m_Camera->ToWorld().GetPosition(), 1.0f));

				pRenderer->apply();

				int idQuery = m_pQueryQueue->getQuery(&directionalLight->m_QueryResult);

				m_pQueryQueue->BeginQuery(idQuery);

				pRenderer->drawArrays(PRIM_TRIANGLES, 0, 6);

				m_pQueryQueue->EndQuery(idQuery);*/

				UINT count = 0;

				if (lightProjectResult >= 0)
				{
					//Calculate shadowMapResolution
					int shadowMapRes = pow2roundup((int)sqrt((float)(16.0f*lightProjectResult)));
					shadowMapRes = /*shadowMapRes <= 1024 ? shadowMapRes :*/ 1024;
					//shadowMapRes = 512;

					directionalLight->m_ShadowMapRes = (float)shadowMapRes;
					if (!m_pRoot->addTextureToAtlas(shadowMapRes, directionalLight->m_ShadowMapPos))
					{
						CHG_ERROR("not enough memory in atlas");
					}

					directionalLight->m_ShadowMapPos = directionalLight->m_ShadowMapPos*m_InverseMaxLevel;

					Vec3 look = lightDirection;
					Vec3 right = (abs(dot(Vec3(0.0f, 1.0f, 0.0f), look)) != 1.0f) ? normalize(cross(Vec3(0.0f, 1.0f, 0.0f), look)) : normalize(cross(Vec3(1.0f, 0.0f, 0.0f), look));
					Vec3 up = normalize(cross(right, look));

					Mat4x4 frustumView = Mat4x4(right.x, up.x, -look.x, lightPos.x,
						right.y, up.y, -look.y, lightPos.y,
						right.z, up.z, -look.z, lightPos.z,
						0.0f, 0.0f, 0.0f, 1.0f);

					std::vector<std::shared_ptr<SceneNode>> objectInProjector;

					pScene->m_Root->TraverseOctreeFromSpecificFrustum(objectInProjector, directionalLight->getLightFrustum(), frustumView, pScene);
					pScene->TraverseDynamicObjectFromSpecificFrustum(objectInProjector, directionalLight->getLightFrustum(), frustumView);

					UINT num_Test = 0;

					//pRenderer->clear(true, true, false, ClearColor, 1.0f);

					for (UINT k = 0; k < objectInProjector.size(); k++)
					{
						//pRenderer->clear(true, true, false, ClearColor, 1.0f);

						Aabb objectAabb = objectInProjector[k]->getNodeAabb().Transform(objectInProjector[k]->ToWorld());
						std::shared_ptr<MeshNode> meshNode = static_pointer_cast<MeshNode>(objectInProjector[k]);

						if (meshNode->getCastShadows())
						{
							//Frustum Object////////////////////////////////////////////////////////////////////////
							float diameterConeBase = length(objectAabb.vcMin - objectAabb.vcMax);
							float radioConeBase = diameterConeBase*0.5f;

							//Mat4x4 objectFrustumProj = orthoMatrixX(diameterConeBase, diameterConeBase, -radioConeBase, directionalLight->getLightFrustum().m_Far);

							Vec3 look = lightDirection;
							Vec3 right = (abs(dot(Vec3(0.0f, 1.0f, 0.0f), look)) != 1.0f) ? normalize(cross(Vec3(0.0f, 1.0f, 0.0f), look)) : normalize(cross(Vec3(1.0f, 0.0f, 0.0f), look));
							Vec3 up = normalize(cross(right, look));

							//Obtain objectFrustumAabb
							float objectToFarDistance = directionalLight->getLightFrustum().m_Far - dot((objectAabb.vcCenter - lightPos), lightDirection);

							Aabb objectFrustumAabb;
							objectFrustumAabb.vcMin = Vec3(-radioConeBase, -radioConeBase, -(objectToFarDistance + radioConeBase)*0.5f);
							objectFrustumAabb.vcMax = Vec3(radioConeBase, radioConeBase, (objectToFarDistance + radioConeBase)*0.5f);

							Vec3 objectFrustumPosition = objectAabb.vcCenter - radioConeBase*lightDirection + 0.5f*(objectToFarDistance + radioConeBase)*lightDirection;

							Mat4x4 objectFrustumMat = Mat4x4(right.x, up.x, look.x, objectFrustumPosition.x,
								right.y, up.y, look.y, objectFrustumPosition.y,
								right.z, up.z, look.z, objectFrustumPosition.z,
								0.0f, 0.0f, 0.0f, 1.0f);

							objectFrustumMat = objectFrustumMat.transformInverse();

							bool avoidTest = !viewFrustum.Inside(objectFrustumAabb, cameraMat, objectFrustumMat);

							//////////////////////////////////////////////////////////////////////////////////////////////

							*meshNode->getQueryResultPreviousFramePtr() = *meshNode->getQueryResultPresentFramePtr();

							if (!avoidTest)
							{
								*meshNode->getQueryResultPreviousFramePtr() = 15;
							}
							else
							{
								*meshNode->getQueryResultPreviousFramePtr() = 0;
							}
						}
						else
						{
							*meshNode->getQueryResultPreviousFramePtr() = 0;
							*meshNode->getQueryResultPresentFramePtr() = 0;
						}
					}

					//printf("Numero Tests: %d\n", num_Test);

					UINT numDraws = 0;

					//Set target for shadow
					pRenderer->setViewport(shadowMapRes, shadowMapRes, (int)(directionalLight->m_ShadowMapPos.x*m_MaxLevel), (int)(directionalLight->m_ShadowMapPos.y*m_MaxLevel));
					pRenderer->changeRenderTarget(TEXTURE_NONE, m_OmniDirectionalAtlasID, -1, VIEWPORT_SIZE);

					//pRenderer->clear(false, true, false, ClearColor, 1.0f);

					pRenderer->reset();

					pRenderer->setRasterizerState(pScene->m_noneFaceCulling);
					pRenderer->setDepthState(pScene->m_depthTest);

					pRenderer->setShader(m_ShadowShaderID);
					pRenderer->setVertexFormat(m_ShadowVertexFormatID);

					pRenderer->setShaderConstant4x4f("View", *((Mat4x4*)&lightView));
					pRenderer->setShaderConstant4x4f("Projection", *((Mat4x4*)&directionalLight->getViewToProjLightMatrix()));

					//Draw Mesh in ShadowMap
					for (UINT k = 0; k < objectInProjector.size(); k++)
					{
						std::shared_ptr<DrawableNode> drawableNode = static_pointer_cast<DrawableNode>(objectInProjector[k]);

						if (*drawableNode->getQueryResultPreviousFramePtr() > 10)
						{
							if (drawableNode->HasShadowShader())
							{								
								drawableNode->GetShadowShaderNode()->setActiveShader();

								pRenderer->setShaderConstant4x4f("View", *((Mat4x4*)&lightView));
								pRenderer->setShaderConstant4x4f("Projection", *((Mat4x4*)&directionalLight->getViewToProjLightMatrix()));
							}
							else
							{
								pRenderer->setShader(m_ShadowShaderID);
								pRenderer->setVertexFormat(m_ShadowVertexFormatID);
							}

							drawableNode->VOnSimpleRender(pScene);
							numDraws++;
						}

						drawableNode->SetInFrustumQueue(false);
					}

					printf("Num Shadow Draws: %d\n", numDraws);
				}
				else
				{
					directionalLight->m_ShadowMapRes = 0;
				}
			}
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		//	POINT LIGHT
		//
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (light->m_pLight->getType() == POINT_LIGHT)
		{
			shared_ptr<PointLight> pointLight = static_pointer_cast<PointLight>(light->m_pLight);
			pointLight->m_ShadowAtlas = m_OmniDirectionalAtlasID;

			Frustum viewFrustum = pScene->GetCamera()->GetFrustum();
			Mat4x4 cameraMat = pScene->GetCamera()->ToWorld();
			Vec3 lightPos = light->ToWorld().GetPosition();

			unsigned long long lightProjectResult[6] = { 0, 0, 0, 0, 0, 0 };

			if (light->m_pLight->getGenerateShadow())
			{
				//Uint32 time = g_pApp->getTick(); /////////////////////////////////////////////////////////////////////////////////////

				for (int j = 0; j<6; j++)
				{
					Mat4x4 cubeView = cubeViewMatrix(j);
					cubeView.rows[3].x = -dot(Vec3(cubeView.rows[0].x, cubeView.rows[1].x, cubeView.rows[2].x), lightPos);
					cubeView.rows[3].y = -dot(Vec3(cubeView.rows[0].y, cubeView.rows[1].y, cubeView.rows[2].y), lightPos);
					cubeView.rows[3].z = -dot(Vec3(cubeView.rows[0].z, cubeView.rows[1].z, cubeView.rows[2].z), lightPos);

					pRenderer->clear(true, true, false, ClearColor, 1.0f);

					pRenderer->reset();

					pRenderer->setShader(m_ShadowTestCoverageShaderID);
					pRenderer->setVertexFormat(m_ShadowTestCoverageVFID);
					pRenderer->setVertexBuffer(0, 1, &pScene->m_ScreenAlignedQuadVBID, offset);

					pRenderer->setTexture("depthTarget", pScene->m_PositionRT);
					pRenderer->setSamplerState("targetSampler", pScene->m_pointClamp);

					pRenderer->setDepthState(pScene->m_noDepthTest);
					pRenderer->setRasterizerState(pScene->m_noneFaceCulling);

					pRenderer->setShaderConstant4x4f("projectorView", *((Mat4x4*)&cubeView));
					pRenderer->setShaderConstant4x4f("projectorProj", *((Mat4x4*)&pointLight->getViewToProjLightMatrix()));
					pRenderer->setShaderConstant4x4f("View", *((Mat4x4*)&pScene->m_Camera->GetView()));
					pRenderer->setShaderConstant4x4f("Projection", *((Mat4x4*)&pScene->m_Camera->GetProjection()));
					pRenderer->setShaderConstant4x4f("InverseView", *((Mat4x4*)&(!pScene->m_Camera->GetView())));
					pRenderer->setShaderConstant4x4f("InverseProjection", *((Mat4x4*)&(!pScene->m_Camera->GetProjection())));
					pRenderer->setShaderConstant4f("cameraPosition", Vec4(pScene->m_Camera->ToWorld().GetPosition(), 1.0f));

					pRenderer->apply();

					lightProjectResult[j] = pointLight->m_QueryResult[j];

					int idQuery = m_pQueryQueue->getQuery(&pointLight->m_QueryResult[j]);

					m_pQueryQueue->BeginQuery(idQuery);

					pRenderer->drawArrays(PRIM_TRIANGLES, 0, 6);

					m_pQueryQueue->EndQuery(idQuery);

					//m_pQueryQueue->UpdateQueryQueue();
				}

				/*for(int j=0; j<6; j++)
				{
					printf("Pixeles luz %d Proyector %d: %d \n", i, j, lightProjectResult[j]);
				}*/

				for (int j = 0; j<6; j++)
				{
					if (lightProjectResult[j] > 10)
					{
						//Calculate shadowMapResolution
						int shadowMapRes = pow2roundup((int)sqrt((float)(4.0f*lightProjectResult[j])));
						shadowMapRes = shadowMapRes <= 512 ? shadowMapRes : 512;
						//shadowMapRes = 512;

						pointLight->m_ShadowMapRes[j] = (float)shadowMapRes;
						if (!m_pRoot->addTextureToAtlas(shadowMapRes, pointLight->m_ShadowMapPos[j]))
						{
							CHG_ERROR("not enough memory in atlas");
						}

						pointLight->m_ShadowMapPos[j] = pointLight->m_ShadowMapPos[j] * m_InverseMaxLevel;

						Mat4x4 cubeView = cubeViewMatrix(j);

						cubeView.rows[0].w = lightPos.x;
						cubeView.rows[1].w = lightPos.y;
						cubeView.rows[2].w = lightPos.z;

						std::vector<std::shared_ptr<SceneNode>> objectInProjector;

						pScene->m_Root->TraverseOctreeFromSpecificFrustum(objectInProjector, pointLight->getLightFrustum(), cubeView, pScene);
						pScene->TraverseDynamicObjectFromSpecificFrustum(objectInProjector, pointLight->getLightFrustum(), cubeView);

						for (UINT k = 0; k < objectInProjector.size(); k++)
						{
							Aabb objectAabb = objectInProjector[k]->getNodeAabb().Transform(objectInProjector[k]->ToWorld());
							std::shared_ptr<MeshNode> meshNode = static_pointer_cast<MeshNode>(objectInProjector[k]);

							if (meshNode->getCastShadows())
							{
								//Frustum Object////////////////////////////////////////////////////////////////////////
								/*float diameterConeBase = length(objectAabb.vcMin - objectAabb.vcMax);
								float radioConeBase = diameterConeBase*0.5f;

								//Mat4x4 objectFrustumProj = orthoMatrixX(diameterConeBase, diameterConeBase, -radioConeBase, directionalLight->getLightFrustum().m_Far);

								Vec3 look = normalize(objectAabb.vcCenter - lightPos);
								Vec3 right = (abs(dot(Vec3(0.0f, 1.0f, 0.0f), look)) != 1.0f) ? normalize(cross(Vec3(0.0f, 1.0f, 0.0f), look)) : normalize(cross(Vec3(1.0f, 0.0f, 0.0f), look));
								right = cross(look, right);
								Vec3 up = normalize(cross(right, look));

								//Obtain objectFrustumAabb
								float lightToObjectDistance = length(objectAabb.vcCenter - lightPos);
								float objectToFarDistance = pointLight->getLightFrustum().m_Far - lightToObjectDistance;

								Aabb objectFrustumAabb;
								objectFrustumAabb.vcMin = Vec3(-(radioConeBase / lightToObjectDistance) * pointLight->getLightFrustum().m_Far, -(radioConeBase / lightToObjectDistance) * pointLight->getLightFrustum().m_Far, -(objectToFarDistance + radioConeBase)*0.5f);
								objectFrustumAabb.vcMax = Vec3((radioConeBase / lightToObjectDistance)  * pointLight->getLightFrustum().m_Far, (radioConeBase / lightToObjectDistance)  * pointLight->getLightFrustum().m_Far, (objectToFarDistance + radioConeBase)*0.5f);

								Vec3 objectFrustumPosition = objectAabb.vcCenter - radioConeBase*look + 0.5f*(objectToFarDistance + radioConeBase)*look;

								Mat4x4 objectFrustumMat = Mat4x4(right.x, up.x, look.x, objectFrustumPosition.x,
									right.y, up.y, look.y, objectFrustumPosition.y,
									right.z, up.z, look.z, objectFrustumPosition.z,
									0.0f, 0.0f, 0.0f, 1.0f);

								objectFrustumMat = objectFrustumMat.transformInverse();

								bool avoidTest = !viewFrustum.Inside(objectFrustumAabb, cameraMat, objectFrustumMat);*/

								if (true)
								{
									*meshNode->getQueryResultPreviousFramePtr() = 15;
								}								
								else
								{
									*meshNode->getQueryResultPreviousFramePtr() = 0;
									*meshNode->getQueryResultPresentFramePtr() = 0;
								}
							}
							else
							{
								*meshNode->getQueryResultPreviousFramePtr() = 0;
								*meshNode->getQueryResultPresentFramePtr() = 0;
							}
						}

						//Set target for shadow
						g_pApp->m_pRenderer->setViewport(shadowMapRes, shadowMapRes, (int)(pointLight->m_ShadowMapPos[j].x*m_MaxLevel), (int)(pointLight->m_ShadowMapPos[j].y*m_MaxLevel));
						g_pApp->m_pRenderer->changeRenderTarget(TEXTURE_NONE, m_OmniDirectionalAtlasID, -1, VIEWPORT_SIZE);

						//g_pApp->m_pRenderer->clear(false, true, false, ClearColor, 1.0f);

						Mat4x4 cubeViewInverse = cubeViewMatrix(j);

						cubeViewInverse.rows[3].x = -dot(Vec3(cubeView.rows[0].x, cubeView.rows[1].x, cubeView.rows[2].x), lightPos);
						cubeViewInverse.rows[3].y = -dot(Vec3(cubeView.rows[0].y, cubeView.rows[1].y, cubeView.rows[2].y), lightPos);
						cubeViewInverse.rows[3].z = -dot(Vec3(cubeView.rows[0].z, cubeView.rows[1].z, cubeView.rows[2].z), lightPos);

						pRenderer->reset();

						pRenderer->setRasterizerState(pScene->m_noneFaceCulling);
						pRenderer->setDepthState(pScene->m_depthTest);

						pRenderer->setShader(m_ShadowShaderID);
						pRenderer->setVertexFormat(m_ShadowVertexFormatID);

						pRenderer->setShaderConstant4x4f("View", *((Mat4x4*)&cubeViewInverse));
						pRenderer->setShaderConstant4x4f("Projection", *((Mat4x4*)&pointLight->getViewToProjLightMatrix()));

						//Draw Mesh in ShadowMap
						for (UINT k = 0; k < objectInProjector.size(); k++)
						{
							std::shared_ptr<MeshNode> meshNode = static_pointer_cast<MeshNode>(objectInProjector[k]);

							if (*meshNode->getQueryResultPreviousFramePtr() > 10)
							{
								meshNode->VOnSimpleRender(pScene);
							}

							meshNode->SetInFrustumQueue(false);
						}
					}
					else
					{
						pointLight->m_ShadowMapRes[j] = 0;
					}

					//printf("Light %d, proyector %d: %d \n", i, j, (int)pointLight->m_ShadowMapRes[j]);
				}
			}
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		//	SPOT LIGHT
		//
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (light->m_pLight->getType() == SPOT_LIGHT)
		{
			shared_ptr<SpotLight> spotLight = static_pointer_cast<SpotLight>(light->m_pLight);
			spotLight->m_ShadowAtlas = m_DirectionalAtlasID;

			Mat4x4 lightView = spotLight->getViewLightMatrix();

			if (light->m_pLight->getGenerateShadow())
			{
				Frustum viewFrustum = pScene->GetCamera()->GetFrustum();
				Mat4x4 cameraMat = pScene->GetCamera()->ToWorld();
				Vec3 lightPos = light->ToWorld().GetPosition();
                Vec3 lightDirection = spotLight->getLightDirection().xyz();

				//Calculate Pixel Extension
                unsigned long long lightProjectResult = spotLight->m_QueryResult;

				pRenderer->clear(true, true, false, ClearColor, 1.0f);

				pRenderer->reset();

				pRenderer->setShader(m_ShadowTestCoverageShaderID);
				pRenderer->setVertexFormat(m_ShadowTestCoverageVFID);
				pRenderer->setVertexBuffer(0, 1, &pScene->m_ScreenAlignedQuadVBID, offset);

				pRenderer->setDepthState(pScene->m_noDepthTest);
				pRenderer->setRasterizerState(pScene->m_noneFaceCulling);

				pRenderer->setTexture("depthTarget", pScene->m_PositionRT);
				pRenderer->setSamplerState("targetSampler", pScene->m_pointClamp);

				pRenderer->setShaderConstant4x4f("projectorView", *((Mat4x4*)&lightView));
                pRenderer->setShaderConstant4x4f("projectorProj", *((Mat4x4*)&spotLight->getViewToProjLightMatrix()));
				pRenderer->setShaderConstant4x4f("View", *((Mat4x4*)&pScene->m_Camera->GetView()));
				pRenderer->setShaderConstant4x4f("Projection", *((Mat4x4*)&pScene->m_Camera->GetProjection()));
				pRenderer->setShaderConstant4x4f("InverseView", *((Mat4x4*)&(!pScene->m_Camera->GetView())));
				pRenderer->setShaderConstant4x4f("InverseProjection", *((Mat4x4*)&(!pScene->m_Camera->GetProjection())));
				pRenderer->setShaderConstant4f("cameraPosition", Vec4(pScene->m_Camera->ToWorld().GetPosition(), 1.0f));

				pRenderer->apply();

                int idQuery = m_pQueryQueue->getQuery(&spotLight->m_QueryResult);

				m_pQueryQueue->BeginQuery(idQuery);

				pRenderer->drawArrays(PRIM_TRIANGLES, 0, 6);

				m_pQueryQueue->EndQuery(idQuery);

				UINT count = 0;

                //printf("Test Result Pixel: %d\n", lightProjectResult);

				if (true)
				{
					//Calculate shadowMapResolution
					int shadowMapRes = pow2roundup((int)sqrt((float)(16.0f*lightProjectResult)));
					shadowMapRes = shadowMapRes <= 1024 ? shadowMapRes : 1024;
					shadowMapRes = 512;

					spotLight->m_ShadowMapRes = (float)shadowMapRes;
                    if (!m_pRoot->addTextureToAtlas(shadowMapRes, spotLight->m_ShadowMapPos))
					{
						CHG_ERROR("not enough memory in atlas");
					}

                    spotLight->m_ShadowMapPos = spotLight->m_ShadowMapPos*m_InverseMaxLevel;

					Vec3 look = lightDirection;
					Vec3 right = (abs(dot(Vec3(0.0f, 1.0f, 0.0f), look)) != 1.0f) ? normalize(cross(Vec3(0.0f, 1.0f, 0.0f), look)) : normalize(cross(Vec3(1.0f, 0.0f, 0.0f), look));
					Vec3 up = normalize(cross(right, look));

					Mat4x4 frustumView = Mat4x4(right.x, up.x, look.x, lightPos.x,
						right.y, up.y, look.y, lightPos.y,
						right.z, up.z, look.z, lightPos.z,
						0.0f, 0.0f, 0.0f, 1.0f);

					std::vector<std::shared_ptr<SceneNode>> objectInProjector;

                    pScene->m_Root->TraverseOctreeFromSpecificFrustum(objectInProjector, spotLight->getLightFrustum(), frustumView, pScene);
                    pScene->TraverseDynamicObjectFromSpecificFrustum(objectInProjector, spotLight->getLightFrustum(), frustumView);

					UINT num_Test = 0;

					//pRenderer->clear(true, true, false, ClearColor, 1.0f);

					for (UINT k = 0; k < objectInProjector.size(); k++)
					{
						//pRenderer->clear(true, true, false, ClearColor, 1.0f);

						Aabb objectAabb = objectInProjector[k]->getNodeAabb().Transform(objectInProjector[k]->ToWorld());
						std::shared_ptr<MeshNode> meshNode = static_pointer_cast<MeshNode>(objectInProjector[k]);

						if (meshNode->getCastShadows())
						{
							//Frustum Object////////////////////////////////////////////////////////////////////////
							/*float diameterConeBase = length(objectAabb.vcMin - objectAabb.vcMax);
							float radioConeBase = diameterConeBase*0.5f;

							//Mat4x4 objectFrustumProj = orthoMatrixX(diameterConeBase, diameterConeBase, -radioConeBase, directionalLight->getLightFrustum().m_Far);

							Vec3 look = lightDirection;
							Vec3 right = (abs(dot(Vec3(0.0f, 1.0f, 0.0f), look)) != 1.0f) ? normalize(cross(Vec3(0.0f, 1.0f, 0.0f), look)) : normalize(cross(Vec3(1.0f, 0.0f, 0.0f), look));
							Vec3 up = normalize(cross(right, look));

							//Obtain objectFrustumAabb
                            float objectToFarDistance = spotLight->getLightFrustum().m_Far - dot((objectAabb.vcCenter - lightPos), lightDirection);

							Aabb objectFrustumAabb;
							objectFrustumAabb.vcMin = Vec3(-radioConeBase, -radioConeBase, -(objectToFarDistance + radioConeBase)*0.5f);
							objectFrustumAabb.vcMax = Vec3(radioConeBase, radioConeBase, (objectToFarDistance + radioConeBase)*0.5f);

							Vec3 objectFrustumPosition = objectAabb.vcCenter - radioConeBase*lightDirection + 0.5f*(objectToFarDistance + radioConeBase)*lightDirection;

							Mat4x4 objectFrustumMat = Mat4x4(right.x, up.x, look.x, objectFrustumPosition.x,
								right.y, up.y, look.y, objectFrustumPosition.y,
								right.z, up.z, look.z, objectFrustumPosition.z,
								0.0f, 0.0f, 0.0f, 1.0f);

							objectFrustumMat = objectFrustumMat.transformInverse();

							bool avoidTest = !viewFrustum.Inside(objectFrustumAabb, cameraMat, objectFrustumMat);

							//////////////////////////////////////////////////////////////////////////////////////////////

							*meshNode->getQueryResultPreviousFramePtr() = *meshNode->getQueryResultPresentFramePtr();*/

							if (true)
							{
								*meshNode->getQueryResultPreviousFramePtr() = 15;
							}
							else
							{
								*meshNode->getQueryResultPreviousFramePtr() = 0;
							}
						}
						else
						{
							*meshNode->getQueryResultPreviousFramePtr() = 0;
							*meshNode->getQueryResultPresentFramePtr() = 0;
						}
					}

					//printf("Numero Tests: %d\n", num_Test);

					UINT numDraws = 0;

					//Set target for shadow
                    pRenderer->setViewport(shadowMapRes, shadowMapRes, (int)(spotLight->m_ShadowMapPos.x*m_MaxLevel), (int)(spotLight->m_ShadowMapPos.y*m_MaxLevel));
					pRenderer->changeRenderTarget(TEXTURE_NONE, m_OmniDirectionalAtlasID, -1, VIEWPORT_SIZE);

					//pRenderer->clear(false, true, false, ClearColor, 1.0f);

					pRenderer->reset();

					pRenderer->setRasterizerState(pScene->m_noneFaceCulling);
					pRenderer->setDepthState(pScene->m_depthTest);

					pRenderer->setShader(m_ShadowShaderID);
					pRenderer->setVertexFormat(m_ShadowVertexFormatID);

					pRenderer->setShaderConstant4x4f("View", *((Mat4x4*)&lightView));
                    pRenderer->setShaderConstant4x4f("Projection", *((Mat4x4*)&spotLight->getViewToProjLightMatrix()));

					//Draw Mesh in ShadowMap
					for (UINT k = 0; k < objectInProjector.size(); k++)
					{
						std::shared_ptr<MeshNode> meshNode = static_pointer_cast<MeshNode>(objectInProjector[k]);

						if (*meshNode->getQueryResultPreviousFramePtr() > 10)
						{
							meshNode->VOnSimpleRender(pScene);
							numDraws++;
						}

						meshNode->SetInFrustumQueue(false);
					}

					//printf("Num Shadow Draws: %d\n", numDraws);
				}
				else
				{
                    spotLight->m_ShadowMapRes = 0;
				}
			}
		}
	}

	return true;
}