//========================================================================
// EvolutioneEvents.cpp : defines game-specific events for TeapotWars
//
//========================================================================

#include <Msvc\CthughaStd.h>
#include "RVSDemoEvents.h"
//#include "..\CthughaEngine\LUAScripting\LuaStateManager.h"


// Note: these are game specific events, which really should be in TeapotEvents.cpp.
// They aren't because of an annoying dependancy in Network.cpp, which creates events
// from a bitstream. Once we get "real" streaming code in we can move these back to TeapotEvents.cpp.
const EventType EvtData_StartThrust::sk_EventType(0x1d62d48c);
const EventType EvtData_EndThrust::sk_EventType(0xe60f88a4);
const EventType EvtData_StartSteer::sk_EventType(0xf0b5b4fd);
const EventType EvtData_EndSteer::sk_EventType(0x176645ef);
const EventType EvtData_StartJump::sk_EventType(0xc8633139);

//const EventType EvtData_PlaySound::sk_EventType(0x3d8118ee);
const EventType EvtData_Set_Controlled_Actor::sk_EventType(0xbe5e3388);


void RegisterEvolutioneEvents(void)
{
    REGISTER_EVENT(EvtData_Set_Controlled_Actor);

    REGISTER_EVENT(EvtData_StartThrust);
    REGISTER_EVENT(EvtData_EndThrust);
    REGISTER_EVENT(EvtData_StartSteer);
    REGISTER_EVENT(EvtData_EndSteer);
	REGISTER_EVENT(EvtData_StartJump);
}

