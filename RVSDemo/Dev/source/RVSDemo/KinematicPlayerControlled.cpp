// KinematicPlayerController.cpp - Controller class for the player teapots
//
//========================================================================

#include <Msvc\CthughaStd.h>

#include <CthughaEngine\CthughaApp.h>
#include <Graphics\SceneNodes.h>
#include <EventManager\EventManager.h>
#include <Physics\PhysicsEventListener.h>
#include <Entities\TransformComponent.h>
#include <Entities\PhysicsComponent.h>

#include "KinematicPlayerController.h"
#include "RVSDemoEvents.h"
#include "RVSDemo.h"

const float ENTITY_VELOCITY = 4.0f;
const float ENTITY_ANGULAR_ACCELERATION = 22.0f;
const float ENTITY_JUMP_ACCELERATION = 100.0f;
const float MOUSE_SENSIBILITY = 0.0007f;

const float PLAYER_VIEW_OFFSET = 0.9f;


////////////////////////////////////////////////////
// PlayerController Implementation
////////////////////////////////////////////////////


//
// PlayerController::PlayerController			
//
KinematicPlayerController::KinematicPlayerController(shared_ptr<Entity> object, shared_ptr<SceneNode> camera)
: m_object(object)
{
	memset(m_bKey, 0x00, sizeof(m_bKey));
	
	m_pTransformComponent = MakeStrongPtr(object->GetComponent<TransformComponent>(TransformComponent::g_Name));

	if(!m_pTransformComponent)
		CHG_ERROR("Transform Component not found");

	m_matToWorld = m_pTransformComponent->GetTransform();
	m_matFromWorld = m_matToWorld.inverse();

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

    m_fTargetPitch = Pitch; 

	///////////////////////////////////////////////////////////////////////
	m_Right = m_matToWorld.rows[0].xyz();
	m_Up = m_matToWorld.rows[1].xyz();
	m_Look = m_matToWorld.rows[2].xyz();
	///////////////////////////////////////////////////////////////////////

	m_maxSpeed = 30.0f;			// 30 meters per second
	m_currentSpeed = 0.0f;

	Vec3 pos = m_matToWorld.GetPosition();

	m_matPosition = translate(pos);

	m_lastMousePos.x = 0.0f;
    m_lastMousePos.y = 0.0f;

	//Init Kinematic
	m_fVerticalVelocity = 0.0f;
	m_fGravity = 3.0f*9.8f;
	m_fJumpSpeed = 10.0f;
	m_fFallSpeed = 55.0f;
	m_currentPosition = pos;
	m_fCurrentStepOffset = 0.0f;
	m_fStepHeight = 0.35f;
	m_fAddedMargin = 0.02f;
	m_fMaxSlopeRadians = 0.125f*CHG_PI;
	m_fMaxSlopeCosine = cos(m_fMaxSlopeRadians);
	m_bWasOnGround = false;
	m_bWasJumping = false;
}



//
// PlayerController::VOnLButtonDown				
//
bool KinematicPlayerController::VOnPointerButtonDown(const int &posX, const int &posY, const int &relPosX, const int &relPosY, const int radius, const CHG_MouseButton &mouseButton)
{
	if (mouseButton != CHG_BUTTON_LEFT)
		return false;

	return true;
}

bool KinematicPlayerController::VOnPointerMove(const int &posX, const int &posY, const int &relPosX, const int &relPosY, const int radius)
{
	Vec2 mousePos;

	mousePos.x = (float)posX;
	mousePos.y = (float)posY;

	m_relativeMousePos += Vec2((float)relPosX, (float)relPosY);

	m_lastMousePos = mousePos;
	
	return true;
}

//
// PlayerController::OnUpdate				
//
void KinematicPlayerController::OnUpdate(DWORD const deltaMilliseconds)  
{
    bool bTranslating = false;
	Vec4 atWorld(0,0,0,0);
	Vec4 rightWorld(0,0,0,0);
	Vec4 upWorld(0,0,0,0);

	float fDeltaSeconds = deltaMilliseconds / 1000.0f;

	//Handling rotation as a result of mouse position
	{
		m_fTargetPitch += -m_relativeMousePos.y*MOUSE_SENSIBILITY;

        if(m_fTargetPitch < 0.5f*CHG_PI && m_fTargetPitch > -0.5f*CHG_PI)
        {
		    ///////////////////////////////////////////////////////////////////////
		    //Pitch Transform
			Mat4x4 pitchRotate = rotateAxis(m_Right, -m_relativeMousePos.y*MOUSE_SENSIBILITY);
		    // rotate _up and _look around _right vector
		    m_Up = (pitchRotate * Vec4(m_Up, 0.0f)).xyz();
		    m_Look = (pitchRotate * Vec4(m_Look, 0.0f)).xyz();            
        }
        else
        {
			m_fTargetPitch -= -m_relativeMousePos.y*MOUSE_SENSIBILITY;
        }

		//Yaw Transform
		Mat4x4 yawRotate = rotateY(m_relativeMousePos.x*MOUSE_SENSIBILITY);
		// rotate _right and _look around _up or y-axis
		m_Up = (yawRotate * Vec4(m_Up, 0.0f)).xyz();
		m_Right = (yawRotate * Vec4(m_Right, 0.0f)).xyz();
		m_Look = (yawRotate * Vec4(m_Look, 0.0f)).xyz();

		m_relativeMousePos.x = 0.0f;
		m_relativeMousePos.y = 0.0f;

		Vec3 objectPos = Vec3(0.0f, PLAYER_VIEW_OFFSET, 0.0f) + m_pTransformComponent->GetTransform().GetPosition();

		/*m_Right = cross(m_Up, m_Look);
		m_Right = normalize(m_Right);
		m_Up = cross(m_Right, m_Look);*/

		m_matFromWorld = Mat4x4(m_Right.x,									m_Up.x,									 m_Look.x,								   0.0f,
								m_Right.y,									m_Up.y,									 m_Look.y,								   0.0f,
								m_Right.z,									m_Up.z,									 m_Look.z,								   0.0f,
								-dot(objectPos, m_Right),                   -dot(objectPos, m_Up),                   -dot(objectPos, m_Look),                   1.0f);
		///////////////////////////////////////////////////////////////////////

		m_matToWorld = Mat4x4(m_Right.x, m_Up.x, m_Look.x,  objectPos.x,
							  m_Right.y, m_Up.y, m_Look.y,  objectPos.y,
							  m_Right.z, m_Up.z, m_Look.z,  objectPos.z,
							  0.0f,	     0.0f,   0.0f,      1.0f);

		m_pCameraNode->VSetTransform(&m_matToWorld, &m_matFromWorld);
	}

	Vec3 Vel = Vec3(0.0f, 0.0f, 0.0f);

	if(m_bKey['w'] || m_bKey['s'] || m_bKey['a'] || m_bKey['d'])
	{
		if(m_bKey['w'])
			Vel += Vec3(m_Look.x, 0.0f, m_Look.z);

		if(m_bKey['s'])
			Vel += Vec3(-m_Look.x, 0.0f, -m_Look.z);

		if(m_bKey['d'])
			Vel += Vec3(m_Right.x, 0.0f, m_Right.z);

		if(m_bKey['a'])
			Vel += Vec3(-m_Right.x, 0.0f, -m_Right.z);

	}

	if(m_bKey[' '])
	{
		jump();
	}

    if (m_object)
    {
		shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(m_object->GetComponent<TransformComponent>(TransformComponent::g_Name));
		shared_ptr<PhysicsComponent> pPhysicsComponent = MakeStrongPtr(m_object->GetComponent<PhysicsComponent>(PhysicsComponent::g_Name));

		if(pPhysicsComponent)
		{
			OnStepPlayer(deltaMilliseconds, ENTITY_VELOCITY*normalize(Vel), pPhysicsComponent);
			pPhysicsComponent->SetPosition(m_currentPosition.x, m_currentPosition.y, m_currentPosition.z);
		}

        if (pTransformComponent)
        {
			Mat4x4 objectTransform = pTransformComponent->GetTransform();
			Vec3 lastPosition = objectTransform.GetPosition();

			if(length(m_currentPosition - lastPosition) > 0.0001f)
			{
				objectTransform.SetPosition(m_currentPosition);
				pTransformComponent->SetTransform(objectTransform);
			
				shared_ptr<EvtData_Move_Actor> pEvent(CHG_NEW EvtData_Move_Actor(m_object->GetId(), objectTransform));
				IEventManager::Get()->VQueueEvent(pEvent);
			}
        }
	}

	/*printf("-----------------------------------------------------------\n");
	printf("Right : ( %f, %f, %f ) \n", m_Right.x, m_Right.y, m_Right.z);
	printf("Up : ( %f, %f, %f ) \n", m_Up.x, m_Up.y, m_Up.z);
	printf("Look : ( %f, %f, %f ) \n", m_Look.x, m_Look.y, m_Look.z);
	printf("Pos : ( %f, %f, %f ) \n", m_currentPosition.x, m_currentPosition.y, m_currentPosition.z);
	printf("-----------------------------------------------------------\n\n\n\n\n\n\n\n\n\n");*/
}

bool KinematicPlayerController::VOnKeyDown(const BYTE c)
{
    // update the key table
    m_bKey[c] = true;

    return true;
}

bool KinematicPlayerController::VOnKeyUp(const BYTE c)
{
    // update the key table
    m_bKey[c] = false;

    return true;
}

void KinematicPlayerController::OnStepPlayer(DWORD const elapsedMs, const Vec3& moveDirection, shared_ptr<PhysicsComponent> pPhysicsComponent)
{
	float dt = (float)elapsedMs*0.001f;

	m_bWasOnGround = onGround();

	//Update fall velocity
	m_fVerticalVelocity -= m_fGravity * dt;
	if(m_fVerticalVelocity > 0.0 && m_fVerticalVelocity > m_fJumpSpeed)
	{
		m_fVerticalVelocity = m_fJumpSpeed;
	}
	if(m_fVerticalVelocity < 0.0 && std::fabs(m_fVerticalVelocity) > std::fabs(m_fFallSpeed))
	{
		m_fVerticalVelocity = -std::fabs(m_fFallSpeed);
	}
	m_fVerticalOffset = m_fVerticalVelocity * dt;

	OnStepUp(pPhysicsComponent);

	OnStepForwardAndStrafe(moveDirection*dt, pPhysicsComponent);

	if(dt < 0.1)
		OnStepDown(dt, pPhysicsComponent);
}

void KinematicPlayerController::OnStepUp(shared_ptr<PhysicsComponent> pPhysicsComponent)
{
	// phase 1: up
	Mat4x4 start, end;
	m_targetPosition = m_currentPosition + Vec3(0.0f, 1.0f, 0.0f) * (m_fStepHeight + (m_fVerticalOffset > 0.0f ? m_fVerticalOffset : 0.0f));

	start = Mat4x4::g_Identity;
	end = Mat4x4::g_Identity;

	/* FIXME: Handle penetration properly */
	start.SetPosition(m_currentPosition + Vec3(0.0f, 1.0f, 0.0f) * m_fAddedMargin);
	end.SetPosition(m_targetPosition);

	TestCollisionAnswer answer;

	if (pPhysicsComponent->CollisionConvexTest(start, end, -Vec3(0.0f, 1.0f, 0.0f), 0.7071f, answer))
	{
		// Only modify the position if the hit was a slope and not a wall or ceiling.
		if(dot(answer.hitNormalPosition, Vec3(0.0f, 1.0f, 0.0f)) > 0.0)
		{
			// we moved up only a fraction of the step height
			m_fCurrentStepOffset = m_fStepHeight * answer.closedHitFraction;

			m_currentPosition = lerp(m_currentPosition, m_targetPosition, answer.closedHitFraction);
		}
		m_fVerticalVelocity = 0.0;
		m_fVerticalOffset = 0.0;
	} else {
		m_fCurrentStepOffset = m_fStepHeight;
		m_currentPosition = m_targetPosition;
	}
}

void KinematicPlayerController::OnStepForwardAndStrafe(const Vec3& moveDirection, shared_ptr<PhysicsComponent> pPhysicsComponent)
{
	Mat4x4 start, end;
	m_targetPosition = m_currentPosition + moveDirection;

    start = Mat4x4::g_Identity;
	end = Mat4x4::g_Identity;
	
	float fraction = 1.0f;
	float distance2 = squaredLength(m_currentPosition - m_targetPosition);

	int maxIter = 10;

	while (fraction > 0.01f && maxIter-- > 0)
	{
		start.SetPosition (m_currentPosition);
		end.SetPosition (m_targetPosition);
		Vec3 sweepDirNegative(m_currentPosition - m_targetPosition);

		/*float margin = m_convexShape->getMargin();
		m_convexShape->setMargin(margin + m_fAddedMargin);*/

		TestCollisionAnswer answer;

		if (pPhysicsComponent->CollisionConvexTest(start, end, sweepDirNegative, 0.0f, answer))
		{	
			// we moved only a fraction
			float hitDistance;
			hitDistance = length(answer.hitPointWorld - m_currentPosition);

			updateTargetPositionBasedOnCollision (answer.hitNormalPosition);
			Vec3 currentDir = m_targetPosition - m_currentPosition;
			distance2 = squaredLength(currentDir);
			if (distance2 > FLT_EPSILON)
			{
				normalize(currentDir);
				
				if (dot(currentDir, normalize(moveDirection)) <= 0.0)
				{
					break;
				}
			} else
			{
				break;
			}

		}
		else
		{
			// we moved whole way
			m_currentPosition = m_targetPosition;
		}
	}
}

void KinematicPlayerController::OnStepDown(float dt, shared_ptr<PhysicsComponent> pPhysicsComponent)
{
	Mat4x4 start, end, end_double;
	TestCollisionAnswer answer1, answer2;
	bool runonce = false;

	// phase 3: down

	//printf("On Ground: %s\n", m_bWasOnGround ? "True" : "False");
	//printf("Step Down Activo\n");
	
	Vec3 orig_position = m_targetPosition;
	
	float downVelocity = (m_fVerticalVelocity < 0.0f ? -m_fVerticalVelocity : 0.0f) * dt;

	if(downVelocity > 0.0 && downVelocity > m_fFallSpeed
		&& (m_bWasOnGround || !m_bWasJumping))
		downVelocity = m_fFallSpeed;

	Vec3 step_drop = Vec3(0.0f, 1.0f, 0.0f) * (m_fCurrentStepOffset + downVelocity);
	m_targetPosition -= step_drop;

	while (1)
	{
		start = Mat4x4::g_Identity;
		end = Mat4x4::g_Identity;

		end_double = Mat4x4::g_Identity;

		start.SetPosition(m_currentPosition);
		end.SetPosition(m_targetPosition);

		//set double test for 2x the step drop, to check for a large drop vs small drop
		end_double.SetPosition(m_targetPosition - step_drop);

		if (!pPhysicsComponent->CollisionConvexTest(start, end, Vec3(0.0f, 1.0f, 0.0f), m_fMaxSlopeCosine, answer1))
		{
			//test a double fall height, to see if the character should interpolate it's fall (full) or not (partial)
			pPhysicsComponent->CollisionConvexTest(start, end_double, Vec3(0.0f, 1.0f, 0.0f), m_fMaxSlopeCosine, answer2);
		}
	
		float downVelocity2 = (m_fVerticalVelocity < 0.0f ? -m_fVerticalVelocity : 0.0f) * dt;
		bool has_hit = false;
		
		has_hit = answer2.hasHit;

		if(downVelocity2 > 0.0 && downVelocity2 < m_fStepHeight && has_hit == true && runonce == false
					&& (m_bWasOnGround || !m_bWasJumping))
		{
			//redo the velocity calculation when falling a small amount, for fast stairs motion
			//for larger falls, use the smoother/slower interpolated movement by not touching the target position

			m_targetPosition = orig_position;
			downVelocity = m_fStepHeight;

			Vec3 step_drop = Vec3(0.0f, 1.0f, 0.0f) * (m_fCurrentStepOffset + downVelocity);
			m_targetPosition -= step_drop;
			runonce = true;
			continue; //re-run previous tests
		}
		break;
	}

	if (answer1.hasHit || runonce == true)
	{
		// we dropped a fraction of the height -> hit floor

		m_currentPosition = lerp(m_currentPosition, m_targetPosition, answer1.closedHitFraction);
		m_currentPosition.y += 0.005f; //Offset

		m_fVerticalVelocity = 0.0;
		m_fVerticalOffset = 0.0;
		m_bWasJumping = false;
	} 
	else 
	{
		m_currentPosition = m_targetPosition;
	}
}

void KinematicPlayerController::updateTargetPositionBasedOnCollision (const Vec3& hitNormal, float tangentMag, float normalMag)
{
	Vec3 movementDirection = m_targetPosition - m_currentPosition;
	float movementLength = length(movementDirection);

	if (movementLength > FLT_EPSILON)
	{
		normalize(movementDirection);

		Vec3 reflectDir = computeReflectionDirection (movementDirection, hitNormal);
		normalize(reflectDir);

		Vec3 parallelDir, perpindicularDir;

		parallelDir = parallelComponent (reflectDir, hitNormal);
		perpindicularDir = perpindicularComponent (reflectDir, hitNormal);

		m_targetPosition = m_currentPosition;
		if (0)//tangentMag != 0.0)
		{
			Vec3 parComponent = parallelDir * tangentMag*movementLength;
//			printf("parComponent=%f,%f,%f\n",parComponent[0],parComponent[1],parComponent[2]);
			m_targetPosition +=  parComponent;
		}

		if (normalMag != 0.0)
		{
			Vec3 perpComponent = perpindicularDir * normalMag*movementLength;
//			printf("perpComponent=%f,%f,%f\n",perpComponent[0],perpComponent[1],perpComponent[2]);
			m_targetPosition += perpComponent;
		}
	} else
	{
//		printf("movementLength don't normalize a zero vector\n");
	}
}

bool KinematicPlayerController::onGround() const
{
	return m_fVerticalVelocity == 0.0 && m_fVerticalOffset == 0.0;
}

bool KinematicPlayerController::canJump() const
{
	return onGround();
}

void KinematicPlayerController::jump ()
{
	if (canJump())
	{
		m_fVerticalVelocity = m_fJumpSpeed;
		m_bWasJumping = true;
	}
}