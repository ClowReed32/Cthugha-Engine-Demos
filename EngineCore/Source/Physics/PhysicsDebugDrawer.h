#pragma once
//========================================================================
// PhysicsDebugDrawer.h - implements a physics debug drawer
//
//========================================================================


#include "btBulletDynamicsCommon.h"

//////////////////////////////////////////////////////////////////////////////
// class BulletDebugDrawer						
//
// Bullet uses this object to draw debug information.  This implementation
//   of btIDebugDraw uses direct3D lines to represent the current state
//   of the physics simulation 
//
class BulletDebugDrawer : public btIDebugDraw
{
	std::vector<Vec3> m_VertexPoints;
	std::vector<Color> m_ColorPoints;
	VertexBufferID m_LineVertexBuffer;
	VertexBufferID m_LineColorBuffer;

	ShaderID m_PhysicDebugRenderShader;
	VertexFormatID m_PhysicDebugRenderShaderVF;

	std::shared_ptr<CameraNode> m_pCameraNode;

	DebugDrawModes               m_DebugModes;
public:
	// btIDebugDraw interface
	bool			Init(UINT MaxVertex);
	virtual void	drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color) override;
	virtual void	reportErrorWarning(const char* warningString) override;
	virtual void	draw3dText(const btVector3& location,const char* textString) override;
	virtual void	setDebugMode(int debugMode) override;
	virtual int		getDebugMode() const override;
	virtual void	drawLine(const btVector3& from,const btVector3& to,const btVector3& color) override;
	void			RenderDebugPhysic();

	// Added post press to read PlayerOptions.xml to turn on physics debug options.
	void ReadOptions(void);
};


