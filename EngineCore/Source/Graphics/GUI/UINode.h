#pragma once

//=============================================================================
// UINode.h - defines the class for manager the element which compose the
//                  user interface.           
//
//=============================================================================

#include "..\..\Msvc\CthughaStd.h"

class UINode;
class UIFunction;

#include "..\..\Entities\GUIComponent.h"

typedef std::vector<std::shared_ptr<UINode>> UINodeList;
typedef std::vector<std::shared_ptr<UIFunction>> UIFunctionList;
typedef BaseGUIComponent* WeakGUIComponentPtr;

enum UIFunctionType { UIFUNTION_GENERIC, UIFUNTION_BUTTON, UIFUNCTION_IMAGE, UIFUNCTION_TEXT, UIFUNCTION_CHECKBOX, UIFUNCTION_MESHVIEWERBUTTON };

//////////////////////////////////////////////////////////////////////////
//
//	UINode class definition
//
//////////////////////////////////////////////////////////////////////////

class UINode
{
	friend class UIScene;

protected:
	UIFunctionList			m_UIFunctions;
	UINodeList				m_Children;
	std::shared_ptr<UINode>	m_pParent;
	EntityId				m_EntityId;

	//Properties
	std::string				m_Name;
	Aabb					m_Aabb;
	bool					m_IsVisible;
	Mat4x4					m_ToWorld;

public:
	UINode() { m_EntityId = INVALID_ENTITY_ID; m_Name = ""; m_IsVisible = false; };
	UINode(const EntityId id, const std::string name, const Aabb aabb, const Mat4x4 toWorld);

	~UINode();

	//////////////////////////////////////////////////////////////////////
	std::string getNodeName() { return m_Name; }
	Aabb getNodeAabb() { return m_Aabb; }
	Mat4x4 const &ToWorld() const { return m_ToWorld; }
	//////////////////////////////////////////////////////////////////////

	void SetTransform(const Mat4x4 *toWorld);
	void Transform(Mat4x4 *toWorld) const;

	bool OnRestore(UIScene *pUIScene);
	bool OnUpdate(UIScene *, DWORD const elapsedMs);
	bool OnMsgProc(CHG_Event msg);
	bool OnLostDevice(UIScene *pUIScene);
	bool OnRender(UIScene *pUIScene);

	bool IsVisible() const { return m_IsVisible; }
	bool IntersectAabb(const Aabb& aabb, UIScene *pUIScene) const;

	bool AddChild(shared_ptr<UINode> kid);
	bool AddUIFunction(shared_ptr<UIFunction> pUIFunction, std::shared_ptr<UINode> pOwner);
	bool RemoveChild(EntityId id) { }; //Useless?
	bool RemoveUIFunction(UIFunctionId) { }; //Useless?
	
	shared_ptr<UIFunction> GetUIFunction(UIFunctionId id);
	shared_ptr<UIFunction> GetUIFunction(UIFunctionType type);
	Aabb GetAabb() { return m_Aabb; }
	EntityId GetEntityID() { return m_EntityId; }

	void SetNodeAabb(const Aabb aabb) { m_Aabb = aabb; }
	void SetVisible(bool isVisible) { m_IsVisible = isVisible; }
};

//////////////////////////////////////////////////////////////////////////
//
//	UIFunction class definition
//
//////////////////////////////////////////////////////////////////////////

class UIFunction
{
protected:
	UIFunctionType			m_Type;
	UIFunctionId			m_Id;
	std::weak_ptr<UINode>	m_pOwner;

    //WeakGUIComponentPtr     m_pGUIComponent;

public:
	UIFunction() { m_Type = UIFUNTION_GENERIC; m_Id = INVALID_UIFUNCTION_ID; }
	~UIFunction();

	virtual bool VOnRender(UIScene *pUIScene) {	return true; }
	virtual bool VOnUpdate(UIScene *pUIScene, DWORD const elapsedMs) { return true; }
	virtual bool VOnRestore(UIScene *pUIScene) { return true; }
	virtual bool VOnMsgProc(CHG_Event msg) { return false; }
	virtual bool VOnLostDevice(UIScene *pUIScene) { return true;	}

	UIFunctionType GetType() { return m_Type; }
	UIFunctionId GetId() { return m_Id; }
	std::shared_ptr<UINode> GetOwner() { return MakeStrongPtr(m_pOwner); }

	void SetOwner(weak_ptr<UINode> pUINode) { m_pOwner = pUINode; }
};