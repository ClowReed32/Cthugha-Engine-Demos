#pragma once
//========================================================================
// BaseGameLogic.h : Defines the Base Game Logic class 
//
//========================================================================

#include "../Mainloop/ProcessManager.h"
#include "../Utilities/Random.h"
#include "../EventManager/EventManager.h"
#include "../Entities/Entity.h"

class PathingGraph;
class ActorFactory;
class LevelManager;

enum BaseGameState
{
    BGS_Invalid,
	BGS_Initializing,
	BGS_MainMenu,
	BGS_WaitingForPlayers,
	BGS_LoadingGameEnvironment,
	BGS_WaitingForPlayersToLoadEnvironment,
	BGS_SpawningPlayersActors,
	BGS_Running
};

typedef std::map<EntityId, StrongEntityPtr> EntityMap;
typedef std::string Level;

class LevelManager
{
public:
	const std::vector<Level> &GetLevels() const { return m_Levels; }
	const int GetCurrentLevel() const { return m_CurrentLevel; }
	bool Initialize(std::vector<std::string> &levels);

protected:
	std::vector<Level> m_Levels;
	int m_CurrentLevel;
};



class BaseGameLogic : public IGameLogic
{
	friend class CthughaApp;						// This is only to gain access to the view list

protected:
	float m_Lifetime;								//indicates how long this game has been in session
	ProcessManager* m_pProcessManager;				// a game logic entity
	CHGRandom m_random;								// our RNG
	EntityMap m_entities;
	EntityId m_LastEntityId;
	GameViewId m_LastGameViewId;
	BaseGameState m_State;							// game state: loading, running, etc.
	int m_ExpectedPlayers;							// how many local human players
	int m_ExpectedRemotePlayers;					// expected remote human players
	int m_ExpectedAI;								// how many AI players
	int m_HumanPlayersAttached;
	int m_AIPlayersAttached;
	int m_HumanGamesLoaded;

	GameViewId m_CurrentHumanView;
	GameViewList m_gameViews;						// views that are attached to our game

    EntityFactory* m_pEntityFactory;

	bool m_RenderDiagnostics;						// Are we rendering diagnostics?
	shared_ptr<IGamePhysics> m_pPhysics;

	LevelManager* m_pLevelManager;					// Manages loading and chaining levels

	bool m_bPause;									// Is GameLogic in pause?

public:

	BaseGameLogic();
	virtual ~BaseGameLogic();
    bool Init(void);

	EntityId GetNewEntityID( void )
	{
		return ++m_LastEntityId;
	}
	
	GameViewId GetNewGameViewID(void)
	{
		return ++m_LastGameViewId;
	}

	CHGRandom& GetRNG(void) { return m_random; }

	virtual void VAddView(shared_ptr<IGameView> pView, EntityId actorId=INVALID_ENTITY_ID);
	virtual void VRemoveView(shared_ptr<IGameView> pView);
    virtual std::shared_ptr<IGameView> VFindView(GameViewId viewId);

	virtual StrongEntityPtr VCreateEntity(const std::string &EntityResource, std::string sName, std::vector<ComponentResource> extraComponents, const Mat4x4 *initialTransform = NULL, const EntityId serversEntityId = INVALID_ENTITY_ID);
    virtual StrongEntityPtr VCreateEntity(const std::string &actorResource, TiXmlElement *overrides, const Mat4x4* initialTransform=NULL, const EntityId serversActorId=INVALID_ENTITY_ID);  // [rez] note: don't store this strong pointer outside of this class
    virtual StrongEntityPtr VCreateStaticEntity(TiXmlElement* overrides, const Mat4x4* initialTransform=NULL, const EntityId serversActorId=INVALID_ENTITY_ID);
	virtual void VDestroyEntity(const EntityId actorId);
    virtual WeakEntityPtr VGetEntity(const EntityId entityId);
    virtual WeakEntityPtr VGetEntity(const std::string entityName);
	virtual void VModifyEntity(const EntityId entityId, TiXmlElement *overrides);

	virtual void VMoveEntity(const EntityId id, Mat4x4 const &mat) {}

    // editor functions
	std::string GetEntityXml(const EntityId id);

	// Level management
	const LevelManager* GetLevelManager() { return m_pLevelManager; }
    virtual bool VLoadGame(const char* levelResource) override;  

	// Logic Update
	virtual void VOnUpdate(float time, float elapsedTime);
	void ToggleGameLogicPause() { m_bPause = !m_bPause; }
	bool IsGamePause() { return m_bPause; }

	// Changing Game Logic State
	virtual void VChangeState(BaseGameState newState);
	const BaseGameState GetState() const { return m_State; }

    // Human View
    GameViewId GetCurrentHumanView() { return m_CurrentHumanView; }
    bool ChangeCurrentHumanView(GameViewId viewId);

	// Render Diagnostics
	void ToggleRenderDiagnostics() { m_RenderDiagnostics = !m_RenderDiagnostics; }
	virtual void VRenderDiagnostics();
	virtual shared_ptr<IGamePhysics> VGetGamePhysics(void) { return m_pPhysics; }
	
	void AttachProcess(StrongProcessPtr pProcess) { if (m_pProcessManager) {m_pProcessManager->AttachProcess(pProcess);} }

    // event delegates
    void RequestDestroyActorDelegate(IEventDataPtr pEventData);

protected:
    virtual EntityFactory* VCreateEntityFactory(void);

    // [rez] Override this function to do any game-specific loading.
    virtual bool VLoadGameDelegate(TiXmlElement* pLevelData) { return true; }
};

