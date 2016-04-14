#ifndef _ACDEMO_ENTITY_FACTORY_H_
#define _ACDEMO_ENTITY_FACTORY_H_

//=======================================================================================================
// ACDemoEntityFactory.h - Defines a factory for creating specific actors & components for Demo
//
//=======================================================================================================

#include <Entities\EntityFactory.h>

//////////////////////////////////////////////////////////////////////////
//
//	ACDemoEntityFactory class definition
//
//////////////////////////////////////////////////////////////////////////

class ACDemoEntityFactory : public EntityFactory
{
public:
	ACDemoEntityFactory();
};

#endif