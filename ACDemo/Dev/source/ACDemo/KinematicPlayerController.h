#pragma once
//========================================================================
// File: KinematicPlayerController.h
//
//========================================================================

#include <CthughaEngine\interfaces.h>

class PhysicsComponent;
class TransformComponent;
class Entity;

class KinematicPlayerController : public IPointerHandler, public IKeyboardHandler
{
protected:
	BYTE						   m_bKey[256];			// Which keys are up and down
	shared_ptr<Entity>			   m_object;
	shared_ptr<SceneNode>		   m_pCameraNode;
	shared_ptr<TransformComponent> m_pTransformComponent;

	Vec2					m_relativeMousePos;
	Vec2					m_lastMousePos;

	Mat4x4  m_matFromWorld;
	Mat4x4	m_matToWorld;
    Mat4x4  m_matPosition;

	// Orientation Controls
	float		m_fTargetPitch;
	float		m_maxSpeed;
	float		m_currentSpeed;
	Vec3		m_Look;
	Vec3		m_Up;
	Vec3		m_Right;

	//Kinematic Attributes
	float m_fVerticalVelocity;
	float m_fGravity;
	float m_fJumpSpeed;
	float m_fVerticalOffset;
	float m_fFallSpeed; //Max Fall Speed
	Vec3 m_currentPosition;
	float m_fCurrentStepOffset;
	float m_fStepHeight;
	Vec3 m_targetPosition;
	float m_fAddedMargin;
	float m_fMaxSlopeRadians;
	float m_fMaxSlopeCosine;

	bool m_bWasOnGround;
	bool m_bWasJumping;

public:
	KinematicPlayerController(shared_ptr<Entity> object, shared_ptr<SceneNode> camera);

	void OnUpdate(DWORD const elapsedMs);

public:
	virtual bool VOnPointerMove(const int &posX, const int &posY, const int &relPosX, const int &relPosY, const int radius);
	virtual bool VOnPointerButtonDown(const int &posX, const int &posY, const int &relPosX, const int &relPosY, const int radius, const CHG_MouseButton &mouseButton);
	virtual bool VOnPointerButtonUp(const int &posX, const int &posY, const int &relPosX, const int &relPosY, const int radius, const CHG_MouseButton &mouseButton) { return (mouseButton == CHG_BUTTON_LEFT); }

    bool VOnKeyDown(const BYTE c);
    bool VOnKeyUp(const BYTE c);

private:
	//Kinematic Methods
	void OnStepPlayer(DWORD const elapsedMs, const Vec3& moveDirection, shared_ptr<PhysicsComponent> pPhysicsComponent);
	void OnStepUp(shared_ptr<PhysicsComponent> pPhysicsComponent);
	void OnStepForwardAndStrafe(const Vec3& moveDirection, shared_ptr<PhysicsComponent> pPhysicsComponent);
	void OnStepDown(float dt, shared_ptr<PhysicsComponent> pPhysicsComponent);
	void updateTargetPositionBasedOnCollision (const Vec3& hitNormal, float tangentMag = 0.0f, float normalMag = 1.0f);
	bool onGround() const;
	bool canJump() const;
	void jump ();
};