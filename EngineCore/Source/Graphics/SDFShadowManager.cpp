//========================================================================
// SDFShadowManager.cpp : implement SDFShadowManager class
//
//========================================================================

#include "SDFShadowManager.h"
#include "Mesh.h"
#include "Light.h"

typedef LightNode* LightNodePtr;
typedef Light*	LightPtr;
typedef PointLight* PointLightPtr;
typedef SpotLight* SpotLightPtr;
typedef DirectionalLight* DirectionalLightPtr;

typedef SceneNode* SceneNodePtr;
typedef MeshNode* MeshNodePtr;

//	ShadowCoordResourceLoader class implementation

shared_ptr<IResourceLoader> CreateShadowCoordResourceLoader()
{
	return shared_ptr<IResourceLoader>(CHG_NEW ShadowCoordResourceLoader());
}

unsigned int ShadowCoordResourceLoader::VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize)
{
	return rawSize;
}

bool ShadowCoordResourceLoader::VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
    if (rawSize == 0)
        return false;

    shared_ptr<ShadowCoordResourceExtraData> extra = shared_ptr<ShadowCoordResourceExtraData>(CHG_NEW ShadowCoordResourceExtraData());

    char* AuxRawBuffer = CHG_NEW char[rawSize + 1];
    memcpy(AuxRawBuffer, rawBuffer, rawSize);
    AuxRawBuffer[rawSize] = '\0';

    char* pch = strtok(AuxRawBuffer, "\n|");

    while (pch != NULL)
    {
        std::string name = pch;
		
		for (UINT i = 0; i < name.size(); i++)
		{
			name[i] = tolower(name[i]);
		}

        pch = strtok(NULL, "\n|");
        Vec3 coord = Vec3(std::string(pch));

		pch = strtok(NULL, "\n|");
		Vec3 dim = Vec3(std::string(pch));

        extra->m_SDFTexCoord[name] = std::pair<Vec3, Vec3>(coord, dim);

        pch = strtok(NULL, "\n|");
    }

    delete[] AuxRawBuffer;

    handle->SetExtra(extra);

	return true;
}

//	SDFResourceLoader class implementation

shared_ptr<IResourceLoader> CreateSDFResourceLoader()
{
	return shared_ptr<IResourceLoader>(CHG_NEW SDFResourceLoader());
}

unsigned int SDFResourceLoader::VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize)
{
	return rawSize;
}

bool SDFResourceLoader::VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
    struct DistanceFieldHeader
    {
        char type[3];
        int w, h, d;
    };

    shared_ptr<SDFResourceResourceExtraData> extra = shared_ptr<SDFResourceResourceExtraData>(CHG_NEW SDFResourceResourceExtraData());

    DistanceFieldHeader header;
    memcpy(&header, rawBuffer, sizeof(DistanceFieldHeader));

    if (strcmp(header.type, "udf") != 0)
    {
        CHG_ERROR("SDF Shader File has not correct format!");
        return false;
    }

    extra->m_SDFTextureID = g_pApp->m_pRenderer->createTexture(rawBuffer + sizeof(DistanceFieldHeader), FORMAT_R32F, header.w, header.h, header.d);
	extra->m_vPackDimensions = Vec3((float)header.w, (float)header.h, (float)header.d);

    handle->SetExtra(extra);

	return true;
}

//	TerrainMesh class implementation

SDFShadowManager::SDFShadowManager()
{
    //Load SDF Texture Coords
    Resource SDFPackCoordResource("Art\\Shadows\\SDFPackPosData.pData");
    shared_ptr<ResHandle> SDFPackCoordHandle = g_pApp->m_ResCache->GetHandle(&SDFPackCoordResource);

    if (SDFPackCoordHandle.get() == NULL)
    {
        CHG_ERROR("Not Found File: Art\\Shadows\\SDFPackPosData.pData");
    }

    m_SDFTextureCoords = ((ShadowCoordResourceExtraData*)SDFPackCoordHandle->GetExtra().get())->m_SDFTexCoord;

    //Load SDF Texture Pack
    Resource SDFTexturePackResource("Art\\Shadows\\SdfMeshPack.psdf");
    shared_ptr<ResHandle> SDFTexturePackHandle = g_pApp->m_ResCache->GetHandle(&SDFTexturePackResource);

    if (SDFTexturePackHandle.get() == NULL)
    {
        CHG_ERROR("Not Found File: Art\\Shadows\\SdfMeshPack.psdf");
    }

    m_texSDFPackTextureID = ((SDFResourceResourceExtraData*)SDFTexturePackHandle->GetExtra().get())->m_SDFTextureID;
	m_vSDFShadowPackDim = ((SDFResourceResourceExtraData*)SDFTexturePackHandle->GetExtra().get())->m_vPackDimensions;

	// Load Shaders
	Resource IntersectionShaderResource("Shaders\\ShadowShaders\\PointLight_CulledObject_Intersection.shd");
	shared_ptr<ResHandle> IntersectionShaderHandle = g_pApp->m_ResCache->GetHandle(&IntersectionShaderResource);

	if (!IntersectionShaderHandle)
	{
		CHG_ERROR("Not Found File: Shaders\\ShadowShaders\\PointLight_CulledObject_Intersection.shd");
	}

	m_shPointLight_Object_Intersection_ShaderID = ((ShaderResourceExtraData*)IntersectionShaderHandle->GetExtra().get())->m_ShaderID;

	Resource DirectionalIntersectionShaderResource("Shaders\\ShadowShaders\\DirectionalLight_CulledObject_Intersection.shd");
	shared_ptr<ResHandle> DirectionalIntersectionShaderHandle = g_pApp->m_ResCache->GetHandle(&DirectionalIntersectionShaderResource);

	if (!DirectionalIntersectionShaderHandle)
	{
		CHG_ERROR("Not Found File: Shaders\\ShadowShaders\\DirectionalLight_CulledObject_Intersection.shd");
	}

	m_shDirectionalLight_Object_Intersection_ShaderID = ((ShaderResourceExtraData*)DirectionalIntersectionShaderHandle->GetExtra().get())->m_ShaderID;

	// Create Buffers
	m_sbCulledObjectBounds = g_pApp->m_pRenderer->createStructuredBuffer(sizeof(Vec4), MAX_CULLED_OBJECT, true, false);
	m_sbPoint_DirectionalLightBounds = g_pApp->m_pRenderer->createStructuredBuffer(sizeof(Vec4), MAX_SHADOW_POINT_LIGHT + MAX_SHADOW_DIRECTIONAL_LIGHT * 2, true, false);
	m_sbCulledObjectSDFParameters = g_pApp->m_pRenderer->createStructuredBuffer(sizeof(Vec4), 7*MAX_CULLED_OBJECT, true, false);
	m_sbPoint_Directional_Light_CulledObject_Intersection = g_pApp->m_pRenderer->createStructuredBuffer(sizeof(UINT), MAX_OBJECT_PER_LIGHT*MAX_SHADOW_POINT_LIGHT + MAX_OBJECT_PER_LIGHT*MAX_SHADOW_DIRECTIONAL_LIGHT, false, true);
}

void SDFShadowManager::OnUpdatePointDirectionalLightAndCullingObject(Scene *pScene, std::list<std::shared_ptr<SceneNode>> LightList)
{
	std::shared_ptr<Renderer> pRenderer = g_pApp->m_pRenderer;

	m_uiPointLightCount = 0;
	m_uiDirectionalLightCount = 0;
	m_uiCulledObjectCount = 0;

	//Update Point And Directiona Light Bounds Buffer
	Vec4* pPoint_DirectionalLightBounds = (Vec4*)pRenderer->mapBuffer(m_sbPoint_DirectionalLightBounds, MAP_WRITE_DISCARD);

	if (pPoint_DirectionalLightBounds)
	{
		for (auto pSceneNode : LightList)
		{
			// Ugly Pointer Cast but It's faster
			LightNodePtr pLightNode = (LightNodePtr)pSceneNode.get();
			LightPtr pLight = (LightPtr)pLightNode->GetLightData().get();

			if (pLight->getType() == POINT_LIGHT)
			{
				PointLightPtr pPointLight = (PointLightPtr)pLight;

				pPoint_DirectionalLightBounds[m_uiPointLightCount++] = Vec4(pLightNode->ToWorld().GetPosition(), pPointLight->getLightRadius());
			}
			else if (pLight->getType() == DIRECTIONAL_LIGHT)
			{
				DirectionalLightPtr pDirectionalLight = (DirectionalLightPtr)pLight;

				pPoint_DirectionalLightBounds[MAX_SHADOW_POINT_LIGHT + 2 * m_uiDirectionalLightCount] = Vec4(pLightNode->ToWorld().GetPosition(), pDirectionalLight->GetMaxLength());
				pPoint_DirectionalLightBounds[MAX_SHADOW_POINT_LIGHT + 2 * m_uiDirectionalLightCount + 1] = Vec4(-pLightNode->ToWorld().rows[0].y, -pLightNode->ToWorld().rows[1].y, -pLightNode->ToWorld().rows[2].y, pDirectionalLight->GetMaxRadius());

				m_uiDirectionalLightCount++;
			}
			else if (pLight->getType() == SPOT_LIGHT)
			{
				SpotLightPtr pSpotLight = (SpotLightPtr)pLight;

				pPoint_DirectionalLightBounds[m_uiPointLightCount++] = Vec4(pLightNode->ToWorld().GetPosition(), pSpotLight->getLightRadius());
			}
		}
	}

	pRenderer->unMapBuffer(m_sbPoint_DirectionalLightBounds);

	//Update Culled Object Bounds Buffer
	Vec4* pCulledObjectBounds = (Vec4*)pRenderer->mapBuffer(m_sbCulledObjectBounds, MAP_WRITE_DISCARD);

	if (pCulledObjectBounds)
	{
		for (auto pISceneNode : pScene->GetStaticObjectList())
		{
			// Ugly Pointer Cast but It's faster
			SceneNodePtr pSceneNode = (SceneNodePtr)pISceneNode.get();

			if (pSceneNode->VGetType() == MESH_NODE)
			{
				MeshNodePtr pMeshNode = (MeshNodePtr)pSceneNode;

				if (pMeshNode->CastSDFShadows())
					pCulledObjectBounds[m_uiCulledObjectCount++] = pMeshNode->GetSphereBoundCenterAndRadius();
			}
		}

		for (auto pISceneNode : pScene->GetDynamicObjectList())
		{
			// Ugly Pointer Cast but It's faster
			SceneNodePtr pSceneNode = (SceneNodePtr)pISceneNode.second.get();

			if (pSceneNode->VGetType() == MESH_NODE)
			{
				MeshNodePtr pMeshNode = (MeshNodePtr)pSceneNode;

				if (pMeshNode->CastSDFShadows())
					pCulledObjectBounds[m_uiCulledObjectCount++] = pMeshNode->GetSphereBoundCenterAndRadius();
			}
		}
	}

	pRenderer->unMapBuffer(m_sbCulledObjectBounds);

	// Intersect Light Sphere Bounds and Object Sphere Bounds in GPU /////////////////////////////////////////////////////////////////////////
	pRenderer->setShader(m_shPointLight_Object_Intersection_ShaderID);

	pRenderer->setStructuredBuffer("sbCulledObjectBounds", m_sbCulledObjectBounds);
	pRenderer->setStructuredBuffer("sbPointLightBounds", m_sbPoint_DirectionalLightBounds);
	pRenderer->setStructuredBuffer("sbPointLight_CulledObject_Intersection", m_sbPoint_Directional_Light_CulledObject_Intersection);

	// Lights Count
	pRenderer->setShaderConstant1i("iNumCulledObject", m_uiCulledObjectCount);

	pRenderer->apply();

	// Dispatch
	pRenderer->dispatchCompute(m_uiPointLightCount, 1, 1);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Intersect Light Cylinder Bounds and Object Sphere Bounds in GPU /////////////////////////////////////////////////////////////////////////
	pRenderer->setShader(m_shDirectionalLight_Object_Intersection_ShaderID);

	pRenderer->setStructuredBuffer("sbCulledObjectBounds", m_sbCulledObjectBounds);
	pRenderer->setStructuredBuffer("sbDirectionalLightBounds", m_sbPoint_DirectionalLightBounds);
	pRenderer->setStructuredBuffer("sbDirectionalLight_CulledObject_Intersection", m_sbPoint_Directional_Light_CulledObject_Intersection);

	// Lights Count
	pRenderer->setShaderConstant1i("iNumCulledObject", m_uiCulledObjectCount);

	pRenderer->apply();

	// Dispatch
	pRenderer->dispatchCompute(m_uiDirectionalLightCount, 1, 1);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

void SDFShadowManager::OnUpdatePointDirectionalLightAndCullingObject(Scene *pScene)
{
	OnUpdatePointDirectionalLightAndCullingObject(pScene, pScene->GetLightQueue());
}

void SDFShadowManager::OnUpdateSDFObjectParameter(Scene *pScene)
{
	std::shared_ptr<Renderer> pRenderer = g_pApp->m_pRenderer;

	//Update Culled Object SDF Parameters Buffer
	Vec4* pCulledObjectSDFParameters = (Vec4*)pRenderer->mapBuffer(m_sbCulledObjectSDFParameters, MAP_WRITE_DISCARD);

	if (pCulledObjectSDFParameters)
	{
		UINT uOffset = 0;
		UINT uObjects = 0;

		for (auto pISceneNode : pScene->GetStaticObjectList())
		{
			// Ugly Pointer Cast but It's faster
			SceneNodePtr pSceneNode = (SceneNodePtr)pISceneNode.get();

			if (pSceneNode->VGetType() == MESH_NODE)
			{
				MeshNodePtr pMeshNode = (MeshNodePtr)pSceneNode;

				if (pMeshNode->CastSDFShadows())
				{
					pMeshNode->SetGPUIndex(uObjects);
					uObjects++;

					pCulledObjectSDFParameters[uOffset++] = Vec4(pMeshNode->GetLocalPositionExtent().xyz(), (float)pMeshNode->ShadowGeneratedAsTwoSided());
					pCulledObjectSDFParameters[uOffset++] = pMeshNode->GetSDFUVOffset();
					pCulledObjectSDFParameters[uOffset++] = pMeshNode->GetSDFUVScaleAndVolumeScale();

					Mat4x4 mWorldToVolume = pMeshNode->GetWorldToVolumeMatrix();

					pCulledObjectSDFParameters[uOffset++] = mWorldToVolume.rows[0];
					pCulledObjectSDFParameters[uOffset++] = mWorldToVolume.rows[1];
					pCulledObjectSDFParameters[uOffset++] = mWorldToVolume.rows[2];
					pCulledObjectSDFParameters[uOffset++] = mWorldToVolume.rows[3];
				}
			}
		}

		for (auto pISceneNode : pScene->GetDynamicObjectList())
		{
			// Ugly Pointer Cast but It's faster
			SceneNodePtr pSceneNode = (SceneNodePtr)pISceneNode.second.get();

			if (pSceneNode->VGetType() == MESH_NODE)
			{
				MeshNodePtr pMeshNode = (MeshNodePtr)pSceneNode;

				if (pMeshNode->CastSDFShadows())
				{
					pMeshNode->SetGPUIndex(uObjects);
					uObjects++;

					pCulledObjectSDFParameters[uOffset++] = Vec4(pMeshNode->GetLocalPositionExtent().xyz(), (float)pMeshNode->ShadowGeneratedAsTwoSided());
					pCulledObjectSDFParameters[uOffset++] = pMeshNode->GetSDFUVOffset();
					pCulledObjectSDFParameters[uOffset++] = pMeshNode->GetSDFUVScaleAndVolumeScale();

					Mat4x4 mWorldToVolume = pMeshNode->GetWorldToVolumeMatrix();

					pCulledObjectSDFParameters[uOffset++] = mWorldToVolume.rows[0];
					pCulledObjectSDFParameters[uOffset++] = mWorldToVolume.rows[1];
					pCulledObjectSDFParameters[uOffset++] = mWorldToVolume.rows[2];
					pCulledObjectSDFParameters[uOffset++] = mWorldToVolume.rows[3];
				}
			}
		}
	}

	pRenderer->unMapBuffer(m_sbCulledObjectSDFParameters);
}

void SDFShadowManager::SetShaderSDFBuffersForLight(Scene* pScene)
{
	std::shared_ptr<Renderer> pRenderer = g_pApp->m_pRenderer;

	pRenderer->setStructuredBuffer("sbCulledObjectBounds", m_sbCulledObjectBounds);
	pRenderer->setStructuredBuffer("sbCulledObjectSDFParameters", m_sbCulledObjectSDFParameters);
	pRenderer->setStructuredBuffer("sbPointLightBounds", m_sbPoint_DirectionalLightBounds);
	pRenderer->setStructuredBuffer("sbPointLight_CulledObject_Intersection", m_sbPoint_Directional_Light_CulledObject_Intersection);

	pRenderer->setShaderConstant1f("fTwoSidedMeshDistanceBias", 0.08f);

	pRenderer->setTexture("tex_SDFTexturePack", m_texSDFPackTextureID);
	pRenderer->setSamplerState("DistanceFieldSampler", pScene->m_linearClamp);
}

bool SDFShadowManager::GetSDFCoords(std::string resourceName, Vec4& coord, Vec4& dim)
{
	std::map<std::string, std::pair<Vec3, Vec3>>::iterator it = m_SDFTextureCoords.find(resourceName);

	if (it != m_SDFTextureCoords.end())
	{
		coord = Vec4(it->second.first, 1.0f);
		dim = Vec4(it->second.second, 0.0f);

		return true;
	}

	return false;
}