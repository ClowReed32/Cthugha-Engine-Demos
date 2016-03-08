#pragma once
//========================================================================
// EvolutioneEvents.h : defines game-specific events for TeapotWars
//
//========================================================================

#include <EventManager\EventManager.h>
#include <EventManager\Events.h>
//#include "..\CthughaEngine\LUAScripting/ScriptEvent.h"


//---------------------------------------------------------------------------------------------------------------------
// class EvtData_StartThrust				        
//---------------------------------------------------------------------------------------------------------------------
class EvtData_StartThrust : public BaseEventData
{
    EntityId m_id;
    Vec3 m_velocity;

public:
    static const EventType sk_EventType;
    virtual const EventType& VGetEventType( void ) const
    {
        return sk_EventType;
    }

    EvtData_StartThrust(void) :
        m_id(INVALID_ENTITY_ID),
        m_velocity(Vec3())
    {
        //
    }

    EvtData_StartThrust(EntityId id, Vec3 velocity) :
        m_id(id),
        m_velocity(velocity)
    {
        //
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr (CHG_NEW EvtData_StartThrust(m_id, m_velocity));
    }

    virtual void VSerialize(std::ostrstream & out) const
    {
        out << m_id << " ";
        out << m_velocity.x << " " << m_velocity.y << " " << m_velocity.z << " ";
    }

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_id;
        in >> m_velocity.x >> m_velocity.y >> m_velocity.z;
    }

    virtual const char* GetName(void) const
    {
        return "EvtData_ThrustStart";
    }

    EntityId GetActorId(void) const
    {
        return m_id;
    }

    Vec3 GetVelocity(void) const
    {
        return m_velocity;
    }

    void Set(EntityId id)
    {
        m_id = id;
    }

    /*virtual bool VBuildEventFromScript(void)
    {
        if (m_eventData.IsTable())
        {
            // ID
            LuaPlus::LuaObject temp = m_eventData.GetByName("id");
            if (temp.IsInteger())
            {
                m_id = temp.GetInteger();
            }
            else
            {
                CHG_ERROR("Invalid id sent to EvtData_StartThrust; type = " + std::string(temp.TypeName()));
                return false;
            }

            // acceleration
            temp = m_eventData.GetByName("acceleration");
            if (temp.IsNumber())
                m_acceleration = temp.GetFloat();
            else
                m_acceleration = 5.0f;  // something reasonable

            return true;
        }

        return false;
    }*/

    //EXPORT_FOR_SCRIPT_EVENT(EvtData_StartThrust);
};


//---------------------------------------------------------------------------------------------------------------------
// class EvtData_EndThrust				        
//---------------------------------------------------------------------------------------------------------------------
class EvtData_EndThrust : public BaseEventData
{
    EntityId m_id;

public:
    static const EventType sk_EventType;
    virtual const EventType & VGetEventType( void ) const
    {
        return sk_EventType;
    }

    EvtData_EndThrust(void) :
        m_id(INVALID_ENTITY_ID)
    {
        //
    }

    EvtData_EndThrust(EntityId id) :
        m_id(id)
    {
        //
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr (CHG_NEW EvtData_EndThrust(m_id));
    }

    virtual void VSerialize(std::ostrstream & out) const
    {
        out << m_id << " ";
    }

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_id;
    }

    virtual const char* GetName(void) const
    {
        return "EvtData_EndThrust";
    }

    EntityId GetActorId(void) const
    {
        return m_id;
    }

    void Set(EntityId id)
    {
        m_id = id;
    }

    /*virtual bool VBuildEventFromScript(void)
    {
        if (m_eventData.IsTable())
        {
            // ID
            LuaPlus::LuaObject temp = m_eventData.GetByName("id");
            if (temp.IsInteger())
            {
                m_id = temp.GetInteger();
            }
            else
            {
                GCC_ERROR("Invalid id sent to EvtData_EndThrust; type = " + std::string(temp.TypeName()));
                return false;
            }

            return true;
        }

        return false;
    }*/

    //EXPORT_FOR_SCRIPT_EVENT(EvtData_EndThrust);
};

//---------------------------------------------------------------------------------------------------------------------
// class EvtData_StartJump				        
//---------------------------------------------------------------------------------------------------------------------
class EvtData_StartJump : public BaseEventData
{
	EntityId m_id;
	float m_acceleration;

	public:
    static const EventType sk_EventType;
    virtual const EventType & VGetEventType( void ) const
    {
        return sk_EventType;
    }

    EvtData_StartJump(void) :
        m_id(INVALID_ENTITY_ID),
        m_acceleration(0)
    {
        //
    }

    EvtData_StartJump(EntityId id, float acceleration) :
        m_id(id),
        m_acceleration(acceleration)
    {
        //
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(CHG_NEW EvtData_StartJump(m_id, m_acceleration));
    }

    virtual void VSerialize(std::ostrstream & out) const
    {
        out << m_id << " ";
        out << m_acceleration << " ";
    }

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_id;
        in >> m_acceleration;
    }

    virtual const char* GetName(void) const
    {
        return "EvtData_StartJump";
    }

    EntityId GetActorId(void) const
    {
        return m_id;
    }

    float GetAcceleration(void) const
    {
        return m_acceleration;
    }

    void Set(EntityId id)
    {
        m_id = id;
    }
};

//---------------------------------------------------------------------------------------------------------------------
// class EvtData_StartSteer				          
//---------------------------------------------------------------------------------------------------------------------
class EvtData_StartSteer : public BaseEventData
{
    EntityId m_id;
    float m_acceleration;

public:
    static const EventType sk_EventType;
    virtual const EventType & VGetEventType( void ) const
    {
        return sk_EventType;
    }

    EvtData_StartSteer(void) :
        m_id(INVALID_ENTITY_ID),
        m_acceleration(0)
    {
        //
    }

    EvtData_StartSteer(EntityId id, float acceleration) :
        m_id(id),
        m_acceleration(acceleration)
    {
        //
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(CHG_NEW EvtData_StartSteer(m_id, m_acceleration));
    }

    virtual void VSerialize(std::ostrstream & out) const
    {
        out << m_id << " ";
        out << m_acceleration << " ";
    }

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_id;
        in >> m_acceleration;
    }

    virtual const char* GetName(void) const
    {
        return "EvtData_StartSteer";
    }

    EntityId GetActorId(void) const
    {
        return m_id;
    }

    float GetAcceleration(void) const
    {
        return m_acceleration;
    }

    void Set(EntityId id)
    {
        m_id = id;
    }

    /*virtual bool VBuildEventFromScript(void)
    {
        if (m_eventData.IsTable())
        {
            // ID
            LuaPlus::LuaObject temp = m_eventData.GetByName("id");
            if (temp.IsInteger())
            {
                m_id = temp.GetInteger();
            }
            else
            {
                GCC_ERROR("Invalid id sent to EvtData_StartSteer; type = " + std::string(temp.TypeName()));
                return false;
            }

            // acceleration
            temp = m_eventData.GetByName("acceleration");
            if (temp.IsNumber())
                m_acceleration = temp.GetFloat();
            else
                m_acceleration = 5.0f;  // something reasonable

            return true;
        }

        return false;
    }*/

    //EXPORT_FOR_SCRIPT_EVENT(EvtData_StartSteer);
};


//---------------------------------------------------------------------------------------------------------------------
// class EvtData_EndSteer				     
//---------------------------------------------------------------------------------------------------------------------
class EvtData_EndSteer : public BaseEventData
{
    EntityId m_id;

public:
    static const EventType sk_EventType;
    virtual const EventType & VGetEventType( void ) const
    {
        return sk_EventType;
    }

    EvtData_EndSteer(void) :
        m_id(INVALID_ENTITY_ID)
    {
        //
    }

    EvtData_EndSteer(EntityId id) :
        m_id(id)
    {
        //
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr (CHG_NEW EvtData_EndThrust(m_id));
    }

    virtual void VSerialize(std::ostrstream & out) const
    {
        out << m_id << " ";
    }

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_id;
    }

    virtual const char* GetName(void) const
    {
        return "EvtData_EndThrust";
    }

    EntityId GetActorId(void) const
    {
        return m_id;
    }

    void Set(EntityId id)
    {
        m_id = id;
    }

    /*virtual bool VBuildEventFromScript(void)
    {
        if (m_eventData.IsTable())
        {
            // ID
            LuaPlus::LuaObject temp = m_eventData.GetByName("id");
            if (temp.IsInteger())
            {
                m_id = temp.GetInteger();
            }
            else
            {
                GCC_ERROR("Invalid id sent to EvtData_EndSteer; type = " + std::string(temp.TypeName()));
                return false;
            }

            return true;
        }

        return false;
    }*/

    //EXPORT_FOR_SCRIPT_EVENT(EvtData_EndSteer);
};

//---------------------------------------------------------------------------------------------------------------------
// class EvtData_Set_Controlled_Actor			
//---------------------------------------------------------------------------------------------------------------------
class EvtData_Set_Controlled_Actor : public BaseEventData
{
    EntityId m_id;

public:
    static const EventType sk_EventType;

    EvtData_Set_Controlled_Actor(void) { }
    EvtData_Set_Controlled_Actor(EntityId actorId)
        : m_id(actorId)
    {
    }

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(CHG_NEW EvtData_Set_Controlled_Actor(m_id));
    }

    virtual void VSerialize(std::ostrstream& out) const
    {
        out << m_id;
    }

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_id;
    }

    const EntityId& GetActorId(void) const
    {
        return m_id;
    }

    virtual const char* GetName(void) const
    {
        return "EvtData_Set_Controlled_Actor";
    }

    //virtual bool VBuildEventFromScript(void);

    //EXPORT_FOR_SCRIPT_EVENT(EvtData_Set_Controlled_Actor);
};