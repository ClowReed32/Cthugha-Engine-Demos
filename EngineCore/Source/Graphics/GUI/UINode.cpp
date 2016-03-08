#pragma once
//========================================================================
// UINode.cpp : classes that define GUI graphics and user interaction
//				like Buttons, Panels, Images, etc.
//
//========================================================================

#include "CthughaStd.h"

#include "..\..\CthughaEngine\CthughaApp.h"
#include "..\..\Entities\EntityComponent.h"
#include "..\..\Entities\GUIComponent.h"

#include "UINode.h"

//---------------------------------------------------------------------------------------------------------------------
// UINode Implementation
//---------------------------------------------------------------------------------------------------------------------

UINode::UINode(const EntityId id, const std::string name, const Aabb aabb, const Mat4x4 toWorld)
{
	m_EntityId = id;
	m_Name = name;
	m_Aabb = aabb;
	m_ToWorld = toWorld;
	m_IsVisible = true;
}

UINode::~UINode()
{
	m_UIFunctions.clear();
}

void UINode::SetTransform(const Mat4x4 *toWorld)
{
	m_ToWorld = *toWorld;
}

void UINode::Transform(Mat4x4 *toWorld) const
{
	*toWorld = m_ToWorld;
}

bool UINode::OnRestore(UIScene *pUIScene)
{
	UIFunctionList::iterator i = m_UIFunctions.begin();
	UIFunctionList::iterator end = m_UIFunctions.end();
	while (i != end)
	{
		(*i)->VOnRestore(pUIScene);
		++i;
	}
	return true;
}

bool UINode::OnUpdate(UIScene *pUIScene, DWORD const elapsedMs)
{
	UIFunctionList::iterator i = m_UIFunctions.begin();
	UIFunctionList::iterator end = m_UIFunctions.end();
	while (i != end)
	{
		(*i)->VOnUpdate(pUIScene, elapsedMs);
		++i;
	}
	return true;
}

bool UINode::OnMsgProc(CHG_Event msg)
{
	bool isActive = false;

	UIFunctionList::iterator i = m_UIFunctions.begin();
	UIFunctionList::iterator end = m_UIFunctions.end();
	while (i != end)
	{
		isActive = isActive || (*i)->VOnMsgProc(msg);
		++i;
	}
	return isActive;
}

bool UINode::IntersectAabb(const Aabb& aabb, UIScene *pUIScene) const
{
	if (!m_IsVisible)
		return false;

	Aabb aabbT = m_Aabb.Transform(m_ToWorld);

	bool instersect = aabbT.Intersects(aabb);
	return instersect;
}

bool UINode::AddChild(shared_ptr<UINode> kid)
{
	m_Children.push_back(kid);
	kid->m_pParent = std::shared_ptr<UINode>(this);

	return true;
}

bool UINode::AddUIFunction(shared_ptr<UIFunction> pUIFunction, std::shared_ptr<UINode> pOwner)
{
	m_UIFunctions.push_back(pUIFunction);
	pUIFunction->SetOwner(std::weak_ptr<UINode>(pOwner));

	/*if (GetUIFunction(UIFUNTION_BUTTON))
	{
		printf("Boton añadido: %d\n", GetEntityID());
	}*/

	return true;
}

bool UINode::OnLostDevice(UIScene *pUIScene)
{
	UIFunctionList::iterator i = m_UIFunctions.begin();
	UIFunctionList::iterator end = m_UIFunctions.end();
	while (i != end)
	{
		(*i)->VOnLostDevice(pUIScene);
		++i;
	}
	return true;
}

bool UINode::OnRender(UIScene *pUIScene)
{
    for (int i = m_UIFunctions.size() - 1; i > -1; i--)
    {
        m_UIFunctions[i]->VOnRender(pUIScene);
    }

	return true;
}

shared_ptr<UIFunction> UINode::GetUIFunction(UIFunctionId id)
{
	UIFunctionList::iterator i = m_UIFunctions.begin();
	UIFunctionList::iterator end = m_UIFunctions.end();
	while (i != end)
	{
		if ((*i)->GetId() == id)
			return *i;

		++i;
	}
	return NULL;
}

shared_ptr<UIFunction> UINode::GetUIFunction(UIFunctionType type)
{
	UIFunctionList::iterator i = m_UIFunctions.begin();
	UIFunctionList::iterator end = m_UIFunctions.end();
	while (i != end)
	{
		if ((*i)->GetType() == type)
			return *i;

		++i;
	}
	return NULL;
}

UIFunction::~UIFunction() 
{
}