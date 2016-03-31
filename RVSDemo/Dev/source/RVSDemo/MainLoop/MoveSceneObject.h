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
	std::shared_ptr<Entity> m_pPointLightEntity0;
	std::shared_ptr<Entity> m_pPointLightEntity1;
	std::shared_ptr<Entity> m_pPointLightEntity2;
	std::shared_ptr<Entity> m_pPointLightEntity3;
	std::shared_ptr<Entity> m_pPointLightSphereEntity0;
	std::shared_ptr<Entity> m_pPointLightSphereEntity1;
	std::shared_ptr<Entity> m_pPointLightSphereEntity2;
	std::shared_ptr<Entity> m_pPointLightSphereEntity3;
	std::shared_ptr<Entity> m_pSilverSphereEntity;

public:
	MoveSceneObjectProcess() { }

protected:
	virtual void VOnInit(void);  // called during the first update; responsible for setting the initial state (typically RUNNING)
	virtual void VOnUpdate(unsigned long deltaMs);  // called every frame
};