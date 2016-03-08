#pragma once
//========================================================================
// File: MovementController.h
//
//========================================================================

#include "..\CthughaEngine\Interfaces.h"
#include "Geometry.h"


// Forward declarations
class SceneNode;

////////////////////////////////////////////////////
//
// MovementController Description
//
//    Implements a WASD style movement controller
//
//  class MovementController		- Chapter 10, page 281
//
////////////////////////////////////////////////////

class MovementController : public IPointerHandler, public IKeyboardHandler
{
protected:
	Mat4x4  m_matFromWorld;
	Mat4x4	m_matToWorld;
    Mat4x4  m_matPosition;

	Vec2					m_relativeMousePos;
	Vec2					m_lastMousePos;
	bool					m_bKey[256];			// Which keys are up and down

	// Orientation Controls
	float		m_fTargetYaw;
	float		m_fTargetPitch;
	float		m_fYaw;
	float		m_fPitch;
	float		m_fPitchOnDown;
	float		m_fYawOnDown;
	float		m_maxSpeed;
	float		m_currentSpeed;
	Vec3		m_Look;
	Vec3		m_Up;
	Vec3		m_Right;

	// Added for Ch19/20 refactor
	bool		m_mouseLButtonDown;
	bool		m_bRotateWhenLButtonDown;

	shared_ptr<SceneNode> m_object;

public:
	MovementController(shared_ptr<SceneNode> object, float initialYaw, float initialPitch, bool rotateWhenLButtonDown);
	void SetObject(shared_ptr<SceneNode> newObject);

	void OnUpdate(DWORD const elapsedMs);

public:
	bool VOnPointerMove(const int &posX, const int &posY, const int &relPosX, const int &relPosY, const int radius);
	bool VOnPointerButtonDown(const int &posX, const int &posY, const int &relPosX, const int &relPosY, const int radius, const CHG_MouseButton &mouseButton);
	bool VOnPointerButtonUp(const int &posX, const int &posY, const int &relPosX, const int &relPosY, const int radius, const CHG_MouseButton &mouseButton);

	bool VOnKeyDown(const BYTE c) { m_bKey[c] = true; return true; }
	bool VOnKeyUp(const BYTE c) { m_bKey[c] = false; return true; }

	const Mat4x4 *GetToWorld() { return &m_matToWorld; }
	const Mat4x4 *GetFromWorld() { return &m_matFromWorld; }	
};

