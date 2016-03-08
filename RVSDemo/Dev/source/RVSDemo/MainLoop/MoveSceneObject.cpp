//============================================================================
// MoveLightProcess.cpp : implement MoveLightProcess class
//
//============================================================================

#include "MoveSceneObject.h"
#include <EventManager\EventManager.h>
#include <Entities\TransformComponent.h>
#include "..\RVSDemoEvents.h"

void MoveSceneObjectProcess::VOnInit(void)
{
	m_pPointLightEntity = MakeStrongPtr(g_pApp->m_pGame->VGetEntity("PointLight_0"));
	m_pEmissiveSphereEntity = MakeStrongPtr(g_pApp->m_pGame->VGetEntity("PointLightSphere"));
	m_pRedSphereEntity = MakeStrongPtr(g_pApp->m_pGame->VGetEntity("RedSphere"));
	m_pGreenSphereEntity = MakeStrongPtr(g_pApp->m_pGame->VGetEntity("GreenSphere"));
	m_pSilverSphereEntity = MakeStrongPtr(g_pApp->m_pGame->VGetEntity("SilverSphere"));
	m_pBrickSphereEntity = MakeStrongPtr(g_pApp->m_pGame->VGetEntity("BrickSphere"));

	m_fPointLightAngularSpeed = 0.3f;
	m_fGreen_Red_Sphere_AngularSpeed = -0.174f;
	m_fSilver_Brick_AngularSpeed = 0.174f;

	m_fSilver_Brick_Sphere_AngularDelta = 0.0f;

	Process::VOnInit();
}

void MoveSceneObjectProcess::VOnUpdate(unsigned long deltaMs)
{
	float fPointLightAngularDelta = m_fPointLightAngularSpeed*(deltaMs / 1000.0f);
	float fGreen_Red_Sphere_AngularDelta = m_fGreen_Red_Sphere_AngularSpeed*(deltaMs / 1000.0f);
	m_fSilver_Brick_Sphere_AngularDelta += m_fSilver_Brick_AngularSpeed*(deltaMs / 1000.0f);

	if (m_pPointLightEntity && m_pEmissiveSphereEntity && m_pRedSphereEntity && m_pGreenSphereEntity)
	{
		shared_ptr<TransformComponent> pPointLightTransformComponent = MakeStrongPtr(m_pPointLightEntity->GetComponent<TransformComponent>(TransformComponent::g_Name));
		shared_ptr<TransformComponent> pEmissiveSphereTransformComponent = MakeStrongPtr(m_pEmissiveSphereEntity->GetComponent<TransformComponent>(TransformComponent::g_Name));
		shared_ptr<TransformComponent> pRedSphereTransformComponent = MakeStrongPtr(m_pRedSphereEntity->GetComponent<TransformComponent>(TransformComponent::g_Name));
		shared_ptr<TransformComponent> pGreenSphereTransformComponent = MakeStrongPtr(m_pGreenSphereEntity->GetComponent<TransformComponent>(TransformComponent::g_Name));
		shared_ptr<TransformComponent> pSilverSphereTransformComponent = MakeStrongPtr(m_pSilverSphereEntity->GetComponent<TransformComponent>(TransformComponent::g_Name));
		shared_ptr<TransformComponent> pBrickSphereTransformComponent = MakeStrongPtr(m_pBrickSphereEntity->GetComponent<TransformComponent>(TransformComponent::g_Name));
		
		if (pPointLightTransformComponent && pEmissiveSphereTransformComponent && pRedSphereTransformComponent && pGreenSphereTransformComponent && pSilverSphereTransformComponent && pBrickSphereTransformComponent)
		{
			pPointLightTransformComponent->RotateY(fPointLightAngularDelta);
			pEmissiveSphereTransformComponent->RotateY(fPointLightAngularDelta);
			pRedSphereTransformComponent->RotateY(fGreen_Red_Sphere_AngularDelta);
			pGreenSphereTransformComponent->RotateY(fGreen_Red_Sphere_AngularDelta);
			pBrickSphereTransformComponent->SetTransform(translate(-1.25f, 3.5f, 0.0f) * rotateAxis(Vec3(-0.707f, 0.707f, 0.0f), m_fSilver_Brick_Sphere_AngularDelta + CHG_PI) * translate(2.5f, 0.0f, 0.0f) * scale(0.5f, 0.5f, 0.5f));
			pSilverSphereTransformComponent->SetTransform(translate(1.25f, 3.5f, 0.0f) * rotateAxis(Vec3(0.707f, 0.707f, 0.0f), -m_fSilver_Brick_Sphere_AngularDelta) * translate(-2.5f, 0.0f, 0.0f) * scale(0.5f, 0.5f, 0.5f));
			
			shared_ptr<EvtData_Move_Actor> pPointLightEvent(CHG_NEW EvtData_Move_Actor(m_pPointLightEntity->GetId(), pPointLightTransformComponent->GetTransform()));
			IEventManager::Get()->VTriggerEvent(pPointLightEvent);

			shared_ptr<EvtData_Move_Actor> pEmissiveSphereEvent(CHG_NEW EvtData_Move_Actor(m_pEmissiveSphereEntity->GetId(), pEmissiveSphereTransformComponent->GetTransform()));
			IEventManager::Get()->VTriggerEvent(pEmissiveSphereEvent);

			shared_ptr<EvtData_Move_Actor> pRedSphereEvent(CHG_NEW EvtData_Move_Actor(m_pRedSphereEntity->GetId(), pRedSphereTransformComponent->GetTransform()));
			IEventManager::Get()->VTriggerEvent(pRedSphereEvent);

			shared_ptr<EvtData_Move_Actor> pGreenSphereEvent(CHG_NEW EvtData_Move_Actor(m_pGreenSphereEntity->GetId(), pGreenSphereTransformComponent->GetTransform()));
			IEventManager::Get()->VTriggerEvent(pGreenSphereEvent);

			shared_ptr<EvtData_Move_Actor> pBrickSphereEvent(CHG_NEW EvtData_Move_Actor(m_pBrickSphereEntity->GetId(), pBrickSphereTransformComponent->GetTransform()));
			IEventManager::Get()->VTriggerEvent(pBrickSphereEvent);

			shared_ptr<EvtData_Move_Actor> pSilverSphereEvent(CHG_NEW EvtData_Move_Actor(m_pSilverSphereEntity->GetId(), pSilverSphereTransformComponent->GetTransform()));
			IEventManager::Get()->VTriggerEvent(pSilverSphereEvent);
		}
	}
}