#include <Msvc/CthughaStd.h>
#include <CthughaEngine/CthughaApp.h>
#include <Physics/Physics.h>
#include <Physics/PhysicsEventListener.h>
#include <MainLoop/Initialization.h>
#include <EventManager/Events.h>
#include <EventManager/EventManagerImpl.h>
#include <Entities/Entity.h>
#include <Entities/PhysicsComponent.h>
#include <Entities/TransformComponent.h>
#include <Utilities/String.h>

#include "RVSDemo.h"
#include "..\RVSDemoView.h"
#include "..\RVSDemoEvents.h"
#include "..\Entity\RVSDemoEntityFactory.h"
#include "..\MainLoop\MoveSceneObject.h"

RVSDemoApp g_EvolutioneApp;

int main(int argc, char *argv[])
{
	return CthughaAppEntry(argc, argv);
}

BaseGameLogic *RVSDemoApp::VCreateGameAndView()
{
	m_pGame = CHG_NEW RVSDemoLogic();
    m_pGame->Init();

	shared_ptr<IGameView> menuView(CHG_NEW MainMenuView(g_pApp->m_pRenderer));
	m_pGame->VAddView(menuView);
    m_pGame->ChangeCurrentHumanView(menuView->VGetId());

	return m_pGame;
}

bool RVSDemoLogic::VLoadGameDelegate(TiXmlElement* pLevelData)
{
    CHG_INFO("------ Init Game ------");

	std::shared_ptr<MoveSceneObjectProcess> pMoveObjectsProcess(CHG_NEW MoveSceneObjectProcess());
	m_pProcessManager->AttachProcess(pMoveObjectsProcess);

	return true;
}

RVSDemoLogic::RVSDemoLogic()
{
	m_pPhysics.reset(CreateGamePhysics());
	RegisterAllDelegates();
}

RVSDemoLogic::~RVSDemoLogic()
{
	RemoveAllDelegates();
}

EntityFactory* RVSDemoLogic::VCreateEntityFactory(void)
{
	return CHG_NEW RVSDemoEntityFactory;
}

void RVSDemoLogic::VChangeState(BaseGameState newState)
{
	BaseGameLogic::VChangeState(newState);

	switch(newState)
	{
		case BGS_WaitingForPlayers:
		{

			// spawn all local players (should only be one, though we might support more in the future)
			CHG_ASSERT(m_ExpectedPlayers == 1);
			for (int i = 0; i < m_ExpectedPlayers; ++i)
			{
				shared_ptr<IGameView> playersView(CHG_NEW AncientCivilizationDemoHumanView(g_pApp->m_pRenderer));
				VAddView(playersView);
				ChangeCurrentHumanView(playersView->VGetId());
			}

			m_HumanPlayersAttached++; //Eliminar despues

			break;
		}


		case BGS_SpawningPlayersActors:
		{
			for (auto it = m_gameViews.begin(); it != m_gameViews.end(); ++it)
			{
				shared_ptr<IGameView> pView = *it;
				if (pView->VGetType() == GameView_Human)
				{
					StrongEntityPtr pActor = VCreateEntity("Entities\\PlayerEntity.ent", NULL);
					if (pActor)
					{
						shared_ptr<EvtData_Set_Controlled_Actor> pSetControlledEvent(CHG_NEW EvtData_Set_Controlled_Actor(pActor->GetId()));
                        IEventManager::Get()->VTriggerEvent(pSetControlledEvent); // --------------------------------------------------------------> CHANGE OTHER C CLASS OR SCRIPT
					}
				}
			}

			break;
		}
	}
}

void RVSDemoLogic::VOnUpdate(float time, float elapsedTime)
{
	BaseGameLogic::VOnUpdate(time, elapsedTime);
}

void RVSDemoLogic::RequestStartGameDelegate(IEventDataPtr pEventData)
{
	VChangeState(BGS_WaitingForPlayers);
}

void RVSDemoLogic::EnvironmentLoadedDelegate(IEventDataPtr pEventData)
{
    ++m_HumanGamesLoaded;
}

void RVSDemoLogic::StartThrustDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EvtData_StartThrust> pCastEventData = static_pointer_cast<EvtData_StartThrust>(pEventData);
    StrongEntityPtr pActor = MakeStrongPtr(VGetEntity(pCastEventData->GetActorId()));
    if (pActor)
    {
        shared_ptr<PhysicsComponent> pPhysicalComponent = MakeStrongPtr(pActor->GetComponent<PhysicsComponent>(PhysicsComponent::g_Name));
        if (pPhysicalComponent)
        {
			Vec3 oldVelocity = pPhysicalComponent->GetVelocity();
			Vec3 newVelocity = pCastEventData->GetVelocity();
            pPhysicalComponent->SetVelocity(Vec3(newVelocity.x, oldVelocity.y, newVelocity.z));
			pPhysicalComponent->SetVelocityFactor(Vec3(1.0f, 1.0f, 1.0f));
        }
    }
}

void RVSDemoLogic::EndThrustDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EvtData_EndThrust> pCastEventData = static_pointer_cast<EvtData_EndThrust>(pEventData);
    StrongEntityPtr pActor = MakeStrongPtr(VGetEntity(pCastEventData->GetActorId()));
    if (pActor)
    {
        shared_ptr<PhysicsComponent> pPhysicalComponent = MakeStrongPtr(pActor->GetComponent<PhysicsComponent>(PhysicsComponent::g_Name));
        if (pPhysicalComponent)
        {
			Vec3 oldVelocity = pPhysicalComponent->GetVelocity();
            pPhysicalComponent->SetVelocity(Vec3(0.0f, oldVelocity.y, 0.0f));
			pPhysicalComponent->SetVelocityFactor(Vec3(0.0f, 1.0f, 0.0f));
        }
    }
}

void RVSDemoLogic::StartSteerDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EvtData_StartSteer> pCastEventData = static_pointer_cast<EvtData_StartSteer>(pEventData);
    StrongEntityPtr pActor = MakeStrongPtr(VGetEntity(pCastEventData->GetActorId()));
    if (pActor)
    {
        shared_ptr<PhysicsComponent> pPhysicalComponent = MakeStrongPtr(pActor->GetComponent<PhysicsComponent>(PhysicsComponent::g_Name));
        if (pPhysicalComponent)
        {
            pPhysicalComponent->ApplyAngularAcceleration(pCastEventData->GetAcceleration());
        }
    }
}

void RVSDemoLogic::EndSteerDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EvtData_EndSteer> pCastEventData = static_pointer_cast<EvtData_EndSteer>(pEventData);
    StrongEntityPtr pActor = MakeStrongPtr(VGetEntity(pCastEventData->GetActorId()));
    if (pActor)
    {
        shared_ptr<PhysicsComponent> pPhysicalComponent = MakeStrongPtr(pActor->GetComponent<PhysicsComponent>(PhysicsComponent::g_Name));
        if (pPhysicalComponent)
        {
            pPhysicalComponent->RemoveAngularAcceleration();
        }
    }
}

void RVSDemoLogic::StartJumpDelegate(IEventDataPtr pEventData)
{
	shared_ptr<EvtData_StartJump> pCastEventData = static_pointer_cast<EvtData_StartJump>(pEventData);
    StrongEntityPtr pActor = MakeStrongPtr(VGetEntity(pCastEventData->GetActorId()));
    if (pActor)
    {
        shared_ptr<PhysicsComponent> pPhysicalComponent = MakeStrongPtr(pActor->GetComponent<PhysicsComponent>(PhysicsComponent::g_Name));
        if (pPhysicalComponent)
        {
			pPhysicalComponent->ApplyAcceleration(Vec3(0.0f, pCastEventData->GetAcceleration(), 0.0f));
        }
    }
}

void RVSDemoLogic::RegisterAllDelegates(void)
{
    IEventManager* pGlobalEventManager = IEventManager::Get();
	pGlobalEventManager->VAddListener(MakeDelegate(this, &RVSDemoLogic::RequestStartGameDelegate), EvtData_Request_Start_Game::sk_EventType);
	pGlobalEventManager->VAddListener(MakeDelegate(this, &RVSDemoLogic::EnvironmentLoadedDelegate), EvtData_Environment_Loaded::sk_EventType);

	pGlobalEventManager->VAddListener(MakeDelegate(this, &RVSDemoLogic::StartThrustDelegate), EvtData_StartThrust::sk_EventType);
	pGlobalEventManager->VAddListener(MakeDelegate(this, &RVSDemoLogic::EndThrustDelegate), EvtData_EndThrust::sk_EventType);
	pGlobalEventManager->VAddListener(MakeDelegate(this, &RVSDemoLogic::StartSteerDelegate), EvtData_StartSteer::sk_EventType);
	pGlobalEventManager->VAddListener(MakeDelegate(this, &RVSDemoLogic::EndSteerDelegate), EvtData_EndSteer::sk_EventType);
	pGlobalEventManager->VAddListener(MakeDelegate(this, &RVSDemoLogic::StartJumpDelegate), EvtData_StartJump::sk_EventType);
}

void RVSDemoLogic::RemoveAllDelegates(void)
{
    IEventManager* pGlobalEventManager = IEventManager::Get();
	pGlobalEventManager->VRemoveListener(MakeDelegate(this, &RVSDemoLogic::RequestStartGameDelegate), EvtData_Request_Start_Game::sk_EventType);
	pGlobalEventManager->VRemoveListener(MakeDelegate(this, &RVSDemoLogic::EnvironmentLoadedDelegate), EvtData_Environment_Loaded::sk_EventType);

	pGlobalEventManager->VRemoveListener(MakeDelegate(this, &RVSDemoLogic::StartThrustDelegate), EvtData_StartThrust::sk_EventType);
	pGlobalEventManager->VRemoveListener(MakeDelegate(this, &RVSDemoLogic::EndThrustDelegate), EvtData_EndThrust::sk_EventType);
	pGlobalEventManager->VRemoveListener(MakeDelegate(this, &RVSDemoLogic::StartSteerDelegate), EvtData_StartSteer::sk_EventType);
	pGlobalEventManager->VRemoveListener(MakeDelegate(this, &RVSDemoLogic::EndSteerDelegate), EvtData_EndSteer::sk_EventType);
	pGlobalEventManager->VRemoveListener(MakeDelegate(this, &RVSDemoLogic::StartJumpDelegate), EvtData_StartJump::sk_EventType);
}

void RVSDemoLogic::VAddView(shared_ptr<IGameView> pView, EntityId entity)
{
	BaseGameLogic::VAddView(pView, entity);
}