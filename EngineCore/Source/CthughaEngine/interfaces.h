#pragma once

class Entity;
class EntityComponent;
class ComponentResource;

typedef unsigned int EntityId;
typedef unsigned int ComponentId;

typedef unsigned int UIFunctionId;

const EntityId INVALID_ENTITY_ID = 0;
const ComponentId INVALID_COMPONENT_ID = 0;

const UIFunctionId INVALID_UIFUNCTION_ID = 0;

typedef shared_ptr<Entity> StrongEntityPtr;
typedef weak_ptr<Entity> WeakEntityPtr;
typedef shared_ptr<EntityComponent> StrongEntityComponentPtr;
typedef weak_ptr<EntityComponent> WeakEntityComponentPtr;

template<class T>
struct SortBy_SharedPtr_Content
{
    bool operator()(const shared_ptr<T> &lhs, const shared_ptr<T> &rhs) const
        { return *lhs < *rhs; }
};

//
// class IScreenElement							
//
class IScreenElement
{
public:
	virtual bool VOnRestore() = 0;
	virtual bool VOnLostDevice() = 0;
	virtual bool VOnRender(double fTime, float fElapsedTime) = 0;
	virtual void VOnUpdate(int deltaMilliseconds) = 0;

	virtual int VGetZOrder() const = 0;
	virtual void VSetZOrder(int const zOrder) = 0;
	virtual bool VIsVisible() const = 0;
	virtual void VSetVisible(bool visible) = 0;

	virtual bool VOnMsgProc( CHG_Event msg )=0;

	virtual ~IScreenElement() { };
	virtual bool const operator <(IScreenElement const &other) { return VGetZOrder() < other.VGetZOrder(); }
};


class IGamePhysics;

class IGameLogic
{
public:
	virtual WeakEntityPtr VGetEntity(const EntityId id)=0;
	virtual StrongEntityPtr VCreateEntity(const std::string &EntityResource, std::string sName, std::vector<ComponentResource> extraComponents, const Mat4x4 *initialTransform = NULL, const EntityId serversEntityId = INVALID_ENTITY_ID) = 0;
    virtual StrongEntityPtr VCreateEntity(const std::string &EntityResource, TiXmlElement *overrides, const Mat4x4 *initialTransform=NULL, const EntityId serversEntityId=INVALID_ENTITY_ID)=0;
	virtual StrongEntityPtr VCreateStaticEntity(TiXmlElement *overrides, const Mat4x4 *initialTransform=NULL, const EntityId serversEntityId=INVALID_ENTITY_ID)=0;
    virtual void VDestroyEntity(const EntityId EntityId)=0;
	virtual bool VLoadGame(const char* levelResource)=0;
	//virtual void VSetProxy()=0;				
	virtual void VOnUpdate(float time, float elapsedTime)=0;
	virtual void VChangeState(enum BaseGameState newState)=0;
	virtual void VMoveEntity(const EntityId id, Mat4x4 const &mat)=0;
	virtual shared_ptr<IGamePhysics> VGetGamePhysics(void) = 0;
};

enum GameViewType
{
	GameView_Human,
	GameView_Remote,
	GameView_AI,
	GameView_Recorder,
	GameView_Other
};

typedef unsigned int GameViewId;
extern const GameViewId gc_InvalidGameViewId;

class IGameView 
{
public:
	virtual bool VOnRestore()=0;
	virtual void VOnRender(double fTime, float fElapsedTime)=0;
	virtual bool VOnLostDevice()=0;
	virtual GameViewType VGetType()=0;
	virtual GameViewId VGetId() const=0;
	virtual void VOnAttach(GameViewId vid, EntityId aid)=0;

	virtual bool VOnMsgProc( CHG_Event msg )=0;
	virtual void VOnUpdate(unsigned long deltaMs)=0;

	virtual ~IGameView() { };
};



typedef std::list<shared_ptr<IScreenElement> > ScreenElementList;
typedef std::list<shared_ptr<IGameView> > GameViewList;


////////////////////////////////////////////////////
//
// IKeyboardHandler Description		- Chapter 9, page 242
// IPointerHandler Description		- Chapter 9, page 242
// IJoystickHandler Description		- Chapter 9, page 242
// IGamepadHandler Description		- Chapter 9, page 242
//
// These are the public APIs for any object that implements reactions
// to events sent by hardware user interface devices.
//
// Note: IJoystickHandler and IGamepadHandler are not currently
//       implemented anywhere in the codebase. They are here
//       as examples, and could require modification to actually work!
//
////////////////////////////////////////////////////

class IKeyboardHandler
{
public:
	virtual bool VOnKeyDown(const BYTE c)=0;
	virtual bool VOnKeyUp(const BYTE c)=0;
};

class IPointerHandler
{
public:
	virtual bool VOnPointerMove(const int &posX, const int &posY, const int &relPosX, const int &relPosY, const int radius) = 0;
	virtual bool VOnPointerButtonDown(const int &posX, const int &posY, const int &relPosX, const int &relPosY, const int radius, const CHG_MouseButton &mouseButton) = 0;
	virtual bool VOnPointerButtonUp(const int &posX, const int &posY, const int &relPosX, const int &relPosY, const int radius, const CHG_MouseButton &mouseButton) = 0;
};

class IJoystickHandler
{
	virtual bool VOnButtonDown(const std::string &buttonName, int const pressure)=0;
	virtual bool VOnButtonUp(const std::string &buttonName)=0;
	virtual bool VOnJoystick(float const x, float const y)=0;
};

class IGamepadHandler
{
	virtual bool VOnTrigger(const std::string &triggerName, float const pressure)=0;
	virtual bool VOnButtonDown(const std::string &buttonName, int const pressure)=0;
	virtual bool VOnButtonUp(const std::string &buttonName)=0;
	virtual bool VOnDirectionalPad(const std::string &direction)=0;
	virtual bool VOnThumbstick(const std::string &stickName, float const x, float const y)=0;
};

////////////////////////////////////////////////////
//
// class IResourceFile						
// The core of a resource cache system
// 
////////////////////////////////////////////////////

class Resource;
class IResourceFile;
class ResHandle;

//
// class IResourceLoader					
//
class IResourceLoader
{
public:
	virtual std::string VGetPattern()=0;
	virtual bool VUseRawFile()=0;
	virtual bool VDiscardRawBufferAfterLoad()=0;
    virtual bool VAddNullZero() { return false; }
	virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize)=0;
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)=0;
};

class IResourceFile
{
public:
	virtual bool VOpen()=0;
	virtual int VGetRawResourceSize(const Resource &r)=0;
	virtual int VGetRawResource(const Resource &r, char *buffer)=0;
	virtual int VGetNumResources() const = 0;
	virtual std::string VGetResourceName(int num) const = 0;
    virtual bool VIsUsingDevelopmentDirectories(void) const = 0;
	virtual ~IResourceFile() { }
};

class Scene;
class SceneNodeProperties;
class RayCast;
class LightNode;

typedef std::list<shared_ptr<LightNode> > Lights;


class IRenderState
{
public:
	virtual std::string VToString()=0;
};

/////////////////////////////////////////////////////////////////////////////
// class ISceneNode					
//
//   This is the public interface for nodes in a 3D scene graph.
/////////////////////////////////////////////////////////////////////////////

enum SceneNodeType { FLUID_SIMULATOR_NODE, OCTREE_NODE, LIGHT_NODE, MESH_NODE, DECAL_NODE, SHADER_NODE, MATERIAL_NODE, CAMERA_NODE, GENERIC_NODE };

class ISceneNode
{
public:
	virtual Aabb VGetAabb() = 0;

	virtual bool VOnUpdate(Scene *pScene, DWORD const elapsedMs)=0;
	virtual bool VOnRestore(Scene *pScene)=0;

	virtual void VSetTransform(const Mat4x4 *toWorld, const Mat4x4 *fromWorld=NULL) = 0;
	virtual bool VIsVisible(Scene *pScene) const=0;
	virtual bool VIsInRenderQueue() const = 0;
	virtual bool VIsStaticNode() const = 0;
	
	virtual bool VAddChild(shared_ptr<ISceneNode> kid)=0;
	virtual bool VRemoveChild(EntityId id)=0;
	virtual void VRemoveParent(EntityId id)=0;
	virtual bool VOnLostDevice(Scene *pScene)=0;
    virtual bool VOnRender(Scene *pScene)=0;
	//virtual bool VPick(Scene *pScene, RayCast *pRayCast)=0;

	virtual void VSetVisible(bool bVisible) = 0;

	virtual SceneNodeType VGetType()=0;
	
	virtual ~ISceneNode() { };
};

////////////////////////////////////////////////////////////////////////////
//
//	struct TestCollisionAnswer
//
////////////////////////////////////////////////////////////////////////////
struct TestCollisionAnswer
{
	bool hasHit;
	Vec3 hitPointWorld;
	Vec3 hitNormalPosition;
	float closedHitFraction;
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// class IGamePhysics							
//
//   The interface defintion for a generic physics API.
/////////////////////////////////////////////////////////////////////////////

class IGamePhysics
{
public:

	// Initialiazation and Maintenance of the Physics World
	virtual bool VInitialize()=0;
	virtual void VSyncVisibleScene() = 0;
	virtual void VOnUpdate( float deltaSeconds ) = 0; 

	// Initialization of Physics Objects
	virtual void VAddSphere(float radius, WeakEntityPtr entity, /*const Mat4x4& initialTransform, */const std::string& densityStr, const std::string& physicsMaterial, const Vec3& rotateConstrains, const Vec3& translateConstrains, const Vec3& shapeOffset)=0;
	virtual void VAddBox(const Vec3& dimensions, WeakEntityPtr gameEntity, /*const Mat4x4& initialTransform, */ const std::string& densityStr, const std::string& physicsMaterial, const Vec3& rotateConstrains, const Vec3& translateConstrains, const Vec3& shapeOffset) = 0;
	virtual void VAddCapsule(float radius, float height, WeakEntityPtr entity, const std::string& densityStr, const std::string& physicsMaterial, const Vec3& rotateConstrains, const Vec3& translateConstrains, const Vec3& shapeOffset) = 0;
	virtual void VAddConvexHull(Vec3 *verts, int numPoints, WeakEntityPtr gameEntity, /*const Mat4x4& initialTransform, */ const std::string& densityStr, const std::string& physicsMaterial, const Vec3& rotateConstrains, const Vec3& translateConstrains)=0;
	virtual void VAddTriangleMesh(float *verts, int num_Vertex, UINT *index, int num_Triangles, WeakEntityPtr gameEntity, /*const Mat4x4& initialTransform, */ const std::string& densityStr, const std::string& physicsMaterial, const Vec3& rotateConstrains, const Vec3& translateConstrains) = 0;
	virtual void VAddHeightField(const float* sHeightmapResource, int iWidth, int iDepth, float fMaxHeight, float fMinHeight, std::string sUpAxis, Vec2 vWorldDimension, WeakEntityPtr gameEntity, const std::string& densityStr, const std::string& physicsMaterial, const Vec3& rotateConstrains, const Vec3& translateConstrains) = 0;
	virtual void VAddGhostSphere(float radius, WeakEntityPtr entity) = 0;
	virtual void VAddGhostBox(const Vec3& dimensions, WeakEntityPtr gameEntity) = 0;
	virtual void VAddGhostCapsule(float radius, float height, WeakEntityPtr entity) = 0;
	virtual void VAddGhostConvexHull(Vec3 *verts, int numPoints, WeakEntityPtr gameEntity) = 0;
	virtual void VRemoveEntity(EntityId id)=0;

	// Debugging
	virtual void VRenderDiagnostics() = 0;

	// Physics world modifiers
	virtual void VCreateTrigger(WeakEntityPtr pGameEntity, const Vec3 &pos, const float dim)=0;
	virtual void VApplyForce(const Vec3 &dir, float newtons, EntityId aid)=0;
	virtual void VApplyTorque(const Vec3 &dir, float newtons, EntityId aid)=0;
	virtual bool VKinematicMove(const Mat4x4 &mat, EntityId aid)=0;
	
	// Ghost Object Collision Test
	virtual bool VCollisionConvexTest(EntityId id, const Mat4x4& start, const Mat4x4& end, const Vec3& dir, float minSlopeDot, TestCollisionAnswer& testCollisionAnswer) = 0;

	// Physics Entity states
	virtual void VRotateY(EntityId entityId, float angleRadians, float time) = 0;
	virtual float VGetOrientationY(EntityId entityId) = 0;
	virtual void VStopEntity(EntityId entityId) = 0;
    virtual Vec3 VGetVelocity(EntityId entityId) = 0;
	virtual void VSetVelocity(EntityId entityId, const Vec3& vel) = 0;
	virtual void VSetVelocityFactor(EntityId actorId, const Vec3& velFactor) = 0;
    virtual Vec3 VGetAngularVelocity(EntityId entityId) = 0;
    virtual void VSetAngularVelocity(EntityId entityId, const Vec3& vel) = 0;
	virtual void VTranslate(EntityId entityId, const Vec3& vec) = 0;

    virtual void VSetTransform(const EntityId id, const Mat4x4& mat) = 0;
    virtual Mat4x4 VGetTransform(const EntityId id) = 0;

	virtual ~IGamePhysics() { };
};


/////////////////////////////////////////////////////////////////////////////
// IScriptManager Description
//
//   This is the public interface for the scripting system.
/////////////////////////////////////////////////////////////////////////////
class IScriptManager
{
public:
    virtual ~IScriptManager(void) {}
    virtual bool VInit(void) = 0;
    virtual void VExecuteFile(const char* resource) = 0;
    virtual void VExecuteString(const char* str) = 0;
};