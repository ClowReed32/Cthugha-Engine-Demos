#pragma once
//========================================================================
// Geometry.h : Collection of code for 3D math and 3D shapes
//
//========================================================================

const float  CHG_PI = 3.14159265358979f;
const float  CHG_2PI = 2 * CHG_PI;

#define RADIANS_TO_DEGREES(x) ((x) * 180.0f / CHG_PI)
#define DEGREES_TO_RADIANS(x) ((x) * CHG_PI / 180.0f)

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

#define POSITIVE_X 0
#define NEGATIVE_X 1
#define POSITIVE_Y 2
#define NEGATIVE_Y 3
#define POSITIVE_Z 4
#define NEGATIVE_Z 5

enum FrustumType {PERSPECTIVE_FRUSTUM, ORTHO_FRUSTUM};
enum PointInPlane {FRONT_PLANE, BACK_PLANE, IN_PLANE};

////////////////////////////////////////////////////
//
// Utility classes for vectors and matrices 
//
////////////////////////////////////////////////////

struct half 
{
	unsigned short sh;

	half(){};
    half(const float x);
	operator float () const;
};

/*class Vec2 : public D3DXVECTOR2
{
public:
	Vec2(D3DXVECTOR2 &v3) { x = v3.x; y = v3.y; }
	Vec2() : D3DXVECTOR2() { x = 0; y = 0; }
	Vec2(const float _x, const float _y) { x=_x; y=_y; }
    Vec2(const double _x, const double _y) { x = (float)_x; y = (float)_y; }
	//inline Vec2(const class Vec4 &v4);

	void operator += (const float s);
	void operator += (const Vec2 &v);
	void operator -= (const float s);
	void operator -= (const Vec2 &v);
	void operator *= (const float s);
	void operator *= (const Vec2 &v);
	void operator /= (const float s);
	void operator /= (const Vec2 &v);
};

inline Vec2 operator* (const Vec2 &u, const Vec2 &v)
{
	return Vec2(v.x * u.x, v.y * u.y);
}*/

class Vec2 
{
public:
	float x, y;

	Vec2(){x = y = 0.0f;}
	Vec2(const float ixy)
	{
		x = y = ixy;
	}
	Vec2(const float ix, const float iy)
	{
		x = ix;
		y = iy;
	}
    Vec2(std::string serial);

	//operator const float *() const { return &x; }
	operator float *() const { return (float *) &x; }
//	float &operator[](unsigned int index){ return *(((float *) &x) + index); }

	void operator += (const float s);
	void operator += (const Vec2 &v);
	void operator -= (const float s);
	void operator -= (const Vec2 &v);
	void operator *= (const float s);
	void operator *= (const Vec2 &v);
	void operator /= (const float s);
	void operator /= (const Vec2 &v);
};

Vec2 operator + (const Vec2 &u, const Vec2 &v);
Vec2 operator + (const Vec2 &v, const float s);
Vec2 operator + (const float s, const Vec2 &v);

Vec2 operator - (const Vec2 &u, const Vec2 &v);
Vec2 operator - (const Vec2 &v, const float s);
Vec2 operator - (const float s, const Vec2 &v);

Vec2 operator - (const Vec2 &v);

Vec2 operator * (const Vec2 &u, const Vec2 &v);
Vec2 operator * (const float s, const Vec2 &v);
Vec2 operator * (const Vec2 &v, const float s);

Vec2 operator / (const Vec2 &u, const Vec2 &v);
Vec2 operator / (const Vec2 &v, const float s);
Vec2 operator / (const float s, const Vec2 &v);

bool operator == (const Vec2 &u, const Vec2 &v);

////////////////////////////////////////////////////
//
// Vec3 Description
//
//
////////////////////////////////////////////////////

/*class Vec3 : public D3DXVECTOR3 
{
public:
	inline float Length() { return D3DXVec3Length(this); }
	inline Vec3 *Normalize() { return static_cast<Vec3 *>(D3DXVec3Normalize(this, this)); }
	inline float Dot(const Vec3 &b) { return D3DXVec3Dot(this, &b); }
	inline Vec3 Cross(const Vec3 &b) const;

	Vec3(D3DXVECTOR3 &v3) { x = v3.x; y = v3.y; z = v3.z; }
	Vec3() : D3DXVECTOR3() { x = 0; y = 0; z = 0; }
	Vec3(const float _x, const float _y, const float _z) { x=_x; y=_y; z=_z; }
    Vec3(const double _x, const double _y, const double _z) { x = (float)_x; y = (float)_y; z = (float)_z; }
	inline Vec3(const class Vec4 &v4);
};

inline Vec3 Vec3::Cross(const Vec3 &b) const
{
	Vec3 out;
	D3DXVec3Cross(&out, this, &b);
	return out;
}

inline Vec3 operator* (const Vec3 &u, const Vec3 &v)
{
	return Vec3(v.x * u.x, v.y * u.y, v.z * u.z);
}*/

class Vec3 
{
public:
	float x, y, z;

	Vec3(){ x = y = z = 0.0f; }
	Vec3(const float ixyz){
		x = y = z = ixyz;
	}
	Vec3(const float ix, const float iy, const float iz){
		x = ix;
		y = iy;
		z = iz;
	}
	Vec3(const Vec2 iv, const float iz){
		x = iv.x;
		y = iv.y;
		z = iz;
	}
	Vec3(const float ix, const Vec2 iv){
		x = ix;
		y = iv.x;
		z = iv.y;
	}
    Vec3(std::string serial);
	//operator const float *() const { return &x; }
	operator float *() const { return (float *) &x; }
//	float &operator[](unsigned int index){ return *(((float *) &x) + index); }

	Vec2 xy() const { return Vec2(x, y); }
	Vec2 yz() const { return Vec2(y, z); }
	Vec2 xz() const { return Vec2(x, z); }

	Vec3 operator += (const float s);
	Vec3 operator += (const Vec3 &v);
	Vec3 operator -= (const float s);
	Vec3 operator -= (const Vec3 &v);
	Vec3 operator *= (const float s);
	Vec3 operator *= (const Vec3 &v);
	Vec3 operator /= (const float s);
	Vec3 operator /= (const Vec3 &v);

	bool operator< (const Vec3 &v) const;
};

Vec3 operator + (const Vec3 &u, const Vec3 &v);
Vec3 operator + (const Vec3 &v, const float s);
Vec3 operator + (const float s, const Vec3 &v);

Vec3 operator - (const Vec3 &u, const Vec3 &v);
Vec3 operator - (const Vec3 &v, const float s);
Vec3 operator - (const float s, const Vec3 &v);

Vec3 operator - (const Vec3 &v);

Vec3 operator * (const Vec3 &u, const Vec3 &v);
Vec3 operator * (const float s, const Vec3 &v);
Vec3 operator * (const Vec3 &v, const float s);

Vec3 operator / (const Vec3 &u, const Vec3 &v);
Vec3 operator / (const Vec3 &v, const float s);
Vec3 operator / (const float s, const Vec3 &v);

bool operator == (const Vec3 &u, const Vec3 &v);


////////////////////////////////////////////////////
//
// Vec4 Description
//
//
////////////////////////////////////////////////////


/*class Vec4 : public D3DXVECTOR4
{
public:
	inline float Length() { return D3DXVec4Length(this); }
	inline Vec4 *Normalize() { return static_cast<Vec4 *>(D3DXVec4Normalize(this, this)); }
	// If you want the cross product, use Vec3::Cross
	inline float Dot(const Vec4 &b) { return D3DXVec4Dot(this, &b); }

	Vec4(D3DXVECTOR4 &v4)  { x = v4.x; y = v4.y; z = v4.z; w = v4.w; }
	Vec4() : D3DXVECTOR4() { }
	Vec4(const float _x, const float _y, const float _z, const float _w) { x=_x; y=_y; z=_z; w=_w; }
	Vec4(const Vec3 &v3) { x = v3.x; y = v3.y; z = v3.z; w = 1.0f; }

};

inline Vec4 operator* (const Vec4 &u, const Vec4 &v)
{
	return Vec4(v.x * u.x, v.y * u.y, v.z * u.z, v.w * u.w);
}

inline Vec3::Vec3(const Vec4 &v4) { x = v4.x; y = v4.y; z = v4.z; }*/

class Vec4 
{
public:

	union
	{
		struct
		{
			float x, y, z, w;
		};

		float data[4];
	};

	Vec4(){ x = y = z = w = 0.0f; }
	Vec4(const float ixyzw){
		x = y = z = w = ixyzw;
	}
	Vec4(const float ix, const float iy, const float iz, const float iw){
		x = ix;
		y = iy;
		z = iz;
		w = iw;
	}
	Vec4(const Vec2 iv, const float iz, const float iw){
		x = iv.x;
		y = iv.y;
		z = iz;
		w = iw;
	}
	Vec4(const float ix, const Vec2 iv, const float iw){
		x = ix;
		y = iv.x;
		z = iv.y;
		w = iw;
	}
	Vec4(const float ix, const float iy, const Vec2 iv){
		x = ix;
		y = iy;
		z = iv.x;
		w = iv.y;
	}
	Vec4(const Vec2 iv0, const Vec2 iv1){
		x = iv0.x;
		y = iv0.y;
		z = iv1.x;
		w = iv1.y;
	}
	Vec4(const Vec3 iv, const float iw){
		x = iv.x;
		y = iv.y;
		z = iv.z;
		w = iw;
	}
	Vec4(const float ix, const Vec3 iv){
		x = ix;
		y = iv.x;
		z = iv.y;
		w = iv.z;
	}
    Vec4(std::string serial);
	//operator const float *() const { return &x; }
	operator float *() const { return (float *) &x; }
//	float &operator[](unsigned int index){ return *(((float *) &x) + index); }

	std::string to_string();
	static bool is_Vec4(std::string serial);

	Vec2 xy() const { return Vec2(x, y); }
	Vec2 xz() const { return Vec2(x, z); }
	Vec2 xw() const { return Vec2(x, w); }
	Vec2 yz() const { return Vec2(y, z); }
	Vec2 yw() const { return Vec2(y, w); }
	Vec2 zw() const { return Vec2(z, w); }
	Vec3 xyz() const { return Vec3(x, y, z); }
	Vec3 yzw() const { return Vec3(y, z, w); }

	void operator += (const float s);
	void operator += (const Vec4 &v);
	void operator -= (const float s);
	void operator -= (const Vec4 &v);
	void operator *= (const float s);
	void operator *= (const Vec4 &v);
	void operator /= (const float s);
	void operator /= (const Vec4 &v);
};

Vec4 operator + (const Vec4 &u, const Vec4 &v);
Vec4 operator + (const Vec4 &v, const float s);
Vec4 operator + (const float s, const Vec4 &v);

Vec4 operator - (const Vec4 &u, const Vec4 &v);
Vec4 operator - (const Vec4 &v, const float s);
Vec4 operator - (const float s, const Vec4 &v);

Vec4 operator - (const Vec4 &v);

Vec4 operator * (const Vec4 &u, const Vec4 &v);
Vec4 operator * (const float s, const Vec4 &v);
Vec4 operator * (const Vec4 &v, const float s);

Vec4 operator / (const Vec4 &u, const Vec4 &v);
Vec4 operator / (const Vec4 &v, const float s);
Vec4 operator / (const float s, const Vec4 &v);

bool operator == (const Vec4 &u, const Vec4 &v);

//===============================================================================================================

float dot(const Vec2 &u, const Vec2 &v);
float dot(const Vec3 &u, const Vec3 &v);
float dot(const Vec4 &u, const Vec4 &v);

float lerp(const float u, const float v, const float x);
Vec2  lerp(const Vec2 &u, const Vec2 &v, const float x);
Vec3  lerp(const Vec3 &u, const Vec3 &v, const float x);
Vec4  lerp(const Vec4 &u, const Vec4 &v, const float x);
Vec2  lerp(const Vec2 &u, const Vec2 &v, const Vec2 &x);
Vec3  lerp(const Vec3 &u, const Vec3 &v, const Vec3 &x);
Vec4  lerp(const Vec4 &u, const Vec4 &v, const Vec4 &x);

float cerp(const float u0, const float u1, const float u2, const float u3, float x);
Vec2  cerp(const Vec2 &u0, const Vec2 &u1, const Vec2 &u2, const Vec2 &u3, float x);
Vec3  cerp(const Vec3 &u0, const Vec3 &u1, const Vec3 &u2, const Vec3 &u3, float x);
Vec4  cerp(const Vec4 &u0, const Vec4 &u1, const Vec4 &u2, const Vec4 &u3, float x);

float sign(const float v);
Vec2  sign(const Vec2 &v);
Vec3  sign(const Vec3 &v);
Vec4  sign(const Vec4 &v);

float clamp(const float v, const float c0, const float c1);
Vec2  clamp(const Vec2 &v, const float c0, const float c1);
Vec2  clamp(const Vec2 &v, const Vec2 &c0, const Vec2 &c1);
Vec3  clamp(const Vec3 &v, const float c0, const float c1);
Vec3  clamp(const Vec3 &v, const Vec3 &c0, const Vec3 &c1);
Vec4  clamp(const Vec4 &v, const float c0, const float c1);
Vec4  clamp(const Vec4 &v, const Vec4 &c0, const Vec4 &c1);

//#define clamp(x, lo, hi) max(min(x, hi), lo)
#define saturate(x) clamp(x, 0, 1)

Vec2 normalize(const Vec2 &v);
Vec3 normalize(const Vec3 &v);
Vec4 normalize(const Vec4 &v);

Vec2 fastNormalize(const Vec2 &v);
Vec3 fastNormalize(const Vec3 &v);
Vec4 fastNormalize(const Vec4 &v);

float length(const Vec2 &v);
float length(const Vec3 &v);
float length(const Vec4 &v);

float squaredLength(const Vec2 &v);
float squaredLength(const Vec3 &v);
float squaredLength(const Vec4 &v);

Vec3 reflect(const Vec3 &v, const Vec3 &normal);

float distance(const Vec2 &u, const Vec2 &v);
float distance(const Vec3 &u, const Vec3 &v);
float distance(const Vec4 &u, const Vec4 &v);

float planeDistance(const Vec3 &normal, const float offset, const Vec3 &point);
float planeDistance(const Vec4 &plane, const Vec3 &point);

float sCurve(const float t);

Vec3 cross(const Vec3 &u, const Vec3 &v);
Vec4 cross(const Vec4 &u, const Vec4 &v, const Vec4 &w);

Vec3 computeReflectionDirection (const Vec3& direction, const Vec3& normal);
Vec3 parallelComponent (const Vec3& direction, const Vec3& normal);
Vec3 perpindicularComponent (const Vec3& direction, const Vec3& normal);


float lineProjection(const Vec3 &line0, const Vec3 &line1, const Vec3 &point);

/*extern Vec3 g_Up;
extern Vec3 g_Right;
extern Vec3 g_Forward;

extern Vec4 g_Up4;
extern Vec4 g_Right4;
extern Vec4 g_Forward4;*/


////////////////////////////////////////////////////
//
// Vec3List Description
// Vec4List Description
//
//   An STL list of Vectors
//
////////////////////////////////////////////////////

typedef std::list<Vec3> Vec3List;
typedef std::list<Vec4> Vec4List;


////////////////////////////////////////////////////
//
// Quaternion Description
//
//
////////////////////////////////////////////////////




/*class Quaternion : public D3DXQUATERNION
{
public:

	// Modifiers
	void Normalize() { D3DXQuaternionNormalize(this, this); };
	void Slerp(const Quaternion &begin, const Quaternion &end, float cooef)
	{
		// performs spherical linear interpolation between begin & end 
		// NOTE: set cooef between 0.0f-1.0f
		D3DXQuaternionSlerp(this, &begin, &end, cooef);
	}

	// Accessors
	void GetAxisAngle(Vec3 &axis, float &angle) const
	{
		D3DXQuaternionToAxisAngle(this, &axis, &angle); 
	}

	// Initializers
	void Build(const class Mat4x4 &mat); 

	void BuildRotYawPitchRoll(
			const float yawRadians, 
			const float pitchRadians, 
			const float rollRadians)
	{
		D3DXQuaternionRotationYawPitchRoll(this, yawRadians, pitchRadians, rollRadians);
	}

	void BuildAxisAngle(const Vec3 &axis, const float radians)
	{
		D3DXQuaternionRotationAxis(this, &axis, radians);
	}
	
	Quaternion(D3DXQUATERNION &q) : D3DXQUATERNION(q) { }
	Quaternion() { }

	static const Quaternion g_Identity;
};

inline Quaternion operator * (const Quaternion &a, const Quaternion &b) 
{
	// for rotations, this is exactly like concatenating
	// matrices - the new quat represents rot A followed by rot B.
	Quaternion out;
	D3DXQuaternionMultiply(&out, &a, &b);
	return out;
}*/



////////////////////////////////////////////////////
//
// Mat4x4 Description
//
//
////////////////////////////////////////////////////

class Mat4x4 
{
public:

	Vec4 rows[4];

	static const Mat4x4 g_Identity;

	Mat4x4(){}
	Mat4x4(const Vec4 &row0, const Vec4 &row1, const Vec4 &row2, const Vec4 &row3)
	{
		rows[0] = row0;
		rows[1] = row1;
		rows[2] = row2;
		rows[3] = row3;
	}
	Mat4x4(const float m00, const float m01, const float m02, const float m03,
		 const float m10, const float m11, const float m12, const float m13,
		 const float m20, const float m21, const float m22, const float m23,
		 const float m30, const float m31, const float m32, const float m33)
	{
		rows[0] = Vec4(m00, m01, m02, m03);
		rows[1] = Vec4(m10, m11, m12, m13);
		rows[2] = Vec4(m20, m21, m22, m23);
		rows[3] = Vec4(m30, m31, m32, m33);
	}

	operator const float *() const { return (const float *) rows; }

    void operator *= (const float s);
    void operator /= (const float s);

	void translate(const Vec3 &v);
	float determinant() const;
	Mat4x4 inverse() const;
	Mat4x4 transformInverse() const;	//Only for Transform Matrix. More efficience

	// Modifiers
	inline void SetPosition(Vec3 const &pos);
	inline void SetPosition(Vec4 const &pos);
	inline void SetScale(Vec3 const &scale);

	// Accessors and Calculation Methods
	inline Vec3 GetPosition() const;
	inline Vec3 GetDirection() const;
	inline Vec3 GetUp() const;
	inline void GetRotationAngles(float &Yaw, float &Pitch, float &Roll) const;
};

inline void Mat4x4::SetPosition(Vec3 const &pos)
{
	rows[0][3] = pos.x;
	rows[1][3] = pos.y;
	rows[2][3] = pos.z;
	rows[3][3] = 1.0f;
}

inline void Mat4x4::SetPosition(Vec4 const &pos)
{
	rows[3][0] = pos.x;
	rows[3][1] = pos.y;
	rows[3][2] = pos.z;
	rows[3][3] = pos.w;
}

inline void Mat4x4::SetScale(Vec3 const &scale)
{
	rows[1][1] = scale.x;
	rows[2][2] = scale.y;
	rows[3][3] = scale.z;
}

inline Vec3 Mat4x4::GetUp() const
{
	return Vec3(rows[0].y, rows[1].y, rows[2].y);
}

inline Vec3 Mat4x4::GetPosition() const
{
	return Vec3(rows[0][3], rows[1][3], rows[2][3]);
}

inline Vec3 Mat4x4::GetDirection() const
{
	Vec3 forward = Vec3(rows[0].z, rows[1].z, rows[2].z);
	return forward;
}

inline void Mat4x4::GetRotationAngles(float &Yaw, float &Pitch, float &Roll) const
{
	if (rows[0].data[0] == 1.0f)
    {
		Yaw = atan2f(rows[0].data[2], rows[2].data[3]);
		Pitch = 0;
		Roll = 0;
    }
	else if (rows[0].data[0] == -1.0f)
    {
		Yaw = atan2f(rows[0].data[2], rows[2].data[3]);
		Pitch = 0;
		Roll = 0;
    }
	else 
    {
		Yaw = atan2(-rows[2].data[0],rows[0].data[0]);
		Pitch = asin(-rows[1].data[2]);
		Roll = atan2(-rows[1].data[2],rows[1].data[1]);
    }
}

Mat4x4 operator + (const Mat4x4 &m, const Mat4x4 &n);
Mat4x4 operator - (const Mat4x4 &m, const Mat4x4 &n);
Mat4x4 operator - (const Mat4x4 &m);

Mat4x4 operator * (const Mat4x4 &m, const Mat4x4 &n);
Vec4 operator * (const Mat4x4 &m, const Vec4 &v);
Mat4x4 operator * (const Mat4x4 &m, const float x);

Mat4x4 transpose(const Mat4x4 &m);
Mat4x4 operator ! (const Mat4x4 &m);

/* --------------------------------------------------------------------------------- */

Mat4x4 rotateX(const float angle);
Mat4x4 rotateY(const float angle);
Mat4x4 rotateZ(const float angle);
Mat4x4 rotateXY(const float angleX, const float angleY);
Mat4x4 rotateYX(const float angleX, const float angleY);
Mat4x4 rotateZXY(const float angleX, const float angleY, const float angleZ);
Mat4x4 rotateXYZ(const float angleX, const float angleY, const float angleZ);
Mat4x4 rotateAxis(const Vec3 axis, const float angle);
Mat4x4 translate(const Vec3 &v);
Mat4x4 translate(const float x, const float y, const float z);
Mat4x4 scale(const float x, const float y, const float z);
Mat4x4 scale(const Vec3 scale);
Mat4x4 perspectiveMatrix(const float fov, const float aspect, const float zNear, const float zFar);
Mat4x4 perspectiveMatrixRH(const float fov, const float aspect, const float zNear, const float zFar);
Mat4x4 perspectiveMatrixX(const float fov, const int width, const int height, const float zNear, const float zFar);
Mat4x4 perspectiveMatrixY(const float fov, const int width, const int height, const float zNear, const float zFar);
Mat4x4 orthoMatrixX(const float left, const float right, const float top, const float bottom, const float zNear, const float zFar);
Mat4x4 orthoMatrixX(const float width, const float height, const float zNear, const float zFar);
Mat4x4 viewMatrix(const Vec3 pos, const Vec3 look, const Vec3 up);
Mat4x4 toD3DProjection(const Mat4x4 &m);
Mat4x4 toGLProjection(const Mat4x4 &m);
Mat4x4 pegToFarPlane(const Mat4x4 &m);
Mat4x4 cubeViewMatrix(const unsigned int side);
Mat4x4 cubeProjectionMatrixGL(const float zNear, const float zFar);
Mat4x4 cubeProjectionMatrixD3D(const float zNear, const float zFar);

/*class Mat4x4 : public D3DXMATRIX
{
public:
	// Modifiers
	inline void SetPosition(Vec3 const &pos);
	inline void SetPosition(Vec4 const &pos);
	inline void SetScale(Vec3 const &scale);

	// Accessors and Calculation Methods
	inline Vec3 GetPosition() const;
	inline Vec3 GetDirection() const;
	inline Vec3 GetUp() const;
	inline Vec3 GetRight() const;
	inline Vec3 GetYawPitchRoll() const;
	inline Vec3 GetScale() const;
	inline Vec4 Xform(Vec4 &v) const;
	inline Vec3 Xform(Vec3 &v) const;
	inline Mat4x4 Inverse() const;

	Mat4x4(D3DXMATRIX &mat) { memcpy(&m, &mat.m, sizeof(mat.m)); }

	Mat4x4() : D3DXMATRIX() { }

	static const Mat4x4 g_Identity;

	// Initialization methods
	inline void BuildTranslation(const Vec3 &pos);
	inline void BuildTranslation(const float x, const float y, const float z );
	inline void BuildRotationX(const float radians) { D3DXMatrixRotationX(this, radians); }
	inline void BuildRotationY(const float radians) { D3DXMatrixRotationY(this, radians); }
	inline void BuildRotationZ(const float radians) { D3DXMatrixRotationZ(this, radians); }
	inline void BuildYawPitchRoll(const float yawRadians, const float pitchRadians, const float rollRadians)
		{ D3DXMatrixRotationYawPitchRoll(this, yawRadians, pitchRadians, rollRadians); }
	inline void BuildRotationQuat(const Quaternion &q) { D3DXMatrixRotationQuaternion(this, &q); }
	inline void BuildRotationLookAt(const Vec3 &eye, const Vec3 &at, const Vec3 &up) { D3DXMatrixLookAtRH(this, &eye, &at, &up); }
	inline void BuildScale(const float x, const float y, const float z );
};



inline void Mat4x4::SetPosition(Vec3 const &pos)
{
	m[3][0] = pos.x;
	m[3][1] = pos.y;
	m[3][2] = pos.z;
	m[3][3] = 1.0f;
}

inline void Mat4x4::SetPosition(Vec4 const &pos)
{
	m[3][0] = pos.x;
	m[3][1] = pos.y;
	m[3][2] = pos.z;
	m[3][3] = pos.w;
}

inline void Mat4x4::SetScale(Vec3 const &scale)
{
	m[1][1] = scale.x;
	m[2][2] = scale.y;
	m[3][3] = scale.z;
}


inline Vec3 Mat4x4::GetPosition() const
{
	return Vec3(m[3][0], m[3][1], m[3][2]);
}

inline Vec3 Mat4x4::GetDirection() const
{
	// Note - the following code can be used to double check the vector construction above.
	Mat4x4 justRot = *this;
	justRot.SetPosition(Vec3(0.f,0.f,0.f));
	Vec3 forward = justRot.Xform(g_Forward);
	return forward;
}

inline Vec3 Mat4x4::GetRight() const
{
	// Note - the following code can be used to double check the vector construction above.
	Mat4x4 justRot = *this;
	justRot.SetPosition(Vec3(0.f,0.f,0.f));
	Vec3 right = justRot.Xform(g_Right);
	return right;
}

inline Vec3 Mat4x4::GetUp() const
{
	// Note - the following code can be used to double check the vector construction above.
	Mat4x4 justRot = *this;
	justRot.SetPosition(Vec3(0.f,0.f,0.f));
	Vec3 up = justRot.Xform(g_Up);
	return up;
}




inline Vec3 Mat4x4::GetYawPitchRoll() const
{
   float yaw, pitch, roll;
	
   pitch = asin(-_32);

   double threshold = 0.001; // Hardcoded constant - burn him, he's a witch
   double test = cos(pitch);

   if(test > threshold) 
   {
      roll = atan2(_12, _22);
      yaw = atan2(_31, _33);
   }
   else 
   {
      roll = atan2(-_21, _11);
      yaw = 0.0;
   }

	return (Vec3(yaw, pitch, roll));
}


inline Vec3 Mat4x4::GetScale() const
{
	return Vec3(m[0][0], m[1][1], m[2][2]);
}

inline Vec4 Mat4x4::Xform(Vec4 &v) const
{
	Vec4 temp;
	D3DXVec4Transform(&temp, &v, this);
	return temp;
}

inline Vec3 Mat4x4::Xform(Vec3 &v) const
{
	Vec4 temp(v);
	Vec4 out;
	D3DXVec4Transform(&out, &temp, this);
	return Vec3(out.x, out.y, out.z);
}

inline Mat4x4 Mat4x4::Inverse() const
{
	Mat4x4 out;
	D3DXMatrixInverse(&out, NULL, this);
	return out;
}

inline void Mat4x4::BuildTranslation(const Vec3 &pos)
{
	*this = Mat4x4::g_Identity;
	m[3][0] = pos.x;
	m[3][1] = pos.y;
	m[3][2] = pos.z;
}

inline void Mat4x4::BuildTranslation(const float x, const float y, const float z )
{
	*this = Mat4x4::g_Identity;
	m[3][0] = x;
	m[3][1] = y;
	m[3][2] = z;
}

inline void Mat4x4::BuildScale(const float x, const float y, const float z )
{
	*this = Mat4x4::g_Identity;
	m[1][1] = x;
	m[2][2] = y;
	m[3][3] = z;
}


inline Mat4x4 operator * (const Mat4x4 &a, const Mat4x4 &b) 
{
	Mat4x4 out;
	D3DXMatrixMultiply(&out, &a, &b);

	return out;
}

inline void Quaternion::Build(const Mat4x4 &mat) 
{
	D3DXQuaternionRotationMatrix(this, &mat); 
}*/


////////////////////////////////////////////////////
//
// Plane Definition
//
////////////////////////////////////////////////////



/*class Plane : public D3DXPLANE
{
public:
	inline void Normalize();

	// normal faces away from you if you send in verts in counter clockwise order....
	inline void Init(const Vec3 &p0, const Vec3 &p1, const Vec3 &p2);
	bool Inside(const Vec3 &point, const float radius) const;
	bool Inside(const Vec3 &point) const;
};

inline void Plane::Normalize()
{
	float mag;
	mag = sqrt(a * a + b * b + c * c);
	a = a / mag;
	b = b / mag;
	c = c / mag;
	d = d / mag;
}

inline void Plane::Init(const Vec3 &p0, const Vec3 &p1, const Vec3 &p2)
{
	D3DXPlaneFromPoints(this, &p0, &p1, &p2);
	Normalize();
}



////////////////////////////////////////////////////
//
// Frustum Definition
//
////////////////////////////////////////////////////

class Frustum
{
public:
	enum Side { Near, Far, Top, Right, Bottom, Left, NumPlanes };

	Plane m_Planes[NumPlanes];	// planes of the frusum in camera space
	Vec3 m_NearClip[4];			// verts of the near clip plane in camera space
	Vec3 m_FarClip[4];			// verts of the far clip plane in camera space

	float m_Fov;				// field of view in radians
	float m_Aspect;				// aspect ratio - width divided by height
	float m_Near;				// near clipping distance
	float m_Far;				// far clipping distance

public:
	Frustum();

	bool Inside(const Vec3 &point) const;
	bool Inside(const Vec3 &point, const float radius) const;
	const Plane &Get(Side side) { return m_Planes[side]; }
	void SetFOV(float fov) { m_Fov=fov; Init(m_Fov, m_Aspect, m_Near, m_Far); }
	void SetAspect(float aspect) { m_Aspect=aspect; Init(m_Fov, m_Aspect, m_Near, m_Far); }
	void SetNear(float nearClip) { m_Near=nearClip; Init(m_Fov, m_Aspect, m_Near, m_Far); }
	void SetFar(float farClip) { m_Far=farClip; Init(m_Fov, m_Aspect, m_Near, m_Far); }
	void Init(const float fov, const float aspect, const float near, const float far);

	void Render();
};*/


inline Vec3 CalcVelocity(Vec3 const &pos0, Vec3 const &pos1, float time)
{
	// CalcVelocity - Chapter 15, page 526
	return (pos1 - pos0) / time;
}

inline Vec3 CalcAcceleration(Vec3 const &vel0, Vec3 const &vel1, float time)
{
	// CalcVelocity - Chapter 15, page 526
	return (vel1 - vel0) / time;
}

inline void HandleAccel(Vec3 &pos, Vec3 &vel, Vec3 &accel, float time)
{
	// CalcVelocity - Chapter 15, page 526
	vel += accel * time;
	pos += vel * time;
}

// converts Barycentric coordinates to world coordinates
// inputs are the 3 verts of the triangle, and the u,v barycentric coordinates
extern Vec3 BarycentricToVec3(Vec3 v0, Vec3 v1, Vec3 v2, float u, float v);

extern bool IntersectTriangle( const Vec3& orig, const Vec3& dir,
                        Vec3& v0, Vec3& v1, Vec3& v2,
                        FLOAT* t, FLOAT* u, FLOAT* v );

class Quaternion
{
public:
	union 
	{
		struct 
		{
			/**
			Holds the real component of the quaternion.
			*/
			float r;

			/**
			* Holds the first complex component of the
			* quaternion.
			*/
			float i;

			/**
			* Holds the second complex component of the
			* quaternion.
			*/
			float j;

			/**
			* Holds the third complex component of the
			* quaternion.
			*/
			float k;
		};

		/**
		* Holds the quaternion data in array form.
		*/
		float data[4];
	};

	// ... other Quaternion code as before ...

	/**
	* The default constructor creates a quaternion representing
	* a zero rotation.
	*/
	Quaternion() : r(1), i(0), j(0), k(0) {}

	/**
	* The explicit constructor creates a quaternion with the given
	* components.
	*
	* @param r The real component of the rigid body's orientation
	* quaternion.
	*
	* @param i The first complex component of the rigid body's
	* orientation quaternion.
	*
	* @param j The second complex component of the rigid body's
	* orientation quaternion.
	*
	* @param k The third complex component of the rigid body's
	* orientation quaternion.
	*
	* @note The given orientation does not need to be normalised,
	* and can be zero. This function will not alter the given
	* values, or normalise the quaternion. To normalise the
	* quaternion (and make a zero quaternion a legal rotation),
	* use the normalise function.
	*
	* @see normalise
	*/
	Quaternion(const float r, const float i, const float j, const float k) : r(r), i(i), j(j), k(k)
	{
	}

	/**
	* Normalises the quaternion to unit length, making it a valid
	* orientation quaternion.
	*/
	void normalise()
	{
		float d = r*r+i*i+j*j+k*k;

		// Check for zero length quaternion, and use the no-rotation
		// quaternion in that case.
		if (d < 1.19209290E-07F) 
		{
			r = 1;
			return;
		}

		d = ((float)1.0f)/sqrtf(d);
		r *= d;
		i *= d;
		j *= d;
		k *= d;
	}

	/**
	* Multiplies the quaternion by the given quaternion.
	*
	* @param multiplier The quaternion by which to multiply.
	*/
	void operator *=(const Quaternion &multiplier)
	{
		Quaternion q = *this;
		r = q.r*multiplier.r - q.i*multiplier.i -
		q.j*multiplier.j - q.k*multiplier.k;
		i = q.r*multiplier.i + q.i*multiplier.r +
		q.j*multiplier.k - q.k*multiplier.j;
		j = q.r*multiplier.j + q.j*multiplier.r +
		q.k*multiplier.i - q.i*multiplier.k;
		k = q.r*multiplier.k + q.k*multiplier.r +
		q.i*multiplier.j - q.j*multiplier.i;
	}

	/**
	* Adds the given vector to this, scaled by the given amount.
	* This is used to update the orientation quaternion by a rotation
	* and time.
	*
	* @param vector The vector to add.
	*
	* @param scale The amount of the vector to add.
	*/
	void addScaledVector(const Vec3& vector, float scale)
	{
		Quaternion q(0,
		vector.x * scale,
		vector.y * scale,
		vector.z * scale);
		q *= *this;
		r += q.r * ((float)0.5f);
		i += q.i * ((float)0.5f);
		j += q.j * ((float)0.5f);
		k += q.k * ((float)0.5f);
	}

	void rotateByVector(const Vec3& vector)
	{
		Quaternion q(0, vector.x, vector.y, vector.z);
		(*this) *= q;
	}
};

class Aabb 
{

public:
	Vec3 vcMin, vcMax; // extreme points
	Vec3 vcCenter; // center point
	bool isInfinite; // infinite AABB
    bool isVoid;
//———————————————————-
	Aabb(void)
	{
		isInfinite = false;
        isVoid = false;
		vcMin = Vec3();
		vcMax = Vec3();
		vcCenter = Vec3();
	}

	Aabb operator+ (const Vec3& tras)
	{
		Aabb res;
		res.vcMax = this->vcMax + tras;
		res.vcMin = this->vcMin + tras;
		res.vcCenter = this->vcCenter + tras;

		return res;
	};

    void SetFromVertexSet(const Vec3* pPoints, const int numPoints);
    Aabb Union(const Aabb aabb); //Generate the envolved aabb of two aabbs
    Aabb Union(Vec3 vMin, Vec3 vMax);
	Aabb Union(const Vec3 vPoint);

	bool Intersects(const Aabb &aabb) const;
	bool Intersects(const Vec3 &dir) const;

    Aabb Transform(const Mat4x4 &mat) const;
};

////////////////////////////////////////////////////
//
// Plane Definition
//
////////////////////////////////////////////////////

class Plane
{
public:
	Vec3 planeNormal;
	Vec3 planePoint;

	float distance;

	Plane()
	{
	}

	void Set(Vec3 planeNormal, Vec3 planePoint)
	{
		distance = - dot(planeNormal, planePoint);
		this->planeNormal = planeNormal;
		this->planePoint = planePoint;
	}

	void Set(Vec3 v0, Vec3 v1, Vec3 v2)
	{
		Vec3 Edge1 = v1 - v0;
		Vec3 Edge2 = v2 - v0;

		planeNormal = normalize(cross(Edge1, Edge2));
		planePoint = v0;
		distance = dot(planeNormal, planePoint);
	}

	float Distance(Vec3 v)
	{
		return (fabs( dot(planeNormal, v) - distance));
	}

	PointInPlane Classify(const Vec3 &v)
	{
        float f = dot(planeNormal, v) + distance;

		if(f > 0.00001) 
			return FRONT_PLANE;
		if(f < -0.00001) 
			return BACK_PLANE;
		
		return IN_PLANE;
	}

	//int Classify(const DarkPolygon &pol);
	bool Intersects(Vec3 v0, Vec3 v1, Vec3 v2);
	Vec3 Intersect(Plane plane1, Plane plane2);
	bool Inside(const Vec3 &point, const float radius) const;
	bool Inside(const Vec3 &point) const;
	//bool Intersects(DarkAabb aabb);
};

////////////////////////////////////////////////////
//
// Ray Definition
//
////////////////////////////////////////////////////

class Ray
{
public:
	Vec3 m_vcOrig, // origin
	m_vcDir; // direction

	Ray(void) { /* nothing to do */ ; }
	inline void Set(Vec3 vcOrig, Vec3 vcDir)
	{
		m_vcOrig = vcOrig;
		m_vcDir = vcDir;
	}
	void DeTransform(const Mat4x4 &m);
	
	// intersecting triangles
	bool Intersects(const Vec3 &vc0, const Vec3 &vc1, const Vec3 &vc2, bool bCull, float *t);
	bool Intersects(const Vec3 &vc0, const Vec3 &vc1, const Vec3 &vc2, bool bCull, float fL, float *t);
	
	// intersecting planes
	bool Intersects(const Plane &plane, bool bCull, float *t, Vec3 *vcHit) const;
	bool Intersects(const Plane &plane, bool bCull, float fL, float *t, Vec3 *vcHit) const;
	bool IntersectsWithoutOrigin(const Plane &plane, bool bCull, float *t, Vec3 *vcHit);
	
	// intersecting aabb
	bool Intersects(const Aabb &aabb, Vec3 *vcHit);

	// intersecting rays
	bool Intersects(const Ray& ray, float fL, Vec3 *vcHit);

	Vec3 closestPointOnLine(Vec3 p, float fLength);
};

////////////////////////////////////////////////////
//
// Frustum Definition
//
////////////////////////////////////////////////////

class Frustum
{
public:
	enum Side { Near, Far, Top, Right, Bottom, Left, NumPlanes };

	Plane m_Planes[NumPlanes];	// planes of the frusum in camera space
	Vec3 m_NearClip[4];			// verts of the near clip plane in camera space
	Vec3 m_FarClip[4];			// verts of the far clip plane in camera space

	float m_Fov;				// field of view in radians
	float m_Aspect;				// aspect ratio - width divided by height
	float m_Near;				// near clipping distance
	float m_Far;				// far clipping distance

	float m_Height;				// Orthografic attributes
	float m_Width;

	FrustumType m_Type;			// Perspective or Orthographic frustum

public:
	Frustum();

	bool Inside(const Vec3 &point) const;
	bool Inside(const Vec3 &point, const Mat4x4 &transform) const;
	bool Inside(const Vec3 &point, const float radius) const;
	bool Inside(const Aabb &aabb) const;
	bool Inside(const Aabb &aabb, const Mat4x4 &transform) const; //Transform frustum
	bool Inside(const Aabb &aabb, const Mat4x4 &frustumMat, const Mat4x4 &aabbMat) const;
	bool Inside(const Mat4x4 &transform1, const Frustum &frustum, const Mat4x4 &transform2) const;
    bool Intersects(const Mat4x4 &transform, const Ray &ray, const float length) const;
	const Plane &Get(Side side) { return m_Planes[side]; }
	void Init(const float fov, const float aspect, const float near, const float far);
	void Init(const float height, const float width, const float aspect, const float near, const float far);
	void SetFOV(float fov) { m_Fov=fov; Init(m_Fov, m_Aspect, m_Near, m_Far); }
	void SetAspect(float aspect) { m_Aspect=aspect; Init(m_Fov, m_Aspect, m_Near, m_Far); }
	void SetNear(float nearClip) { m_Near=nearClip; Init(m_Fov, m_Aspect, m_Near, m_Far); }
	void SetFar(float farClip) { m_Far=farClip; Init(m_Fov, m_Aspect, m_Near, m_Far); }
	void SetHeight(float height) { m_Height = height; Init(m_Height, m_Width, m_Aspect, m_Near, m_Far);}
	Aabb GetFrustumAabb();

	//void Render();
};

