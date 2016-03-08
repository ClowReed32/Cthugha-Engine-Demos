//========================================================================
// Light.h - Defines the Light class
//
//========================================================================
#ifndef LIGHT_H_
#define LIGHT_H_

#include "Renderer.h"
#include "Scene.h"
#include "SceneNodes.h"

#define MAX_POINT_LIGHTS 256
#define MAX_DIRECTIONAL_LIGHTS 32

#define COMPUTE_SHADER_TILE_GROUP_DIM 16

enum LightType {DIRECTIONAL_LIGHT, POINT_LIGHT, SPOT_LIGHT};

class LightNode;

//struct FrustumPartition////////////////////////////////////////////
struct FrustumPartition
{
	float fBeginPartiton;
	float fEndPartition;
	Vec2 unused;	//ssbo alignment
	Vec4 scale;
	Vec4 bias;
};

struct FrustumPartitionUint
{
	UINT fBeginPartiton;
	UINT fEndPartition;
	Vec2 unused;	//ssbo alignment
	Vec4 scale;
	Vec4 bias;
};

//class Light////////////////////////////////////////////////////////

class Light
{
protected:
	Vec4 m_LightColor;
	float m_LightIntensitive;
	Vec4 m_LightPosition;	//Light Position in world coordinates. In Directional Light is the center of shadow mapping

	bool m_GenerateShadow;
	bool m_bGenerateVolumetricShadow;
	AtlasTextureID m_ShadowAtlas;	//In Atlas find all shadow Texture.
	Mat4x4 m_ViewToProjLightMatrix;
	Frustum m_LightFrustum;

	LightType m_Type;

	friend class LightRenderComponent;
	friend class LightNode;
	friend class ShadowMapManager;

public:
	Light()
	{
		m_LightPosition = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	//Get Functions
	Vec4 getLightColor() {return m_LightColor;}
	float getLightIntensitive() {return m_LightIntensitive;}
	Vec4 getLightPosition() { return m_LightPosition; }

	bool getGenerateShadow() {return m_GenerateShadow;}
	bool getGenerateVolumetricShadow() { return m_bGenerateVolumetricShadow; }
	AtlasTextureID getShadowAtlas() {return m_ShadowAtlas;}
	Frustum getLightFrustum() { return m_LightFrustum; }
    Mat4x4 getViewToProjLightMatrix() { return m_ViewToProjLightMatrix; }

	LightType getType() {return m_Type;}

    void SetLightIntensitive(float fIntensitive) { m_LightIntensitive = fIntensitive; }
	void SetLightColor(Vec4 vLightColor) { m_LightColor = vLightColor; }
};

//class DirectionalLight////////////////////////////////////////////////////////

class DirectionalLight : public Light
{
private:
	Vec4 m_LightDirection;
	float m_fMaxRadius;
	float m_fMaxLength;

	Mat4x4 m_mLightDirMat;

    Vec2 m_ShadowMapPos;
    float m_ShadowMapRes;

	UINT m_NumFrustumPartition;
	ObjectBufferID m_FrustumPartitionBuffer;

	Mat4x4 m_ViewLightMat;
	unsigned long long m_QueryResult;

    friend class LightRenderComponent;
	friend class ShadowMapManager;
	friend class LightNode;

public:
	DirectionalLight()
	{
		m_Type = DIRECTIONAL_LIGHT;
	}

	//Get Functions
	Vec4 getLightDirection() { return m_LightDirection; }
	Vec2 getShadowPos() {  return m_ShadowMapPos; }
	ObjectBufferID getFrustumBufferID() { return m_FrustumPartitionBuffer; };
    float getShadowMapRes() { return m_ShadowMapRes; }
	Mat4x4 getViewLightMatrix() { return m_ViewLightMat; }
	float GetMaxRadius() { return m_fMaxRadius; }
	float GetMaxLength() { return m_fMaxLength; }
	Mat4x4 GetLightDirectionMatrix() { return m_mLightDirMat; }

    void SetLightDirection(Vec3 vDir) { m_LightDirection = Vec4(vDir, 0.0f); }
};

//class PointLight////////////////////////////////////////////////////////

class PointLight : public Light
{
private:
	float m_LightRadius;

    //ShadowMap Attributes
	Vec2 m_ShadowMapPos[6];
    float m_ShadowMapRes[6];

	unsigned long long m_QueryResult[6];

    friend class LightRenderComponent;
	friend class ShadowMapManager;
	friend class LightNode;

public:
	PointLight()
	{
		m_Type = POINT_LIGHT;
	}

	//Get Functions
    float getLightRadius() { return m_LightRadius; }
	Vec2 getShadowPos(int index) {return m_ShadowMapPos[index];}
	float getShadowRes(int index) {return m_ShadowMapRes[index];}
};

//class SpotLight////////////////////////////////////////////////////////

class SpotLight : public Light
{
private:
	Vec4 m_LightDirection;
	float m_LightRadius;
	float m_thetaAngle;	//Shadow Amplitude
	float m_phiAngle; //Penumbre Amplitude
	
    Vec2 m_ShadowMapPos;
    float m_ShadowMapRes;

	Mat4x4 m_ViewLightMat;
    unsigned long long m_QueryResult;

    friend class LightRenderComponent;
	friend class ShadowMapManager;
    friend class LightNode;

public:
	SpotLight()
	{
		m_Type = SPOT_LIGHT;
	}

	//Get Functions
    float getLightRadius() { return m_LightRadius; }
	Vec4 getLightDirection() {return m_LightDirection;}
	float getThetaAngle() { return m_thetaAngle; }
	float getPhiAngle() { return m_phiAngle; }
	Vec2 getShadowPos() {return m_ShadowMapPos;}
	Mat4x4 getViewLightMatrix() { return m_ViewLightMat; }
    float getShadowMapRes() { return m_ShadowMapRes; }
};

////////////////////////////////////////////////////
//
// class LightManager					
//
// Se encarga de empaquetar las luces y mandarlas al 
// Shader correspondiente
////////////////////////////////////////////////////

struct DirectionalLightShaderData
{
	Vec3 vColor;
	float fIntensitive;
	Vec3 vLightDirection;
	int iShadows;
	Vec3 vViewInitPosition;
	float fLength;
	Vec3 vPadding;
	float fMaxRadius;
};

struct PointSpotLightShaderData
{
	Vec3 vColor;
	float fIntensitive;
	Vec3 vViewPosition;
	float fRadius;
	int iShadows;
	float fTheta;
	float fPhi;
	int iType;
	Vec3 vLightDirection;
	int iUnused;
};

class LightManager
{
private:
	PointSpotLightShaderData m_SpotPointLights[MAX_POINT_LIGHTS];
	DirectionalLightShaderData m_DirectionalLights[MAX_DIRECTIONAL_LIGHTS];

	StructuredBufferID m_sbSpotPointLightBuffer;
	StructuredBufferID m_sbTiledLightIndices;

	int m_NumActiveSpotPointLights;
	int m_NumActiveDirectionalLights;

	UINT m_uNumWidthTiles;
	UINT m_uNumHeightTiles;

public:
	LightManager();

	void UpdateLightBuffers(Scene* pScene);
	void UpdateLightBuffers(Scene* pScene, std::list<std::shared_ptr<SceneNode>> LightList);
	void SetLightBuffers(Scene* pScene);

	StructuredBufferID GetTiledLightIndices() { return m_sbTiledLightIndices; }
	UINT GetNumWidthTiles() { return m_uNumWidthTiles; }
	UINT GetNumHeightTiles() { return m_uNumHeightTiles; }
};

////////////////////////////////////////////////////
//
// class LightNode					
//
////////////////////////////////////////////////////

class LightNode : public SceneNode
{
private:
	WeakBaseRenderComponentPtr m_pRenderComponent;
	shared_ptr<Light> m_pLight;

	friend class ShadowMapManager;
	friend class LightManager;

public:
	LightNode(const EntityId entityId, 
		WeakBaseRenderComponentPtr renderComponent,
		shared_ptr<Light> light, const Mat4x4 *world);

	~LightNode();

	virtual bool VOnRestore(Scene *pScene);
	virtual bool VOnLostDevice(Scene *pScene) { return true; }
	virtual bool VIsVisible(Scene *pScene) const { return this->m_IsVisible; }
    virtual bool VOnRender(Scene *pScene);

	virtual SceneNodeType VGetType() { return LIGHT_NODE; }

	bool ActiveLight();
	const shared_ptr<Light> GetLightData() { return m_pLight; }
};

#endif