// PlayerController.cpp - Controller class for the player teapots
//
//========================================================================

#include <Msvc\CthughaStd.h>

#include <CthughaEngine\CthughaApp.h>
#include <Graphics\SceneNodes.h>
#include <EventManager\EventManager.h>
#include <Physics\PhysicsEventListener.h>

#include "DynamicPlayerController.h"
#include "ACDemoEvents.h"
#include "ACDemo.h"

const float ENTITY_VELOCITY = 7.0f;
const float ENTITY_ANGULAR_ACCELERATION = 22.0f;
const float ENTITY_JUMP_ACCELERATION = 100.0f;


////////////////////////////////////////////////////
// PlayerController Implementation
////////////////////////////////////////////////////


//
// PlayerController::PlayerController			
//
PlayerController::PlayerController(shared_ptr<SceneNode> object, shared_ptr<SceneNode> camera)
: m_object(object)
{
	memset(m_bKey, 0x00, sizeof(m_bKey));
	
	object->Transform(&m_matToWorld, &m_matFromWorld);
	m_pCameraNode = camera;

	//Correct right Vector
	m_matToWorld.rows[0].x = -m_matToWorld.rows[0].x;
	m_matToWorld.rows[1].x = -m_matToWorld.rows[1].x;
	m_matToWorld.rows[2].x = -m_matToWorld.rows[2].x;

	m_matFromWorld.rows[0].x = -m_matFromWorld.rows[0].x;
	m_matFromWorld.rows[1].x = -m_matFromWorld.rows[1].x;
	m_matFromWorld.rows[2].x = -m_matFromWorld.rows[2].x;

	m_pCameraNode->VSetTransform(&m_matToWorld, &m_matFromWorld);
	m_object = object;

    //Init euler angles from matrix
	float Yaw, Pitch, Row;
	m_matToWorld.GetRotationAngles(Yaw, Pitch, Row);

    m_fTargetYaw = m_fYaw = Yaw;
    m_fTargetPitch = m_fPitch = Pitch; 

	///////////////////////////////////////////////////////////////////////
	m_Right = m_matToWorld.rows[0].xyz();
	m_Up = m_matToWorld.rows[1].xyz();
	m_Look = m_matToWorld.rows[2].xyz();
	///////////////////////////////////////////////////////////////////////

	m_maxSpeed = 30.0f;			// 30 meters per second
	m_currentSpeed = 0.0f;

	Vec3 pos = m_matToWorld.GetPosition();

	m_matPosition = translate(pos);

    int x0, y0;
    SDL_GetMouseState(&x0, &y0); //----------------------------------------------> This is not good
	SDL_SetRelativeMouseMode(SDL_TRUE); //----------------------------------------
	m_lastMousePos.x = (float)x0;
    m_lastMousePos.y = (float)y0;
}



//
// PlayerController::VOnLButtonDown				
//
bool PlayerController::VOnPointerButtonDown(const int &posX, const int &posY, const int &relPosX, const int &relPosY, const int radius, const CHG_MouseButton &mouseButton)
{
	if (mouseButton != CHG_BUTTON_LEFT)
		return false;

	return true;
}

bool PlayerController::VOnPointerMove(const int &posX, const int &posY, const int &relPosX, const int &relPosY, const int radius)
{
	Vec2 mousePos;

	mousePos.x = (float)posX;
	mousePos.y = (float)posY;

	m_relativeMousePos = Vec2((float)relPosX, (float)relPosY);

	m_lastMousePos = mousePos;
	
	return true;
}

//
// PlayerController::OnUpdate				
//
void PlayerController::OnUpdate(DWORD const deltaMilliseconds)  
{
    bool bTranslating = false;
	Vec4 atWorld(0,0,0,0);
	Vec4 rightWorld(0,0,0,0);
	Vec4 upWorld(0,0,0,0);

	//Handling rotation as a result of mouse position
	{
		m_fTargetYaw += m_relativeMousePos.x*0.007f;
		m_fTargetYaw = m_fTargetYaw > 2.0f*CHG_PI ? m_fTargetYaw - 2.0f*CHG_PI : m_fTargetYaw;
		m_fTargetPitch += -m_relativeMousePos.y*0.007f;

        if(m_fTargetPitch < 0.5f*CHG_PI && m_fTargetPitch > -0.5f*CHG_PI)
        {
		    ///////////////////////////////////////////////////////////////////////
		    //Pitch Transform
		    Mat4x4 pitchRotate = rotateAxis(m_Right, -m_relativeMousePos.y*0.007f);
		    // rotate _up and _look around _right vector
		    m_Up = (pitchRotate * Vec4(m_Up, 0.0f)).xyz();
		    m_Look = (pitchRotate * Vec4(m_Look, 0.0f)).xyz();            
        }
        else
        {
            m_fTargetPitch -= -m_relativeMousePos.y*0.007f;
        }

		//Yaw Transform
		Mat4x4 yawRotate = rotateY(m_relativeMousePos.x*0.007f);
		// rotate _right and _look around _up or y-axis
		m_Up = (yawRotate * Vec4(m_Up, 0.0f)).xyz();
		m_Right = (yawRotate * Vec4(m_Right, 0.0f)).xyz();
		m_Look = (yawRotate * Vec4(m_Look, 0.0f)).xyz();

		m_relativeMousePos.x = 0.0f;
		m_relativeMousePos.y = 0.0f;

        Vec3 objectPos = m_object->ToWorld().GetPosition();

		/*m_Right = cross(m_Up, m_Look);
		m_Right = normalize(m_Right);
		m_Up = cross(m_Right, m_Look);*/

		m_matFromWorld = Mat4x4(m_Right.x,									m_Up.x,									 -m_Look.x,								   0.0f,
								m_Right.y,									m_Up.y,									 -m_Look.y,								   0.0f,
								m_Right.z,									m_Up.z,									 -m_Look.z,								   0.0f,
								-dot(objectPos, m_Right),                   -dot(objectPos, m_Up),                   dot(objectPos, m_Look),                   1.0f);
		///////////////////////////////////////////////////////////////////////

		m_matToWorld = Mat4x4(m_Right.x, m_Up.x, m_Look.x,  objectPos.x,
							  m_Right.y, m_Up.y, m_Look.y,  objectPos.y,
							  m_Right.z, m_Up.z, m_Look.z,  objectPos.z,
							  0.0f,	  0.0f,    0.0f,      1.0f);

		/*printf("-----------------------------------------------------------\n");
		printf("Right : ( %f, %f, %f ) \n", m_Right.x, m_Right.y, m_Right.z);
		printf("Up : ( %f, %f, %f ) \n", m_Up.x, m_Up.y, m_Up.z);
		printf("Look : ( %f, %f, %f ) \n", m_Look.x, m_Look.y, m_Look.z);
        printf("Pos : ( %f, %f, %f ) \n", m_matPosition.GetPosition().x, m_matPosition.GetPosition().y, m_matPosition.GetPosition().z);
		printf("-----------------------------------------------------------\n");*/

		m_pCameraNode->VSetTransform(&m_matToWorld, &m_matFromWorld);
	}

	if(m_bKey['w'] || m_bKey['s'] || m_bKey['a'] || m_bKey['d'])
	{
		Vec3 Vel = Vec3(0.0f, 0.0f, 0.0f);

		if(m_bKey['w'])
			Vel += Vec3(m_Look.x, 0.0f, m_Look.z);

		if(m_bKey['s'])
			Vel += Vec3(-m_Look.x, 0.0f, -m_Look.z);

		if(m_bKey['d'])
			Vel += Vec3(m_Right.x, 0.0f, m_Right.z);

		if(m_bKey['a'])
			Vel += Vec3(-m_Right.x, 0.0f, -m_Right.z);

		const EntityId entityId = m_object->VGetEntityID();
        shared_ptr<EvtData_StartThrust> pEvent(CHG_NEW EvtData_StartThrust(entityId, ENTITY_VELOCITY*normalize(Vel)));
        IEventManager::Get()->VQueueEvent(pEvent);
	}
}

bool PlayerController::VOnKeyDown(const BYTE c)
{
    // update the key table
    m_bKey[c] = true;

	if (c == ' ')
	{
		const EntityId entityId = m_object->VGetEntityID();
        shared_ptr<EvtData_StartJump> pEvent(CHG_NEW EvtData_StartJump(entityId, ENTITY_JUMP_ACCELERATION));
        IEventManager::Get()->VQueueEvent(pEvent);
	}

    return true;
}

bool PlayerController::VOnKeyUp(const BYTE c)
{
    // update the key table
    m_bKey[c] = false;

    // send an end steer event if necessary
    if (!m_bKey['w'] && !m_bKey['s'] && !m_bKey['a'] && !m_bKey['d'])
    {
        const EntityId entityId = m_object->VGetEntityID();
        shared_ptr<EvtData_EndThrust> pEvent(CHG_NEW EvtData_EndThrust(entityId));
        IEventManager::Get()->VQueueEvent(pEvent);
    }

    return true;
}
