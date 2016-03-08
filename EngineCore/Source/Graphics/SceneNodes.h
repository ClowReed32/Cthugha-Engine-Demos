#pragma once
//========================================================================
// SceneNodes.h - defines the base class for the 3D graphics scene graph, and
//                  derived classes like RootNode, Alpha scene nodes, Camera, Grids, etc.                  
//
//========================================================================


#include "Geometry.h"
//#include "Material.h"
#include "..\Msvc\CthughaStd.h"
//#include "Shaders.h"

// Forward declarations
class SceneNode;
class Scene;
//class RayCast;
//class MovementController;
//class IResourceExtraData;
class EntityComponent;
class BaseRenderComponent;
class ShaderNode;
class MaterialNode;

typedef BaseRenderComponent* WeakBaseRenderComponentPtr;

struct RenderProperties
{
	bool bGlobalIllumination;
	bool bShadowGeneratedAsTwoSided;
	bool bCastVolumetricShadows;
	bool CastShadows;
	bool ActivePOM;
	bool Mirrored;
};

//////////////////////////////////////////////////////////////
//
// SceneNodeList
//
//   Every scene node has a list of its children - this 
//   is implemented as a vector since it is expected that
//   we won't add/delete nodes very often, and we'll want 
//   fast random access to each child.
//
//////////////////////////////////////////////////////////////

typedef std::vector<shared_ptr<ISceneNode> > SceneNodeList;


//////////////////////////////////////////////////////////////
//
// SceneNode
//
//   Implements ISceneNode. Forms the base class for any node
//   that can exist in the 3D scene graph managed by class Scene.
//
//////////////////////////////////////////////////////////////

class SceneNode : public ISceneNode
{
	friend class Scene;

protected:
	SceneNodeList			m_Children;
	std::map<EntityId, SceneNode*>	m_pParent;
	EntityId				m_EntityID;	//Some class don´t need this, but need for removeChild;

	//Properties
	std::string				m_Name;
	Aabb					m_Aabb;
	bool					m_IsVisible;
	Mat4x4					m_ToWorld, m_FromWorld;
	bool					m_InQueueRender;
	bool					m_InQueueFrustum;
	bool					m_InQueueLight;
	bool					m_bWasDrawn;

	bool					m_bCastShadow;
	bool					m_bIsStatic;
	bool					m_bIsDrawable;

	UINT					m_QueryResultPreviousFrame; //OcclusionQuery Result
	UINT					m_QueryResultPresentFrame;

public:
	SceneNode();

	virtual ~SceneNode();

	//////////////////////////////////////////////////////////////////////
	std::string getNodeName() { return m_Name; }
	Aabb getNodeAabb() { return m_Aabb; }
	Mat4x4 const &ToWorld() const { return m_ToWorld; }
	Mat4x4 const &FromWorld() const { return m_FromWorld; }
	//////////////////////////////////////////////////////////////////////

	virtual void VSetTransform(const Mat4x4 *toWorld, const Mat4x4 *fromWorld=NULL);
	void Transform(Mat4x4 *toWorld, Mat4x4 *fromWorld) const;

	virtual bool VOnRestore(Scene *pScene);
	virtual bool VOnUpdate(Scene *, DWORD const elapsedMs);

	virtual bool VIsDrawable() const { return m_bIsDrawable; }
	virtual bool VIsVisible() const;
	virtual bool VWasDrawn() const { return m_bWasDrawn; }
	virtual void VSetWasDrawn(const bool bWasDraw) { m_bWasDrawn = bWasDraw; }
    virtual int VGetOrder() { return -1; }
	bool IsVisibleFromCamera(Scene *pScene) const;
	bool IsVisibleFromFrustum(Frustum frustum, Mat4x4 frustumMat, Scene *pScene) const;
	bool IntersectAabb(const Aabb& aabb, Scene *pScene) const;
	virtual bool VIsInRenderQueue() const { return m_InQueueRender; }
	virtual bool VIsStaticNode() const { return m_bIsStatic; }
	bool IsInFrustumQueue() const { return m_InQueueFrustum; }
	void SetInFrustumQueue(bool isIn) { m_InQueueFrustum = isIn; }
	bool getCastShadows() { return m_bCastShadow; }
	UINT* getQueryResultPreviousFramePtr() { return &m_QueryResultPreviousFrame; }
	UINT* getQueryResultPresentFramePtr() { return &m_QueryResultPresentFrame; }
	
	virtual void VSetVisible(bool bVisible) { m_IsVisible = bVisible; }

	virtual bool VAddChild(shared_ptr<ISceneNode> kid);
	virtual bool VRemoveChild(EntityId id);
	virtual bool VOnLostDevice(Scene *pScene);
	virtual void VRemoveParent(EntityId id);
    virtual bool VOnRender(Scene *pScene) { return true; }
	virtual bool VOnSimpleRender(Scene *pScene) { return true; }
	virtual SceneNodeType VGetType() { return GENERIC_NODE; }
	//virtual HRESULT VPick(Scene *pScene, RayCast *pRayCast);
	
	virtual Aabb VGetAabb() { return m_Aabb; }
	virtual EntityId VGetEntityID() { return m_EntityID; }
	void SetNodeAabb(const Aabb aabb) { m_Aabb = aabb; }
};

/////////////////////////////////////////////////////////////////////
//
//	DrawableNode
//
//	Nodes that can be drawn
//
//////////////////////////////////////////////////////////////////////

class DrawableNode : public SceneNode
{
protected:
	bool m_bHasShadowShader, m_bHasGIShader, m_bStatic;
    shared_ptr<ShaderNode> m_rShadowShaderNode, m_pGIShaderNode;
	shared_ptr<MaterialNode> m_pGIMaterialNode;

public:
	DrawableNode() { m_bHasShadowShader = false; m_bHasGIShader = false; m_bIsDrawable = true; }
	~DrawableNode() { }

	virtual bool OnRenderWithExternalShader(Scene *pScene) { return true; }

	bool HasShadowShader() { return m_bHasShadowShader; }
	bool HasGIShader() { return m_bHasGIShader; }
	shared_ptr<ShaderNode> GetShadowShaderNode() { return m_rShadowShaderNode; }
	shared_ptr<ShaderNode> GetGIShaderNode() { return m_pGIShaderNode; }
	shared_ptr<MaterialNode> GetGIMaterialNode() { return m_pGIMaterialNode; }
};

////////////////////////////////////////////////////
//
// SceneActorMap Description
//
//   An STL map that allows fast lookup of a scene node given an ActorId.
//
////////////////////////////////////////////////////

typedef std::map<EntityId, shared_ptr<ISceneNode> > SceneActorMap;


////////////////////////////////////////////////////
//
// Scene Description
//
// A heirarchical container of scene nodes, which
// are classes that implemente the ISceneNode interface
//
////////////////////////////////////////////////////

class CameraNode;

////////////////////////////////////////////////////
//
// class OctreeNode					
//
////////////////////////////////////////////////////

class OctreeNode : public SceneNode
{
private:
	bool m_IsLeaf;
	std::shared_ptr<OctreeNode> m_vOctreeChildren[8];
	int m_NumNodes;

public:
	OctreeNode() { m_NumNodes = 0; m_IsLeaf = true; m_ToWorld = m_ToWorld.g_Identity; m_FromWorld = m_FromWorld.g_Identity; m_bIsDrawable = false; }

	virtual bool VOnRestore(Scene *pScene);
	virtual bool VOnLostDevice(Scene *pScene) { return true; }
	virtual bool VIsVisible(Scene *pScene) const { return true; }

	virtual SceneNodeType VGetType() { return OCTREE_NODE; }

	bool IsLeaf() { return m_IsLeaf; } 
	bool AddChildrenToOctree(Scene *pScene, shared_ptr<ISceneNode> kid);
	bool AddChildrenToNode(Scene *pScene, std::list<std::shared_ptr<ISceneNode>> nodeList);
    bool AddChildrenToNode(Scene *pScene, std::map<EntityId, std::shared_ptr<ISceneNode>> nodeList);
    void InitializeChildren();
	bool RemoveChildrenToOctree(Scene *pScene, EntityId id);
	void TraverseOctree(Scene *pScene);
	void TraverseOctreeFromSpecificFrustum(std::vector<std::shared_ptr<SceneNode>> &frustumQueue, Frustum frustum, Mat4x4 frustumMat, Scene *pScene);
	void TraverseOctreeFromAabb(std::list<std::shared_ptr<SceneNode>> &AabbQueue, std::list<std::shared_ptr<SceneNode>> &AabbLightQueue, Aabb aabb, Scene *pScene);
	void TraverseOctreeFromAabb(std::vector<std::shared_ptr<SceneNode>> &AabbLightQueue, Aabb aabb, Scene *pScene);

private:
	void RemoveChildrenFromThisNode();
};

////////////////////////////////////////////////////
//
// class RootNode					
//
//    This is the root node of the scene graph.
//
////////////////////////////////////////////////////

/*class RootNode : public SceneNode
{
public:
	RootNode();
	virtual bool VAddChild(shared_ptr<ISceneNode> kid);
	virtual HRESULT VRenderChildren(Scene *pScene);
	virtual bool VRemoveChild(ActorId id);
	virtual bool VIsVisible(Scene *pScene) const { return true; }
};*/


////////////////////////////////////////////////////
//
// class CameraNode					- Chapter 16, page 548
//
//    A camera node controls the D3D view transform and holds the view
//    frustum definition
//
////////////////////////////////////////////////////

class CameraNode : public SceneNode
{
public:
	CameraNode(Mat4x4 const *t, Frustum const &frustum) 
	 : SceneNode(),
	   m_Frustum(frustum),
	   m_bActive(true),
	   m_DebugCamera(false),
	   m_pTarget(shared_ptr<SceneNode>()),
	   m_CamOffsetVector( 0.0f, 1.0f, -10.0f, 0.0f )
	{ 
		m_bIsDrawable = false;
	}

	virtual bool VOnRestore(Scene *pScene);
	virtual bool VIsVisible(Scene *pScene) const { return m_bActive; }

	virtual SceneNodeType VGetType() { return CAMERA_NODE; }

	const Frustum &GetFrustum() { return m_Frustum; }
	void SetTarget(shared_ptr<SceneNode> pTarget)
		{ m_pTarget = pTarget; }
	void ClearTarget() { m_pTarget = shared_ptr<SceneNode>(); }
	shared_ptr<SceneNode> GetTarget() { return m_pTarget; }

	Mat4x4 GetViewProjection(Scene *pScene);
	bool SetViewTransform(Scene *pScene);

	Mat4x4 GetProjection() { return m_Projection; }
	Mat4x4 GetInverseProjection() { return m_InverseProjection; }
	Mat4x4 GetView() { return m_View; }
	Mat4x4 GetInverseView() { return m_InverseView; }

	void SetCameraOffset( const Vec4 & cameraOffset )
	{
		m_CamOffsetVector = cameraOffset;
	}

protected:

	Frustum			m_Frustum;
    Mat4x4			m_Projection;
	Mat4x4			m_InverseProjection;
	Mat4x4			m_View;
	Mat4x4			m_InverseView;
	bool			m_bActive;
	bool			m_DebugCamera;
	shared_ptr<SceneNode> m_pTarget;
	Vec4			m_CamOffsetVector;	//Direction of camera relative to target.
};

