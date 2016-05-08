//========================================================================
// File: MovementController.cpp
//
//========================================================================

#include "CthughaStd.h"

#include "Geometry.h"
#include "MovementController.h"
#include "SceneNodes.h"


////////////////////////////////////////////////////
// MovementController Implementation
////////////////////////////////////////////////////

#define MAX(a, b) ((a) >= (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

MovementController::MovementController(shared_ptr<SceneNode> object, float initialYaw, float initialPitch, bool rotateWhenLButtonDown)
: m_object(object)
{
	m_object->Transform(&m_matToWorld, &m_matFromWorld);

    //Init euler angles from matrix
    m_fTargetYaw = m_fYaw = RADIANS_TO_DEGREES(-acos(m_matToWorld.rows[2].z));
    m_fTargetPitch = m_fPitch = RADIANS_TO_DEGREES(atan2(m_matToWorld.rows[2].x, m_matToWorld.rows[2].y)); 

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
	//SDL_SetRelativeMouseMode(SDL_TRUE); //----------------------------------------
	m_lastMousePos.x = (float)x0;
    m_lastMousePos.y = (float)y0;

	memset(m_bKey, 0x00, sizeof(m_bKey));

	m_mouseLButtonDown = false;
	m_bRotateWhenLButtonDown = rotateWhenLButtonDown;
}

//
// MovementController::VOnPointerButtonDown
//
bool MovementController::VOnPointerButtonDown(const int &posX, const int &posY, const int &relPosX, const int &relPosY, const int radius, const CHG_MouseButton &mouseButton)
{
	if (mouseButton == CHG_BUTTON_LEFT)
	{
		m_mouseLButtonDown = true;

		// We want mouse movement to be relative to the position
		// the cursor was at when the user first presses down on
		// the left button
		m_lastMousePos.x = (float)posX;
		m_lastMousePos.y = (float)posY;
		return true;
	}
	return false;
}

bool MovementController::VOnPointerButtonUp(const int &posX, const int &posY, const int &relPosX, const int &relPosY, const int radius, const CHG_MouseButton &mouseButton)
{
	if (mouseButton == CHG_BUTTON_LEFT)
	{
		m_mouseLButtonDown = false;
		return true;
	}
	return false;
}


//  class MovementController::VOnMouseMove		- Chapter 10, page 282

bool MovementController::VOnPointerMove(const int &posX, const int &posY, const int &relPosX, const int &relPosY, const int radius)
{
	// There are two modes supported by this controller.
	Vec2 mousePos;

	mousePos.x = (float)posX;
	mousePos.y = (float)posY;

	if (m_bRotateWhenLButtonDown)
	{
		// Mode 1 - rotate the view only when the left mouse button is down
		// Only look around if the left button is down
		if(m_lastMousePos!=mousePos && m_mouseLButtonDown)
		{
			m_fTargetYaw = m_fTargetYaw + (m_lastMousePos.x - mousePos.x);
			m_fTargetPitch = m_fTargetPitch + (mousePos.y - m_lastMousePos.y);

			m_relativeMousePos = m_lastMousePos - mousePos;

			m_lastMousePos = mousePos;
		}
	}
	else if(m_lastMousePos!=mousePos)
	{
		// Mode 2 - rotate the controller when the mouse buttons are up
		m_fTargetYaw = m_fTargetYaw + (m_lastMousePos.x - mousePos.x);
		m_fTargetPitch = m_fTargetPitch + (mousePos.y - m_lastMousePos.y);

		m_relativeMousePos = m_lastMousePos - mousePos;

		m_lastMousePos = mousePos;
	}

	return true;
}

//  class MovementController::OnUpdate			- Chapter 10, page 283

void MovementController::OnUpdate(DWORD const deltaMilliseconds)
{
	bool bTranslating = false;
	Vec4 atWorld(0,0,0,0);
	Vec4 rightWorld(0,0,0,0);
	Vec4 upWorld(0,0,0,0);

	if (m_bKey['w'] || m_bKey['s'])
	{
		// In D3D, the "look at" default is always
		// the positive Z axis.
		Vec4 at = Vec4(m_Look, 0.0f); 
		if (m_bKey['s'])
			at *= -1;

		atWorld = at;
		bTranslating = true;
	}

	if (m_bKey['a'] || m_bKey['d'])
	{
		// In D3D, the "right" default is always
		// the positive X axis.
		Vec4 right = Vec4(m_Right, 0.0f); 
		if (m_bKey['a'])
			right *= -1;

		rightWorld = right;
		bTranslating = true;
	}

	if (m_bKey[' '] || m_bKey['c'] || m_bKey['x'])
	{
		// In D3D, the "up" default is always
		// the positive Y axis.
		Vec4 up = Vec4(m_Up, 0.0f); 
		if (!m_bKey[' '])
			up *= -1;

		upWorld = up;
		bTranslating = true;
	}

	//Handling rotation as a result of mouse position
	{
		///////////////////////////////////////////////////////////////////////
		//Pitch Transform
		Mat4x4 pitchRotate = rotateAxis(m_Right, -m_relativeMousePos.y*0.007f);
		// rotate _up and _look around _right vector
		m_Up = (pitchRotate * Vec4(m_Up, 0.0f)).xyz();
		m_Look = (pitchRotate * Vec4(m_Look, 0.0f)).xyz();

		//Yaw Transform
		Mat4x4 yawRotate = rotateY(m_relativeMousePos.x*0.007f);
		// rotate _right and _look around _up or y-axis
		m_Up = (yawRotate * Vec4(m_Up, 0.0f)).xyz();
		m_Right = (yawRotate * Vec4(m_Right, 0.0f)).xyz();
		m_Look = (yawRotate * Vec4(m_Look, 0.0f)).xyz();

		m_relativeMousePos.x = 0.0f;
		m_relativeMousePos.y = 0.0f;

		/*m_Right = cross(m_Up, m_Look);
		m_Right = normalize(m_Right);
		m_Up = cross(m_Right, m_Look);*/

		m_matFromWorld = Mat4x4(m_Right.x,									m_Up.x,									 m_Look.x,								   0.0f,
								m_Right.y,									m_Up.y,									 m_Look.y,								   0.0f,
								m_Right.z,									m_Up.z,									 m_Look.z,								   0.0f,
								-dot(m_matPosition.GetPosition(), m_Right), -dot(m_matPosition.GetPosition(), m_Up), -dot(m_matPosition.GetPosition(), m_Look), 1.0f);
		///////////////////////////////////////////////////////////////////////

		m_matToWorld = Mat4x4(m_Right.x, m_Up.x, m_Look.x,  m_matPosition.GetPosition().x,
							  m_Right.y, m_Up.y, m_Look.y,  m_matPosition.GetPosition().y,
							  m_Right.z, m_Up.z, m_Look.z,  m_matPosition.GetPosition().z,
							  0.0f,	     0.0f,   0.0f,      1.0f);

		/*printf("-----------------------------------------------------------\n");
		printf("Right : ( %f, %f, %f ) \n", m_Right.x, m_Right.y, m_Right.z);
		printf("Up : ( %f, %f, %f ) \n", m_Up.x, m_Up.y, m_Up.z);
		printf("Look : ( %f, %f, %f ) \n", m_Look.x, m_Look.y, m_Look.z);
        printf("Pos : ( %f, %f, %f ) \n", m_matPosition.GetPosition().x, m_matPosition.GetPosition().y, m_matPosition.GetPosition().z);
		printf("-----------------------------------------------------------\n");*/

		m_object->VSetTransform(&m_matToWorld, &m_matFromWorld);
	}

	if (bTranslating)
	{
		float elapsedTime = (float)deltaMilliseconds / 1000.0f;

		Vec3 direction = (atWorld + rightWorld + upWorld).xyz();
		direction = normalize(direction); 

		// Ramp the acceleration by the elapsed time.
		float numberOfSeconds = 5.f;
		m_currentSpeed += m_maxSpeed * ( (elapsedTime*elapsedTime) / numberOfSeconds);
		if (m_currentSpeed > m_maxSpeed)
			m_currentSpeed = m_maxSpeed;

		direction *= m_currentSpeed;

		Vec3 pos = m_matPosition.GetPosition() + direction;
		m_matPosition.SetPosition(pos);
		m_matToWorld.SetPosition(Vec3(m_matPosition.GetPosition().x, m_matPosition.GetPosition().y, m_matPosition.GetPosition().z));

		m_matFromWorld = Mat4x4(m_Right.x,									m_Up.x,									 m_Look.x,								   0.0f,
								m_Right.y,									m_Up.y,									 m_Look.y,								   0.0f,
								m_Right.z,									m_Up.z,									 m_Look.z,								   0.0f,
								-dot(m_matPosition.GetPosition(), m_Right), -dot(m_matPosition.GetPosition(), m_Up), -dot(m_matPosition.GetPosition(), m_Look), 1.0f);

		m_object->VSetTransform(&m_matToWorld, &m_matFromWorld);
	}
	else
	{
		m_currentSpeed = 0.0f;
	}
}

