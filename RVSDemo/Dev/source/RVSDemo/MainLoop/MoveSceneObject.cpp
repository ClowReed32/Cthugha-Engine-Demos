//============================================================================
// MoveLightProcess.cpp : implement MoveLightProcess class
//
//============================================================================

#include "MoveSceneObject.h"
#include <EventManager\EventManager.h>
#include <Entities\TransformComponent.h>
#include "..\RVSDemoEvents.h"

#define POINTLIGHT0_ANGULAR_SPEED 0.3f
#define POINTLIGHT1_ANGULAR_SPEED 0.3f
#define POINTLIGHT2_ANGULAR_SPEED 0.3f
#define POINTLIGHT3_ANGULAR_SPEED 0.5f

void MoveSceneObjectProcess::VOnInit(void)
{
	m_pPointLightEntity0 = MakeStrongPtr(g_pApp->m_pGame->VGetEntity("PointLight_0"));
	m_pPointLightEntity1 = MakeStrongPtr(g_pApp->m_pGame->VGetEntity("PointLight_1"));
	m_pPointLightEntity2 = MakeStrongPtr(g_pApp->m_pGame->VGetEntity("PointLight_2"));
	m_pPointLightEntity3 = MakeStrongPtr(g_pApp->m_pGame->VGetEntity("PointLight_3"));
	m_pPointLightSphereEntity0 = MakeStrongPtr(g_pApp->m_pGame->VGetEntity("PointLightSphere_0"));
	m_pPointLightSphereEntity1 = MakeStrongPtr(g_pApp->m_pGame->VGetEntity("PointLightSphere_1"));
	m_pPointLightSphereEntity2 = MakeStrongPtr(g_pApp->m_pGame->VGetEntity("PointLightSphere_2"));
	m_pPointLightSphereEntity3 = MakeStrongPtr(g_pApp->m_pGame->VGetEntity("PointLightSphere_3"));
	m_pSilverSphereEntity = MakeStrongPtr(g_pApp->m_pGame->VGetEntity("SilverSphere"));

	Process::VOnInit();
}

void MoveSceneObjectProcess::VOnUpdate(unsigned long deltaMs)
{
	float fDeltaTime = deltaMs / 1000.0f;

	float fPointLightAngularDelta0 = POINTLIGHT0_ANGULAR_SPEED*fDeltaTime;
	float fPointLightAngularDelta1 = POINTLIGHT1_ANGULAR_SPEED*fDeltaTime;
	float fPointLightAngularDelta2 = POINTLIGHT2_ANGULAR_SPEED*fDeltaTime;
	float fPointLightAngularDelta3 = POINTLIGHT3_ANGULAR_SPEED*fDeltaTime;

	if (m_pPointLightEntity0 && m_pPointLightEntity1 && m_pPointLightEntity2 && m_pPointLightEntity3 && 
		m_pPointLightSphereEntity0 && m_pPointLightSphereEntity1 && m_pPointLightSphereEntity2 && 
		m_pPointLightSphereEntity3 && m_pSilverSphereEntity)
	{
		shared_ptr<TransformComponent> pPointLight0TransformComponent = MakeStrongPtr(m_pPointLightEntity0->GetComponent<TransformComponent>(TransformComponent::g_Name));
		shared_ptr<TransformComponent> pPointLight1TransformComponent = MakeStrongPtr(m_pPointLightEntity1->GetComponent<TransformComponent>(TransformComponent::g_Name));
		shared_ptr<TransformComponent> pPointLight2TransformComponent = MakeStrongPtr(m_pPointLightEntity2->GetComponent<TransformComponent>(TransformComponent::g_Name));
		shared_ptr<TransformComponent> pPointLight3TransformComponent = MakeStrongPtr(m_pPointLightEntity3->GetComponent<TransformComponent>(TransformComponent::g_Name));
		shared_ptr<TransformComponent> pPointLightSphere0TransformComponent = MakeStrongPtr(m_pPointLightSphereEntity0->GetComponent<TransformComponent>(TransformComponent::g_Name));
		shared_ptr<TransformComponent> pPointLightSphere1TransformComponent = MakeStrongPtr(m_pPointLightSphereEntity1->GetComponent<TransformComponent>(TransformComponent::g_Name));
		shared_ptr<TransformComponent> pPointLightSphere2TransformComponent = MakeStrongPtr(m_pPointLightSphereEntity2->GetComponent<TransformComponent>(TransformComponent::g_Name));
		shared_ptr<TransformComponent> pPointLightSphere3TransformComponent = MakeStrongPtr(m_pPointLightSphereEntity3->GetComponent<TransformComponent>(TransformComponent::g_Name));
		shared_ptr<TransformComponent> pSilverSphereTransformComponent = MakeStrongPtr(m_pSilverSphereEntity->GetComponent<TransformComponent>(TransformComponent::g_Name));
		
		if (pPointLight0TransformComponent && pPointLight1TransformComponent && pPointLight2TransformComponent && pPointLight3TransformComponent && pPointLightSphere0TransformComponent && 
			pPointLightSphere1TransformComponent && pPointLightSphere2TransformComponent && pPointLightSphere3TransformComponent && pSilverSphereTransformComponent)
		{
			pPointLight0TransformComponent->RotateY(fPointLightAngularDelta0);
			pPointLight1TransformComponent->RotateY(fPointLightAngularDelta1);
			pPointLight2TransformComponent->RotateY(fPointLightAngularDelta2);
			pPointLight3TransformComponent->RotateY(fPointLightAngularDelta3);
			pPointLightSphere0TransformComponent->RotateY(fPointLightAngularDelta0);
			pPointLightSphere1TransformComponent->RotateY(fPointLightAngularDelta1);
			pPointLightSphere2TransformComponent->RotateY(fPointLightAngularDelta2);
			pPointLightSphere3TransformComponent->RotateY(fPointLightAngularDelta3);
			pSilverSphereTransformComponent->RotateY(fPointLightAngularDelta0);
			
			shared_ptr<EvtData_Move_Actor> pPointLightEvent0(CHG_NEW EvtData_Move_Actor(m_pPointLightEntity0->GetId(), pPointLight0TransformComponent->GetTransform()));
			IEventManager::Get()->VTriggerEvent(pPointLightEvent0);

			shared_ptr<EvtData_Move_Actor> pPointLightEvent1(CHG_NEW EvtData_Move_Actor(m_pPointLightEntity1->GetId(), pPointLight1TransformComponent->GetTransform()));
			IEventManager::Get()->VTriggerEvent(pPointLightEvent1);

			shared_ptr<EvtData_Move_Actor> pPointLightEvent2(CHG_NEW EvtData_Move_Actor(m_pPointLightEntity2->GetId(), pPointLight2TransformComponent->GetTransform()));
			IEventManager::Get()->VTriggerEvent(pPointLightEvent2);

			shared_ptr<EvtData_Move_Actor> pPointLightEvent3(CHG_NEW EvtData_Move_Actor(m_pPointLightEntity3->GetId(), pPointLight3TransformComponent->GetTransform()));
			IEventManager::Get()->VTriggerEvent(pPointLightEvent3);

			shared_ptr<EvtData_Move_Actor> pPointLightSphereEvent0(CHG_NEW EvtData_Move_Actor(m_pPointLightSphereEntity0->GetId(), pPointLightSphere0TransformComponent->GetTransform()));
			IEventManager::Get()->VTriggerEvent(pPointLightSphereEvent0);

			shared_ptr<EvtData_Move_Actor> pPointLightSphereEvent1(CHG_NEW EvtData_Move_Actor(m_pPointLightSphereEntity1->GetId(), pPointLightSphere1TransformComponent->GetTransform()));
			IEventManager::Get()->VTriggerEvent(pPointLightSphereEvent1);

			shared_ptr<EvtData_Move_Actor> pPointLightSphereEvent2(CHG_NEW EvtData_Move_Actor(m_pPointLightSphereEntity2->GetId(), pPointLightSphere2TransformComponent->GetTransform()));
			IEventManager::Get()->VTriggerEvent(pPointLightSphereEvent2);

			shared_ptr<EvtData_Move_Actor> pPointLightSphereEvent3(CHG_NEW EvtData_Move_Actor(m_pPointLightSphereEntity3->GetId(), pPointLightSphere3TransformComponent->GetTransform()));
			IEventManager::Get()->VTriggerEvent(pPointLightSphereEvent3);

			shared_ptr<EvtData_Move_Actor> pSilverSphereEvent(CHG_NEW EvtData_Move_Actor(m_pSilverSphereEntity->GetId(), pSilverSphereTransformComponent->GetTransform()));
			IEventManager::Get()->VTriggerEvent(pSilverSphereEvent);
		}
	}
}