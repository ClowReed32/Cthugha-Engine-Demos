//========================================================================
// MoveLightProcess.h - defines the class for move Point and Sphere Test
//
//========================================================================

#pragma once

#include <Msvc\CthughaStd.h>
#include <Graphics\Light.h>

//////////////////////////////////////////////////////////////////////////
//
//	MoveSceneObjectProcess class definition
//
//////////////////////////////////////////////////////////////////////////

class MoveSceneObjectProcess : public Process
{
	std::shared_ptr<Entity> m_pPointLightEntity;
	std::shared_ptr<Entity> m_pEmissiveSphereEntity;
	std::shared_ptr<Entity> m_pRedSphereEntity;
	std::shared_ptr<Entity> m_pGreenSphereEntity;
	std::shared_ptr<Entity> m_pSilverSphereEntity;
	std::shared_ptr<Entity> m_pBrickSphereEntity;

	float m_fPointLightAngularSpeed, m_fGreen_Red_Sphere_AngularSpeed, m_fSilver_Brick_AngularSpeed;
	float m_fSilver_Brick_Sphere_AngularDelta;

public:
	MoveSceneObjectProcess() { }

protected:
	virtual void VOnInit(void);  // called during the first update; responsible for setting the initial state (typically RUNNING)
	virtual void VOnUpdate(unsigned long deltaMs);  // called every frame
};