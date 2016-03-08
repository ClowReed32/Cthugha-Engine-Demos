#pragma once
//========================================================================
// PhysicsEventListener.cpp : implements the events sent FROM the phsyics system
//
//========================================================================


#include "../EventManager/EventManager.h"
#include "../CthughaEngine/CthughaApp.h"
//#include "../LUAScripting/ScriptEvent.h"

//
// Physics event implementation 
//

class EvtData_PhysTrigger_Enter : public BaseEventData
{
	int m_triggerID;
    EntityId m_other;

public:
	static const EventType sk_EventType;

	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	EvtData_PhysTrigger_Enter()
	{
		m_triggerID = -1;
		m_other = INVALID_ENTITY_ID;
	}

	explicit EvtData_PhysTrigger_Enter( int triggerID, EntityId other )
		: m_triggerID(triggerID),
		  m_other(other)
	{}

	IEventDataPtr VCopy() const 
	{
		return IEventDataPtr(CHG_NEW EvtData_PhysTrigger_Enter(m_triggerID, m_other));
	}

    virtual const char* GetName(void) const
    {
        return "EvtData_PhysTrigger_Enter";
    }

    int GetTriggerId(void) const
    {
        return m_triggerID;
    }

    EntityId GetOtherActor(void) const
    {
        return m_other;
    }
};

class EvtData_PhysTrigger_Leave : public BaseEventData
{
	int m_triggerID;
    EntityId m_other;

public:
	static const EventType sk_EventType;

	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	EvtData_PhysTrigger_Leave()
	{
		m_triggerID = -1;
		m_other = INVALID_ENTITY_ID;
	}

	explicit EvtData_PhysTrigger_Leave( int triggerID, EntityId other )
		: m_triggerID(triggerID),
		  m_other(other)
	{}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr ( CHG_NEW EvtData_PhysTrigger_Leave(m_triggerID, m_other) );
	}

    virtual const char* GetName(void) const
    {
        return "EvtData_PhysTrigger_Leave";
    }

    int GetTriggerId(void) const
    {
        return m_triggerID;
    }
    
    EntityId GetOtherActor(void) const
    {
        return m_other;
    }
};

class EvtData_PhysCollision : public BaseEventData
{
	EntityId m_ActorA;
    EntityId m_ActorB;
	Vec3 m_SumNormalForce;
    Vec3 m_SumFrictionForce;
    Vec3List m_CollisionPoints;

public:
	static const EventType sk_EventType;

	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	EvtData_PhysCollision()
	{
		m_ActorA = INVALID_ENTITY_ID;
		m_ActorB = INVALID_ENTITY_ID;
		m_SumNormalForce = Vec3(0.0f, 0.0f, 0.0f);
		m_SumFrictionForce = Vec3(0.0f, 0.0f, 0.0f);
	}

	explicit EvtData_PhysCollision(EntityId actorA,
								EntityId actorB,
								Vec3 sumNormalForce,
								Vec3 sumFrictionForce,
								Vec3List collisionPoints )
		: m_ActorA(actorA),
		m_ActorB(actorB),
		m_SumNormalForce(sumNormalForce),
		m_SumFrictionForce(sumFrictionForce),
		m_CollisionPoints(collisionPoints)
	{}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr ( CHG_NEW EvtData_PhysCollision(m_ActorA, m_ActorB, m_SumNormalForce, m_SumFrictionForce, m_CollisionPoints));
	}

    virtual const char* GetName(void) const
    {
        return "EvtData_PhysCollision";
    }

    EntityId GetActorA(void) const
    {
        return m_ActorA;
    }

    EntityId GetActorB(void) const
    {
        return m_ActorB;
    }

    const Vec3& GetSumNormalForce(void) const
    {
        return m_SumNormalForce;
    }

    const Vec3& GetSumFrictionForce(void) const
    {
        return m_SumFrictionForce;
    }

    const Vec3List& GetCollisionPoints(void) const
    {
        return m_CollisionPoints;
    }

    virtual void VBuildEventData(void);

    //EXPORT_FOR_SCRIPT_EVENT(EvtData_PhysCollision);
};


class EvtData_PhysSeparation : public BaseEventData
{
	EntityId m_ActorA;
    EntityId m_ActorB;

public:
	static const EventType sk_EventType;

	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	EvtData_PhysSeparation()
	{

		m_ActorA = INVALID_ENTITY_ID;
		m_ActorB = INVALID_ENTITY_ID;
	}

	explicit EvtData_PhysSeparation(EntityId actorA, EntityId actorB)
		: m_ActorA(actorA)
		, m_ActorB(actorB)
	{}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr ( CHG_NEW EvtData_PhysSeparation(m_ActorA, m_ActorB) );
	}

    virtual const char* GetName(void) const
    {
        return "EvtData_PhysSeparation";
    }

    EntityId GetActorA(void) const
    {
        return m_ActorA;
    }

    EntityId GetActorB(void) const
    {
        return m_ActorB;
    }
};

