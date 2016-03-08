#pragma once
//========================================================================
// PhysicsComponent.h - Component for physical movement and collision detection
//
//========================================================================

#include "EntityComponent.h"

class TiXmlElement;

class ShapeProperties
{
public:
	ShapeProperties() { m_ShapeType = "NullType"; m_bGhostObject = false; m_Offset = Vec3(0.0f, 0.0f, 0.0f); }
	std::string GetShapeType() { return m_ShapeType; }
	bool IsGhost() { return m_bGhostObject; }
	void SetGhostObject(bool bIsGhost) { m_bGhostObject = bIsGhost; }
	void SetOffset(Vec3 offset) { m_Offset = offset; }
	Vec3 GetOffset() { return m_Offset; }

protected:
	std::string m_ShapeType;
	bool m_bGhostObject;
	Vec3 m_Offset;
};

class SphereProperties : public ShapeProperties
{
public:
	SphereProperties() { m_ShapeType = "Sphere"; }
	SphereProperties(float radius) { m_ShapeType = "Sphere"; m_Radius = radius; }

	float GetRadius() { return m_Radius; }

private:
	float m_Radius;
};

class BoxProperties : public ShapeProperties
{
public:
	BoxProperties() { m_ShapeType = "Box"; }
	BoxProperties(float width, float height, float depth) { m_ShapeType = "Box"; m_Width = width; m_Height = height; m_Depth = depth; }

	Vec3 GetBoxDimension() { return Vec3(m_Width, m_Height, m_Depth); }

private:
	float m_Width;
	float m_Height;
	float m_Depth;
};

class CapsuleProperties : public ShapeProperties
{
public:
	CapsuleProperties() { m_ShapeType = "Capsule"; }
	CapsuleProperties(float radius, float height) { m_ShapeType = "Capsule"; m_Radius = radius; m_Height = height; }

	float GetRadius() { return m_Radius; }
	float GetHeight() { return m_Height; }

private:
	float m_Radius;
	float m_Height;
};

class ConvexHullProperties : public ShapeProperties
{
public:
	ConvexHullProperties() { m_ShapeType = "ConvexHull"; }
	ConvexHullProperties(std::string collisionMeshFilename);
	~ConvexHullProperties();

	std::shared_ptr<Vec3> GetVertexList() { return m_pVertex; }
	UINT GetNumVertex() { return m_NumVertex; }

private:
	std::shared_ptr<Vec3> m_pVertex;
	UINT m_NumVertex;
};

class TriangleMeshProperties : public ShapeProperties
{
public:
	TriangleMeshProperties() { m_ShapeType = "TriangleMesh"; }
	TriangleMeshProperties(std::string collisionMeshFilename);
	~TriangleMeshProperties();

	std::shared_ptr<Vec3> GetVertexList() { return m_pVertex; }
	std::shared_ptr<UINT> GetIndexList() { return m_pIndex; }
	UINT GetNumVertex() { return m_NumVertex; }
	UINT GetNumIndex() { return m_NumIndex; }

private:
	std::shared_ptr<Vec3> m_pVertex;
	std::shared_ptr<UINT> m_pIndex;
	UINT m_NumVertex;
	UINT m_NumIndex;
};

class HeightFieldProperties : public ShapeProperties
{
public:
	HeightFieldProperties() { m_ShapeType = "HeightField"; }
	HeightFieldProperties(std::string sHeightmapResource, int iWidth, int iDepth, float fMaxHeight, float fMinHeight, float fWorldWidth, float fWorldDepth, std::string sUpAxis, std::string sHeightMapValueType);
	~HeightFieldProperties();

	float* GetHeightmapRaw() { return m_pHeightMapRaw; }
	int GetWidth() { return m_iWidth; }
	int GetDepth() { return m_iDepth; }
	float GetWorldWidth() { return m_fWorldWidth; }
	float GetWorldDepth() { return m_fWorldDepth; }
	float GetMaxHeight() { return m_fMaxHeight; }
	float GetMinHeight() { return m_fMinHeight; }
	std::string GetUpAxis() { return m_sUpAxis; }
	std::string GetHeightMapValueType() { return m_sHeightMapValueType; }

private:
	float* m_pHeightMapRaw;
	int m_iWidth;
	int m_iDepth;
	float m_fWorldWidth;
	float m_fWorldDepth;
	float m_fMaxHeight;
	float m_fMinHeight;
	std::string m_sUpAxis;
	std::string m_sHeightMapValueType;
};

class PhysicsComponent : public EntityComponent
{
public:
	const static char *g_Name;
	virtual const char *VGetName() const override { return PhysicsComponent::g_Name; }

public:
    PhysicsComponent(void);
    virtual ~PhysicsComponent(void);
    virtual TiXmlElement* VGenerateXml(void) override;

    // ActorComponent interface
    virtual bool VInit(TiXmlElement* pData) override;
    virtual bool VInit(std::shared_ptr<ComponentResource> pData) override;
    virtual void VPostInit(void) override;
    virtual void VUpdate(int deltaMs) override;

    // Physics functions
    void ApplyForce(const Vec3& direction, float forceNewtons);
    void ApplyTorque(const Vec3& direction, float forceNewtons);
	bool KinematicMove(const Mat4x4& transform);

    // acceleration
	void ApplyAcceleration(const Vec3& acceleration);
    void RemoveAcceleration(void);
    void ApplyAngularAcceleration(float acceleration);
    void RemoveAngularAcceleration(void);

	//void RotateY(float angleRadians);
    Vec3 GetVelocity(void);
    void SetVelocity(const Vec3& velocity);
	void SetVelocityFactor(const Vec3& velocityFactor);
	Vec3 GetAngularVelocity(void);
	void SetAngularVelocity(const Vec3& velocity);
    void RotateY(float angleRadians);
    void SetPosition(float x, float y, float z);
    void Stop(void);

	//Ghost Object Test Collision
	bool CollisionConvexTest(const Mat4x4& start, const Mat4x4& end, const Vec3& dir, float minSlopeDot, TestCollisionAnswer& testCollisionAnswer);

protected:
    void CreateShape();
    void BuildRigidBodyTransform(TiXmlElement* pTransformElement);
	void BuildRigidBodyConstrains(TiXmlElement* pConstrainElement);
	void BuildRigidBodyShape(TiXmlElement* pShapeElement);
	void BuildRigidBodyTransform(std::shared_ptr<PhysicComponentResourceData> pTransformElement);
    void BuildRigidBodyConstrains(std::shared_ptr<PhysicComponentResourceData> pConstrainElement);
	void BuildRigidBodyShape(std::shared_ptr<PhysicComponentResourceData> pShapeElement);

    float m_angularAcceleration;
	Vec3 m_acceleration;
    float m_maxVelocity, m_maxAngularVelocity;

    std::string m_density;
    std::string m_material;

	std::shared_ptr<ShapeProperties> m_pShapeProperties;
	
	Vec3 m_RigidBodyLocation;		// this isn't world position! This is how the rigid body is offset from the position of the actor.
	Vec3 m_RigidBodyOrientation;	// ditto, orientation
	Vec3 m_RigidBodyScale;			// ditto, scale

	Vec3 m_RigidBodyRotateConstrains;
	Vec3 m_RigidBodyTranslateConstrains;
	
    shared_ptr<IGamePhysics> m_pGamePhysics;  // might be better as a weak ptr...
};


