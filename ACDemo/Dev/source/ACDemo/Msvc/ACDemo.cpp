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

#include "ACDemo.h"
#include "..\ACDemoView.h"
#include "..\ACDemoEvents.h"
#include "..\Entity\ACDemoEntityFactory.h"
#include "..\MainLoop\MoveSceneObject.h"

AncientCivilizationDemoApp g_EvolutioneApp;

int main(int argc, char *argv[])
{
	return CthughaAppEntry(argc, argv);
}

BaseGameLogic *AncientCivilizationDemoApp::VCreateGameAndView()
{
	m_pGame = CHG_NEW AncientCivilizationDemoLogic();
    m_pGame->Init();

	shared_ptr<IGameView> menuView(CHG_NEW MainMenuView(g_pApp->m_pRenderer));
	m_pGame->VAddView(menuView);
    m_pGame->ChangeCurrentHumanView(menuView->VGetId());

	return m_pGame;
}

bool AncientCivilizationDemoLogic::VLoadGameDelegate(TiXmlElement* pLevelData)
{
    CHG_INFO("------ Init Game ------");

	g_pApp->m_pVoxelManager->SetActiveVolumeLighting(false);

	std::shared_ptr<MoveSceneObjectProcess> pMoveObjectsProcess(CHG_NEW MoveSceneObjectProcess());
	m_pProcessManager->AttachProcess(pMoveObjectsProcess);

	return true;
}

AncientCivilizationDemoLogic::AncientCivilizationDemoLogic()
{
	m_pPhysics.reset(CreateGamePhysics());
	RegisterAllDelegates();
}

AncientCivilizationDemoLogic::~AncientCivilizationDemoLogic()
{
	RemoveAllDelegates();
}

EntityFactory* AncientCivilizationDemoLogic::VCreateEntityFactory(void)
{
	return CHG_NEW ACDemoEntityFactory;
}

void AncientCivilizationDemoLogic::VChangeState(BaseGameState newState)
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

void AncientCivilizationDemoLogic::VOnUpdate(float time, float elapsedTime)
{
	BaseGameLogic::VOnUpdate(time, elapsedTime);
}

void AncientCivilizationDemoLogic::RequestStartGameDelegate(IEventDataPtr pEventData)
{
	VChangeState(BGS_WaitingForPlayers);
}

void AncientCivilizationDemoLogic::EnvironmentLoadedDelegate(IEventDataPtr pEventData)
{
    ++m_HumanGamesLoaded;
}

void AncientCivilizationDemoLogic::StartThrustDelegate(IEventDataPtr pEventData)
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

void AncientCivilizationDemoLogic::EndThrustDelegate(IEventDataPtr pEventData)
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

void AncientCivilizationDemoLogic::StartSteerDelegate(IEventDataPtr pEventData)
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

void AncientCivilizationDemoLogic::EndSteerDelegate(IEventDataPtr pEventData)
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

void AncientCivilizationDemoLogic::StartJumpDelegate(IEventDataPtr pEventData)
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

void AncientCivilizationDemoLogic::RegisterAllDelegates(void)
{
    IEventManager* pGlobalEventManager = IEventManager::Get();
	pGlobalEventManager->VAddListener(MakeDelegate(this, &AncientCivilizationDemoLogic::RequestStartGameDelegate), EvtData_Request_Start_Game::sk_EventType);
	pGlobalEventManager->VAddListener(MakeDelegate(this, &AncientCivilizationDemoLogic::EnvironmentLoadedDelegate), EvtData_Environment_Loaded::sk_EventType);

	pGlobalEventManager->VAddListener(MakeDelegate(this, &AncientCivilizationDemoLogic::StartThrustDelegate), EvtData_StartThrust::sk_EventType);
	pGlobalEventManager->VAddListener(MakeDelegate(this, &AncientCivilizationDemoLogic::EndThrustDelegate), EvtData_EndThrust::sk_EventType);
	pGlobalEventManager->VAddListener(MakeDelegate(this, &AncientCivilizationDemoLogic::StartSteerDelegate), EvtData_StartSteer::sk_EventType);
	pGlobalEventManager->VAddListener(MakeDelegate(this, &AncientCivilizationDemoLogic::EndSteerDelegate), EvtData_EndSteer::sk_EventType);
	pGlobalEventManager->VAddListener(MakeDelegate(this, &AncientCivilizationDemoLogic::StartJumpDelegate), EvtData_StartJump::sk_EventType);
}

void AncientCivilizationDemoLogic::RemoveAllDelegates(void)
{
    IEventManager* pGlobalEventManager = IEventManager::Get();
	pGlobalEventManager->VRemoveListener(MakeDelegate(this, &AncientCivilizationDemoLogic::RequestStartGameDelegate), EvtData_Request_Start_Game::sk_EventType);
	pGlobalEventManager->VRemoveListener(MakeDelegate(this, &AncientCivilizationDemoLogic::EnvironmentLoadedDelegate), EvtData_Environment_Loaded::sk_EventType);

	pGlobalEventManager->VRemoveListener(MakeDelegate(this, &AncientCivilizationDemoLogic::StartThrustDelegate), EvtData_StartThrust::sk_EventType);
	pGlobalEventManager->VRemoveListener(MakeDelegate(this, &AncientCivilizationDemoLogic::EndThrustDelegate), EvtData_EndThrust::sk_EventType);
	pGlobalEventManager->VRemoveListener(MakeDelegate(this, &AncientCivilizationDemoLogic::StartSteerDelegate), EvtData_StartSteer::sk_EventType);
	pGlobalEventManager->VRemoveListener(MakeDelegate(this, &AncientCivilizationDemoLogic::EndSteerDelegate), EvtData_EndSteer::sk_EventType);
	pGlobalEventManager->VRemoveListener(MakeDelegate(this, &AncientCivilizationDemoLogic::StartJumpDelegate), EvtData_StartJump::sk_EventType);
}

void AncientCivilizationDemoLogic::VAddView(shared_ptr<IGameView> pView, EntityId entity)
{
	BaseGameLogic::VAddView(pView, entity);
}