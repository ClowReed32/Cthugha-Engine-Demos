#pragma once
//========================================================================
// File: DynamicPlayerController.h
//
//========================================================================

#include <CthughaEngine\interfaces.h>

class SceneNode;

class PlayerController : public IPointerHandler, public IKeyboardHandler
{
protected:
	BYTE					m_bKey[256];			// Which keys are up and down
	shared_ptr<SceneNode>	m_object;
	shared_ptr<SceneNode>	m_pCameraNode;

	Vec2					m_relativeMousePos;
	Vec2					m_lastMousePos;

	Mat4x4  m_matFromWorld;
	Mat4x4	m_matToWorld;
    Mat4x4  m_matPosition;

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

public:
	PlayerController(shared_ptr<SceneNode> object, shared_ptr<SceneNode> camera);
//	void SetObject(shared_ptr<SceneNode> newObject);

	void OnUpdate(DWORD const elapsedMs);

public:
	virtual bool VOnPointerMove(const int &posX, const int &posY, const int &relPosX, const int &relPosY, const int radius);
	virtual bool VOnPointerButtonDown(const int &posX, const int &posY, const int &relPosX, const int &relPosY, const int radius, const CHG_MouseButton &mouseButton);
	virtual bool VOnPointerButtonUp(const int &posX, const int &posY, const int &relPosX, const int &relPosY, const int radius, const CHG_MouseButton &mouseButton) { return (mouseButton == CHG_BUTTON_LEFT); }

    bool VOnKeyDown(const BYTE c);
    bool VOnKeyUp(const BYTE c);
};

