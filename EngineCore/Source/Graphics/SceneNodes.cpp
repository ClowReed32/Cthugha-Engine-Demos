//========================================================================
// SceneNodes.cpp - defines the base class for the 3D graphics scene graph, and
//                  derived classes like RootNode, Alpha scene nodes, Camera, Grids, etc.                  
//
//========================================================================

#include "CthughaStd.h"

#include "..\CthughaEngine\CthughaApp.h"
#include "..\Entities\EntityComponent.h"
#include "..\Entities\RenderComponent.h"

#include "Renderer.h"
#include "Geometry.h"
#include "Light.h"
//#include "Raycast.h"
#include "SceneNodes.h"
//#include "Shaders.h"
#include "tchar.h"
#include "../ResourceCache/ResCache.h"

#define UP_NE 0 // upper north east
#define UP_NW 1 // upper north west
#define UP_SE 2 // upper south east
#define UP_SW 3 // upper south west
#define LW_NE 4 // lower north east
#define LW_NW 5 // lower north west
#define LW_SE 6 // lower south east
#define LW_SW 7 // lower south west

////////////////////////////////////////////////////
// SceneNode Implementation
////////////////////////////////////////////////////

SceneNode::SceneNode() 
{ 
	m_IsVisible = true;
	m_EntityID = -1;
	m_InQueueRender = false;
	m_bCastShadow = false;
	m_QueryResultPreviousFrame = 0;
	m_QueryResultPresentFrame = 0;
	m_bIsDrawable = false;
}


SceneNode::~SceneNode()
{
}

//
// SceneNode::VOnRestore					- Chapter 16, page 532
//
bool SceneNode::VOnRestore(Scene *pScene)
{
	// This is meant to be called from any class
	// that inherits from SceneNode and overloads
	// VOnRestore()

	SceneNodeList::iterator i = m_Children.begin();
	SceneNodeList::iterator end = m_Children.end();
	while (i != end)
	{
		(*i)->VOnRestore(pScene);
		++i;
	}
	return true;
}


//
// SceneNode::VOnLostDevice					
//
bool SceneNode::VOnLostDevice(Scene *pScene)
{
	// This is meant to be called from any class
	// that inherits from SceneNode and overloads
	// VOnRestore()

	SceneNodeList::iterator i = m_Children.begin();
	SceneNodeList::iterator end = m_Children.end();
	while (i != end)
	{
		(*i)->VOnLostDevice(pScene);
		++i;
	}
	return true;
}

//
// SceneNode::VIsVisible				
//	
bool SceneNode::VIsVisible() const
{
	return m_IsVisible;
}

//
// SceneNode::IsVisibleFromCamera
//
bool SceneNode::IsVisibleFromCamera(Scene *pScene) const
{
	if(!m_IsVisible)
		return false;

	Frustum const &frustum = pScene->GetCamera()->GetFrustum();

    Aabb aabbT = m_Aabb.Transform(m_ToWorld);

	bool isVisible = frustum.Inside(aabbT, pScene->GetCamera()->ToWorld());
	return isVisible;	
}

//
// SceneNode::IsVisibleFromFrustum
//
bool SceneNode::IsVisibleFromFrustum(Frustum frustum, Mat4x4 frustumMat, Scene *pScene) const
{
	if(!m_IsVisible)
		return false;

	Frustum const &frustumC = frustum;

    Aabb aabbT = m_Aabb.Transform(m_ToWorld);

	bool isVisible = frustumC.Inside(aabbT, frustumMat);
	return isVisible;	
}

//
// SceneNode::IntersectAabb
//
bool SceneNode::IntersectAabb(const Aabb& aabb, Scene *pScene) const
{
	if(!m_IsVisible)
		return false;

    Mat4x4 mAbsToWorld = Mat4x4(abs(m_ToWorld.rows[0].data[0]), abs(m_ToWorld.rows[0].data[1]), abs(m_ToWorld.rows[0].data[2]), 0.0f,
        abs(m_ToWorld.rows[1].data[0]), abs(m_ToWorld.rows[1].data[1]), abs(m_ToWorld.rows[1].data[2]), 0.0f,
        abs(m_ToWorld.rows[2].data[0]), abs(m_ToWorld.rows[2].data[1]), abs(m_ToWorld.rows[2].data[2]), 0.0f,
        abs(m_ToWorld.rows[3].data[0]), abs(m_ToWorld.rows[3].data[1]), abs(m_ToWorld.rows[3].data[2]), 1.0f);

    Vec3 vcCenterWorld = (m_ToWorld*Vec4(m_Aabb.vcCenter, 1.0f)).xyz();
    Vec3 vcExtendWorld = (mAbsToWorld*Vec4(m_Aabb.vcMax - m_Aabb.vcCenter, 0.0f)).xyz();

	Aabb aabbT;
    aabbT.vcMin = vcCenterWorld - vcExtendWorld;
    aabbT.vcMax = vcCenterWorld + vcExtendWorld;

	bool instersect = aabbT.Intersects(aabb);
	return instersect;
}

//
// SceneNode::VOnUpdate					- Chapter 16, page 532
//
bool SceneNode::VOnUpdate(Scene *pScene, DWORD const elapsedMs)
{
	// This is meant to be called from any class
	// that inherits from SceneNode and overloads
	// VOnUpdate()

	SceneNodeList::iterator i = m_Children.begin();
	SceneNodeList::iterator end = m_Children.end();

	while (i != end)
	{
		(*i)->VOnUpdate(pScene, elapsedMs);
		++i;
	}
	return S_OK;
}

//
// SceneNode::VAddChild					
//
bool SceneNode::VAddChild(shared_ptr<ISceneNode> ikid)
{
	m_Children.push_back(ikid); 

	shared_ptr<SceneNode> kid = static_pointer_cast<SceneNode>(ikid);

	kid->m_pParent[m_EntityID] = this;					

	return true; 
}

void SceneNode::VRemoveParent(EntityId id)
{
	m_pParent.erase(id);
}

//
// SceneNode::VRemoveChild					
//
//   If an actor is destroyed it should be removed from the scene graph.
//   Generally the HumanView will recieve a message saying the actor has been
//   destroyed, and it will then call Scene::RemoveChild which will traverse 
//   the scene graph to find the child that needs removing.
//
bool SceneNode::VRemoveChild(EntityId id)
{
	for(SceneNodeList::iterator i=m_Children.begin(); i!=m_Children.end(); ++i)
	{
		if(m_EntityID != INVALID_ENTITY_ID && id == m_EntityID)
		{
			i = m_Children.erase(i);	//this can be expensive for vectors
			return true;
		}
	}
	return false;
}

//
// SceneNode::VSetTransform
//

void SceneNode::VSetTransform(const Mat4x4 *toWorld, const Mat4x4 *fromWorld)
{
	m_ToWorld = *toWorld;
	if (!fromWorld)
	{
		m_FromWorld = m_ToWorld.inverse();
	}
	else
	{
		m_FromWorld = *fromWorld;
	}
}

//
// SceneNodeProperties::Transform
//
void SceneNode::Transform(Mat4x4 *toWorld, Mat4x4 *fromWorld) const
{
	if (toWorld)
		*toWorld = m_ToWorld;
	
	if (fromWorld)
		*fromWorld = m_FromWorld;
}

////////////////////////////////////////////////////
// RootNode Implementation
////////////////////////////////////////////////////

void OctreeNode::RemoveChildrenFromThisNode()
{
	for(UINT i=0; i < m_Children.size(); i++)
	{
		m_Children[i]->VRemoveParent(m_EntityID);
	}

	m_Children.clear();
}

bool OctreeNode::VOnRestore(Scene *pScene)
{
	if(IsLeaf())
	{
		for(UINT i=0; i < m_Children.size(); i++)
		{
			m_Children[i]->VOnRestore(pScene);
		}
	}
	else
	{
		for(UINT i=0; i < 8; i++)
		{
			m_vOctreeChildren[i]->VOnRestore(pScene);
		}
	}

	return true;
}

void OctreeNode::InitializeChildren()
{
	m_vOctreeChildren[0] = std::shared_ptr<OctreeNode>(CHG_NEW OctreeNode());
    m_vOctreeChildren[0]->m_Aabb.vcMax = Vec3(m_Aabb.vcCenter.x, m_Aabb.vcMax.y, m_Aabb.vcMax.z);
    m_vOctreeChildren[0]->m_Aabb.vcMin = Vec3(m_Aabb.vcMin.x, m_Aabb.vcCenter.y, m_Aabb.vcCenter.z);
    m_vOctreeChildren[0]->m_Aabb.vcCenter = (m_vOctreeChildren[0]->m_Aabb.vcMax + m_vOctreeChildren[0]->m_Aabb.vcMin) * 0.5f;
    
	m_vOctreeChildren[1] = std::shared_ptr<OctreeNode>(CHG_NEW OctreeNode());
	m_vOctreeChildren[1]->m_Aabb.vcMax = m_Aabb.vcMax;
    m_vOctreeChildren[1]->m_Aabb.vcMin = m_Aabb.vcCenter;
    m_vOctreeChildren[1]->m_Aabb.vcCenter = (m_vOctreeChildren[1]->m_Aabb.vcMax + m_vOctreeChildren[1]->m_Aabb.vcMin) * 0.5f;
    
	m_vOctreeChildren[2] = std::shared_ptr<OctreeNode>(CHG_NEW OctreeNode());
    m_vOctreeChildren[2]->m_Aabb.vcMax = Vec3(m_Aabb.vcCenter.x, m_Aabb.vcMax.y, m_Aabb.vcCenter.z);
    m_vOctreeChildren[2]->m_Aabb.vcMin = Vec3(m_Aabb.vcMin.x, m_Aabb.vcCenter.y, m_Aabb.vcMin.z);
    m_vOctreeChildren[2]->m_Aabb.vcCenter = (m_vOctreeChildren[2]->m_Aabb.vcMax + m_vOctreeChildren[2]->m_Aabb.vcMin) * 0.5f;
    
	m_vOctreeChildren[3] = std::shared_ptr<OctreeNode>(CHG_NEW OctreeNode());
    m_vOctreeChildren[3]->m_Aabb.vcMax = Vec3(m_Aabb.vcMax.x, m_Aabb.vcMax.y, m_Aabb.vcCenter.z);
    m_vOctreeChildren[3]->m_Aabb.vcMin = Vec3(m_Aabb.vcCenter.x, m_Aabb.vcCenter.y, m_Aabb.vcMin.z);
    m_vOctreeChildren[3]->m_Aabb.vcCenter = (m_vOctreeChildren[3]->m_Aabb.vcMax + m_vOctreeChildren[3]->m_Aabb.vcMin) * 0.5f;
    
	m_vOctreeChildren[4] = std::shared_ptr<OctreeNode>(CHG_NEW OctreeNode());
    m_vOctreeChildren[4]->m_Aabb.vcMax = Vec3(m_Aabb.vcCenter.x, m_Aabb.vcCenter.y, m_Aabb.vcMax.z);
    m_vOctreeChildren[4]->m_Aabb.vcMin = Vec3(m_Aabb.vcMin.x, m_Aabb.vcMin.y, m_Aabb.vcCenter.z);
    m_vOctreeChildren[4]->m_Aabb.vcCenter = (m_vOctreeChildren[4]->m_Aabb.vcMax + m_vOctreeChildren[4]->m_Aabb.vcMin) * 0.5f;
    
	m_vOctreeChildren[5] = std::shared_ptr<OctreeNode>(CHG_NEW OctreeNode());
    m_vOctreeChildren[5]->m_Aabb.vcMax = Vec3(m_Aabb.vcMax.x, m_Aabb.vcCenter.y, m_Aabb.vcMax.z);
    m_vOctreeChildren[5]->m_Aabb.vcMin = Vec3(m_Aabb.vcCenter.x, m_Aabb.vcMin.y, m_Aabb.vcCenter.z);
    m_vOctreeChildren[5]->m_Aabb.vcCenter = (m_vOctreeChildren[5]->m_Aabb.vcMax + m_vOctreeChildren[5]->m_Aabb.vcMin) * 0.5f;
    
	m_vOctreeChildren[6] = std::shared_ptr<OctreeNode>(CHG_NEW OctreeNode());
    m_vOctreeChildren[6]->m_Aabb.vcMax = m_Aabb.vcCenter;
    m_vOctreeChildren[6]->m_Aabb.vcMin = m_Aabb.vcMin;
    m_vOctreeChildren[6]->m_Aabb.vcCenter = (m_vOctreeChildren[6]->m_Aabb.vcMax + m_vOctreeChildren[6]->m_Aabb.vcMin) * 0.5f;
    
	m_vOctreeChildren[7] = std::shared_ptr<OctreeNode>(CHG_NEW OctreeNode());
    m_vOctreeChildren[7]->m_Aabb.vcMax = Vec3(m_Aabb.vcMax.x, m_Aabb.vcCenter.y, m_Aabb.vcCenter.z);
    m_vOctreeChildren[7]->m_Aabb.vcMin = Vec3(m_Aabb.vcCenter.x, m_Aabb.vcMin.y, m_Aabb.vcMin.z);
    m_vOctreeChildren[7]->m_Aabb.vcCenter = (m_vOctreeChildren[7]->m_Aabb.vcMax + m_vOctreeChildren[7]->m_Aabb.vcMin) * 0.5f;
}

bool OctreeNode::AddChildrenToNode(Scene *pScene, std::map<EntityId, std::shared_ptr<ISceneNode>> nodeList)
{
	m_NumNodes = nodeList.size();

	if (nodeList.size() > MAX_OCTREE_NODES)
	{
		std::list<std::shared_ptr<ISceneNode>> UNENodeList, UNWNodeList, USENodeList, USWNodeList,
			LNENodeList, LNWNodeList, LSENodeList, LSWNodeList;

		InitializeChildren();

		for (auto node : nodeList)
		{
			std::shared_ptr<SceneNode> sceneNode = std::static_pointer_cast<SceneNode>(node.second);
			Aabb aabbNode = node.second->VGetAabb().Transform(sceneNode->ToWorld());

			if (m_vOctreeChildren[0]->getNodeAabb().Intersects(aabbNode))
			{
				UNENodeList.push_back(node.second);
			}
			if (m_vOctreeChildren[1]->getNodeAabb().Intersects(aabbNode))
			{
				UNWNodeList.push_back(node.second);
			}
			if (m_vOctreeChildren[2]->getNodeAabb().Intersects(aabbNode))
			{
				USENodeList.push_back(node.second);
			}
			if (m_vOctreeChildren[3]->getNodeAabb().Intersects(aabbNode))
			{
				USWNodeList.push_back(node.second);
			}
			if (m_vOctreeChildren[4]->getNodeAabb().Intersects(aabbNode))
			{
				LNENodeList.push_back(node.second);
			}
			if (m_vOctreeChildren[5]->getNodeAabb().Intersects(aabbNode))
			{
				LNWNodeList.push_back(node.second);
			}
			if (m_vOctreeChildren[6]->getNodeAabb().Intersects(aabbNode))
			{
				LSENodeList.push_back(node.second);
			}
			if (m_vOctreeChildren[7]->getNodeAabb().Intersects(aabbNode))
			{
				LSWNodeList.push_back(node.second);
			}
		}

		m_vOctreeChildren[0]->AddChildrenToNode(pScene, UNENodeList);
		m_vOctreeChildren[1]->AddChildrenToNode(pScene, UNWNodeList);
		m_vOctreeChildren[2]->AddChildrenToNode(pScene, USENodeList);
		m_vOctreeChildren[3]->AddChildrenToNode(pScene, USWNodeList);
		m_vOctreeChildren[4]->AddChildrenToNode(pScene, LNENodeList);
		m_vOctreeChildren[5]->AddChildrenToNode(pScene, LNWNodeList);
		m_vOctreeChildren[6]->AddChildrenToNode(pScene, LSENodeList);
		m_vOctreeChildren[7]->AddChildrenToNode(pScene, LSWNodeList);

		m_IsLeaf = false;
	}
	else
	{
		for (auto node : nodeList)
		{
			VAddChild(node.second);
		}
	}

	return true;
}

bool OctreeNode::AddChildrenToNode(Scene *pScene, std::list<std::shared_ptr<ISceneNode>> nodeList)
{
	m_NumNodes = nodeList.size();

    if (nodeList.size() > MAX_OCTREE_NODES)
    {
        std::list<std::shared_ptr<ISceneNode>> UNENodeList, UNWNodeList, USENodeList, USWNodeList,
            LNENodeList, LNWNodeList, LSENodeList, LSWNodeList;

        InitializeChildren();

        for (auto node : nodeList)
        {
            std::shared_ptr<SceneNode> sceneNode = std::static_pointer_cast<SceneNode>(node);
            Aabb aabbNode = node->VGetAabb().Transform(sceneNode->ToWorld());

            if (m_vOctreeChildren[0]->getNodeAabb().Intersects(aabbNode))
            {
                UNENodeList.push_back(node);
            }
            if (m_vOctreeChildren[1]->getNodeAabb().Intersects(aabbNode))
            {
                UNWNodeList.push_back(node);
            }
            if (m_vOctreeChildren[2]->getNodeAabb().Intersects(aabbNode))
            {
                USENodeList.push_back(node);
            }
            if (m_vOctreeChildren[3]->getNodeAabb().Intersects(aabbNode))
            {
                USWNodeList.push_back(node);
            }
            if (m_vOctreeChildren[4]->getNodeAabb().Intersects(aabbNode))
            {
                LNENodeList.push_back(node);
            }
            if (m_vOctreeChildren[5]->getNodeAabb().Intersects(aabbNode))
            {
                LNWNodeList.push_back(node);
            }
            if (m_vOctreeChildren[6]->getNodeAabb().Intersects(aabbNode))
            {
                LSENodeList.push_back(node);
            }
            if (m_vOctreeChildren[7]->getNodeAabb().Intersects(aabbNode))
            {
                LSWNodeList.push_back(node);
            }
        }

        m_vOctreeChildren[0]->AddChildrenToNode(pScene, UNENodeList);
        m_vOctreeChildren[1]->AddChildrenToNode(pScene, UNWNodeList);
        m_vOctreeChildren[2]->AddChildrenToNode(pScene, USENodeList);
        m_vOctreeChildren[3]->AddChildrenToNode(pScene, USWNodeList);
        m_vOctreeChildren[4]->AddChildrenToNode(pScene, LNENodeList);
        m_vOctreeChildren[5]->AddChildrenToNode(pScene, LNWNodeList);
        m_vOctreeChildren[6]->AddChildrenToNode(pScene, LSENodeList);
        m_vOctreeChildren[7]->AddChildrenToNode(pScene, LSWNodeList);

		m_IsLeaf = false;
    }
    else
    {
		for (auto node : nodeList)
        {
            VAddChild(node);
        }
    }

    return true;
}


bool OctreeNode::AddChildrenToOctree(Scene *pScene, shared_ptr<ISceneNode> kid)
{
	if(m_NumNodes < MAX_OCTREE_NODES)
	{
		m_NumNodes++;
		return VAddChild(kid);
	}
	else
	{
		m_NumNodes++;

		if(IsLeaf())
		{
			m_IsLeaf = false;
			VAddChild(kid);

			for(UINT i = 0; i < 8; i++)
			{
				//Initialize children octree nodes
				shared_ptr<OctreeNode> node(CHG_NEW OctreeNode());
				m_vOctreeChildren[i] = node;
				m_vOctreeChildren[i]->m_pParent[m_EntityID] = this;
				m_vOctreeChildren[i]->m_EntityID = -pScene->m_NumOctreeNodes - 2;
                m_vOctreeChildren[i]->m_IsLeaf = true;

				switch(i)
				{
					case UP_NW:
						m_vOctreeChildren[i]->m_Aabb.vcMax = Vec3(m_Aabb.vcCenter.x, m_Aabb.vcMax.y, m_Aabb.vcMax.z);
						m_vOctreeChildren[i]->m_Aabb.vcMin = Vec3(m_Aabb.vcMin.x, m_Aabb.vcCenter.y, m_Aabb.vcCenter.z);
						m_vOctreeChildren[i]->m_Aabb.vcCenter = (m_vOctreeChildren[i]->m_Aabb.vcMax + m_vOctreeChildren[i]->m_Aabb.vcMin) * 0.5f;
					break;
					case UP_NE:
						m_vOctreeChildren[i]->m_Aabb.vcMax = m_Aabb.vcMax;
						m_vOctreeChildren[i]->m_Aabb.vcMin = m_Aabb.vcCenter;
						m_vOctreeChildren[i]->m_Aabb.vcCenter = (m_vOctreeChildren[i]->m_Aabb.vcMax + m_vOctreeChildren[i]->m_Aabb.vcMin) * 0.5f;
					break;
					case UP_SW:
						m_vOctreeChildren[i]->m_Aabb.vcMax = Vec3(m_Aabb.vcCenter.x, m_Aabb.vcMax.y, m_Aabb.vcCenter.z);
						m_vOctreeChildren[i]->m_Aabb.vcMin = Vec3(m_Aabb.vcMin.x, m_Aabb.vcCenter.y, m_Aabb.vcMin.z);
						m_vOctreeChildren[i]->m_Aabb.vcCenter = (m_vOctreeChildren[i]->m_Aabb.vcMax + m_vOctreeChildren[i]->m_Aabb.vcMin) * 0.5f;
					break;
					case UP_SE:
						m_vOctreeChildren[i]->m_Aabb.vcMax = Vec3(m_Aabb.vcMax.x, m_Aabb.vcMax.y, m_Aabb.vcCenter.z);
						m_vOctreeChildren[i]->m_Aabb.vcMin = Vec3(m_Aabb.vcCenter.x, m_Aabb.vcCenter.y, m_Aabb.vcMin.z);
						m_vOctreeChildren[i]->m_Aabb.vcCenter = (m_vOctreeChildren[i]->m_Aabb.vcMax + m_vOctreeChildren[i]->m_Aabb.vcMin) * 0.5f;
					break;
					case LW_NW:
						m_vOctreeChildren[i]->m_Aabb.vcMax = Vec3(m_Aabb.vcCenter.x, m_Aabb.vcCenter.y, m_Aabb.vcMax.z);
						m_vOctreeChildren[i]->m_Aabb.vcMin = Vec3(m_Aabb.vcMin.x, m_Aabb.vcMin.y, m_Aabb.vcCenter.z);
						m_vOctreeChildren[i]->m_Aabb.vcCenter = (m_vOctreeChildren[i]->m_Aabb.vcMax + m_vOctreeChildren[i]->m_Aabb.vcMin) * 0.5f;
					break;
					case LW_NE:
						m_vOctreeChildren[i]->m_Aabb.vcMax = Vec3(m_Aabb.vcMax.x, m_Aabb.vcCenter.y, m_Aabb.vcMax.z);
						m_vOctreeChildren[i]->m_Aabb.vcMin = Vec3(m_Aabb.vcCenter.x, m_Aabb.vcMin.y, m_Aabb.vcCenter.z);
						m_vOctreeChildren[i]->m_Aabb.vcCenter = (m_vOctreeChildren[i]->m_Aabb.vcMax + m_vOctreeChildren[i]->m_Aabb.vcMin) * 0.5f;
					break;
					case LW_SW:
						m_vOctreeChildren[i]->m_Aabb.vcMax = m_Aabb.vcCenter;
						m_vOctreeChildren[i]->m_Aabb.vcMin = m_Aabb.vcMin;
						m_vOctreeChildren[i]->m_Aabb.vcCenter = (m_vOctreeChildren[i]->m_Aabb.vcMax + m_vOctreeChildren[i]->m_Aabb.vcMin) * 0.5f;
					break;
					case LW_SE:
						m_vOctreeChildren[i]->m_Aabb.vcMax = Vec3(m_Aabb.vcMax.x, m_Aabb.vcCenter.y, m_Aabb.vcCenter.z);
						m_vOctreeChildren[i]->m_Aabb.vcMin = Vec3(m_Aabb.vcCenter.x, m_Aabb.vcMin.y, m_Aabb.vcMin.z);
						m_vOctreeChildren[i]->m_Aabb.vcCenter = (m_vOctreeChildren[i]->m_Aabb.vcMax + m_vOctreeChildren[i]->m_Aabb.vcMin) * 0.5f;
					break;
		
					default: break;
				}

				//Put nodes in appropriate node
				for(UINT j=0; j < m_Children.size(); j++)
				{
					Aabb aabbT = m_Children[j]->VGetAabb().Transform(static_pointer_cast<SceneNode>(m_Children[j])->ToWorld());

					if(m_vOctreeChildren[i]->m_Aabb.Intersects(aabbT))
					{
						m_vOctreeChildren[i]->AddChildrenToOctree(pScene, m_Children[j]);
					}
				}
			}

			RemoveChildrenFromThisNode();
		}
		else
		{
			//Put nodes in appropriate node
			Aabb aabbT = kid->VGetAabb().Transform(static_pointer_cast<SceneNode>(kid)->ToWorld());

			for(int i=0; i < 8; i++)
			{
				if(m_vOctreeChildren[i]->m_Aabb.Intersects(aabbT))
				{
					m_vOctreeChildren[i]->AddChildrenToOctree(pScene, kid);
				}
			}
		}

        return true;
	}
}

bool OctreeNode::RemoveChildrenToOctree(Scene *pScene, EntityId id)
{
	return VRemoveChild(id);

	//---------------------------------------------------->Revision    
}

void OctreeNode::TraverseOctree(Scene *pScene)
{
	if(IsVisibleFromCamera(pScene))
	{
		if(IsLeaf())
		{
			for(UINT i=0; i < m_Children.size(); i++)
			{
				std::shared_ptr<SceneNode> childNode = static_pointer_cast<SceneNode>(m_Children[i]);

				if(childNode->IsVisibleFromCamera(pScene))
				{
					if(!childNode->VIsInRenderQueue())
					{
						pScene->AddNodeToRenderQueue(childNode);

						if(childNode->VGetType() == LIGHT_NODE)
							pScene->AddNodeToLightQueue(childNode);
					}
				}
				/*else
					if(i!=0)
						printf("No se ve objeto %d \n", i);*/
			}
		}
		else
		{
			for(UINT i=0; i < 8; i++)
			{
				if(m_vOctreeChildren[i]->m_NumNodes > 0)
				{
					m_vOctreeChildren[i]->TraverseOctree(pScene);
				}
			}
		}
	}
	/*else
		printf("No se ve el nodo");*/
}

void OctreeNode::TraverseOctreeFromSpecificFrustum(std::vector<std::shared_ptr<SceneNode>> &frustumQueue, Frustum frustum, Mat4x4 frustumMat, Scene *pScene)
{
	if(IsVisibleFromFrustum(frustum, frustumMat, pScene))
	{
		if(IsLeaf())
		{
			for(UINT i=0; i < m_Children.size(); i++)
			{
				std::shared_ptr<SceneNode> childNode = static_pointer_cast<SceneNode>(m_Children[i]);

				if(childNode->IsVisibleFromFrustum(frustum, frustumMat, pScene))
				{
					if(!childNode->IsInFrustumQueue())
					{
						if(childNode->VGetType() == MESH_NODE)
						{
							frustumQueue.push_back(childNode);
							childNode->SetInFrustumQueue(true);
						}
					}
				}
				/*else
					if(i!=0)
						printf("No se ve objeto %d \n", i);*/
			}
		}
		else
		{
			for(UINT i=0; i < 8; i++)
			{
				if(m_vOctreeChildren[i]->m_NumNodes > 0)
				{
					m_vOctreeChildren[i]->TraverseOctreeFromSpecificFrustum(frustumQueue, frustum, frustumMat, pScene);
				}
			}
		}
	}
	/*else
		printf("No se ve el nodo");*/
}

void OctreeNode::TraverseOctreeFromAabb(std::vector<std::shared_ptr<SceneNode>> &AabbLightQueue, Aabb aabb, Scene *pScene)
{
	if (IntersectAabb(aabb, pScene))
	{
		if (IsLeaf())
		{
			for (UINT i = 0; i < m_Children.size(); i++)
			{
				std::shared_ptr<SceneNode> childNode = static_pointer_cast<SceneNode>(m_Children[i]);

				if (childNode->IntersectAabb(aabb, pScene))
				{
					if (!childNode->IsInFrustumQueue())
					{
						if (childNode->VGetType() == LIGHT_NODE)
						{
							AabbLightQueue.push_back(childNode);
							childNode->SetInFrustumQueue(true);
						}					
					}
				}
				/*else
				if(i!=0)
				printf("No se ve objeto %d \n", i);*/
			}
		}
		else
		{
			for (UINT i = 0; i < 8; i++)
			{
				if (m_vOctreeChildren[i]->m_NumNodes > 0)
				{
					m_vOctreeChildren[i]->TraverseOctreeFromAabb(AabbLightQueue, aabb, pScene);
				}
			}
		}
	}
	/*else
	printf("No se ve el nodo");*/
}

void OctreeNode::TraverseOctreeFromAabb(std::list<std::shared_ptr<SceneNode>> &AabbQueue, std::list<std::shared_ptr<SceneNode>> &AabbLightQueue, Aabb aabb, Scene *pScene)
{
	if(IntersectAabb(aabb, pScene))
	{
		if(IsLeaf())
		{
			for(UINT i=0; i < m_Children.size(); i++)
			{
				std::shared_ptr<SceneNode> childNode = static_pointer_cast<SceneNode>(m_Children[i]);

				if(childNode->IntersectAabb(aabb, pScene))
				{
					if(!childNode->IsInFrustumQueue())
					{
						AabbQueue.push_back(childNode);

						if(childNode->VGetType() == LIGHT_NODE)
							AabbLightQueue.push_back(childNode);

						childNode->SetInFrustumQueue(true);
					}
				}
				/*else
					if(i!=0)
						printf("No se ve objeto %d \n", i);*/
			}
		}
		else
		{
			for(UINT i=0; i < 8; i++)
			{
				if(m_vOctreeChildren[i]->m_NumNodes > 0)
				{
					m_vOctreeChildren[i]->TraverseOctreeFromAabb(AabbQueue, AabbLightQueue, aabb, pScene);
				}
			}
		}
	}
	/*else
		printf("No se ve el nodo");*/
}

////////////////////////////////////////////////////
// CameraNode Implementation
////////////////////////////////////////////////////

//
// CameraNode::VOnRestore				- Chapter 16, page 550
//
bool CameraNode::VOnRestore(Scene *pScene)
{
	m_Frustum.SetAspect(g_pApp->GetScreenSize().x / g_pApp->GetScreenSize().y);

	if(m_Frustum.m_Type == PERSPECTIVE_FRUSTUM)
		m_Projection = perspectiveMatrix(m_Frustum.m_Fov, m_Frustum.m_Aspect, m_Frustum.m_Near, m_Frustum.m_Far);
	else
		m_Projection = orthoMatrixX(m_Frustum.m_Width, m_Frustum.m_Height, m_Frustum.m_Near, m_Frustum.m_Far);;

	m_InverseProjection = !m_Projection;

	return true;
}

//
// CameraNode::SetView					
//
//    Note: this is incorrectly called CameraNode::SetView in the book
//
bool CameraNode::SetViewTransform(Scene *pScene)
{
	m_View = FromWorld();
	m_InverseView = !FromWorld();

	return true;
}

//
// CameraNode::GetWorldViewProjection			
//
//    Returns the concatenation of the world and view projection, which is generally sent into vertex shaders
//
Mat4x4 CameraNode::GetViewProjection(Scene *pScene)
{ 
	Mat4x4 view = FromWorld();
	return view * m_Projection;
}

