//========================================================================
// VoxelManager.h - Defines the manager which make the scene
// voxelization.
//
//========================================================================

#ifndef VOXEL_MANAGER_H_
#define VOXEL_MANAGER_H_

#include "Scene.h"
#include "Renderer.h"

// VoxelManager class //////////////////////////////////////////////////////

class VoxelManager
{
private:
	// Cascades Attributes ///////////////////////
	bool m_bGIActive, m_bActiveVolumeLighting, m_bActiveFill, m_bActiveTwoBounces;
	TextureID m_rVoxelGBuffer;
	TextureID m_rVoxelRadiance;
	UINT m_uiNumCascades;
	UINT m_uiCascadeResolution;
	UINT m_uiVolume;
	float m_vfCascadesDimensions[6]; //Fixed cascade dimension

	Vec4 m_vVolumeAmbientColor;
	Vec3 m_vVolumeLightingGridResolution;

	Mat4x4 *m_pWorldToCascadeMatrix;
	Mat4x4 *m_pCascadeToWorldMatrix;
	Mat4x4 *m_pWorldToUnitMatrix;
	Aabb *m_pCascadeAabb;
	//////////////////////////////////////////////

    float m_fLog2Near;
    float m_fLog2Far_Near;
	float m_fGridVoxelTam;

	// Cone Tracing Target ////////////////////
	TextureID m_rIndirectLightRT;
	//////////////////////////////////////////////

    // Volumetric Effect Target /////////////////
    TextureID m_rVolumeLightingRT;
    /////////////////////////////////////////////

	// Volumetric Effect Grid ///////////////////
	TextureID m_rVolumeLightingData;
	TextureID m_rAccumulateVolumeLightingData;
	//////////////////////////////////////////////

	// Auxiliar Textures /////////////////////////
	
	TextureID m_rBounceVoxelGridAux;
	//////////////////////////////////////////////

	// Shaders ///////////////////////////////////
	// GI Shaders
	ShaderID m_rInjectLightShaderID;
	ShaderID m_rClearVoxelShaderID;
	ShaderID m_rOccluderVoxelCountShaderID;
	ShaderID m_rMipMapShaderID;
	ShaderID m_rDrawVoxelShaderID;
	ShaderID m_rCopyVoxelGridShaderID;
	ShaderID m_rVoxelConeTracingShaderID;
	ShaderID m_rTwoBounceRadianceShaderID;
	ShaderID m_rCopyTwoBounceRadianceShaderID;
	VertexFormatID m_rDrawVoxelShaderVFID;
	VertexFormatID m_rVoxelConeTracingShaderVFID;

	// Volume Lighting Shaders
	ShaderID m_rVolumeLightingShaderID;
	ShaderID m_rScatteringSolverShaderID;
	ShaderID m_rVolumeRenderToRenderTargetID;

	VertexFormatID m_rVolumeRenderToRenderTargetVFID;
	//////////////////////////////////////////////

	float m_fCellTam;
	float m_fCascadeExcentrity;

	// Debug Attributes //////////////////////////	
	StructuredBufferID m_rIndirectDrawArgBuffer, m_rVoxelPositionBuffer;
	VertexBufferID m_rCubeVertexBufferID;
	IndexBufferID m_rCubeIndexBufferID;
	Vec3 m_vLastCascadePosition[6];

	float m_fDebugCellTam;
	bool m_bActiveDebug;
	UINT m_uiDebugCascade;
	UINT m_uiDebugCascadeFace;
	//////////////////////////////////////////////

	// Private Methods /////////////////////////////
	int ChooseCascade();
	void ClearCascade(int cascade);
	void VoxelizeScene(int cascade, Scene* pScene);
	void InjectLightToVoxel(std::list<std::shared_ptr<SceneNode>> LightList, int cascade, Scene* pScene);
	void UpdateMatrix(int cascade, Scene* pScene);
    void UpdateMipMap(int cascade);

    void DensityEstimationVolumeLighting(Scene* pScene);
	void SolveScattering(Scene* pScene);
	void VolumeDataToRenderTarget(Scene* pScene);
	/////////////////////////////////////////////////

	UINT m_uiCount;	// Choose cascade count

public:
	TextureID m_rVoxelGridMipMaps[2];

	VoxelManager() {
		m_bGIActive = false; m_bActiveVolumeLighting = false; m_uiCount = 0; m_bActiveDebug = false; m_rIndirectLightRT = -1; m_uiDebugCascade = 0;
		m_rVolumeLightingRT = -1; m_rOccluderVoxelCountShaderID = -1; m_rIndirectDrawArgBuffer = -1; m_rVoxelPositionBuffer = -1; m_rCubeVertexBufferID = -1; m_rCubeIndexBufferID = -1;
		m_uiDebugCascadeFace = 1; m_fCascadeExcentrity = 0.0f; m_rVolumeLightingData = -1; m_rVoxelRadiance = -1; m_rVoxelGBuffer = -1;
	}
	~VoxelManager();

	bool Initialize(const bool bActiveGI, const bool bActiveVL, const UINT voxelTextureResolution, const Vec3 volumeLightingGridResolution, const UINT num_VoxelCascades, const float fCascadeExcentrity);
	bool SetShaderForVoxel(int cascadeIndex);
	bool UpdateGICascades(Scene* pScene);
	
	void setActiveGI(bool active);
	void SetActiveTwoBounces(bool active);
	void SetActiveVolumeLighting(bool active);
	bool IsActiveGI() { return m_bGIActive; }
	bool IsActiveVolumeLighting() { return m_bActiveVolumeLighting; }
	void setActiveFill(bool active) { m_bActiveFill = active; }
	bool IsActiveFill() { return m_bActiveFill; }
	void SetCascadeExcentrity(float fCascadeExcentrity) { m_fCascadeExcentrity = 0.5f*fCascadeExcentrity; }

	// Voxel Cone Tracing Methods ///////////////////////////
	void VoxelConeTracing(Scene* pScene);
	void TwoBounceRadiance(int cascadeIndex, Scene* pScene);
	/////////////////////////////////////////////////////////

    // Volume Lighting Methods ///////////////////
    void SolveVolumeLighting(Scene* pScene);
    /////////////////////////////////////////////////////////

	// Debug Methods ////////////////////////////////////////
	bool ActiveDebugMode(const bool active);
	void SetDebugCascade(const UINT cascade) { m_uiDebugCascade = cascade; }
	void DrawVoxels(const UINT type, Scene* pScene);
	void UpdateDebug();
	/////////////////////////////////////////////////////////

	//TextureID getTextureCascade(int cascadeIndex, int indexTexture) { return ( cascadeIndex < m_vVoxelCascades.size() ) ? m_vVoxelCascades[cascadeIndex].voxelRadiance[indexTexture] : TEXTURE_NONE; }
	//int getNumVoxelCascades() { return m_vVoxelCascades.size(); }
	int getCascadeResolution() { return m_uiCascadeResolution; }
	TextureID getVoxelizationDataScene() { return m_rVoxelGBuffer; }
	TextureID getGISceneData() { return m_rVoxelRadiance; }
	TextureID getIndirectDiffuseRT() { return m_rIndirectLightRT; }
    TextureID getVolumeLightingRT() { return m_rVolumeLightingRT; }
	bool insideCascade(Aabb aabb, int cascadeIndex);
};

#endif