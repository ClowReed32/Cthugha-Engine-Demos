#include "CthughaStd.h"

#include "VoxelManager.h"
#include "Mesh.h"
#include "Light.h"
#include "Shader.h"
#include "SDFShadowManager.h"

// Auxiliar Function: Count Trailing zeros ///////////////////////////////////////////////////

UINT countTrailingZeros(UINT num)
{
	UINT c = 32; // c will be the number of zero bits on the right
	
	if (num) c--;
	if (num & 0x0000FFFF) c -= 16;
	if (num & 0x00FF00FF) c -= 8;
	if (num & 0x0F0F0F0F) c -= 4;
	if (num & 0x33333333) c -= 2;
	if (num & 0x55555555) c -= 1;

	return c;
}

///////////////////////////////////////////////////////////////////////////////////////////////

bool GISceneNodeComp(std::shared_ptr<SceneNode> first, std::shared_ptr<SceneNode> second)
{
	return first->VGetOrder() < second->VGetOrder();
}

VoxelManager::~VoxelManager()
{
	SAFE_DELETE_ARRAY(m_pCascadeAabb);
	SAFE_DELETE_ARRAY(m_pWorldToCascadeMatrix);
	SAFE_DELETE_ARRAY(m_pCascadeToWorldMatrix);
	SAFE_DELETE_ARRAY(m_pWorldToUnitMatrix);
}

bool VoxelManager::Initialize(const bool bActiveGI, const bool bActiveVL, const UINT voxelTextureResolution, const Vec3 volumeLightingGridResolution, const UINT num_VoxelCascades, const float fCascadeExcentrity)
{
    m_uiNumCascades = num_VoxelCascades;
	m_uiCascadeResolution = voxelTextureResolution;
	m_vVolumeLightingGridResolution = volumeLightingGridResolution;
	m_fCascadeExcentrity = 0.5f*fCascadeExcentrity;
	m_vVolumeAmbientColor = Vec4(0.05f, 0.05f, 0.05f, 0.01f);
	m_bGIActive = bActiveGI;
	m_bActiveVolumeLighting = bActiveVL;
	m_bActiveTwoBounces = true;
	m_uiDebugCascade = 0;
	m_uiDebugCascadeFace = 2;

	m_vfCascadesDimensions[0] = 20;
	m_vfCascadesDimensions[1] = 40;
	m_vfCascadesDimensions[2] = 80;
	m_vfCascadesDimensions[3] = 160;
	m_vfCascadesDimensions[4] = 320;
	m_vfCascadesDimensions[5] = 640;

	m_fDebugCellTam = (float)m_vfCascadesDimensions[m_uiDebugCascade] / (float)m_uiCascadeResolution;

	// Voxel Grid Textures

	int num_MipMaps = (int)(log((float)m_uiCascadeResolution) / log(2.0f) + 1);

	if (m_bGIActive)
	{
		m_rVoxelGBuffer = g_pApp->m_pRenderer->addRenderTarget(m_uiCascadeResolution, m_uiCascadeResolution * 2, m_uiCascadeResolution*m_uiNumCascades, 1, 1, FORMAT_R32UI, 1, SS_NONE, UAV_TEXTURE);
		m_rVoxelRadiance = g_pApp->m_pRenderer->addRenderTarget(m_uiCascadeResolution, m_uiCascadeResolution * 8, m_uiCascadeResolution*m_uiNumCascades, num_MipMaps, 1, FORMAT_RGBA16F, 1, SS_NONE, UAV_TEXTURE);
		m_rBounceVoxelGridAux = g_pApp->m_pRenderer->addRenderTarget(m_uiCascadeResolution, m_uiCascadeResolution, m_uiCascadeResolution, 1, 1, FORMAT_RGBA16F, 1, SS_NONE, UAV_TEXTURE);
		
		if (m_bActiveTwoBounces)
			m_rVoxelGridMipMaps[0] = g_pApp->m_pRenderer->addRenderTarget(m_uiCascadeResolution >> 1, (m_uiCascadeResolution >> 1) * 16, m_uiCascadeResolution >> 1, 1, 1, FORMAT_RGBA16F, 1, SS_NONE, UAV_TEXTURE);
			m_rVoxelGridMipMaps[1] = g_pApp->m_pRenderer->addRenderTarget(m_uiCascadeResolution >> 2, (m_uiCascadeResolution >> 2) * 16, m_uiCascadeResolution >> 2, 1, 1, FORMAT_RGBA16F, 1, SS_NONE, UAV_TEXTURE);
	}
	
	if (m_bActiveVolumeLighting)
	{
		m_rVolumeLightingData = g_pApp->m_pRenderer->addRenderTarget((int)m_vVolumeLightingGridResolution.x, (int)m_vVolumeLightingGridResolution.y, (int)m_vVolumeLightingGridResolution.z,
			1, 1, FORMAT_RGBA16F, 1, SS_NONE, UAV_TEXTURE);
		m_rAccumulateVolumeLightingData = g_pApp->m_pRenderer->addRenderTarget((int)m_vVolumeLightingGridResolution.x, (int)m_vVolumeLightingGridResolution.y, (int)m_vVolumeLightingGridResolution.z,
			1, 1, FORMAT_RGBA16F, 1, SS_NONE, UAV_TEXTURE);
	}

	// Render Target
	m_rIndirectLightRT = g_pApp->m_pRenderer->addRenderTarget((int)g_pApp->GetScreenSize().x, (int)g_pApp->GetScreenSize().y, 1, 1, 1, FORMAT_RGBA16F, 1, SS_NONE);
	m_rVolumeLightingRT = g_pApp->m_pRenderer->addRenderTarget((int)g_pApp->GetScreenSize().x, (int)g_pApp->GetScreenSize().y, 1, 1, 1, FORMAT_RGBA16F, 1, SS_NONE);

	m_pCascadeAabb = CHG_NEW Aabb[m_uiNumCascades];
	m_pWorldToCascadeMatrix = CHG_NEW Mat4x4[m_uiNumCascades];
	m_pCascadeToWorldMatrix = CHG_NEW Mat4x4[m_uiNumCascades];
	m_pWorldToUnitMatrix = CHG_NEW Mat4x4[m_uiNumCascades];

	for (UINT i = 0; i<m_uiNumCascades; i++)
	{
		//Init Aabb
		m_pCascadeAabb[i].vcMin = -0.5f*m_vfCascadesDimensions[i];
		m_pCascadeAabb[i].vcMax = 0.5f*m_vfCascadesDimensions[i];
		m_pCascadeAabb[i].vcCenter = Vec3(0.0f, 0.0f, 0.0f);
	}

	//Load Grid Clear Shader
	Resource GridClearShaderResource("Shaders\\ClearVoxelShader.shd");
	shared_ptr<ResHandle> ClearGridShaderHandle = g_pApp->m_ResCache->GetHandle(&GridClearShaderResource);

	if (ClearGridShaderHandle.get() == NULL)
	{
		CHG_ERROR("Not found Clear Grid Shader");

		return false;
	}

	m_rClearVoxelShaderID = ((ShaderResourceExtraData*)ClearGridShaderHandle->GetExtra().get())->m_ShaderID;

	//Load Copy Grid Shader
	Resource GridCopyShaderResource("Shaders\\GI\\CopyGridShader.shd");
	shared_ptr<ResHandle> CopyGridShaderHandle = g_pApp->m_ResCache->GetHandle(&GridCopyShaderResource);

	if (CopyGridShaderHandle.get() == NULL)
	{
		CHG_ERROR("Not found Copy Grid Shader");

		return false;
	}

	m_rCopyVoxelGridShaderID = ((ShaderResourceExtraData*)CopyGridShaderHandle->GetExtra().get())->m_ShaderID;

	//Load Inject Light Shader
	Resource InjectLightShaderResource("Shaders\\GI\\GI_Radiance_Injection.shd");
	shared_ptr<ResHandle> InjectLightShaderHandle = g_pApp->m_ResCache->GetHandle(&InjectLightShaderResource);

	if (InjectLightShaderHandle.get() == NULL)
	{
		CHG_ERROR("Not found Light Injection Shader");

		return false;
	}

	m_rInjectLightShaderID = ((ShaderResourceExtraData*)InjectLightShaderHandle->GetExtra().get())->m_ShaderID;

    //Load MipMap Shader
    Resource MipMapShaderResource("Shaders\\GI\\ComputeMipmapShader.shd");
	shared_ptr<ResHandle> MipMapShaderHandle = g_pApp->m_ResCache->GetHandle(&MipMapShaderResource);

	if(MipMapShaderHandle.get() == NULL)
	{
		CHG_ERROR("Not found MipMap Shader");

		return false;
	}

	m_rMipMapShaderID = ((ShaderResourceExtraData*)MipMapShaderHandle->GetExtra().get())->m_ShaderID;

	//Load Voxel Cone Tracing Shader
	Resource VoxelConeTracingShaderResource("Shaders\\VoxelConeTracingShader.shd");
	shared_ptr<ResHandle> VoxelConeTracingShaderHandle = g_pApp->m_ResCache->GetHandle(&VoxelConeTracingShaderResource);

	if (VoxelConeTracingShaderHandle.get() == NULL)
	{
		CHG_ERROR("Not found Voxel Cone Tracing Shader");
	}

	m_rVoxelConeTracingShaderID = ((ShaderResourceExtraData*)VoxelConeTracingShaderHandle->GetExtra().get())->m_ShaderID;

	//Load Two Bounce Voxel Cone Tracing Shader
	Resource TwoBounceRadianceShaderResource("Shaders\\GI\\BounceRadianceShader.shd");
	shared_ptr<ResHandle> TwoBounceRadianceShaderHandle = g_pApp->m_ResCache->GetHandle(&TwoBounceRadianceShaderResource);

	if (TwoBounceRadianceShaderHandle.get() == NULL)
	{
		CHG_ERROR("Not found two bounce radiance shader");

		return false;
	}

	m_rTwoBounceRadianceShaderID = ((ShaderResourceExtraData*)TwoBounceRadianceShaderHandle->GetExtra().get())->m_ShaderID;

	//Load Copy Two Bounce Voxel Cone Tracing Shader
	Resource CopyTwoBounceRadianceShaderResource("Shaders\\GI\\CopyTwoBounceRadianceShader.shd");
	shared_ptr<ResHandle> CopyTwoBounceRadianceShaderHandle = g_pApp->m_ResCache->GetHandle(&CopyTwoBounceRadianceShaderResource);

	if (CopyTwoBounceRadianceShaderHandle.get() == NULL)
	{
		CHG_ERROR("Not found copy two bounce radiance shader");

		return false;
	}

	m_rCopyTwoBounceRadianceShaderID = ((ShaderResourceExtraData*)CopyTwoBounceRadianceShaderHandle->GetExtra().get())->m_ShaderID;

    //Load Volume Lighting Shader
    Resource VolumeLightingShaderResource("Shaders\\VolumeLighting\\VolumeLightingShader.shd");
    shared_ptr<ResHandle> VolumeLightingShaderHandle = g_pApp->m_ResCache->GetHandle(&VolumeLightingShaderResource);

    if (VolumeLightingShaderHandle.get() == NULL)
    {
        CHG_ERROR("Not found Volume Lighting Shader");

        return false;
    }

	m_rVolumeLightingShaderID = ((ShaderResourceExtraData*)VolumeLightingShaderHandle->GetExtra().get())->m_ShaderID;

    //Load Scattering Solver Shader
    Resource ScatteringSolverShaderResource("Shaders\\VolumeLighting\\ScatteringSolverShader.shd");
    shared_ptr<ResHandle> ScatteringSolverShaderHandle = g_pApp->m_ResCache->GetHandle(&ScatteringSolverShaderResource);

    if (ScatteringSolverShaderHandle.get() == NULL)
    {
        CHG_ERROR("Not found Scattering Solver Shader");

        return false;
    }

	m_rScatteringSolverShaderID = ((ShaderResourceExtraData*)ScatteringSolverShaderHandle->GetExtra().get())->m_ShaderID;

    //Load Volume Render To Render Target Shader
    Resource VolumeRenderToRTShaderResource("Shaders\\VolumeLighting\\VolumeToRenderTargetShader.shd");
    shared_ptr<ResHandle> VolumeRenderToRTShaderHandle = g_pApp->m_ResCache->GetHandle(&VolumeRenderToRTShaderResource);

    if (VolumeRenderToRTShaderHandle.get() == NULL)
    {
        CHG_ERROR("Not found Volume Render To Render Target Shader");

        return false;
    }

	m_rVolumeRenderToRenderTargetID = ((ShaderResourceExtraData*)VolumeRenderToRTShaderHandle->GetExtra().get())->m_ShaderID;

    //Voxelization Vertex Format
    FormatDesc *vertexDesc = CHG_NEW FormatDesc[3];

	vertexDesc[0].stream = 0;
	vertexDesc[0].format = FORMAT_FLOAT;
	vertexDesc[0].type = TYPE_VERTEX;
	vertexDesc[0].size = 3;

	vertexDesc[1].stream = 1;
	vertexDesc[1].format = FORMAT_FLOAT;
	vertexDesc[1].type = TYPE_NORMAL;
	vertexDesc[1].size = 3;

	vertexDesc[2].stream = 2;
	vertexDesc[2].format = FORMAT_FLOAT;
	vertexDesc[2].type = TYPE_TEXCOORD;
	vertexDesc[2].size = 2;

	if (m_rVoxelConeTracingShaderID != SHADER_NONE)
		m_rVoxelConeTracingShaderVFID = g_pApp->m_pRenderer->addVertexFormat(vertexDesc, 1, m_rVoxelConeTracingShaderID);
    if (m_rVolumeRenderToRenderTargetID != SHADER_NONE)
        m_rVolumeRenderToRenderTargetVFID = g_pApp->m_pRenderer->addVertexFormat(vertexDesc, 1, m_rVolumeRenderToRenderTargetID);

	delete vertexDesc;
	
	return true;
}

void VoxelManager::setActiveGI(bool active)
{
	m_bGIActive = active;
	m_bActiveDebug = m_bActiveDebug && active;

	if (!m_bGIActive)
	{
		float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

		TextureID GIbufferRTs[] = { m_rIndirectLightRT };
		g_pApp->m_pRenderer->changeRenderTargets(GIbufferRTs, elementsOf(GIbufferRTs), TEXTURE_NONE);

		g_pApp->m_pRenderer->clear(true, false, false, ClearColor, 1.0f);
	}

	if (m_bGIActive)
	{
		int num_MipMaps = (int)(log((float)m_uiCascadeResolution) / log(2.0f) + 1);

		if (m_rVoxelGBuffer == TEXTURE_NONE)
			m_rVoxelGBuffer = g_pApp->m_pRenderer->addRenderTarget(m_uiCascadeResolution, m_uiCascadeResolution * 2, m_uiCascadeResolution*m_uiNumCascades, 1, 1, FORMAT_R32UI, 1, SS_NONE, UAV_TEXTURE);
		if (m_rVoxelRadiance == TEXTURE_NONE)
			m_rVoxelRadiance = g_pApp->m_pRenderer->addRenderTarget(m_uiCascadeResolution, m_uiCascadeResolution * 8, m_uiCascadeResolution*m_uiNumCascades, num_MipMaps, 1, FORMAT_RGBA8, 1, SS_NONE, UAV_TEXTURE);
		if (!m_rVoxelGridMipMaps)
		{
			m_rVoxelGridMipMaps[0] = g_pApp->m_pRenderer->addRenderTarget(m_uiCascadeResolution >> 1, (m_uiCascadeResolution >> 1) * 8, m_uiCascadeResolution >> 1, 1, 1, FORMAT_RGBA16F, 1, SS_NONE, UAV_TEXTURE);
			m_rVoxelGridMipMaps[1] = g_pApp->m_pRenderer->addRenderTarget(m_uiCascadeResolution >> 2, (m_uiCascadeResolution >> 2) * 8, m_uiCascadeResolution >> 2, 1, 1, FORMAT_RGBA16F, 1, SS_NONE, UAV_TEXTURE);
		}
	}
}

void VoxelManager::SetActiveVolumeLighting(bool active)
{ 
	m_bActiveVolumeLighting = active; 

	if (m_bActiveVolumeLighting)
	{
		if (m_rVolumeLightingData = TEXTURE_NONE)
		{
			m_rVolumeLightingData = g_pApp->m_pRenderer->addRenderTarget((int)m_vVolumeLightingGridResolution.x, (int)m_vVolumeLightingGridResolution.y, (int)m_vVolumeLightingGridResolution.z,
				1, 1, FORMAT_RGBA16F, 1, SS_NONE, UAV_TEXTURE);
			m_rAccumulateVolumeLightingData = g_pApp->m_pRenderer->addRenderTarget((int)m_vVolumeLightingGridResolution.x, (int)m_vVolumeLightingGridResolution.y, (int)m_vVolumeLightingGridResolution.z,
				1, 1, FORMAT_RGBA16F, 1, SS_NONE, UAV_TEXTURE);
		}
	}
}

void VoxelManager::ClearCascade(int cascadeIndex)
{
	UINT workGroupSize[3] = { 16, 8, 1 };

	g_pApp->m_pRenderer->reset();
	g_pApp->m_pRenderer->setShader(m_rClearVoxelShaderID);

	g_pApp->m_pRenderer->setImageBuffer("giSceneData", m_rVoxelRadiance, 0, FORMAT_RGBA8, WRITE_ONLY);
	g_pApp->m_pRenderer->setImageBuffer("voxelizationSceneData", m_rVoxelGBuffer, 0, FORMAT_R32UI, WRITE_ONLY);
	g_pApp->m_pRenderer->setShaderConstant1i("cascadeResolution", m_uiCascadeResolution);
	g_pApp->m_pRenderer->setShaderConstant1i("cascade", cascadeIndex);

	g_pApp->m_pRenderer->apply();

	g_pApp->m_pRenderer->dispatchCompute(
		(m_uiCascadeResolution + workGroupSize[0] - 1) / workGroupSize[0],
		(m_uiCascadeResolution + workGroupSize[1] - 1) / workGroupSize[1],
		(m_uiCascadeResolution + workGroupSize[2] - 1) / workGroupSize[2]);
}

void VoxelManager::VoxelConeTracing(Scene* pScene)
{
	if (!m_bGIActive)
		return;

	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	UINT offset[] = { 0 };

	Vec4 cascadeBounds[12];
	int count = 0;

	for (UINT i = 0; i < 2 * m_uiNumCascades; i += 2)
	{
		Vec3 vCascadePos = m_vLastCascadePosition[count];
		Mat4x4 cameraTransform = translate(vCascadePos);

		Aabb aabb = m_pCascadeAabb[count].Transform(cameraTransform);
		cascadeBounds[i] = Vec4(aabb.vcMin, 1.0f);
		cascadeBounds[i + 1] = Vec4(aabb.vcMax, 1.0f);

		count++;
	}

	shared_ptr<Renderer> pRenderer = g_pApp->m_pRenderer;

	pRenderer->setViewport((int)(g_pApp->GetScreenSize().x * 0.5f), (int)(g_pApp->GetScreenSize().y * 0.5f));
	TextureID VoxelConeBufferRTs[] = { m_rIndirectLightRT };
	pRenderer->changeRenderTargets(VoxelConeBufferRTs, elementsOf(VoxelConeBufferRTs), TEXTURE_NONE);

	pRenderer->clear(true, false, false, ClearColor, 1.0f);

	pRenderer->reset();

	pRenderer->setShader(m_rVoxelConeTracingShaderID);
	pRenderer->setVertexFormat(m_rVoxelConeTracingShaderVFID);
	pRenderer->setVertexBuffer(0, 1, &pScene->m_ScreenAlignedQuadVBID, offset);
	pRenderer->setRasterizerState(pScene->m_noneFaceCulling);

	pRenderer->setTexture("depthTarget", pScene->m_PositionRT);
	pRenderer->setTexture("normalTarget", pScene->m_NormalRT);
	pRenderer->setTexture("baseTarget", pScene->m_ColorRT);
	pRenderer->setTexture("MREC_Target", pScene->m_MREC_RT);
	pRenderer->setTexture("tangentTarget", pScene->m_TangentRT);

	pRenderer->setShaderConstant4f("cameraPosition", Vec4(pScene->m_Camera->ToWorld().GetPosition(), 1.0f));
	pRenderer->setShaderConstant4x4f("InverseView", pScene->m_Camera->GetInverseView());
	pRenderer->setShaderConstant4x4f("InverseProjection", pScene->m_Camera->GetInverseProjection());

	pRenderer->setSamplerState("VoxelFilter", pScene->m_linearClamp);
	pRenderer->setSamplerState("TargetSample", pScene->m_linearWrap);

	pRenderer->setShaderConstant1f("minVoxelDiameter", 1.0f / m_uiCascadeResolution);
	pRenderer->setShaderConstant1f("minVoxelDiameterInv", (float)m_uiCascadeResolution);
	pRenderer->setTexture("giSceneData", m_rVoxelRadiance);

	pRenderer->setShaderConstantArray4x4f("worldToVoxelTex", m_pWorldToUnitMatrix, 6);
	pRenderer->setShaderConstantArray4x4f("voxelTexToWorld", m_pCascadeToWorldMatrix, 6);
	pRenderer->setShaderConstantArray4f("cascadeBounds", cascadeBounds, m_uiNumCascades * 2);
	pRenderer->setShaderConstant1i("numCascades", m_uiNumCascades);

	pRenderer->apply();

	pRenderer->drawArrays(PRIM_TRIANGLES, 0, 6);
}

void VoxelManager::TwoBounceRadiance(int cascadeIndex, Scene* pScene)
{
	UINT workGroupSize[3] = { 16, 8, 1 };

	shared_ptr<Renderer> pRenderer = g_pApp->m_pRenderer;
	std::shared_ptr<LightManager> pLightManager = pScene->m_pLightManager;

	Vec3 vCascadePos = m_vLastCascadePosition[cascadeIndex];

	pRenderer->reset();
	pRenderer->setShader(m_rTwoBounceRadianceShaderID);

	pRenderer->setImageBuffer("GI_OutputRadiance", m_rBounceVoxelGridAux, 0, FORMAT_RGBA16F, WRITE_ONLY);
	pRenderer->setTexture("GI_GBuffer", m_rVoxelGBuffer);
	pRenderer->setTexture("GI_Radiance", m_rVoxelRadiance);
	pRenderer->setSamplerState("VoxelFilter", pScene->m_linearClamp);

	pRenderer->setShaderConstant1i("iCascadeResolution", m_uiCascadeResolution);
	pRenderer->setShaderConstant1i("iCascadeIndex", cascadeIndex);
	pRenderer->setShaderConstant1f("fMinVoxelDiameter", 1.0f / m_uiCascadeResolution);
	pRenderer->setShaderConstant1f("fMinVoxelDiameterInv", (float)m_uiCascadeResolution);
	pRenderer->setShaderConstant1i("iNumCascades", m_uiNumCascades);
	pRenderer->setShaderConstant3f("g_SnappedGridPosition", vCascadePos);
	pRenderer->setShaderConstant1f("g_CellSize", m_fCellTam);
	
	pRenderer->setShaderConstantArray4x4f("mWorldToVoxelTex", m_pWorldToUnitMatrix, 6);

	pRenderer->apply();

	pRenderer->dispatchCompute(
		(m_uiCascadeResolution + workGroupSize[0] - 1) / workGroupSize[0],
		(m_uiCascadeResolution + workGroupSize[1] - 1) / workGroupSize[1],
		(m_uiCascadeResolution + workGroupSize[2] - 1) / workGroupSize[2]);

	// Copy to radiance texture ////////////////////////////////////////////////////////////

	pRenderer->reset();
	pRenderer->setShader(m_rCopyTwoBounceRadianceShaderID);

	pRenderer->setImageBuffer("GI_RadianceOuput", m_rVoxelRadiance, 0, FORMAT_RGBA16F, WRITE_ONLY);
	pRenderer->setTexture("GI_GBuffer", m_rVoxelGBuffer);
	pRenderer->setTexture("GI_Radiance", m_rBounceVoxelGridAux);

	pRenderer->setShaderConstant1i("iCascadeResolution", m_uiCascadeResolution);
	pRenderer->setShaderConstant1i("iCascadeIndex", cascadeIndex);

	pRenderer->apply();

	pRenderer->dispatchCompute(
		(m_uiCascadeResolution + workGroupSize[0] - 1) / workGroupSize[0],
		(m_uiCascadeResolution + workGroupSize[1] - 1) / workGroupSize[1],
		(m_uiCascadeResolution + workGroupSize[2] - 1) / workGroupSize[2]);

	////////////////////////////////////////////////////////////////////////////////////////

	// Compute Mip Map /////////////////////////////////////////////////////////////////////

	UpdateMipMap(cascadeIndex);

	////////////////////////////////////////////////////////////////////////////////////////
}

void VoxelManager::InjectLightToVoxel(std::list<std::shared_ptr<SceneNode>> LightList, int cascadeIndex, Scene* pScene)
{
	UINT workGroupSize[3] = { 16, 8, 1 };

	shared_ptr<Renderer> pRenderer = g_pApp->m_pRenderer;
	std::shared_ptr<LightManager> pLightManager = pScene->m_pLightManager;

	Vec3 vCascadePos = m_vLastCascadePosition[cascadeIndex];

	pRenderer->reset();
	pRenderer->setShader(m_rInjectLightShaderID);

	pRenderer->setImageBuffer("GI_Radiance", m_rVoxelRadiance, 0, FORMAT_RGBA8, WRITE_ONLY);
	pRenderer->setTexture("GI_GBuffer", m_rVoxelGBuffer);

	pRenderer->setShaderConstant1i("iCascadeResolution", m_uiCascadeResolution);
	pRenderer->setShaderConstant1i("iCascadeIndex", cascadeIndex);
	pRenderer->setShaderConstant3f("g_SnappedGridPosition", vCascadePos);
	pRenderer->setShaderConstant1f("g_CellSize", m_fCellTam);

	pRenderer->setShaderConstant4x4f("mView", pScene->GetCamera()->GetView());
	pRenderer->setShaderConstant4x4f("mInverseView", pScene->GetCamera()->GetInverseView());
	g_pApp->m_pRenderer->setShaderConstant1i("iActiveShadows", 1);

	g_pApp->m_pSDFShadowManager->SetShaderSDFBuffersForLight(pScene);
	pScene->m_pLightManager->SetLightBuffers(pScene);	

	pRenderer->apply();

	pRenderer->dispatchCompute(
		(m_uiCascadeResolution + workGroupSize[0] - 1) / workGroupSize[0],
		(m_uiCascadeResolution + workGroupSize[1] - 1) / workGroupSize[1],
		(m_uiCascadeResolution + workGroupSize[2] - 1) / workGroupSize[2]);

}

void VoxelManager::UpdateMipMap(int cascadeIndex)
{
	shared_ptr<Renderer> pRenderer = g_pApp->m_pRenderer;

	UINT workGroupSize[3] = { 16, 8, 1 };

	int num_MipMaps = (int)(log((float)m_uiCascadeResolution) / log(2.0f) + 1);

	pRenderer->reset();
	pRenderer->setShader(m_rMipMapShaderID);

	pRenderer->setTexture("srcTex", m_rVoxelRadiance);
	pRenderer->setImageBuffer("dstMip", m_rVoxelGridMipMaps[0], 0, FORMAT_RGBA8, WRITE_ONLY);
	
	pRenderer->setShaderConstant1i("srcMipLevel", 0);
	pRenderer->setShaderConstant1i("cascade", cascadeIndex);
	pRenderer->setShaderConstant1i("cascadeResolution", m_uiCascadeResolution);
	pRenderer->setShaderConstant1i("iReadOffset", 0);
	pRenderer->setShaderConstant1i("iWriteOffset", 0);

	pRenderer->apply();

	pRenderer->dispatchCompute(
		((m_uiCascadeResolution >> 1) + workGroupSize[0] - 1) / workGroupSize[0],
		((m_uiCascadeResolution >> 1) + workGroupSize[1] - 1) / workGroupSize[1],
		((m_uiCascadeResolution >> 1) + workGroupSize[2] - 1) / workGroupSize[2]);

	int iOffset[2] = { 0, 0 };

	for (int mip = 2; mip < num_MipMaps; mip++)
	{
		pRenderer->reset(RESET_RWSTRUCTUREDBUFFER);

		int iCascadeResolution = m_uiCascadeResolution >> mip;
		int iReadCascadeResolution = m_uiCascadeResolution >> (mip - 1);

		pRenderer->setTexture("srcTex", m_rVoxelGridMipMaps[mip%2]);
		pRenderer->setImageBuffer("dstMip", m_rVoxelGridMipMaps[(mip + 1)%2], 0, FORMAT_RGBA8, WRITE_ONLY);

		pRenderer->setShaderConstant1i("srcMipLevel", 0);
		pRenderer->setShaderConstant1i("cascade", 0);
		pRenderer->setShaderConstant1i("cascadeResolution", iReadCascadeResolution);
		pRenderer->setShaderConstant1i("iReadOffset", iOffset[mip % 2]);
		pRenderer->setShaderConstant1i("iWriteOffset", iOffset[(mip + 1) % 2]);

		iOffset[mip % 2] += 6*iReadCascadeResolution;

		pRenderer->apply();

		pRenderer->dispatchCompute(
			((m_uiCascadeResolution >> mip) + workGroupSize[0] - 1) / workGroupSize[0],
			((m_uiCascadeResolution >> mip) + workGroupSize[1] - 1) / workGroupSize[1],
			((m_uiCascadeResolution >> mip) + workGroupSize[2] - 1) / workGroupSize[2]);
	}

	pRenderer->reset();
	pRenderer->setShader(m_rCopyVoxelGridShaderID);

	iOffset[0] = 0;
	iOffset[1] = 0;

	for (int mip = 1; mip < num_MipMaps; mip++)
	{		
		int iCascadeResolution = m_uiCascadeResolution >> mip;

		pRenderer->setTexture("srcTex", m_rVoxelGridMipMaps[(mip + 1) % 2]);
		pRenderer->setImageBuffer("dstMip", m_rVoxelRadiance, mip, FORMAT_RGBA8, WRITE_ONLY);
		pRenderer->setShaderConstant1i("cascade", cascadeIndex);
		pRenderer->setShaderConstant1i("cascadeResolution", iCascadeResolution);
		pRenderer->setShaderConstant1i("iOffset", iOffset[(mip + 1) % 2]);

		iOffset[(mip + 1) % 2] += 6*(m_uiCascadeResolution >> mip);

		pRenderer->apply();

		pRenderer->dispatchCompute(
			((m_uiCascadeResolution >> mip) + workGroupSize[0] - 1) / workGroupSize[0],
			((m_uiCascadeResolution >> mip) + workGroupSize[1] - 1) / workGroupSize[1],
			((m_uiCascadeResolution >> mip) + workGroupSize[2] - 1) / workGroupSize[2]);
	}
}

void VoxelManager::VoxelizeScene(int cascade, Scene* pScene)
{
	std::shared_ptr<Renderer> pRenderer = g_pApp->m_pRenderer;

	m_fCellTam = (m_pCascadeAabb[cascade].vcMax.x - m_pCascadeAabb[cascade].vcMin.x) / m_uiCascadeResolution;

	Aabb cascadeAabb = m_pCascadeAabb[cascade];
	cascadeAabb = cascadeAabb.Transform(translate(m_vLastCascadePosition[cascade]));

	std::list<std::shared_ptr<SceneNode>> ObjectList;
	std::list<std::shared_ptr<SceneNode>> LightList;

	pScene->m_Root->TraverseOctreeFromAabb(ObjectList, LightList, cascadeAabb, pScene);
	pScene->TraverseDynamicObjectFromAabb(ObjectList, LightList, cascadeAabb);

	//printf("Num Objects: %d\n", ObjectList.size());

	ObjectList.sort(GISceneNodeComp);

	pRenderer->setViewport(m_uiCascadeResolution, m_uiCascadeResolution);
	pRenderer->changeToMainFramebuffer();

	pRenderer->reset();

	//pRenderer->setBlendState(pScene->m_blendAdd);
	pRenderer->setDepthState(pScene->m_noDepthTest);
	pRenderer->setRasterizerState(pScene->m_noneFaceCulling);

	while (!ObjectList.empty())
	{
		if (ObjectList.back()->VIsDrawable())
		{
			shared_ptr<DrawableNode> object = static_pointer_cast<DrawableNode>(ObjectList.back());

			if (object->HasGIShader())
			{
				object->GetGIShaderNode()->setActiveShader();
				object->GetGIMaterialNode()->setActiveMaterial();

				pRenderer->setShaderConstant4x4f("worldToUnitCube", *((Mat4x4*)&m_pWorldToCascadeMatrix[cascade]));
				pRenderer->setShaderConstant1i("cascade", cascade);
				pRenderer->setShaderConstant1i("cascadeResolution", m_uiCascadeResolution);
				pRenderer->setImageBuffer("voxelDataGrid", m_rVoxelGBuffer, 0, FORMAT_R32UI, WRITE_ONLY);

				//Set Sampler States/////////////////////////////////////////////////////////////////////////////////////////////////////
				pRenderer->setSamplerState("BaseFilter", pScene->m_linearWrap);
				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				object->OnRenderWithExternalShader(pScene);
			}
		}

		ObjectList.back()->SetInFrustumQueue(false);
		ObjectList.pop_back();
	}

	// Update Shadow culling
	g_pApp->m_pSDFShadowManager->OnUpdatePointDirectionalLightAndCullingObject(pScene, LightList);

	// Update Light Buffer
	pScene->m_pLightManager->UpdateLightBuffers(pScene, LightList);

	// Inject Light in Voxels
	InjectLightToVoxel(LightList, cascade, pScene);

	//Update voxel grid
	if (m_bActiveDebug)
	{
		if (m_uiDebugCascade == cascade)
		{
			UpdateDebug();
		}
	}
}

void VoxelManager::UpdateMatrix(int cascade, Scene* pScene)
{
	//Modify ToUnitMatrix transform
	float fDiscreteDisplacement = m_vfCascadesDimensions[cascade] / m_uiCascadeResolution;
	//Vec3 vCascadePos = pScene->GetCamera()->ToWorld().GetPosition() + pScene->GetCamera()->ToWorld().GetDirection()*m_vfCascadesDimensions[cascade]*m_fCascadeExcentrity;
	Vec3 vCascadePos = Vec3(0.0f, 2.0f, 0.0f);
	vCascadePos = Vec3(floorf(vCascadePos.x / fDiscreteDisplacement)*fDiscreteDisplacement, floorf(vCascadePos.y / fDiscreteDisplacement)*fDiscreteDisplacement, floorf(vCascadePos.z / fDiscreteDisplacement)*fDiscreteDisplacement);

	m_vLastCascadePosition[cascade] = vCascadePos;

	//if (cascade == 0)
		//printf("Cascade %d -> %f %f %f\n", cascade, vCascadePos.x, vCascadePos.y, vCascadePos.z);

	Vec3 sceneScale = 1.0f / (m_pCascadeAabb[cascade].vcMax - m_pCascadeAabb[cascade].vcMin);
	float VoxelScale = min(sceneScale.x, min(sceneScale.y, sceneScale.z));
	Vec3 VoxelBias = (translate(-vCascadePos)*Vec4(-m_pCascadeAabb[cascade].vcMin, 1.0f)).xyz();

	m_pWorldToUnitMatrix[cascade] = scale(VoxelScale, VoxelScale, VoxelScale) * translate(VoxelBias);

	m_pWorldToCascadeMatrix[cascade] = transpose(
	translate(-1.0f, -1.0f, -1.0f) * scale(2.0f, 2.0f, 2.0f) *
	m_pWorldToUnitMatrix[cascade]);

	m_pWorldToUnitMatrix[cascade] = transpose(m_pWorldToUnitMatrix[cascade]);

	m_pCascadeToWorldMatrix[cascade] = !m_pWorldToUnitMatrix[cascade];

	//voxelCascade.toVoxelUnitTransformNorm = transpose(scale(VoxelScale, VoxelScale, VoxelScale) * translate(VoxelBias));
}

int VoxelManager::ChooseCascade()
{
	UINT cascadeID = countTrailingZeros((m_uiCount++ % 4294967295) & ((1 << (m_uiNumCascades - 1)) - 1));

	return (cascadeID >= m_uiNumCascades) ? m_uiNumCascades - 1 : cascadeID;
}

void VoxelManager::SetActiveTwoBounces(bool bActive)
{
	m_bActiveTwoBounces = bActive;

	if (bActive)
	{
		if (m_rVoxelGridMipMaps[0] == TEXTURE_NONE)
			m_rVoxelGridMipMaps[0] = g_pApp->m_pRenderer->addRenderTarget(m_uiCascadeResolution >> 1, (m_uiCascadeResolution >> 1) * 16, m_uiCascadeResolution >> 1, 1, 1, FORMAT_RGBA16F, 1, SS_NONE, UAV_TEXTURE);

		if (m_rVoxelGridMipMaps[1] == TEXTURE_NONE)
			m_rVoxelGridMipMaps[1] = g_pApp->m_pRenderer->addRenderTarget(m_uiCascadeResolution >> 2, (m_uiCascadeResolution >> 2) * 16, m_uiCascadeResolution >> 2, 1, 1, FORMAT_RGBA16F, 1, SS_NONE, UAV_TEXTURE);
	}
}

bool VoxelManager::UpdateGICascades(Scene* pScene)
{
	if(!m_bGIActive)
		return false;

	//Choose one cascade to Update -> Tomorrow Children method
	int cascade = ChooseCascade();

	//Update position-scale matrix for cascade
	UpdateMatrix(cascade, pScene);

	//Clear choosen cascade
	ClearCascade(cascade);

	//Voxelize object in the cascade
	VoxelizeScene(cascade, pScene);

	//Generate mipmap
	UpdateMipMap(cascade);

	//Two Bounce pass
	if (m_bActiveTwoBounces)
		TwoBounceRadiance(cascade, pScene);

	return true;
}

bool VoxelManager::SetShaderForVoxel(int cascadeIndex)
{
	if(m_bActiveFill)
	{
		//g_pApp->m_pRenderer->setShaderConstant1f("minVoxelDiameter", 1.0f/m_vVoxelCascades[cascadeIndex].voxelTextureResolution);
		//g_pApp->m_pRenderer->setShaderConstant1f("minVoxelDiameterInv", m_vVoxelCascades[cascadeIndex].voxelTextureResolution);
		g_pApp->m_pRenderer->setShaderConstant1i("ActiveVoxel", 1);
		//g_pApp->m_pRenderer->setTextureArray("voxelGrids", m_vVoxelCascades[cascadeIndex].voxelRadiance, 6);

		//g_pApp->m_pRenderer->setShaderConstant4x4f("worldToVoxelTex", *((Mat4x4*)&m_vVoxelCascades[cascadeIndex].toVoxelUnitTransformNorm));
	}
	else
	{
		g_pApp->m_pRenderer->setShaderConstant1i("ActiveVoxel", 0);
	}

	return true;
}

bool VoxelManager::insideCascade(Aabb aabb, int cascadeIndex)
{
	Aabb cascadeAabb = m_pCascadeAabb[cascadeIndex].Transform(m_pWorldToCascadeMatrix[cascadeIndex]);

	return cascadeAabb.Intersects(aabb);
}

void VoxelManager::DensityEstimationVolumeLighting(Scene* pScene)
{
	UINT workGroupSize[3] = { 16, 8, 1 };

	shared_ptr<Renderer> pRenderer = g_pApp->m_pRenderer;
	std::shared_ptr<LightManager> pLightManager = pScene->m_pLightManager;
	std::list<std::shared_ptr<SceneNode>> lLightList = pScene->m_LightQueue;

	pRenderer->reset();
	pRenderer->setShader(m_rVolumeLightingShaderID);

	pRenderer->setImageBuffer("VolumeLightingData", m_rVolumeLightingData, 0, FORMAT_RGBA16F, WRITE_ONLY);

	pRenderer->setShaderConstant1f("fFrustumDepth", pScene->GetCamera()->GetFrustum().m_Far - pScene->GetCamera()->GetFrustum().m_Near);

	pRenderer->setShaderConstant3f("vGridDimension", m_vVolumeLightingGridResolution);
	pRenderer->setShaderConstant1f("fInverseZDimension", 1.0f / m_vVolumeLightingGridResolution.z);
	pRenderer->setShaderConstant4f("vAmbientColorAndConstantDensity", m_vVolumeAmbientColor);
	pRenderer->setShaderConstant1f("fFogRange", 50.0f);
	pRenderer->setShaderConstant1i("g_iFrameNumber", g_pApp->GetFrameNumber());

	pRenderer->setShaderConstant4x4f("mInverseView", pScene->GetCamera()->GetInverseView());
	pRenderer->setShaderConstant4x4f("mInverseProjection", pScene->GetCamera()->GetInverseProjection());

	g_pApp->m_pRenderer->setShaderConstant1i("iActiveShadows", 1);

	g_pApp->m_pSDFShadowManager->SetShaderSDFBuffersForLight(pScene);
	pScene->m_pLightManager->SetLightBuffers(pScene);

	pRenderer->apply();

	pRenderer->dispatchCompute(
		((UINT)m_vVolumeLightingGridResolution.x + workGroupSize[0] - 1) / workGroupSize[0],
		((UINT)m_vVolumeLightingGridResolution.y + workGroupSize[1] - 1) / workGroupSize[1],
		((UINT)m_vVolumeLightingGridResolution.z + workGroupSize[2] - 1) / workGroupSize[2]);
}

void VoxelManager::SolveScattering(Scene* pScene)
{
	shared_ptr<Renderer> pRenderer = g_pApp->m_pRenderer;

	UINT workGroupSize[3] = { 16, 8, 1 };

	pRenderer->reset();
	pRenderer->setShader(m_rScatteringSolverShaderID);

	pRenderer->setTexture("VolumeLightingData", m_rVolumeLightingData);
	pRenderer->setImageBuffer("AccumulateVolumeLightingData", m_rAccumulateVolumeLightingData, 0, FORMAT_RGBA8, READ_WRITE);
	pRenderer->setShaderConstant3f("vGridDimension", m_vVolumeLightingGridResolution);
	pRenderer->setShaderConstant1f("fGridVoxelTam", m_fGridVoxelTam);

	pRenderer->apply();

	pRenderer->dispatchCompute(
		((UINT)m_vVolumeLightingGridResolution.x + workGroupSize[0] - 1) / workGroupSize[0],
		((UINT)m_vVolumeLightingGridResolution.y + workGroupSize[1] - 1) / workGroupSize[1],
		1);
}

void VoxelManager::VolumeDataToRenderTarget(Scene* pScene)
{
	UINT offset[] = { 0 };
	
	float zFar = pScene->GetCamera()->GetFrustum().m_Far;
	float zNear = pScene->GetCamera()->GetFrustum().m_Near;

	shared_ptr<Renderer> pRenderer = g_pApp->m_pRenderer;

	pRenderer->setViewport((int)g_pApp->GetScreenSize().x, (int)g_pApp->GetScreenSize().y);
	TextureID VolumeLightingRTs[] = { m_rVolumeLightingRT };
	pRenderer->changeRenderTargets(VolumeLightingRTs, elementsOf(VolumeLightingRTs), TEXTURE_NONE);

	//pRenderer->clear(true, false, false, ClearColor, 1.0f);

	pRenderer->reset();

	pRenderer->setShader(m_rVolumeRenderToRenderTargetID);
	pRenderer->setVertexFormat(m_rVolumeRenderToRenderTargetVFID);
	pRenderer->setVertexBuffer(0, 1, &pScene->m_ScreenAlignedQuadVBID, offset);
	pRenderer->setRasterizerState(pScene->m_noneFaceCulling);

	pRenderer->setTexture("depthTarget", pScene->m_PositionRT);
	pRenderer->setTexture("VolumeLightingData", m_rAccumulateVolumeLightingData);

	pRenderer->setShaderConstant4f("vCameraPosition", Vec4(pScene->m_Camera->ToWorld().GetPosition(), 1.0f));
	pRenderer->setShaderConstant4x4f("mInverseView", pScene->m_Camera->GetInverseView());
	pRenderer->setShaderConstant4x4f("mInverseProjection", pScene->m_Camera->GetInverseProjection());
	pRenderer->setShaderConstant4x4f("mView", pScene->m_Camera->GetView());
	pRenderer->setShaderConstant4x4f("mProjection", pScene->m_Camera->GetProjection());

	pRenderer->setShaderConstant3f("vGridDimension", m_vVolumeLightingGridResolution);
	pRenderer->setShaderConstant1f("fFrustumDepth", pScene->GetCamera()->GetFrustum().m_Far - pScene->GetCamera()->GetFrustum().m_Near);
	pRenderer->setShaderConstant1f("g_ReprojectDepthScale", (zFar - zNear) / (-zFar * zNear));
	pRenderer->setShaderConstant1f("g_ReprojectDepthBias", zFar / (zFar * zNear));
	pRenderer->setShaderConstant1f("fFogRange", 50.0f);

	pRenderer->setSamplerState("BilinearSample", pScene->m_bilinearClamp);
	pRenderer->setSamplerState("TargetSample", pScene->m_pointClamp);

	pRenderer->apply();

	pRenderer->drawArrays(PRIM_TRIANGLES, 0, 6);
}

void VoxelManager::SolveVolumeLighting(Scene* pScene)
{
	if (m_bActiveVolumeLighting)
	{
		DensityEstimationVolumeLighting(pScene);
		SolveScattering(pScene);
		VolumeDataToRenderTarget(pScene);
	}
}

bool VoxelManager::ActiveDebugMode(bool active)
{
	m_bActiveDebug = active;

	if (!m_bActiveDebug)
		return true;

	//Create Shaders to debug voxelization

	if (m_rOccluderVoxelCountShaderID == -1)
	{
		Resource OccluderVoxelCountShaderResource("Shaders\\OccluderVoxelCountShader.shd");
		shared_ptr<ResHandle> OccluderVoxelCountShaderHandle = g_pApp->m_ResCache->GetHandle(&OccluderVoxelCountShaderResource);

		if (OccluderVoxelCountShaderHandle.get() == NULL)
		{
			CHG_ERROR("Not found OccluderVoxelCount Shader");

			return false;
		}

		m_rOccluderVoxelCountShaderID = ((ShaderResourceExtraData*)OccluderVoxelCountShaderHandle->GetExtra().get())->m_ShaderID;

		Resource DrawVoxelShaderResource("Shaders\\VoxelDrawShader.shd");
		shared_ptr<ResHandle> DrawVoxelShaderShaderHandle = g_pApp->m_ResCache->GetHandle(&DrawVoxelShaderResource);

		if (DrawVoxelShaderShaderHandle.get() == NULL)
		{
			CHG_ERROR("Not found OccluderVoxelCount Shader");

			return false;
		}

		m_rDrawVoxelShaderID = ((ShaderResourceExtraData*)DrawVoxelShaderShaderHandle->GetExtra().get())->m_ShaderID;

		FormatDesc *vertexDesc = CHG_NEW FormatDesc[1];

		vertexDesc[0].stream = 0;
		vertexDesc[0].format = FORMAT_FLOAT;
		vertexDesc[0].type = TYPE_VERTEX;
		vertexDesc[0].size = 3;

		m_rDrawVoxelShaderVFID = g_pApp->m_pRenderer->addVertexFormat(vertexDesc, 1, m_rDrawVoxelShaderID);
	}

	// Create Debug buffer
	//	->Argument Buffer for draw function
	//	->Buffer for voxel position
	if (m_rIndirectDrawArgBuffer == -1)
	{
		m_rIndirectDrawArgBuffer = g_pApp->m_pRenderer->createIndirectDrawArgBuffer();
		m_rVoxelPositionBuffer = g_pApp->m_pRenderer->createStructuredBuffer(3 * sizeof(UINT), m_uiCascadeResolution*m_uiCascadeResolution*m_uiCascadeResolution, false, true, BUFFER_UAV_FLAG_APPEND);
	}

	// Create vertex and index buffer for draw voxels like cubes
	if (m_rCubeVertexBufferID == -1)
	{
		// Create the voxel vertices.
		static const float g_cube_vertex_buffer_data[] =
		{
			-0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, 0.5f,

			-0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, 0.5f,
			-0.5f, -0.5f, 0.5f
		};

		// Create index buffer
		static const WORD g_cube_index_buffer_data[] =
		{
			3, 1, 0,
			2, 1, 3,

			6, 4, 5,
			7, 4, 6,

			3, 4, 7,
			0, 4, 3,

			1, 6, 5,
			2, 6, 1,

			0, 5, 4,
			1, 5, 0,

			2, 7, 6,
			3, 7, 2
		};

		m_rCubeVertexBufferID = g_pApp->m_pRenderer->addVertexBuffer(24 * sizeof(float), STATIC, g_cube_vertex_buffer_data);
		m_rCubeIndexBufferID = g_pApp->m_pRenderer->addIndexBuffer(36, sizeof(WORD), STATIC, g_cube_index_buffer_data);
	}

	return true;

}

void VoxelManager::UpdateDebug()
{
	if (!m_bActiveDebug)
		return;

	shared_ptr<Renderer> pRenderer = g_pApp->m_pRenderer;

	UINT workGroupSize[3] = { 16, 8, 1 };

	pRenderer->reset();
	pRenderer->setShader(m_rOccluderVoxelCountShaderID);

	pRenderer->setTexture("voxelGrids", m_rVoxelGBuffer);
	pRenderer->setImageBuffer("argBuffer", m_rIndirectDrawArgBuffer, -1, FORMAT_RGBA8, WRITE_ONLY);
	pRenderer->setStructuredBuffer("perInstancePosition", m_rVoxelPositionBuffer);

	pRenderer->setShaderConstant1i("cascadeResolution", m_uiCascadeResolution);
	pRenderer->setShaderConstant1i("cascade", m_uiDebugCascade);

	pRenderer->apply();

	pRenderer->dispatchCompute(
		(m_uiCascadeResolution + workGroupSize[0] - 1) / workGroupSize[0],
		(m_uiCascadeResolution + workGroupSize[1] - 1) / workGroupSize[1],
		(m_uiCascadeResolution + workGroupSize[2] - 1) / workGroupSize[2]);
}

void VoxelManager::DrawVoxels(const UINT type, Scene* pScene)
{
	if (!m_bActiveDebug)
		return;

	shared_ptr<Renderer> pRenderer = g_pApp->m_pRenderer;

	Vec3 vCascadePos = m_vLastCascadePosition[m_uiDebugCascade];

	//Draw Voxels

	//pRenderer->setViewport(1024, 1024);
	//pRenderer->changeToMainFramebuffer();

	pRenderer->reset();
	pRenderer->setShader(m_rDrawVoxelShaderID);

	//pRenderer->setDepthState(pScene->m_noDepthTest);

	pRenderer->setShaderConstant4x4f("View", *((Mat4x4*)&pScene->GetCamera()->GetView()));
	pRenderer->setShaderConstant4x4f("Projection", *((Mat4x4*)&pScene->GetCamera()->GetProjection()));

	pRenderer->setShaderConstant1i("NBCELLS", m_uiCascadeResolution);
	pRenderer->setShaderConstant3f("g_SnappedGridPosition", vCascadePos);
	pRenderer->setShaderConstant1f("g_CellSize", m_fDebugCellTam);

	pRenderer->setTexture("voxelGrids", m_rVoxelRadiance);
	pRenderer->setStructuredBuffer("voxelParameters", m_rVoxelPositionBuffer);

	pRenderer->setShaderConstant1i("cascadeResolution", m_uiCascadeResolution);
	pRenderer->setShaderConstant1i("cascade", m_uiDebugCascade);
	pRenderer->setShaderConstant1i("cascadeFace", m_uiDebugCascadeFace);

	VertexBufferID vertexBuffer[1] = { m_rCubeVertexBufferID };
	UINT offset[1] = { 0 };

	pRenderer->setVertexFormat(m_rDrawVoxelShaderVFID);
	pRenderer->setVertexBuffer(0, 1, vertexBuffer, offset);
	pRenderer->setIndexBuffer(m_rCubeIndexBufferID);

	pRenderer->apply();

	pRenderer->drawElementsInstancedIndirect(PRIM_TRIANGLES, m_rIndirectDrawArgBuffer, 0);
}