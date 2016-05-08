//========================================================================
// BaseGameLogic.cpp : defines game logic class
//
//========================================================================

#include "CthughaStd.h"

#include <mmsystem.h>

//#include "../AI/Pathing.h"
#include "../EventManager/Events.h"				// only for EvtData_Game_State
#include "../Mainloop/Initialization.h"			// only for GameOptions
#include "../MainLoop/Process.h"
//#include "../Network/Network.h"
#include "../ResourceCache/XmlResource.h"
#include "../Physics/Physics.h"
#include "../Entities/Entity.h"
#include "../Entities/EntityFactory.h"
#include "../Utilities/String.h"
#include "../UserInterface/HumanView.h" 

#include "BaseGameLogic.h"

//========================================================================
//
// LevelManager implementation
//
//========================================================================

bool LevelManager::Initialize(std::vector<std::string> &levels)
{
	std::vector<std::string>::iterator i = levels.begin();
	while (i != levels.end())
	{
		m_Levels.push_back(*i);
		++i;
	}
	return true;
}




//========================================================================
//
// BaseGameLogic implementation
//
//========================================================================


BaseGameLogic::BaseGameLogic()
{
	m_LastEntityId = 0;
	m_LastGameViewId = 0;
	m_Lifetime = 0;
	m_pProcessManager = CHG_NEW ProcessManager;
	m_random.Randomize();
	m_State = BGS_Initializing;
	m_RenderDiagnostics = false;
	m_ExpectedPlayers = 0;
	m_ExpectedRemotePlayers = 0;
	m_ExpectedAI = 0;
	m_HumanPlayersAttached = 0;
	m_AIPlayersAttached = 0;
	m_HumanGamesLoaded = 0;
    m_pEntityFactory = NULL;
	m_bPause = false;
    m_CurrentHumanView = -1;

	m_pLevelManager = CHG_NEW LevelManager;
	CHG_ASSERT(m_pProcessManager && m_pLevelManager);
	m_pLevelManager->Initialize(g_pApp->m_ResCache->Match("world\\*.xml"));
}

BaseGameLogic::~BaseGameLogic()
{
	//   Added this to explicitly remove views from the game logic list.
	while (!m_gameViews.empty())
		m_gameViews.pop_front();

	SAFE_DELETE(m_pLevelManager);
	SAFE_DELETE(m_pProcessManager);
    SAFE_DELETE(m_pEntityFactory);

    // destroy all actors
    for (auto it = m_entities.begin(); it != m_entities.end(); ++it)
        it->second->Destroy();
    m_entities.clear();
}

bool BaseGameLogic::Init(void)
{
    m_pEntityFactory = VCreateEntityFactory();

    return true;
}

std::string BaseGameLogic::GetEntityXml(const EntityId id)
{
    StrongEntityPtr pEntity = MakeStrongPtr(VGetEntity(id));
    if (pEntity)
            return pEntity->ToXML();
    else
        CHG_ERROR("Couldn't find Entity: " + ToStr(id));

    return std::string();
}

bool BaseGameLogic::VLoadGame(const char* levelResource)
{
	TiXmlElement* pRoot = NULL;

    // Grab the root XML node
	if (strcmp(levelResource, "") != 0)
	{
		pRoot = XmlResourceLoader::LoadAndReturnRootXmlElement(levelResource);

		if (!pRoot)
		{
			CHG_ERROR("Failed to find level resource file: " + std::string(levelResource));
			return false;
		}		
	}		

	// pre and post load scripts
	const char* preLoadScript = NULL;
	const char* postLoadScript = NULL;

	if (pRoot)
	{
		// parse the pre & post script attributes
		TiXmlElement* pScriptElement = pRoot->FirstChildElement("Script");
		if (pScriptElement)
		{
			preLoadScript = pScriptElement->Attribute("preLoad");
			postLoadScript = pScriptElement->Attribute("postLoad");
		}

		// load the pre-load script if there is one
		if (preLoadScript)
		{
			Resource resource(preLoadScript);
			shared_ptr<ResHandle> pResourceHandle = g_pApp->m_ResCache->GetHandle(&resource);  // this actually loads the XML file from the zip file
		}

		TiXmlElement* pAMapData = pRoot->FirstChildElement("MapData");
		TiXmlElement* pMapContents = pAMapData->FirstChildElement("MapContents");

		// load all initial StaticObjects
		TiXmlElement* pStaticObjectsNode = pMapContents->FirstChildElement("StaticObjects");
		if (pStaticObjectsNode)
		{
			for (TiXmlElement* pNode = pStaticObjectsNode->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
			{
				StrongEntityPtr pEntity = VCreateStaticEntity(pNode);
				if (pEntity)
				{
					// fire an event letting everyone else know that we created a new actor
					shared_ptr<EvtData_New_Actor> pNewActorEvent(CHG_NEW EvtData_New_Actor(pEntity->GetId()));
					IEventManager::Get()->VQueueEvent(pNewActorEvent);
				}
			}
		}

		// load all Entities
		TiXmlElement* pEntitiesNode = pMapContents->FirstChildElement("Entities");
		if (pEntitiesNode)
		{
			for (TiXmlElement* pNode = pEntitiesNode->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
			{
				std::string resourceFileName = pNode->Attribute("resource");

				StrongEntityPtr pEntity = VCreateEntity(resourceFileName, pNode);
				if (pEntity)
				{
					// fire an event letting everyone else know that we created a new actor
					shared_ptr<EvtData_New_Actor> pNewActorEvent(CHG_NEW EvtData_New_Actor(pEntity->GetId()));
					IEventManager::Get()->VQueueEvent(pNewActorEvent);
				}
			}
		}

		// load all Ligths
		TiXmlElement* pLightsNode = pMapContents->FirstChildElement("Lights");
		if (pLightsNode)
		{
			for (TiXmlElement* pNode = pLightsNode->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
			{
				StrongEntityPtr pEntity = VCreateStaticEntity(pNode);
				if (pEntity)
				{
					// fire an event letting everyone else know that we created a new actor
					shared_ptr<EvtData_New_Actor> pNewActorEvent(CHG_NEW EvtData_New_Actor(pEntity->GetId()));
					IEventManager::Get()->VQueueEvent(pNewActorEvent);
				}
			}
		}
	}

    // initialize all human views
    for (auto it = m_gameViews.begin(); it != m_gameViews.end(); ++it)
    {
        shared_ptr<IGameView> pView = *it;
        if (pView->VGetType() == GameView_Human)
        {
            shared_ptr<HumanView> pHumanView = static_pointer_cast<HumanView, IGameView>(pView);
            pHumanView->LoadGame(pRoot);
        }
    }

    // call the delegate load function
    if (!VLoadGameDelegate(pRoot))
        return false;  // no error message here because it's assumed VLoadGameDelegate() kicked out the error

    // load the post-load script if there is one
    if (postLoadScript)
    {
        Resource resource(postLoadScript);
        shared_ptr<ResHandle> pResourceHandle = g_pApp->m_ResCache->GetHandle(&resource);  // this actually loads the XML file from the zip file
    }

	// trigger the Environment Loaded Game event - only then can player actors and AI be spawned!
    shared_ptr<EvtData_Environment_Loaded> pNewGameEvent(CHG_NEW EvtData_Environment_Loaded);
	IEventManager::Get()->VTriggerEvent(pNewGameEvent);
	
    return true;
}

bool BaseGameLogic::ChangeCurrentHumanView(GameViewId viewId)
{
    std::shared_ptr<HumanView> pCurrentView = std::static_pointer_cast<HumanView>(VFindView(m_CurrentHumanView));

    if (pCurrentView)
        pCurrentView->ToggleActive();

    std::shared_ptr<HumanView> pView = std::static_pointer_cast<HumanView>(VFindView(viewId));

    if (pView)
    {
        pView->ToggleActive();
        m_CurrentHumanView = viewId;

        return true;
    }

    return false;
}

StrongEntityPtr BaseGameLogic::VCreateEntity(const std::string &EntityResource, std::string sName, std::vector<ComponentResource> extraComponents, const Mat4x4 *initialTransform, const EntityId serversEntityId)
{
	CHG_ASSERT(m_pEntityFactory);

	StrongEntityPtr pActor = m_pEntityFactory->CreateEntity(EntityResource.c_str(), sName, extraComponents, initialTransform, serversEntityId);
	if (pActor)
	{
		m_entities.insert(std::make_pair(pActor->GetId(), pActor));
		return pActor;
	}
	else
	{
		// FUTURE WORK: Log error: couldn't create actor
		return StrongEntityPtr();
	}
}

StrongEntityPtr BaseGameLogic::VCreateEntity(const std::string &actorResource, TiXmlElement* overrides, const Mat4x4* initialTransform, const EntityId serversActorId)
{
    CHG_ASSERT(m_pEntityFactory);

    StrongEntityPtr pActor = m_pEntityFactory->CreateEntity(actorResource.c_str(), overrides, initialTransform, serversActorId);
    if (pActor)
    {
        m_entities.insert(std::make_pair(pActor->GetId(), pActor));
        return pActor;
    }
    else
    {
        // FUTURE WORK: Log error: couldn't create actor
        return StrongEntityPtr();
    }
}

StrongEntityPtr BaseGameLogic::VCreateStaticEntity(TiXmlElement* overrides, const Mat4x4* initialTransform, const EntityId serversActorId)
{
    CHG_ASSERT(m_pEntityFactory);

    StrongEntityPtr pActor = m_pEntityFactory->CreateStaticObject(overrides, initialTransform, serversActorId);
    if (pActor)
    {
        m_entities.insert(std::make_pair(pActor->GetId(), pActor));
        return pActor;
    }
    else
    {
        // FUTURE WORK: Log error: couldn't create actor
        return StrongEntityPtr();
    }
}

void BaseGameLogic::VDestroyEntity(const EntityId actorId)
{
    // We need to trigger a synchronous event to ensure that any systems responding to this event can still access a 
    // valid actor if need be.  The actor will be destroyed after this.
    shared_ptr<EvtData_Destroy_Actor> pEvent(CHG_NEW EvtData_Destroy_Actor(actorId));
    IEventManager::Get()->VTriggerEvent(pEvent);

    auto findIt = m_entities.find(actorId);
    if (findIt != m_entities.end())
    {
        findIt->second->Destroy();
        m_entities.erase(findIt);
    }
}

WeakEntityPtr BaseGameLogic::VGetEntity(const EntityId actorId)
{
    EntityMap::iterator findIt = m_entities.find(actorId);
    if (findIt != m_entities.end())
        return findIt->second;
    return WeakEntityPtr();
}

WeakEntityPtr BaseGameLogic::VGetEntity(const std::string entityName)
{
    for (auto pEntity : m_entities)
    {
        if (pEntity.second->GetName() == entityName)
            return pEntity.second;
    }

    return WeakEntityPtr();
}

void BaseGameLogic::VModifyEntity(const EntityId actorId, TiXmlElement* overrides)
{
    CHG_ASSERT(m_pEntityFactory);
	if (!m_pEntityFactory)
		return;

	auto findIt = m_entities.find(actorId);
    if (findIt != m_entities.end())
    {
		m_pEntityFactory->ModifyEntity(findIt->second, overrides);
	}
}

void BaseGameLogic::VOnUpdate(float time, float elapsedTime)
{
	int deltaMilliseconds = int(elapsedTime * 1000.0f);
	m_Lifetime += elapsedTime;

	switch(m_State)
	{
		case BGS_Initializing:
			// If we get to here we're ready to attach players
			VChangeState(BGS_MainMenu);
			break;

		case BGS_MainMenu:
			break;

		case BGS_LoadingGameEnvironment:

			break;

		case BGS_WaitingForPlayersToLoadEnvironment:
			if (m_ExpectedPlayers + m_ExpectedRemotePlayers <= m_HumanGamesLoaded)
			{
				VChangeState(BGS_SpawningPlayersActors);
			}
			break;

		case BGS_SpawningPlayersActors:
			VChangeState(BGS_Running);
			break;

		case BGS_WaitingForPlayers:
			if (m_ExpectedPlayers + m_ExpectedRemotePlayers == m_HumanPlayersAttached ) 
			{
				// We have to wait until it arrives before loading the level
				if (!g_pApp->m_Options.m_Level.empty())
				{
					VChangeState(BGS_LoadingGameEnvironment);
				}
			}
			break;

		case BGS_Running:
			m_pProcessManager->UpdateProcesses(deltaMilliseconds);

            if(m_pPhysics /*&& !m_bProxy*/)
            {
                m_pPhysics->VOnUpdate(elapsedTime);
                m_pPhysics->VSyncVisibleScene();
            }

			break;

		default:
			CHG_ERROR("Unrecognized state.");
	}

    // update all game views
    for (GameViewList::iterator it = m_gameViews.begin(); it != m_gameViews.end(); ++it)
	{
		(*it)->VOnUpdate(deltaMilliseconds);
	}

	// update game entities
	if (!m_bPause)
	{
		for (EntityMap::const_iterator it = m_entities.begin(); it != m_entities.end(); ++it)
		{
			it->second->Update(deltaMilliseconds);
		}
	}
}

//
// BaseGameLogic::VChangeState
//
void BaseGameLogic::VChangeState(BaseGameState newState)
{
	if (newState==BGS_WaitingForPlayers)
	{
		// Get rid of the Main Menu...
		m_gameViews.pop_front();

		// Note: Split screen support would require this to change!
		m_ExpectedPlayers = 1;
		m_ExpectedRemotePlayers = g_pApp->m_Options.m_expectedPlayers - 1;
		m_ExpectedAI = g_pApp->m_Options.m_numAIs;

		if (!g_pApp->m_Options.m_gameHost.empty())
		{
			//VSetProxy();					
			m_ExpectedAI = 0;				// the server will create these
			m_ExpectedRemotePlayers = 0;	// the server will create these
			
			
			// Throw up a main menu
			VChangeState(BGS_MainMenu);
			return;
			
		}		
	}
	else if (newState == BGS_LoadingGameEnvironment)
	{
		m_State = newState;
		if (!g_pApp->VLoadGame())
		{
			CHG_ERROR("The game failed to load.");
			g_pApp->AbortGame();
		}
		else
		{
			VChangeState(BGS_WaitingForPlayersToLoadEnvironment);			// we must wait for all human player to report their environments are loaded.
			return;
		}
	}

	m_State = newState;
}


// Chapter 19/20 refactor work
//   - move physics pointer into BaseGameLogic
//   - but make the inherited logic choose the implementation
void BaseGameLogic::VRenderDiagnostics() 
{ 
	if (m_RenderDiagnostics)
	{
		m_pPhysics->VRenderDiagnostics();
	}
}


void BaseGameLogic::VAddView(shared_ptr<IGameView> pView, EntityId actorId)
{
    static int iViewIdGenerator = 0;

	// This makes sure that all views have a non-zero view id.
    int viewId = static_cast<int>(iViewIdGenerator++);
	m_gameViews.push_back(pView);
	pView->VOnAttach(viewId, actorId);
	pView->VOnRestore();
}

std::shared_ptr<IGameView> BaseGameLogic::VFindView(GameViewId viewId)
{
    std::list<std::shared_ptr<IGameView>>::iterator i = m_gameViews.begin();
    while (i != m_gameViews.end())
    {
        if ((*i)->VGetId() == viewId)
            return *i;

		++i;
    }

    return std::shared_ptr < IGameView > ();
}

void BaseGameLogic::VRemoveView(shared_ptr<IGameView> pView)
{
	m_gameViews.remove(pView);
}


EntityFactory* BaseGameLogic::VCreateEntityFactory(void)
{
    return CHG_NEW EntityFactory;
}


