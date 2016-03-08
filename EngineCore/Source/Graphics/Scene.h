#pragma once
//========================================================================
// File: Scene.h - implements the container class for 3D Graphics scenes
//
//========================================================================


#include "Geometry.h"
#include "SceneNodes.h"

// Forward declarations
////////////////////////////////////////////////////
//
// SceneActorMap Description				- Chapter 16, page Y
//
//   An STL map that allows fast lookup of a scene node given an ActorId.
//
////////////////////////////////////////////////////

typedef std::map<EntityId, shared_ptr<ISceneNode> > SceneActorMap;

// Flat framebuffer RGBA16-encoded
struct FramebufferFlatElement
{
	UINT rb;
	UINT ga;
};


////////////////////////////////////////////////////
//
// Scene Description						- Chapter 16, page 536
//
// A heirarchical container of scene nodes, which
// are classes that implemente the ISceneNode interface
//
////////////////////////////////////////////////////

class CameraNode;
class LightManager;
class ScreenPostProcessing;
//class LightNode;

class Scene
{
protected:
	enum RenderState { VOXELIZATION_STATE, FILLBUFFER_STATE, LIGHTING_STATE, DEPTH_STATE, POSTPROCESSING_STATE };

	shared_ptr<OctreeNode> 	m_Root; //Static Object Octree Root
	std::map<EntityId, shared_ptr<ISceneNode>>   m_DynamicObjectList; //Dynamic object List
    std::list<std::shared_ptr<ISceneNode>> m_lStaticSceneNodes; //Static Object List
	shared_ptr<CameraNode> 	m_Camera;
	shared_ptr<Renderer>	m_pRenderer;

	std::shared_ptr<ScreenPostProcessing> m_pScreenPostprocessing;

	//Manage blocks of light for shader
	shared_ptr<LightManager> m_pLightManager;

	//ID3DXMatrixStack 		*m_MatrixStack;
	SceneActorMap 			m_ActorMap;

	std::list<shared_ptr<SceneNode>> m_RenderQueue;
	std::list<shared_ptr<SceneNode>> m_ForwardRenderQueue;
	std::list<shared_ptr<SceneNode>> m_DecalRenderQueue;
	std::list<shared_ptr<SceneNode>> m_LightQueue;

    //screen aligned quad attributes
    //Take it to other place?
	VertexFormatID m_DepthShaderVFID;
	VertexFormatID m_TestTargetShaderVFID;

    VertexBufferID m_ScreenAlignedQuadVBID;
	VertexBufferID m_ZeroCenteredUnitCubeVBID;

	ShaderID	   m_TileShaderID;
	ShaderID       m_LightShaderID;
	ShaderID	   m_DepthShaderID;
    ShaderID       m_TestTargetShaderID;
	
    //Target Surface for G-Buffer deferred Rendering
	TextureID		   m_TilePositionRT;
    TextureID          m_PositionRT;
    TextureID          m_ColorRT;
    TextureID		   m_DepthRT;
	TextureID		   m_NormalRT;
	TextureID		   m_TangentRT;
	TextureID		   m_MREC_RT;
	TextureID		   m_ForwardRenderSceneRT;
	StructuredBufferID m_LightingSceneRT;
	//TextureID	       m_PostProcessingRT;
	
	//Custom Postprocessing Effect
	void* m_pUserContext;
	void(*m_tOnCustomPostProcessingEffect)(Scene* pScene, void* pUserContext);

	friend class ShadowMapManager;
	friend class VoxelManager;
	friend class SDFShadowManager;
	friend class ScreenPostProcessing;

public:
	//Render States can be used by others render class//
	//Sampler States
	SamplerStateID m_linearClamp;
	SamplerStateID m_bilinearClamp;
    SamplerStateID m_trilinearClamp;
	SamplerStateID m_trilinearWrap;
	SamplerStateID m_trilinearMirror;
	SamplerStateID m_pointClamp;
	SamplerStateID m_linearWrap;

    SamplerStateID m_trilinear[3];

	//Rasterize States
	RasterizerStateID m_backFaceCulling;
	RasterizerStateID m_noneFaceCulling;

	//Depth States
	DepthStateID m_depthTest;
	DepthStateID m_noDepthTest;
	DepthStateID m_depthTestNoWrite;

	//Blend States
	BlendStateID m_blendAdd;
	BlendStateID m_blendSrcAlpha;
	//////////////////////////////////////////////////////

	int						m_NumOctreeNodes; //Help to EntityId OctreeNodes
    RenderState             m_RenderPass;

	Scene(shared_ptr<Renderer> renderer);
	virtual ~Scene();

	bool OnRender();
	bool OnRestore();
	bool OnLostDevice();
	bool OnUpdate(const int deltaMilliseconds);
	shared_ptr<ISceneNode> FindEntity(EntityId id);
	bool AddChild(EntityId id, shared_ptr<ISceneNode> kid);
	bool AddDynamicSceneNode(EntityId id, shared_ptr<ISceneNode> object);
    bool BuildOctree();
	bool RemoveChild(EntityId id);
	void AddNodeToRenderQueue(shared_ptr<SceneNode> node);
	void AddNodeToLightQueue(shared_ptr<SceneNode> node) { m_LightQueue.push_back(node); node->m_InQueueLight = true; }
	void RemoveLastNodeToRenderQueue() { m_RenderQueue.back()->m_InQueueRender = false; m_RenderQueue.back()->m_bWasDrawn = true; m_RenderQueue.pop_back(); }
	void RemoveLastNodeToFordwardRenderQueue() { m_ForwardRenderQueue.back()->m_InQueueRender = false; m_ForwardRenderQueue.back()->m_bWasDrawn = true; m_ForwardRenderQueue.pop_back(); }
	void RemoveLastNodeToDecalRenderQueue() { m_DecalRenderQueue.back()->m_InQueueRender = false; m_DecalRenderQueue.back()->m_bWasDrawn = true; m_DecalRenderQueue.pop_back(); }
	void RemoveFrontLightNodeToLightQueue() { m_LightQueue.front()->m_InQueueLight = false; m_LightQueue.pop_front(); }
	void TraverseDynamicObject();
	void TraverseDynamicObjectFromSpecificFrustum(std::vector<std::shared_ptr<SceneNode>> &frustumQueue, Frustum frustum, Mat4x4 frustumMat);
	void TraverseDynamicObjectFromAabb(std::list<std::shared_ptr<SceneNode>> &AabbQueue, std::list<std::shared_ptr<SceneNode>> &AabbLightQueue, Aabb aabb);
	void TraverseDynamicObjectFromAabb(std::vector<std::shared_ptr<SceneNode>> &AabbLightQueue, Aabb aabb);
	std::list<std::shared_ptr<SceneNode>> TraverseDynamicObjectFromAabb(const Aabb aabb);

    // event delegates
    void NewRenderComponentDelegate(IEventDataPtr pEventData);
    void ModifiedRenderComponentDelegate(IEventDataPtr pEventData);			// added post-press!
    void DestroyActorDelegate(IEventDataPtr pEventData);
    void MoveActorDelegate(IEventDataPtr pEventData);
    //void LevelLoadedDelegate(IEventDataPtr pEventData);

	void SetCamera(shared_ptr<CameraNode> camera) { m_Camera = camera; }
	const shared_ptr<CameraNode> GetCamera() const { return m_Camera; }

	const VertexBufferID GetAlignedScreenVertexBufferID() const { return m_ScreenAlignedQuadVBID; }
	const VertexBufferID GetZeroCenteredUnitCubeVertexBufferID() const { return m_ZeroCenteredUnitCubeVBID; }
	const std::list<shared_ptr<SceneNode>> GetLightQueue() const { return m_LightQueue; }
	const TextureID GetPositionRT() { return m_PositionRT; }
	const TextureID GetNormalRT() { return m_NormalRT; }
	const TextureID GetLightingSceneRT() { return m_LightingSceneRT; }
	std::map<EntityId, shared_ptr<ISceneNode>> GetDynamicObjectList() { return m_DynamicObjectList; }
	std::list<std::shared_ptr<ISceneNode>> GetStaticObjectList() { return m_lStaticSceneNodes; }
	std::shared_ptr<LightManager> GetLightManager() { return m_pLightManager; }
	std::shared_ptr<ScreenPostProcessing> GetScreenPostProcessing() { return m_pScreenPostprocessing; }

	void SetCustomPostProcessingMethod(void(*pOnRenderShadowMethods)(Scene* pScene, void* pUserContext), void* pUserContext)
	{
		m_tOnCustomPostProcessingEffect = pOnRenderShadowMethods;
		m_pUserContext = pUserContext;
	}

	//HRESULT Pick(RayCast *pRayCast) { return m_Root->VPick(this, pRayCast); }

	shared_ptr<Renderer> GetRenderer() { return m_pRenderer; }
};

