//========================================================================
// UIScene.cpp - defines the class for manager GUI element in the scene
//                  (button, slider, text, etc...)                  
//
//========================================================================

#include "CthughaStd.h"

#include "..\CthughaEngine\CthughaApp.h"
#include "..\EventManager\EventManager.h"
#include "..\EventManager\Events.h"
#include "..\..\Entities\RectTransformComponent.h"

#include "UIScene.h"

//---------------------------------------------------------------------------------------------------------------------
// UINode Implementation
//---------------------------------------------------------------------------------------------------------------------

UIScene::UIScene(std::shared_ptr<Renderer> renderer, GameViewId gameViewId)
{
    m_GameViewId = gameViewId;
    m_pRenderer = renderer;

    //Init screen aligned quad attributes//////////////////////////////////////////////////////////////////

    static const float g_quad_vertex_buffer_data[] =
    {
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
    };

    m_ScreenAlignedQuadVBID = m_pRenderer->addVertexBuffer(18 * sizeof(GLfloat), STATIC, g_quad_vertex_buffer_data);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    //Init Target Surfaces////////////////////////////////////////////////////////////////////////////////////

    m_GUIRT = m_pRenderer->addRenderTarget((int)g_pApp->GetScreenSize().x, (int)g_pApp->GetScreenSize().y, 1, 1, 1, FORMAT_RGBA8, 1, SS_NONE);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    //Init RenderStates///////////////////////////////////////////////////////////////////////////////////////

    //BlendModes
    m_blendSrcAlpha = m_pRenderer->addBlendState(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
    m_blendAdd = m_pRenderer->addBlendState(ONE, ONE);

    //RenderStates
    m_backFaceCulling = m_pRenderer->addRasterizerState(CULL_FRONT, SOLID, true, false, 0.0f, 0.0f);
    m_noneFaceCulling = m_pRenderer->addRasterizerState(CULL_NONE, SOLID, true, false, 0.0f, 0.0f);

    // Depth states - use reversed depth (1 to 0) to improve precision
    m_noDepthTest = m_pRenderer->addDepthState(false, false);
	m_depthTest = m_pRenderer->addDepthState(true, true, LEQUAL);

    //SamplerStates
    m_linearClamp = m_pRenderer->addSamplerState(LINEAR, CLAMP, CLAMP, CLAMP);
    m_bilinearClamp = m_pRenderer->addSamplerState(BILINEAR, CLAMP, CLAMP, CLAMP);
	m_trilinearClamp = m_pRenderer->addSamplerState(TRILINEAR_ANISO, CLAMP, CLAMP, CLAMP);
    m_pointClamp = m_pRenderer->addSamplerState(NEAREST, CLAMP, CLAMP, CLAMP);
    m_linearWrap = m_pRenderer->addSamplerState(NEAREST, WRAP, WRAP, WRAP);
    m_trilinearWrap = m_pRenderer->addSamplerState(TRILINEAR_ANISO, WRAP, WRAP, WRAP);
    m_trilinearMirror = m_pRenderer->addSamplerState(TRILINEAR_ANISO, MIRROR, MIRROR, MIRROR);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    // event delegates were added post-press
    IEventManager* pEventMgr = IEventManager::Get();
    pEventMgr->VAddListener(MakeDelegate(this, &UIScene::NewGUIComponentDelegate), EvtData_New_GUI_Component::sk_EventType);
    pEventMgr->VAddListener(MakeDelegate(this, &UIScene::DestroyGUIActorDelegate), EvtData_Destroy_Actor::sk_EventType);
    pEventMgr->VAddListener(MakeDelegate(this, &UIScene::MoveGUIActorDelegate), EvtData_Move_Actor::sk_EventType);
    pEventMgr->VAddListener(MakeDelegate(this, &UIScene::ModifiedGUIComponentDelegate), EvtData_Modified_GUI_Component::sk_EventType);

}

UIScene::~UIScene()
{
    // event delegates were added post-press!
    IEventManager* pEventMgr = IEventManager::Get();
    pEventMgr->VRemoveListener(MakeDelegate(this, &UIScene::NewGUIComponentDelegate), EvtData_New_GUI_Component::sk_EventType);
    pEventMgr->VRemoveListener(MakeDelegate(this, &UIScene::DestroyGUIActorDelegate), EvtData_Destroy_Actor::sk_EventType);
    pEventMgr->VRemoveListener(MakeDelegate(this, &UIScene::MoveGUIActorDelegate), EvtData_Move_Actor::sk_EventType);
    pEventMgr->VRemoveListener(MakeDelegate(this, &UIScene::ModifiedGUIComponentDelegate), EvtData_Modified_GUI_Component::sk_EventType);

	m_EntityMap.clear();
	int size = m_UINodes.size();

	for (int i = 0; i < size; i++)
	{
		g_pApp->m_pGame->VDestroyEntity(m_UINodes.back()->GetEntityID());
		m_UINodes.pop_back();
	}	
}

bool UIScene::OnRender()
{
    float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    m_pRenderer->setViewport((int)g_pApp->GetScreenSize().x, (int)g_pApp->GetScreenSize().y);
	m_pRenderer->changeToMainFramebuffer();

    for (UINT i = 0; i < m_UINodes.size(); i++)
    {
        if (m_UINodes[i]->IsVisible())
        {
            m_UINodes[i]->OnRender(this);
        }
    }

    return true;
}

bool UIScene::OnRestore()
{
    UINodeList::iterator i = m_UINodes.begin();
	UINodeList::iterator end = m_UINodes.end();
    while (i != end)
    {
        (*i)->OnRestore(this);
        ++i;
    }
    return true;
}

bool UIScene::OnLostDevice()
{
	UINodeList::iterator i = m_UINodes.begin();
	UINodeList::iterator end = m_UINodes.end();
    while (i != end)
    {
        (*i)->OnLostDevice(this);
        ++i;
    }
    return true;
}

bool UIScene::OnUpdate(const int deltaMilliseconds)
{
	UINodeList::iterator i = m_UINodes.begin();
	UINodeList::iterator end = m_UINodes.end();
    while (i != end)
    {
        (*i)->OnUpdate(this, deltaMilliseconds);
        ++i;
    }
    return true;
}

bool UIScene::OnMsgProc(CHG_Event msg)
{
	bool isActive = false;;

	UINodeList::iterator i = m_UINodes.begin();
	UINodeList::iterator end = m_UINodes.end();
	while (i != end)
	{
		if ((*i)->IsVisible())
		{
			isActive = (*i)->OnMsgProc(msg) || isActive;
		}
        
		++i;
	}
	return isActive;
}

bool UIScene::AddGUIFunction(EntityId id, shared_ptr<Entity> pEntity, shared_ptr<UIFunction> uiFunction)
{
	if (id != INVALID_ENTITY_ID)
	{
		UISceneActorMap::iterator i = m_EntityMap.find(id);
		
		if (i == m_EntityMap.end())
		{
			//Obtain Entity data and transform data
			shared_ptr<RectTransformComponent> pTransformComponent = MakeStrongPtr(pEntity->GetComponent<RectTransformComponent>(RectTransformComponent::g_Name));

			if (!pTransformComponent)
			{
				CHG_ERROR("RectTransformComponent not found in GUI Entity");
				return false;
			}

			Aabb nodeAabb;
			nodeAabb.vcMin = Vec3();
			nodeAabb.vcMax = Vec3(pTransformComponent->GetRectDimension().x, pTransformComponent->GetRectDimension().y, 0.0f);
			nodeAabb.vcCenter = (nodeAabb.vcMin + nodeAabb.vcMax) / 2.0f;

			std::shared_ptr<UINode> node = std::shared_ptr<UINode>(CHG_NEW UINode(id, pEntity->GetName(), nodeAabb, pTransformComponent->GetTransform()));
			m_UINodes.push_back(node);

			m_EntityMap[id] = m_UINodes.back();

			i = m_EntityMap.find(id);
		}

		i->second->AddUIFunction(uiFunction, i->second);

		return true;
	}

	return false;
}

shared_ptr<UINode> UIScene::FindEntity(EntityId id)
{
	UISceneActorMap::iterator i = m_EntityMap.find(id);
	if (i == m_EntityMap.end())
	{
		return shared_ptr<UINode>();
	}

	return i->second;
}

void UIScene::NewGUIComponentDelegate(IEventDataPtr pEventData)
{
	shared_ptr<EvtData_New_GUI_Component> pCastEventData = static_pointer_cast<EvtData_New_GUI_Component>(pEventData);

	EntityId actorId = pCastEventData->GetActorId();
	shared_ptr<UIFunction> pUIFunction(pCastEventData->GetUIFunction());
	GameViewId gameViewId = pCastEventData->GetGameViewId();
    shared_ptr<Entity> pEntity = pCastEventData->GetEntity();

	if (gameViewId != m_GameViewId)
		return;

	// FUTURE WORK: Add better error handling here.		
	if (FAILED(pUIFunction->VOnRestore(this)))
	{
		std::string error = "Failed to restore user interface node to the scene for actorid " + ToStr(actorId);
		CHG_ERROR(error);
		return;
	}

    AddGUIFunction(actorId, pEntity, pUIFunction);
}

void UIScene::ModifiedGUIComponentDelegate(IEventDataPtr pEventData)
{
	shared_ptr<EvtData_Modified_GUI_Component> pCastEventData = static_pointer_cast<EvtData_Modified_GUI_Component>(pEventData);

	EntityId actorId = pCastEventData->GetActorId();
	GameViewId gameViewId = pCastEventData->GetGameViewId();

	if (gameViewId != m_GameViewId)
		return;

	if (actorId == INVALID_ENTITY_ID)
	{
		CHG_ERROR("Scene::ModifiedGUIComponentDelegate - unknown actor id!");
		return;
	}

	if (g_pApp->GetGameLogic()->GetState() == BGS_LoadingGameEnvironment)
		return;

	shared_ptr<UINode> pUINode = FindEntity(actorId);
	// FUTURE WORK: Add better error handling here.		
	if (!pUINode || FAILED(pUINode->OnRestore(this)))
	{
		CHG_ERROR("Failed to restore user interface node to the scene for actorid " + ToStr(actorId));
	}
}

void UIScene::DestroyGUIActorDelegate(IEventDataPtr pEventData)
{
	shared_ptr<EvtData_Destroy_Actor> pCastEventData = static_pointer_cast<EvtData_Destroy_Actor>(pEventData);
	//RemoveChild(pCastEventData->GetId());
}

void UIScene::MoveGUIActorDelegate(IEventDataPtr pEventData)
{
	shared_ptr<EvtData_Move_Actor> pCastEventData = static_pointer_cast<EvtData_Move_Actor>(pEventData);

	EntityId id = pCastEventData->GetId();
	Mat4x4 transform = pCastEventData->GetMatrix();

	shared_ptr<UINode> pNode = FindEntity(id);
	if (pNode)
	{
		pNode->SetTransform(&transform);
	}
}