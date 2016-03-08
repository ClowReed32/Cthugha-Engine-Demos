//========================================================================
// Physics.cpp : Implements the IGamePhysics interface with Bullet
//
//========================================================================

#include "CthughaStd.h"
#include "..\CthughaEngine\CthughaApp.h"
#include "Physics.h"
#include "../Entities/Entity.h"
#include "../Entities/TransformComponent.h"
#include "../ResourceCache/XmlResource.h"
#include "../EventManager/EventManager.h"
#include "PhysicsDebugDrawer.h"


/////////////////////////////////////////////////////////////////////////////
// g_Materials Description						
//
//   Predefines some useful physics materials. Define new ones here, and 
//   have similar objects use it, so if you ever need to change it you'll 
//   only have to change it here.
//
/////////////////////////////////////////////////////////////////////////////
struct MaterialPhysicData
{
	float m_restitution;
	float m_friction;

	MaterialPhysicData(float restitution, float friction)
    {
        m_restitution = restitution;
        m_friction = friction;
    }

	MaterialPhysicData(const MaterialPhysicData& other)
    {
        m_restitution = other.m_restitution;
        m_friction = other.m_friction;
    }
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// a physics implementation which does nothing.  used if physics is disabled.
//
class NullPhysics : public IGamePhysics
{
public:
	NullPhysics() { }
	virtual ~NullPhysics() { }

	// Initialization and Maintenance of the Physics World
	virtual bool VInitialize() { return true; }
	virtual void VSyncVisibleScene() { };
	virtual void VOnUpdate( float ) { }

	// Initialization of Physics Objects
	virtual void VAddSphere(float radius, WeakEntityPtr gameEntity, const std::string& densityStr, const std::string& physicsMaterial, const Vec3& rotateConstrains, const Vec3& translateConstrains, const Vec3& shapeOffset) { }
	virtual void VAddBox(const Vec3& dimensions, WeakEntityPtr gameEntity, const std::string& densityStr, const std::string& physicsMaterial, const Vec3& rotateConstrains, const Vec3& translateConstrains, const Vec3& shapeOffset) { }
	virtual void VAddCapsule(float radius, float height, WeakEntityPtr entity, const std::string& densityStr, const std::string& physicsMaterial, const Vec3& rotateConstrains, const Vec3& translateConstrains, const Vec3& shapeOffset) { }
	virtual void VAddConvexHull(Vec3 *verts, int numPoints, WeakEntityPtr gameEntity, const std::string& densityStr, const std::string& physicsMaterial, const Vec3& rotateConstrains, const Vec3& translateConstrains) { }
	virtual void VAddTriangleMesh(float *verts, int num_Vertex, UINT *index, int num_Index, WeakEntityPtr gameEntity, /*const Mat4x4& initialTransform, */ const std::string& densityStr, const std::string& physicsMaterial, const Vec3& rotateConstrains, const Vec3& translateConstrains) { };
	virtual void VAddHeightField(const float* sHeightmapResource, int iWidth, int iDepth, float fMaxHeight, float fMinHeight, std::string sUpAxis, Vec2 vWorldDimension, WeakEntityPtr gameEntity, const std::string& densityStr, const std::string& physicsMaterial, const Vec3& rotateConstrains, const Vec3& translateConstrains) { }
	virtual void VAddGhostSphere(float radius, WeakEntityPtr entity) { };
	virtual void VAddGhostBox(const Vec3& dimensions, WeakEntityPtr gameEntity) { };
	virtual void VAddGhostCapsule(float radius, float height, WeakEntityPtr entity) { };
	virtual void VAddGhostConvexHull(Vec3 *verts, int numPoints, WeakEntityPtr gameEntity) { };
	virtual void VRemoveEntity(EntityId id) { }

	// Debugging
	virtual void VRenderDiagnostics() { }

	// Ghost Object Collision Test
	virtual bool VCollisionConvexTest(EntityId id, const Mat4x4& start, const Mat4x4& end, const Vec3& dir, float minSlopeDot, TestCollisionAnswer& testCollisionAnswer) { return false; };

	// Physics world modifiers
	virtual void VCreateTrigger(WeakEntityPtr pGameActor, const Vec3 &pos, const float dim) { }
	virtual void VApplyForce(const Vec3 &dir, float newtons, EntityId aid) { }
	virtual void VApplyTorque(const Vec3 &dir, float newtons, EntityId aid) { }
	virtual bool VKinematicMove(const Mat4x4 &mat, EntityId aid) { return true; }

	// Physics actor states
	virtual void VRotateY(EntityId entityId, float angleRadians, float time) { }
	virtual float VGetOrientationY(EntityId entityId) { return 0.0f; }
	virtual void VStopEntity(EntityId entityId) { }
    virtual Vec3 VGetVelocity(EntityId entityId) { return Vec3(); }
    virtual void VSetVelocity(EntityId entityId, const Vec3& vel) { }
	virtual void VSetVelocityFactor(EntityId actorId, const Vec3& velFactor) { };
    virtual Vec3 VGetAngularVelocity(EntityId entityId) { return Vec3(); }
    virtual void VSetAngularVelocity(EntityId entityId, const Vec3& vel) { }
	virtual void VTranslate(EntityId entityId, const Vec3& vec) { }
	virtual void VSetTransform(const EntityId id, const Mat4x4& mat) { }
    virtual Mat4x4 VGetTransform(const EntityId id) { return Mat4x4::g_Identity; }
};

#ifndef DISABLE_PHYSICS


#include "../Graphics/Geometry.h"
#include "../EventManager/Events.h"

//#include "PhysicsDebugDrawer.h"
#include "PhysicsEventListener.h"

#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"
#include "BulletCollision\CollisionShapes\btHeightfieldTerrainShape.h"
#include "BulletCollision\CollisionDispatch\btGhostObject.h"

#include <set>
#include <iterator>
#include <map>

////////////////////////////////////////////////////////////////////////////
//
//	struct GhostCollisionObject to help in character and trigger collision
//
////////////////////////////////////////////////////////////////////////////
struct GhostCollisionObject
{
	btGhostObject* pBulletGhostObject;
	btConvexShape* pGhostObjectShape;
};

/////////////////////////////////////////////////////////////////////////////

// helpers for conversion to and from Bullet's data types
static btVector3 Vec3_to_btVector3( Vec3 const & vec3 )
{
	return btVector3( vec3.x, vec3.y, vec3.z );
}

static Vec3 btVector3_to_Vec3( btVector3 const & btvec )
{
	return Vec3( btvec.x(), btvec.y(), btvec.z() );
}

static btTransform Mat4x4_to_btTransform( Mat4x4 const & mat )
{
	// convert from Mat4x4 (GameCode) to btTransform (Bullet)
	btMatrix3x3 bulletRotation;
	btVector3 bulletPosition;
	
	// copy rotation matrix
	for ( int row=0; row<3; ++row )
		for ( int column=0; column<3; ++column )
			bulletRotation[row][column] = mat.rows[row].data[column]; // note the reversed indexing (row/column vs. column/row)
			                                                  //  this is because Mat4x4s are row-major matrices and
			                                                  //  btMatrix3x3 are column-major.  This reversed indexing
			                                                  //  implicitly transposes (flips along the diagonal) 
			                                                  //  the matrix when it is copied.
	
	// copy position
	for ( int column=0; column<3; ++column )
		bulletPosition[column] = mat.rows[column].data[3];
		
	return btTransform( bulletRotation, bulletPosition );
}

static Mat4x4 btTransform_to_Mat4x4( btTransform const & trans )
{
	Mat4x4 returnValue = Mat4x4::g_Identity;

	// convert from btTransform (Bullet) to Mat4x4 (GameCode)
	btMatrix3x3 const & bulletRotation = trans.getBasis();
	btVector3 const & bulletPosition = trans.getOrigin();
	
	// copy rotation matrix
	for ( int row=0; row<3; ++row )
		for ( int column=0; column<3; ++column )
			returnValue.rows[row].data[column] = bulletRotation[row][column]; 
			          // note the reversed indexing (row/column vs. column/row)
			          //  this is because Mat4x4s are row-major matrices and
			          //  btMatrix3x3 are column-major.  This reversed indexing
			          //  implicitly transposes (flips along the diagonal) 
			          //  the matrix when it is copied.
	
	// copy position
	for ( int column=0; column<3; ++column )
		returnValue.rows[column].data[3] = bulletPosition[column];
		
	return returnValue;
}

/////////////////////////////////////////////////////////////////////////////
// struct ActorMotionState
//
// Interface that Bullet uses to communicate position and orientation changes
//   back to the game.  note:  this assumes that the actor's center of mass
//   and world position are the same point.  If that was not the case,
//   an additional transformation would need to be stored here to represent
//   that difference.
//
struct ActorMotionState : public btMotionState
{
	Mat4x4 m_worldToPositionTransform;
	
	ActorMotionState( Mat4x4 const & startingTransform )
	  : m_worldToPositionTransform( startingTransform ) { }
	
	// btMotionState interface:  Bullet calls these
	virtual void getWorldTransform( btTransform& worldTrans ) const
	   { worldTrans = Mat4x4_to_btTransform( m_worldToPositionTransform ); }

	virtual void setWorldTransform( const btTransform& worldTrans )
	   { m_worldToPositionTransform = btTransform_to_Mat4x4( worldTrans ); }
};

/////////////////////////////////////////////////////////////////////////////
//
//	Auxiliar Classes
//
/////////////////////////////////////////////////////////////////////////////

class btKinematicClosestNotMeRayResultCallback : public btCollisionWorld::ClosestRayResultCallback
{
public:
	btKinematicClosestNotMeRayResultCallback (btCollisionObject* me) : btCollisionWorld::ClosestRayResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0))
	{
		m_me = me;
	}

	virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult,bool normalInWorldSpace)
	{
		if (rayResult.m_collisionObject == m_me)
			return 1.0;

		return ClosestRayResultCallback::addSingleResult (rayResult, normalInWorldSpace);
	}
protected:
	btCollisionObject* m_me;
};

class btKinematicClosestNotMeConvexResultCallback : public btCollisionWorld::ClosestConvexResultCallback
{
public:
	btKinematicClosestNotMeConvexResultCallback (btCollisionObject* me, const btVector3& up, btScalar minSlopeDot)
	: btCollisionWorld::ClosestConvexResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0))
	, m_me(me)
	, m_up(up)
	, m_minSlopeDot(minSlopeDot)
	{
	}

	virtual btScalar addSingleResult(btCollisionWorld::LocalConvexResult& convexResult,bool normalInWorldSpace)
	{
		if (convexResult.m_hitCollisionObject == m_me)
			return btScalar(1.0);

		if (!convexResult.m_hitCollisionObject->hasContactResponse())
			return btScalar(1.0);

		btVector3 hitNormalWorld;
		if (normalInWorldSpace)
		{
			hitNormalWorld = convexResult.m_hitNormalLocal;
		} else
		{
			///need to transform normal into worldspace
			hitNormalWorld = convexResult.m_hitCollisionObject->getWorldTransform().getBasis()*convexResult.m_hitNormalLocal;
		}

		btScalar dotUp = m_up.dot(hitNormalWorld);
		if (dotUp < m_minSlopeDot) {
			return btScalar(1.0);
		}

		return ClosestConvexResultCallback::addSingleResult (convexResult, normalInWorldSpace);
	}
protected:
	btCollisionObject* m_me;
	const btVector3 m_up;
	btScalar m_minSlopeDot;
};

/////////////////////////////////////////////////////////////////////////////
// BaseGamePhysics								
//
//   The implementation of IGamePhysics interface using the Bullet SDK.
//
/////////////////////////////////////////////////////////////////////////////

class BulletPhysics : public IGamePhysics, CHG_noncopyable
{
	// use auto pointers to automatically call delete on these objects
	//   during ~BulletPhysics
	
	// these are all of the objects that Bullet uses to do its work.
	//   see BulletPhysics::VInitialize() for some more info.
	btDynamicsWorld*                 m_dynamicsWorld;
    btGhostPairCallback*             m_ghostPairCallback;
	btBroadphaseInterface*           m_broadphase;
	btCollisionDispatcher*           m_dispatcher;
	btConstraintSolver*              m_solver;
	btDefaultCollisionConfiguration* m_collisionConfiguration;
	BulletDebugDrawer*               m_debugDrawer;

    // tables read from the XML
    typedef std::map<std::string, float> DensityTable;
	typedef std::map<std::string, MaterialPhysicData> MaterialTable;
    DensityTable m_densityTable;
    MaterialTable m_materialTable;

	void LoadXml();
    float LookupSpecificGravity(const std::string& densityStr);
	MaterialPhysicData LookupMaterialData(const std::string& materialStr);

	// keep track of the existing rigid bodies:  To check them for updates
	//   to the actors' positions, and to remove them when their lives are over.
	typedef std::map<EntityId, btRigidBody*> ActorIDToBulletRigidBodyMap;
	ActorIDToBulletRigidBodyMap m_actorIdToRigidBody;
	btRigidBody * FindBulletRigidBody( EntityId id ) const;
	
	// also keep a map to get the actor id from the btRigidBody*
	typedef std::map<btRigidBody const *, EntityId> BulletRigidBodyToActorIDMap;
	BulletRigidBodyToActorIDMap m_rigidBodyToActorId;
	EntityId FindActorID( btRigidBody const * ) const;

	// keep track of the existing ghost object
	typedef std::map<EntityId, GhostCollisionObject*> EntityIDToGhostCollisionObjectMap;
	EntityIDToGhostCollisionObjectMap m_EntityIdToGhostCollisionObject;
	GhostCollisionObject* FindGhostCollisionObject(EntityId id) const;

	// also keep a map to get the actor id from the ghostObject
	typedef std::map<GhostCollisionObject const *, EntityId> GhostCollisionObjectToEntityIDMap;
	GhostCollisionObjectToEntityIDMap m_GhostCollisionObjectToEntityId;
	EntityId FindActorID( GhostCollisionObject const *) const;
	
	// data used to store which collision pair (bodies that are touching) need
	//   Collision events sent.  When a new pair of touching bodies are detected,
	//   they are added to m_previousTickCollisionPairs and an event is sent.
	//   When the pair is no longer detected, they are removed and another event
	//   is sent.
	typedef std::pair< btRigidBody const *, btRigidBody const * > CollisionPair;
	typedef std::set< CollisionPair > CollisionPairs;
	CollisionPairs m_previousTickCollisionPairs;
	
	// helpers for sending events relating to collision pairs
	void SendCollisionPairAddEvent( btPersistentManifold const * manifold, btRigidBody const * body0, btRigidBody const * body1 );
	void SendCollisionPairRemoveEvent( btRigidBody const * body0, btRigidBody const * body1 );
	
	// common functionality used by VAddSphere, VAddBox, etc
	void AddShape(StrongEntityPtr pGameEntity, btCollisionShape* shape, float mass, const std::string& physicsMaterial, const Vec3& rotateConstrains, const Vec3& translateConstrains, const Vec3& shapeOffset);
	
	// helper for cleaning up objects
	void RemoveCollisionObject( btCollisionObject * removeMe );

	// callback from bullet for each physics time step.  set in VInitialize
	static void BulletInternalTickCallback( btDynamicsWorld * const world, btScalar const timeStep );
	
public:
	BulletPhysics();				// [mrmike] This was changed post-press to add event registration!
	virtual ~BulletPhysics();

	// Initialiazation and Maintenance of the Physics World
	virtual bool VInitialize() override;
	virtual void VSyncVisibleScene() override; 
	virtual void VOnUpdate( float deltaSeconds ) override; 

	// Initialization of Physics Objects
	virtual void VAddSphere(float radius, WeakEntityPtr pGameActor, const std::string& densityStr, const std::string& physicsMaterial, const Vec3& rotateConstrains, const Vec3& translateConstrains, const Vec3& shapeOffset) override;
	virtual void VAddBox(const Vec3& dimensions, WeakEntityPtr pGameActor, const std::string& densityStr, const std::string& physicsMaterial, const Vec3& rotateConstrains, const Vec3& translateConstrains, const Vec3& shapeOffset) override;
	virtual void VAddCapsule(float radius, float height, WeakEntityPtr entity, const std::string& densityStr, const std::string& physicsMaterial, const Vec3& rotateConstrains, const Vec3& translateConstrains, const Vec3& shapeOffset) override;
	virtual void VAddConvexHull(Vec3 *verts, int numPoints, WeakEntityPtr pGameActor, const std::string& densityStr, const std::string& physicsMaterial, const Vec3& rotateConstrains, const Vec3& translateConstrains) override;
	virtual void VAddTriangleMesh(float *verts, int num_Vertex, UINT *index, int num_Index, WeakEntityPtr gameEntity, /*const Mat4x4& initialTransform, */ const std::string& densityStr, const std::string& physicsMaterial, const Vec3& rotateConstrains, const Vec3& translateConstrains);
	virtual void VAddHeightField(const float* sHeightmapResource, int iWidth, int iDepth, float fMaxHeight, float fMinHeight, std::string sUpAxis, Vec2 vWorldDimension, WeakEntityPtr gameEntity, const std::string& densityStr, const std::string& physicsMaterial, const Vec3& rotateConstrains, const Vec3& translateConstrains);
	virtual void VAddGhostSphere(float radius, WeakEntityPtr entity) override;
	virtual void VAddGhostBox(const Vec3& dimensions, WeakEntityPtr gameEntity) override;
	virtual void VAddGhostCapsule(float radius, float height, WeakEntityPtr entity) override;
	virtual void VAddGhostConvexHull(Vec3 *verts, int numPoints, WeakEntityPtr gameEntity) override;
	virtual void VRemoveEntity(EntityId id) override;

	// Debugging
	virtual void VRenderDiagnostics() override;

	// Ghost Object Collision Test
	virtual bool VCollisionConvexTest(EntityId id, const Mat4x4& start, const Mat4x4& end, const Vec3& dir, float minSlopeDot, TestCollisionAnswer& testCollisionAnswer);

	// Physics world modifiers
	virtual void VCreateTrigger(WeakEntityPtr pGameActor, const Vec3 &pos, const float dim) override;
	virtual void VApplyForce(const Vec3 &dir, float newtons, EntityId aid) override;
	virtual void VApplyTorque(const Vec3 &dir, float newtons, EntityId aid) override;
	virtual bool VKinematicMove(const Mat4x4 &mat, EntityId aid) override;
	
	virtual void VRotateY(EntityId actorId, float angleRadians, float time);
	virtual float VGetOrientationY(EntityId actorId);
	virtual void VStopEntity(EntityId actorId);
    virtual Vec3 VGetVelocity(EntityId actorId);
    virtual void VSetVelocity(EntityId actorId, const Vec3& vel);
	virtual void VSetVelocityFactor(EntityId actorId, const Vec3& velFactor);
    virtual Vec3 VGetAngularVelocity(EntityId actorId);
    virtual void VSetAngularVelocity(EntityId actorId, const Vec3& vel);
	virtual void VTranslate(EntityId actorId, const Vec3& vec);

    virtual void VSetTransform(const EntityId id, const Mat4x4& mat);

	virtual Mat4x4 VGetTransform(const EntityId id);
};


BulletPhysics::BulletPhysics()
{
	// [mrmike] This was changed post-press to add event registration!
	REGISTER_EVENT(EvtData_PhysTrigger_Enter);
	REGISTER_EVENT(EvtData_PhysTrigger_Leave);
	REGISTER_EVENT(EvtData_PhysCollision);
	REGISTER_EVENT(EvtData_PhysSeparation);
}


/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::~BulletPhysics
//
BulletPhysics::~BulletPhysics()
{
	// delete any physics objects which are still in the world
	
	// iterate backwards because removing the last object doesn't affect the
	//  other objects stored in a vector-type array
	for ( int ii=m_dynamicsWorld->getNumCollisionObjects()-1; ii>=0; --ii )
	{
		btCollisionObject * const obj = m_dynamicsWorld->getCollisionObjectArray()[ii];
		
		RemoveCollisionObject( obj );
	}
	
	m_rigidBodyToActorId.clear();

	SAFE_DELETE(m_debugDrawer);
	SAFE_DELETE(m_dynamicsWorld);
	SAFE_DELETE(m_solver);
	SAFE_DELETE(m_broadphase);
    SAFE_DELETE(m_dispatcher);
	SAFE_DELETE(m_collisionConfiguration);
    SAFE_DELETE(m_ghostPairCallback);
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::LoadXml
//
//    Loads the physics materials from an XML file
//
void BulletPhysics::LoadXml()
{
    // Load the physics config file and grab the root XML node
    TiXmlElement* pRoot = XmlResourceLoader::LoadAndReturnRootXmlElement("config\\Physics.xml");
    CHG_ASSERT(pRoot);

    // load all materials
    TiXmlElement* pParentNode = pRoot->FirstChildElement("PhysicsMaterials");
    CHG_ASSERT(pParentNode);
    for (TiXmlElement* pNode = pParentNode->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
    {
        double restitution = 0;
        double friction = 0;
        pNode->Attribute("restitution", &restitution);
        pNode->Attribute("friction", &friction);
		m_materialTable.insert(std::make_pair(pNode->Value(), MaterialPhysicData((float)restitution, (float)friction)));
    }

    // load all densities
    pParentNode = pRoot->FirstChildElement("DensityTable");
    CHG_ASSERT(pParentNode);
    for (TiXmlElement* pNode = pParentNode->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
    {
        m_densityTable.insert(std::make_pair(pNode->Value(), (float)atof(pNode->FirstChild()->Value())));
    }
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::VInitialize					
//
bool BulletPhysics::VInitialize()
{
	LoadXml();

	// this controls how Bullet does internal memory management during the collision pass
	m_collisionConfiguration = CHG_NEW btDefaultCollisionConfiguration();

	// this manages how Bullet detects precise collisions between pairs of objects
	m_dispatcher = CHG_NEW btCollisionDispatcher( m_collisionConfiguration);

	// Bullet uses this to quickly (imprecisely) detect collisions between objects.
	//   Once a possible collision passes the broad phase, it will be passed to the
	//   slower but more precise narrow-phase collision detection (btCollisionDispatcher).
	m_broadphase = CHG_NEW btDbvtBroadphase();
    m_ghostPairCallback = CHG_NEW btGhostPairCallback();
    m_broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(m_ghostPairCallback);

	// Manages constraints which apply forces to the physics simulation.  Used
	//  for e.g. springs, motors.  We don't use any constraints right now.
	m_solver = new btSequentialImpulseConstraintSolver; //--------------------------------------------> Resolve Use CHG_NEW

	// This is the main Bullet interface point.  Pass in all these components to customize its behavior.
	m_dynamicsWorld = new btDiscreteDynamicsWorld( m_dispatcher, //--------------------------------------------> Resolve Use CHG_NEW
	                                                        m_broadphase, 
	                                                        m_solver, 
	                                                        m_collisionConfiguration );

	m_debugDrawer = CHG_NEW BulletDebugDrawer;
	m_debugDrawer->Init(15000);
	m_debugDrawer->ReadOptions();

	if(!m_collisionConfiguration || !m_dispatcher || !m_broadphase ||
			  !m_solver || !m_dynamicsWorld || !m_debugDrawer)
	{
		CHG_ERROR("BulletPhysics::VInitialize failed!");
		return false;
	}

	m_dynamicsWorld->setDebugDrawer( m_debugDrawer );

	
	// and set the internal tick callback to our own method "BulletInternalTickCallback"
	m_dynamicsWorld->setInternalTickCallback( BulletInternalTickCallback );
	m_dynamicsWorld->setWorldUserInfo( this );

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::VOnUpdate						
//
void BulletPhysics::VOnUpdate( float const deltaSeconds )
{
	// Bullet uses an internal fixed timestep (default 1/60th of a second)
	//   We pass in 4 as a max number of sub steps.  Bullet will run the simulation
	//   in increments of the fixed timestep until "deltaSeconds" amount of time has
	//   passed, but will only run a maximum of 4 steps this way.
	m_dynamicsWorld->stepSimulation( deltaSeconds, 4 );
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::VSyncVisibleScene				
//
void BulletPhysics::VSyncVisibleScene()
{
	// Keep physics & graphics in sync
	
	// check all the existing actor's bodies for changes. 
	//  If there is a change, send the appropriate event for the game system.
	for ( ActorIDToBulletRigidBodyMap::const_iterator it = m_actorIdToRigidBody.begin();
	      it != m_actorIdToRigidBody.end();
	      ++it )
	{ 
		EntityId const id = it->first;
		
		// get the MotionState.  this object is updated by Bullet.
		// it's safe to cast the btMotionState to ActorMotionState, because all the bodies in m_actorIdToRigidBody
		//   were created through AddShape()
		ActorMotionState const * const actorMotionState = static_cast<ActorMotionState*>(it->second->getMotionState());
		CHG_ASSERT( actorMotionState );
		
		StrongEntityPtr pGameActor = MakeStrongPtr(g_pApp->m_pGame->VGetEntity(id));
		if (pGameActor && actorMotionState)
		{
			if (!it->second->isStaticObject())
			{
				shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(pGameActor->GetComponent<TransformComponent>(TransformComponent::g_Name));
				if (pTransformComponent)
				{
					if (pTransformComponent->GetTransform() != actorMotionState->m_worldToPositionTransform)
					{
						// Bullet has moved the actor's physics object.  Sync the transform and inform the game an actor has moved
						pTransformComponent->SetTransform(actorMotionState->m_worldToPositionTransform * scale(pTransformComponent->GetScale()));
						shared_ptr<EvtData_Move_Actor> pEvent(CHG_NEW EvtData_Move_Actor(id, actorMotionState->m_worldToPositionTransform * scale(pTransformComponent->GetScale())));
						IEventManager::Get()->VQueueEvent(pEvent);
					}
				}
			}            
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::AddShape						
//
void BulletPhysics::AddShape(StrongEntityPtr pGameActor, btCollisionShape* shape, float mass, const std::string& physicsMaterial, const Vec3& rotateConstrains, const Vec3& translateConstrains, const Vec3& shapeOffset)
{
    CHG_ASSERT(pGameActor);

    EntityId actorID = pGameActor->GetId();
	CHG_ASSERT(m_actorIdToRigidBody.find( actorID ) == m_actorIdToRigidBody.end() && "Entity with more than one physics body?");

    // lookup the material
	MaterialPhysicData material(LookupMaterialData(physicsMaterial));

	// localInertia defines how the object's mass is distributed
	btVector3 localInertia( 0.f, 0.f, 0.f );
	if ( mass > 0.f )
		shape->calculateLocalInertia( mass, localInertia );


	Mat4x4 transform = Mat4x4::g_Identity;
    shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(pGameActor->GetComponent<TransformComponent>(TransformComponent::g_Name));
	CHG_ASSERT(pTransformComponent);
    if (pTransformComponent)
    {
		transform = pTransformComponent->GetTransform();
	}
	else
	{
		// Physics can't work on an actor that doesn't have a TransformComponent!
		return;
	}

	Mat4x4 localPhysicMat = translate(shapeOffset);
	transform = transform * localPhysicMat;

	// set the initial transform of the body from the actor
	ActorMotionState * const myMotionState = CHG_NEW ActorMotionState(transform);
	
	btRigidBody::btRigidBodyConstructionInfo rbInfo( mass, myMotionState, shape, localInertia );
	
	// set up the materal properties
	rbInfo.m_restitution = material.m_restitution;
	rbInfo.m_friction    = material.m_friction;
	
	btRigidBody * const body = new btRigidBody(rbInfo);
	
	body->setLinearFactor(btVector3(translateConstrains.x, translateConstrains.y, translateConstrains.z));
	body->setAngularFactor(btVector3(rotateConstrains.x, rotateConstrains.y, rotateConstrains.z));

	m_dynamicsWorld->addRigidBody( body );
	
	// add it to the collection to be checked for changes in VSyncVisibleScene
	m_actorIdToRigidBody[actorID] = body;
	m_rigidBodyToActorId[body] = actorID;
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::RemoveCollisionObject
//
//    Removes a collision object from the game world
//
void BulletPhysics::RemoveCollisionObject( btCollisionObject * const removeMe )
{
	// first remove the object from the physics sim
	m_dynamicsWorld->removeCollisionObject( removeMe );
	
	// then remove the pointer from the ongoing contacts list
	for ( CollisionPairs::iterator it = m_previousTickCollisionPairs.begin();
	      it != m_previousTickCollisionPairs.end(); )
    {
		CollisionPairs::iterator next = it;
		++next;
		
		if ( it->first == removeMe || it->second == removeMe )
		{
			SendCollisionPairRemoveEvent( it->first, it->second );
			m_previousTickCollisionPairs.erase( it );
		}
		
		it = next;
    }
	
	// if the object is a RigidBody (all of ours are RigidBodies, but it's good to be safe)
	if ( btRigidBody * const body = btRigidBody::upcast(removeMe) )
	{
		// delete the components of the object
		delete body->getMotionState();
		delete body->getCollisionShape();
		delete body->getUserPointer();
		delete body->getUserPointer();
		
		for ( int ii=body->getNumConstraintRefs()-1; ii >= 0; --ii )
		{
			btTypedConstraint * const constraint = body->getConstraintRef( ii );
			m_dynamicsWorld->removeConstraint( constraint );
			delete constraint;
		}
	}
	
	delete removeMe;
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::FindBulletRigidBody			
//    Finds a Bullet rigid body given an actor ID
//
btRigidBody* BulletPhysics::FindBulletRigidBody( EntityId const id ) const
{
	ActorIDToBulletRigidBodyMap::const_iterator found = m_actorIdToRigidBody.find( id );
	if ( found != m_actorIdToRigidBody.end() )
		return found->second;

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::FindActorID				
//    Finds an Actor ID given a Bullet rigid body 
//
EntityId BulletPhysics::FindActorID( btRigidBody const * const body ) const
{
	BulletRigidBodyToActorIDMap::const_iterator found = m_rigidBodyToActorId.find( body );
	if ( found != m_rigidBodyToActorId.end() )
		return found->second;
		
	return EntityId();
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::FindGhostCollisionObject			
//    Finds a Ghost Collision object given an actor ID
//
GhostCollisionObject* BulletPhysics::FindGhostCollisionObject( EntityId const id ) const
{
	EntityIDToGhostCollisionObjectMap::const_iterator found = m_EntityIdToGhostCollisionObject.find( id );
	if ( found != m_EntityIdToGhostCollisionObject.end() )
		return found->second;

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::FindActorID				
//    Finds an Actor ID given a ghost collision object 
//
EntityId BulletPhysics::FindActorID( GhostCollisionObject const * const body ) const
{
	GhostCollisionObjectToEntityIDMap::const_iterator found = m_GhostCollisionObjectToEntityId.find( body );
	if ( found != m_GhostCollisionObjectToEntityId.end() )
		return found->second;
		
	return EntityId();
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::VAddSphere					
//
void BulletPhysics::VAddSphere(float const radius, WeakEntityPtr pGameActor, const std::string& densityStr, const std::string& physicsMaterial, const Vec3& rotateConstrains, const Vec3& translateConstrains, const Vec3& shapeOffset)
{
    StrongEntityPtr pStrongActor = MakeStrongPtr(pGameActor);
    if (!pStrongActor)
        return;  // FUTURE WORK - Add a call to the error log here
	
	// create the collision body, which specifies the shape of the object
	btSphereShape * const collisionShape = new btSphereShape( radius );

	// calculate absolute mass from specificGravity
    float specificGravity = LookupSpecificGravity(densityStr);
	float const volume = (4.f / 3.f) * CHG_PI * radius * radius * radius;
	btScalar const mass = volume * specificGravity;
	
	AddShape(pStrongActor, /*initialTransform,*/ collisionShape, mass, physicsMaterial, rotateConstrains, translateConstrains, shapeOffset);
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::VAddBox
//
void BulletPhysics::VAddBox(const Vec3& dimensions, WeakEntityPtr pGameActor, const std::string& densityStr, const std::string& physicsMaterial, const Vec3& rotateConstrains, const Vec3& translateConstrains, const Vec3& shapeOffset)
{
    StrongEntityPtr pStrongActor = MakeStrongPtr(pGameActor);
    if (!pStrongActor)
        return;  // FUTURE WORK: Add a call to the error log here

	// create the collision body, which specifies the shape of the object
	btBoxShape * const boxShape = new btBoxShape( Vec3_to_btVector3( 0.5f*dimensions ) );
	
	// calculate absolute mass from specificGravity
    float specificGravity = LookupSpecificGravity(densityStr);
	float const volume = dimensions.x * dimensions.y * dimensions.z;
	btScalar const mass = volume * specificGravity;
	
	AddShape(pStrongActor,/* initialTransform,*/ boxShape, mass, physicsMaterial, rotateConstrains, translateConstrains, shapeOffset);
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::VAddCapsule
void BulletPhysics::VAddCapsule(float radius, float height, WeakEntityPtr entity, const std::string& densityStr, const std::string& physicsMaterial, const Vec3& rotateConstrains, const Vec3& translateConstrains, const Vec3& shapeOffset)
{
	StrongEntityPtr pStrongEntity = MakeStrongPtr(entity);
	if(!pStrongEntity)
		return;

	// create the collision body, which specifies the shape of the object
	btCapsuleShape * const capsuleShape = new btCapsuleShape( radius, height );

	// calculate absolute mass from specificGravity
	float specificGravity = LookupSpecificGravity(densityStr);
	float const volume = CHG_PI*radius*radius*height + (4.f / 3.f)*CHG_PI*radius*radius*radius;
	btScalar const mass = volume * specificGravity;
	AddShape(pStrongEntity, capsuleShape, mass, physicsMaterial, rotateConstrains, translateConstrains, shapeOffset);
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::VAddConvexHull				
//
void BulletPhysics::VAddConvexHull(Vec3 *verts, int numPoints, WeakEntityPtr pGameActor, /*const Mat4x4& initialTransform,*/ const std::string& densityStr, const std::string& physicsMaterial, const Vec3& rotateConstrains, const Vec3& translateConstrains)
{
    StrongEntityPtr pStrongActor = MakeStrongPtr(pGameActor);
    if (!pStrongActor)
        return;  // FUTURE WORK: Add a call to the error log here
	
	btConvexHullShape * const shape = new btConvexHullShape();

    Mat4x4 scaleMat = Mat4x4::g_Identity;
    shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(pStrongActor->GetComponent<TransformComponent>(TransformComponent::g_Name));
    CHG_ASSERT(pTransformComponent);
    if (pTransformComponent)
    {
        scaleMat = scale(pTransformComponent->GetScale());
    }
    else
    {
        // Physics can't work on an actor that doesn't have a TransformComponent!
        return;
    }

	// add the points to the shape one at a time
	for ( int ii=0; ii<numPoints; ++ii )
	{
        //verts[ii] = (scaleMat * Vec4(verts[ii], 1.0f)).xyz();
        shape->addPoint(  Vec3_to_btVector3( verts[ii] ) );
	}
	
	// approximate absolute mass using bounding box
	btVector3 aabbMin(0,0,0), aabbMax(0,0,0);
	shape->getAabb( btTransform::getIdentity(), aabbMin, aabbMax );
	
	btVector3 const aabbExtents = aabbMax - aabbMin;
	
    float specificGravity = LookupSpecificGravity(densityStr);
	float const volume = aabbExtents.x() * aabbExtents.y() * aabbExtents.z();
	btScalar const mass = volume * specificGravity;
	
	Vec3 offsetShape = Vec3();

	AddShape(pStrongActor, shape, mass, physicsMaterial, rotateConstrains, translateConstrains, offsetShape);
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::VAddTriangleMesh				
//
void BulletPhysics::VAddTriangleMesh(float *verts, int num_Vertex, UINT *index, int num_Triangles, WeakEntityPtr gameEntity, /*const Mat4x4& initialTransform, */ const std::string& densityStr, const std::string& physicsMaterial, const Vec3& rotateConstrains, const Vec3& translateConstrains)
{
	StrongEntityPtr pStrongActor = MakeStrongPtr(gameEntity);
	if (!pStrongActor)
		return;  // FUTURE WORK: Add a call to the error log here

	btTriangleIndexVertexArray *mesh = new btTriangleIndexVertexArray(num_Triangles, (int*)index, 3*sizeof(int), num_Vertex, verts, 3*sizeof(float));
	btBvhTriangleMeshShape * const shape = new btBvhTriangleMeshShape(mesh, true, true);

	Mat4x4 scaleMat = Mat4x4::g_Identity;
	shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(pStrongActor->GetComponent<TransformComponent>(TransformComponent::g_Name));
	CHG_ASSERT(pTransformComponent);
	if (pTransformComponent)
	{
		scaleMat = scale(pTransformComponent->GetScale());
	}
	else
	{
		// Physics can't work on an actor that doesn't have a TransformComponent!
		return;
	}

	// approximate absolute mass using bounding box
	btVector3 aabbMin(0, 0, 0), aabbMax(0, 0, 0);
	shape->getAabb(btTransform::getIdentity(), aabbMin, aabbMax);

	btVector3 const aabbExtents = aabbMax - aabbMin;

	float specificGravity = LookupSpecificGravity(densityStr);
	float const volume = aabbExtents.x() * aabbExtents.y() * aabbExtents.z();
	btScalar const mass = volume * specificGravity;

	Vec3 offsetShape = Vec3();

	AddShape(pStrongActor, shape, mass, physicsMaterial, rotateConstrains, translateConstrains, offsetShape);
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::VAddHeightField				
//

void BulletPhysics::VAddHeightField(const float* sHeightmapResource, int iWidth, int iDepth, float fMaxHeight, float fMinHeight, std::string sUpAxis, Vec2 vWorldDimension, WeakEntityPtr gameEntity, const std::string& densityStr, const std::string& physicsMaterial, const Vec3& rotateConstrains, const Vec3& translateConstrains)
{
	StrongEntityPtr pStrongActor = MakeStrongPtr(gameEntity);
	if (!pStrongActor)
		return;  // FUTURE WORK: Add a call to the error log here

	int iUpAxis;

	if (sUpAxis == "X")
	{
		iUpAxis = 0;
	}
	else if (sUpAxis == "Y")
	{
		iUpAxis = 1;
	}
	else
	{
		iUpAxis = 2;
	}

	btHeightfieldTerrainShape * const shape = new btHeightfieldTerrainShape(iWidth, iDepth, sHeightmapResource, 1.0f, btScalar(fMinHeight), btScalar(fMaxHeight), iUpAxis, PHY_FLOAT, false);
	shape->setLocalScaling(btVector3(vWorldDimension.x / iWidth, 1.0f, vWorldDimension.y / iDepth));

	Mat4x4 scaleMat = Mat4x4::g_Identity;
	shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(pStrongActor->GetComponent<TransformComponent>(TransformComponent::g_Name));
	CHG_ASSERT(pTransformComponent);
	if (pTransformComponent)
	{
		scaleMat = scale(pTransformComponent->GetScale());
	}
	else
	{
		// Physics can't work on an actor that doesn't have a TransformComponent!
		return;
	}

	// approximate absolute mass using bounding box
	btVector3 aabbMin(0, 0, 0), aabbMax(0, 0, 0);
	shape->getAabb(btTransform::getIdentity(), aabbMin, aabbMax);

	btVector3 const aabbExtents = aabbMax - aabbMin;

	float specificGravity = LookupSpecificGravity(densityStr);
	float const volume = aabbExtents.x() * aabbExtents.y() * aabbExtents.z();
	btScalar const mass = volume * specificGravity;

	Vec3 offsetShape = Vec3(0.0f, (fMinHeight + fMaxHeight)*0.5f, 0.0f);

	AddShape(pStrongActor, shape, mass, physicsMaterial, rotateConstrains, translateConstrains, offsetShape);
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::VAddGhostSphere				
//
void BulletPhysics::VAddGhostSphere(float radius, WeakEntityPtr pEntity)
{
	StrongEntityPtr pStrongActor = MakeStrongPtr(pEntity);
    if (!pStrongActor)
        return;  // FUTURE WORK - Add a call to the error log here
	
	// create the collision body, which specifies the shape of the object
	btSphereShape * const collisionShape = new btSphereShape( radius );

	// create ghost object for collision
	btGhostObject* pGhostObject = new btPairCachingGhostObject();

	Mat4x4 transform = Mat4x4::g_Identity;
    shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(pStrongActor->GetComponent<TransformComponent>(TransformComponent::g_Name));
	CHG_ASSERT(pTransformComponent);
    if (pTransformComponent)
    {
		transform = pTransformComponent->GetTransform();
	}
	else
	{
		// Physics can't work on an actor that doesn't have a TransformComponent!
		return;
	}

	pGhostObject->setWorldTransform(Mat4x4_to_btTransform(transform));

	pGhostObject->setCollisionShape(collisionShape);
	pGhostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

	m_dynamicsWorld->addCollisionObject(pGhostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter|btBroadphaseProxy::DefaultFilter);

	GhostCollisionObject* pObject = new GhostCollisionObject;
	pObject->pBulletGhostObject = pGhostObject;
	pObject->pGhostObjectShape = collisionShape;

	m_EntityIdToGhostCollisionObject[pStrongActor->GetId()] = pObject;
	m_GhostCollisionObjectToEntityId[pObject] = pStrongActor->GetId();
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::VAddGhostBox				
//
void BulletPhysics::VAddGhostBox(const Vec3& dimensions, WeakEntityPtr pEntity)
{
	StrongEntityPtr pStrongActor = MakeStrongPtr(pEntity);
    if (!pStrongActor)
        return;  // FUTURE WORK - Add a call to the error log here
	
	// create the collision body, which specifies the shape of the object
	btBoxShape * const collisionShape = new btBoxShape( Vec3_to_btVector3( 0.5f*dimensions ) );

	// create ghost object for collision
	btGhostObject* pGhostObject = new btPairCachingGhostObject();

	Mat4x4 transform = Mat4x4::g_Identity;
    shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(pStrongActor->GetComponent<TransformComponent>(TransformComponent::g_Name));
	CHG_ASSERT(pTransformComponent);
    if (pTransformComponent)
    {
		transform = pTransformComponent->GetTransform();
	}
	else
	{
		// Physics can't work on an actor that doesn't have a TransformComponent!
		return;
	}

	pGhostObject->setWorldTransform(Mat4x4_to_btTransform(transform));

	pGhostObject->setCollisionShape(collisionShape);
	pGhostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

	m_dynamicsWorld->addCollisionObject(pGhostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter|btBroadphaseProxy::DefaultFilter);

	GhostCollisionObject* pObject = new GhostCollisionObject;
	pObject->pBulletGhostObject = pGhostObject;
	pObject->pGhostObjectShape = collisionShape;

	m_EntityIdToGhostCollisionObject[pStrongActor->GetId()] = pObject;
	m_GhostCollisionObjectToEntityId[pObject] = pStrongActor->GetId();
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::VAddGhostCapsule				
//
void BulletPhysics::VAddGhostCapsule(float radius, float height, WeakEntityPtr pEntity)
{
	StrongEntityPtr pStrongActor = MakeStrongPtr(pEntity);
    if (!pStrongActor)
        return;  // FUTURE WORK - Add a call to the error log here
	
	// create the collision body, which specifies the shape of the object
	btCapsuleShape * const collisionShape = new btCapsuleShape( radius, height );

	// create ghost object for collision
	btGhostObject* pGhostObject = new btPairCachingGhostObject();

	Mat4x4 transform = Mat4x4::g_Identity;
    shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(pStrongActor->GetComponent<TransformComponent>(TransformComponent::g_Name));
	CHG_ASSERT(pTransformComponent);
    if (pTransformComponent)
    {
		transform = pTransformComponent->GetTransform();
	}
	else
	{
		// Physics can't work on an actor that doesn't have a TransformComponent!
		return;
	}

	pGhostObject->setWorldTransform(Mat4x4_to_btTransform(transform));

	pGhostObject->setCollisionShape(collisionShape);
	pGhostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

	m_dynamicsWorld->addCollisionObject(pGhostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter|btBroadphaseProxy::DefaultFilter);

	GhostCollisionObject* pObject = new GhostCollisionObject;
	pObject->pBulletGhostObject = pGhostObject;
	pObject->pGhostObjectShape = collisionShape;

	m_EntityIdToGhostCollisionObject[pStrongActor->GetId()] = pObject;
	m_GhostCollisionObjectToEntityId[pObject] = pStrongActor->GetId();
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::VAddGhostConvexHull				
//
void BulletPhysics::VAddGhostConvexHull(Vec3 *verts, int numPoints, WeakEntityPtr pEntity)
{
	StrongEntityPtr pStrongActor = MakeStrongPtr(pEntity);
    if (!pStrongActor)
        return;  // FUTURE WORK - Add a call to the error log here
	
	// create the collision body, which specifies the shape of the object
	btConvexHullShape * const collisionShape = new btConvexHullShape();

	// add the points to the shape one at a time
	for ( int ii=0; ii<numPoints; ++ii )
	{
		collisionShape->addPoint(  Vec3_to_btVector3( verts[ii] ) );
	}

	// create ghost object for collision
	btGhostObject* pGhostObject = new btPairCachingGhostObject();

	Mat4x4 transform = Mat4x4::g_Identity;
    shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(pStrongActor->GetComponent<TransformComponent>(TransformComponent::g_Name));
	CHG_ASSERT(pTransformComponent);
    if (pTransformComponent)
    {
		transform = pTransformComponent->GetTransform();
	}
	else
	{
		// Physics can't work on an actor that doesn't have a TransformComponent!
		return;
	}

	pGhostObject->setWorldTransform(Mat4x4_to_btTransform(transform));

	pGhostObject->setCollisionShape(collisionShape);
	pGhostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

	m_dynamicsWorld->addCollisionObject(pGhostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter|btBroadphaseProxy::DefaultFilter);

	GhostCollisionObject* pObject = new GhostCollisionObject;
	pObject->pBulletGhostObject = pGhostObject;
	pObject->pGhostObjectShape = collisionShape;

	m_EntityIdToGhostCollisionObject[pStrongActor->GetId()] = pObject;
	m_GhostCollisionObjectToEntityId[pObject] = pStrongActor->GetId();
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::VRemoveActor					
//
//    Implements the method to remove actors from the physics simulation
//
void BulletPhysics::VRemoveEntity(EntityId id)
{
	if ( btRigidBody * const body = FindBulletRigidBody( id ) )
	{
		// destroy the body and all its components
		RemoveCollisionObject( body );
		m_actorIdToRigidBody.erase ( id );
		m_rigidBodyToActorId.erase( body );
	}
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::VRenderDiagnostics			
//
void BulletPhysics::VRenderDiagnostics()
{
	m_dynamicsWorld->debugDrawWorld();
	m_debugDrawer->RenderDebugPhysic();
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::VCollisionConvexTest			
//
bool BulletPhysics::VCollisionConvexTest(EntityId id, const Mat4x4& start, const Mat4x4& end, const Vec3& dir, float minSlopeDot, TestCollisionAnswer& testCollisionAnswer)
{
	if (distance(end.GetPosition(), start.GetPosition()) <= 0)
		return false;

	GhostCollisionObject* pObject = FindGhostCollisionObject(id);

	//Vec3 startPos = start.GetPosition();
	//Vec3 endPos = start.GetPosition();

	//printf("S Position: ( %f, %f, %f )\n", startPos.x, startPos.y, startPos.z);
	//printf("E Position: ( %f, %f, %f )\n", endPos.x, endPos.y, endPos.z);

	btKinematicClosestNotMeConvexResultCallback callback (pObject->pBulletGhostObject, Vec3_to_btVector3(dir), btScalar(minSlopeDot));
	callback.m_collisionFilterGroup = pObject->pBulletGhostObject->getBroadphaseHandle()->m_collisionFilterGroup;
	callback.m_collisionFilterMask = pObject->pBulletGhostObject->getBroadphaseHandle()->m_collisionFilterMask;
	
	pObject->pBulletGhostObject->convexSweepTest(pObject->pGhostObjectShape, Mat4x4_to_btTransform(start), Mat4x4_to_btTransform(end), callback);

	testCollisionAnswer.closedHitFraction = callback.m_closestHitFraction;
	testCollisionAnswer.hitNormalPosition = btVector3_to_Vec3(callback.m_hitNormalWorld);
	testCollisionAnswer.hitPointWorld = btVector3_to_Vec3(callback.m_hitPointWorld);

	/*if (callback.hasHit())
		printf("Hit Position: ( %f, %f, %f )\n", testCollisionAnswer.hitPointWorld.x, testCollisionAnswer.hitPointWorld.y, testCollisionAnswer.hitPointWorld.z);*/

	return testCollisionAnswer.hasHit = callback.hasHit();
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::VCreateTrigger				
//
// FUTURE WORK: Mike create a trigger actor archetype that can be instantiated in the editor!!!!!
//
void BulletPhysics::VCreateTrigger(WeakEntityPtr pGameActor, const Vec3 &pos, const float dim)
{
    StrongEntityPtr pStrongActor = MakeStrongPtr(pGameActor);
    if (!pStrongActor)
        return;  // FUTURE WORK: Add a call to the error log here

	// create the collision body, which specifies the shape of the object
	btBoxShape * const boxShape = new btBoxShape( Vec3_to_btVector3( Vec3(dim,dim,dim) ) );
	
	// triggers are immoveable.  0 mass signals this to Bullet.
	btScalar const mass = 0;

	// set the initial position of the body from the actor
	Mat4x4 triggerTrans = Mat4x4::g_Identity;
	triggerTrans.SetPosition( pos );
	ActorMotionState * const myMotionState = CHG_NEW ActorMotionState( triggerTrans );
	
	btRigidBody::btRigidBodyConstructionInfo rbInfo( mass, myMotionState, boxShape, btVector3(0,0,0) );
	btRigidBody * const body = new btRigidBody(rbInfo);
	
	m_dynamicsWorld->addRigidBody( body );
	
	// a trigger is just a box that doesn't collide with anything.  That's what "CF_NO_CONTACT_RESPONSE" indicates.
	body->setCollisionFlags( body->getCollisionFlags() | btRigidBody::CF_NO_CONTACT_RESPONSE );

	m_actorIdToRigidBody[pStrongActor->GetId()] = body;
	m_rigidBodyToActorId[body] = pStrongActor->GetId();
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::VApplyForce					
//
void BulletPhysics::VApplyForce(const Vec3 &dir, float newtons, EntityId aid)
{
	if ( btRigidBody * const body = FindBulletRigidBody( aid ) )
	{
		body->setActivationState(DISABLE_DEACTIVATION);

		btVector3 const force( dir.x * newtons,
		                       dir.y * newtons,
		                       dir.z * newtons );

		body->applyCentralImpulse( force );
	}
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::VApplyTorque					
//
void BulletPhysics::VApplyTorque(const Vec3 &dir, float magnitude, EntityId aid)
{
	if ( btRigidBody * const body = FindBulletRigidBody( aid ) )
	{
		body->setActivationState(DISABLE_DEACTIVATION);

		btVector3 const torque( dir.x * magnitude,
		                        dir.y * magnitude,
		                        dir.z * magnitude );

		body->applyTorqueImpulse( torque );
	}
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::VKinematicMove				
//
//    Forces a phyics object to a new location/orientation
//
bool BulletPhysics::VKinematicMove(const Mat4x4 &mat, EntityId aid)
{
	if ( btRigidBody * const body = FindBulletRigidBody( aid ) )
	{
        body->setActivationState(DISABLE_DEACTIVATION);

		// warp the body to the new position
		body->setWorldTransform( Mat4x4_to_btTransform( mat ) );
		return true;
	}
	else if(GhostCollisionObject* object = FindGhostCollisionObject(aid))
	{
		object->pBulletGhostObject->setWorldTransform(Mat4x4_to_btTransform(mat));
	}
	
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::VGetTransform					
//
//   Returns the current transform of the phyics object
//
Mat4x4 BulletPhysics::VGetTransform(const EntityId id)
{
    btRigidBody * pRigidBody = FindBulletRigidBody(id);
    CHG_ASSERT(pRigidBody);

    const btTransform& actorTransform = pRigidBody->getCenterOfMassTransform();
    return btTransform_to_Mat4x4(actorTransform);
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::VSetTransform					
//
//   Sets the current transform of the phyics object
//
void BulletPhysics::VSetTransform(EntityId actorId, const Mat4x4& mat)
{
    VKinematicMove(mat, actorId);
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::VRotateY						
//
//   A helper function used to turn objects to a new heading
//
void BulletPhysics::VRotateY( EntityId const actorId, float const deltaAngleRadians, float const time )
{
	btRigidBody * pRigidBody = FindBulletRigidBody(actorId);
	CHG_ASSERT(pRigidBody);

	// create a transform to represent the additional turning this frame
	btTransform angleTransform;
	angleTransform.setIdentity();
	angleTransform.getBasis().setEulerYPR( 0, deltaAngleRadians, 0 ); // rotation about body Y-axis
			
	// concatenate the transform onto the body's transform
	pRigidBody->setCenterOfMassTransform( pRigidBody->getCenterOfMassTransform() * angleTransform );
}



/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::VGetOrientationY				
//
//   A helper functions use to access the current heading of a physics object
//
float BulletPhysics::VGetOrientationY(EntityId actorId)
{
	btRigidBody * pRigidBody = FindBulletRigidBody(actorId);
	CHG_ASSERT(pRigidBody);
	
	const btTransform& actorTransform = pRigidBody->getCenterOfMassTransform();
	btMatrix3x3 actorRotationMat(actorTransform.getBasis());  // should be just the rotation information

	btVector3 startingVec(0,0,1);
	btVector3 endingVec = actorRotationMat * startingVec; // transform the vector

	endingVec.setY(0);  // we only care about rotation on the XZ plane

	float const endingVecLength = endingVec.length();
	if (endingVecLength < 0.001)
	{
		// gimbal lock (orientation is straight up or down)
		return 0;
	}

	else
	{
		btVector3 cross = startingVec.cross(endingVec);
		float sign = cross.getY() > 0 ? 1.0f : -1.0f;
		return (acosf(startingVec.dot(endingVec) / endingVecLength) * sign);
	}

	return FLT_MAX;  // fail...
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::VStopEntity				
//
void BulletPhysics::VStopEntity(EntityId actorId)
{
   VSetVelocity(actorId, Vec3(0.f, 0.f, 0.f));
}

/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::VGetVelocity			
//
Vec3 BulletPhysics::VGetVelocity(EntityId actorId)
{
    btRigidBody* pRigidBody = FindBulletRigidBody(actorId);
    CHG_ASSERT(pRigidBody);
    if (!pRigidBody)
        return Vec3();
    btVector3 btVel = pRigidBody->getLinearVelocity();
    return btVector3_to_Vec3(btVel);
}

/////////////////////////////////////////////////////////////////////////////
void BulletPhysics::VSetVelocity(EntityId actorId, const Vec3& vel)
{
	btRigidBody * pRigidBody = FindBulletRigidBody(actorId);
	CHG_ASSERT(pRigidBody);
	if (!pRigidBody)
		return;
	btVector3 btVel = Vec3_to_btVector3(vel);
	pRigidBody->setLinearVelocity(btVel);

	if (vel.x != 0.0f || vel.y!= 0.0f || vel.z!= 0.0f)
		pRigidBody->setActivationState(ACTIVE_TAG);
}
/////////////////////////////////////////////////////////////////////////////
void BulletPhysics::VSetVelocityFactor(EntityId actorId, const Vec3& velFactor)
{
	btRigidBody * pRigidBody = FindBulletRigidBody(actorId);
	CHG_ASSERT(pRigidBody);
	if (!pRigidBody)
		return;
	btVector3 btVelFactor = Vec3_to_btVector3(velFactor);
	pRigidBody->setLinearFactor(btVelFactor);
}

/////////////////////////////////////////////////////////////////////////////
Vec3 BulletPhysics::VGetAngularVelocity(EntityId actorId)
{
    btRigidBody* pRigidBody = FindBulletRigidBody(actorId);
    CHG_ASSERT(pRigidBody);
    if (!pRigidBody)
        return Vec3();
    btVector3 btVel = pRigidBody->getAngularVelocity();
    return btVector3_to_Vec3(btVel);
}

/////////////////////////////////////////////////////////////////////////////
void BulletPhysics::VSetAngularVelocity(EntityId actorId, const Vec3& vel)
{
    btRigidBody * pRigidBody = FindBulletRigidBody(actorId);
    CHG_ASSERT(pRigidBody);
    if (!pRigidBody)
        return;
    btVector3 btVel = Vec3_to_btVector3(vel);
    pRigidBody->setAngularVelocity(btVel);
}

/////////////////////////////////////////////////////////////////////////////
void BulletPhysics::VTranslate(EntityId actorId, const Vec3& vec)
{
	btRigidBody * pRigidBody = FindBulletRigidBody(actorId);
	CHG_ASSERT(pRigidBody);
	btVector3 btVec = Vec3_to_btVector3(vec);
	pRigidBody->translate(btVec);
}


/////////////////////////////////////////////////////////////////////////////
// BulletPhysics::BulletInternalTickCallback	
//
// This function is called after bullet performs its internal update.  We
//   use it to detect collisions between objects for Game code.
//
void BulletPhysics::BulletInternalTickCallback( btDynamicsWorld * const world, btScalar const timeStep )
{
	CHG_ASSERT( world );
	
	CHG_ASSERT( world->getWorldUserInfo() );
	BulletPhysics * const bulletPhysics = static_cast<BulletPhysics*>( world->getWorldUserInfo() );
	
	CollisionPairs currentTickCollisionPairs;
	
	// look at all existing contacts
	btDispatcher * const dispatcher = world->getDispatcher();
	for ( int manifoldIdx=0; manifoldIdx<dispatcher->getNumManifolds(); ++manifoldIdx )
	{
		// get the "manifold", which is the set of data corresponding to a contact point
		//   between two physics objects
		btPersistentManifold const * const manifold = dispatcher->getManifoldByIndexInternal( manifoldIdx );
		CHG_ASSERT( manifold );
		
		// get the two bodies used in the manifold.  Bullet stores them as void*, so we must cast
		//  them back to btRigidBody*s.  Manipulating void* pointers is usually a bad
		//  idea, but we have to work with the environment that we're given.  We know this
		//  is safe because we only ever add btRigidBodys to the simulation
		btRigidBody const * const body0 = static_cast<btRigidBody const *>(manifold->getBody0());
		btRigidBody const * const body1 = static_cast<btRigidBody const *>(manifold->getBody1());
		
		// always create the pair in a predictable order
		bool const swapped = body0 > body1;
		
		btRigidBody const * const sortedBodyA = swapped ? body1 : body0;
		btRigidBody const * const sortedBodyB = swapped ? body0 : body1;
		
		CollisionPair const thisPair = std::make_pair( sortedBodyA, sortedBodyB );
		currentTickCollisionPairs.insert( thisPair );
		
		if ( bulletPhysics->m_previousTickCollisionPairs.find( thisPair ) == bulletPhysics->m_previousTickCollisionPairs.end() )
		{
			// this is a new contact, which wasn't in our list before.  send an event to the game.
			bulletPhysics->SendCollisionPairAddEvent( manifold, body0, body1 );
		}
	}
	
	CollisionPairs removedCollisionPairs;
	
	// use the STL set difference function to find collision pairs that existed during the previous tick but not any more
	std::set_difference( bulletPhysics->m_previousTickCollisionPairs.begin(), bulletPhysics->m_previousTickCollisionPairs.end(),
	                     currentTickCollisionPairs.begin(), currentTickCollisionPairs.end(),
	                     std::inserter( removedCollisionPairs, removedCollisionPairs.begin() ) );
	
	for ( CollisionPairs::const_iterator it = removedCollisionPairs.begin(), 
         end = removedCollisionPairs.end(); it != end; ++it )
	{
		btRigidBody const * const body0 = it->first;
		btRigidBody const * const body1 = it->second;
		
		bulletPhysics->SendCollisionPairRemoveEvent( body0, body1 );
	}
	
	// the current tick becomes the previous tick.  this is the way of all things.
	bulletPhysics->m_previousTickCollisionPairs = currentTickCollisionPairs;
}

//////////////////////////////////////////////////////////////////////////////////////////
void BulletPhysics::SendCollisionPairAddEvent( btPersistentManifold const * manifold, btRigidBody const * const body0, btRigidBody const * const body1 )
{
	if ( body0->getUserPointer() || body1->getUserPointer() )
	{
		// only triggers have non-NULL userPointers
		
		// figure out which actor is the trigger
		btRigidBody const * triggerBody, * otherBody;
	
		if ( body0->getUserPointer() )
		{
			triggerBody = body0;
			otherBody = body1;
		}
		else
		{
			otherBody = body0;
			triggerBody = body1;
		}
		
		// send the trigger event.
		int const triggerId = *static_cast<int*>(triggerBody->getUserPointer());
        shared_ptr<EvtData_PhysTrigger_Enter> pEvent(CHG_NEW EvtData_PhysTrigger_Enter(triggerId, FindActorID(otherBody)));
        IEventManager::Get()->VQueueEvent(pEvent);
	}
	else
	{
		EntityId const id0 = FindActorID( body0 );
		EntityId const id1 = FindActorID( body1 );
		
		if (id0 == INVALID_ENTITY_ID || id1 == INVALID_ENTITY_ID)
		{
			// something is colliding with a non-actor.  we currently don't send events for that
			return;
		}
		
		// this pair of colliding objects is new.  send a collision-begun event
		Vec3List collisionPoints;
		Vec3 sumNormalForce;
		Vec3 sumFrictionForce;
		
		for ( int pointIdx = 0; pointIdx < manifold->getNumContacts(); ++pointIdx )
		{
			btManifoldPoint const & point = manifold->getContactPoint( pointIdx );
		
			collisionPoints.push_back( btVector3_to_Vec3( point.getPositionWorldOnB() ) );
			
			sumNormalForce += btVector3_to_Vec3( point.m_combinedRestitution * point.m_normalWorldOnB );
			sumFrictionForce += btVector3_to_Vec3( point.m_combinedFriction * point.m_lateralFrictionDir1 );
		}
		
		// send the event for the game
        shared_ptr<EvtData_PhysCollision> pEvent(CHG_NEW EvtData_PhysCollision(id0, id1, sumNormalForce, sumFrictionForce, collisionPoints));
        IEventManager::Get()->VQueueEvent(pEvent);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void BulletPhysics::SendCollisionPairRemoveEvent( btRigidBody const * const body0, btRigidBody const * const body1 )
{
	if ( body0->getUserPointer() || body1->getUserPointer() )
	{
		// figure out which actor is the trigger
		btRigidBody const * triggerBody, * otherBody;
	
		if ( body0->getUserPointer() )
		{
			triggerBody = body0;
			otherBody = body1;
		}
		else
		{
			otherBody = body0;
			triggerBody = body1;
		}
		
		// send the trigger event.
		int const triggerId = *static_cast<int*>(triggerBody->getUserPointer());
        shared_ptr<EvtData_PhysTrigger_Leave> pEvent(CHG_NEW EvtData_PhysTrigger_Leave(triggerId, FindActorID( otherBody)));
        IEventManager::Get()->VQueueEvent(pEvent);
	}
	else
	{
		EntityId const id0 = FindActorID( body0 );
		EntityId const id1 = FindActorID( body1 );
	
		if ( id0 == INVALID_ENTITY_ID || id1 == INVALID_ENTITY_ID )
		{
			// collision is ending between some object(s) that don't have actors.  we don't send events for that.
			return;
		}

        shared_ptr<EvtData_PhysSeparation> pEvent(CHG_NEW EvtData_PhysSeparation(id0, id1));
        IEventManager::Get()->VQueueEvent(pEvent);
	}
}

float BulletPhysics::LookupSpecificGravity(const std::string& densityStr)
{
    float density = 0;
    auto densityIt = m_densityTable.find(densityStr);
    if (densityIt != m_densityTable.end())
        density = densityIt->second;
    // else: dump error

    return density;
}

MaterialPhysicData BulletPhysics::LookupMaterialData(const std::string& materialStr)
{
    auto materialIt = m_materialTable.find(materialStr);
    if (materialIt != m_materialTable.end())
        return materialIt->second;
    else
		return MaterialPhysicData(0, 0);
}


#endif // #ifndef DISABLE_PHYSICS

/////////////////////////////////////////////////////////////////////////////
//
// CreateGamePhysics 
//   The free function that creates an object that implements the IGamePhysics interface.
//
IGamePhysics *CreateGamePhysics()
{
	std::auto_ptr<IGamePhysics> gamePhysics;
	gamePhysics.reset( CHG_NEW BulletPhysics );

	if (gamePhysics.get() && !gamePhysics->VInitialize())
	{
		// physics failed to initialize.  delete it.
		gamePhysics.reset();
	}

	return gamePhysics.release();
}

IGamePhysics *CreateNullPhysics()
{
	std::auto_ptr<IGamePhysics> gamePhysics;
	gamePhysics.reset( CHG_NEW NullPhysics );
	if (gamePhysics.get() && !gamePhysics->VInitialize())
	{
		// physics failed to initialize.  delete it.
		gamePhysics.reset();
	}

	return gamePhysics.release();
}
