//========================================================================
// Geometry.cpp : Collection of code for 3D math and 3D shapes
//
//========================================================================

#include "CthughaStd.h"
#include "Geometry.h"

//========================================================================


const Mat4x4 Mat4x4::g_Identity(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);

inline float rsqrtf(const float v)
{
	union {
		float vh;
		int i0;
	};

	union {
		float vr;
		int i1;
	};

	vh = v * 0.5f;
	i1 = 0x5f3759df - (i0 >> 1);
	return vr * (1.5f - vh * vr * vr);
}

half::half(const float x)
{
	union {
		float floatI;
		unsigned int i;
	};
	floatI = x;

//	unsigned int i = *((unsigned int *) &x);
	int e = ((i >> 23) & 0xFF) - 112;
	int m =  i & 0x007FFFFF;

	sh = (i >> 16) & 0x8000;
	if (e <= 0){
		// Denorm
		m = ((m | 0x00800000) >> (1 - e)) + 0x1000;
		sh |= (m >> 13);
	} else if (e == 143){
		sh |= 0x7C00;
		if (m != 0){
			// NAN
			m >>= 13;
			sh |= m | (m == 0);
		}
	} else {
		m += 0x1000;
		if (m & 0x00800000){
			// Mantissa overflow
			m = 0;
			e++;
		}
		if (e >= 31){
			// Exponent overflow
			sh |= 0x7C00;
		} else {
			sh |= (e << 10) | (m >> 13);
		}
	}
}

half::operator float () const 
{
	union {
		unsigned int s;
		float result;
	};

	s = (sh & 0x8000) << 16;
	unsigned int e = (sh >> 10) & 0x1F;
	unsigned int m = sh & 0x03FF;

	if (e == 0){
		// +/- 0
		if (m == 0) return result;

		// Denorm
		while ((m & 0x0400) == 0){
			m += m;
			e--;
		}
		e++;
		m &= ~0x0400;
	} else if (e == 31){
		// INF / NAN
		s |= 0x7F800000 | (m << 13);
		return result;
	}

	s |= ((e + 112) << 23) | (m << 13);

	return result;
}

/* --------------------------------------------------------------------------------- */

Vec2::Vec2(std::string serial)
{
    float aux[2];
    unsigned int init = 0, k = 0;

    for (UINT i = 0; i < serial.size(); i++)
    {
        if (serial[i] == ' ')
        {
            std::string auxNumber = serial;
            auxNumber.erase(auxNumber.begin(), auxNumber.begin() + init);
            auxNumber.erase(auxNumber.begin() + (i - init), auxNumber.end());
            aux[k] = (float)atof(auxNumber.c_str());
            k++;
            init = i + 1;
        }
    }

    std::string auxNumber = serial;
    auxNumber.erase(auxNumber.begin(), auxNumber.begin() + init);
    auxNumber.erase(auxNumber.begin() + (serial.size() - init), auxNumber.end());
    aux[k] = (float)atof(auxNumber.c_str());

    x = aux[0];
    y = aux[1];
}

void Vec2::operator += (const float s)
{
	x += s;
	y += s;
}

void Vec2::operator += (const Vec2 &v)
{
	x += v.x;
	y += v.y;
}

void Vec2::operator -= (const float s)
{
	x -= s;
	y -= s;
}

void Vec2::operator -= (const Vec2 &v)
{
	x -= v.x;
	y -= v.y;
}

void Vec2::operator *= (const float s)
{
	x *= s;
	y *= s;
}

void Vec2::operator *= (const Vec2 &v)
{
	x *= v.x;
	y *= v.y;
}

void Vec2::operator /= (const float s)
{
	x /= s;
	y /= s;
}

void Vec2::operator /= (const Vec2 &v)
{
	x /= v.x;
	y /= v.y;
}

Vec2 operator + (const Vec2 &u, const Vec2 &v)
{
	return Vec2(u.x + v.x, u.y + v.y);
}

Vec2 operator + (const Vec2 &v, const float s)
{
	return Vec2(v.x + s, v.y + s);
}

Vec2 operator + (const float s, const Vec2 &v)
{
	return Vec2(v.x + s, v.y + s);
}

Vec2 operator - (const Vec2 &u, const Vec2 &v)
{
	return Vec2(u.x - v.x, u.y - v.y);
}

Vec2 operator - (const Vec2 &v, const float s)
{
	return Vec2(v.x - s, v.y - s);
}

Vec2 operator - (const float s, const Vec2 &v)
{
	return Vec2(s - v.x, s - v.y);
}

Vec2 operator - (const Vec2 &v){
	return Vec2(-v.x, -v.y);
}

Vec2 operator * (const Vec2 &u, const Vec2 &v)
{
	return Vec2(u.x * v.x, u.y * v.y);
}

Vec2 operator * (const float s, const Vec2 &v)
{
	return Vec2(v.x * s, v.y * s);
}

Vec2 operator * (const Vec2 &v, const float s)
{
	return Vec2(v.x * s, v.y * s);
}

Vec2 operator / (const Vec2 &u, const Vec2 &v)
{
	return Vec2(u.x / v.x, u.y / v.y);
}

Vec2 operator / (const Vec2 &v, const float s)
{
	return Vec2(v.x / s, v.y / s);
}

Vec2 operator / (const float s, const Vec2 &v)
{
	return Vec2(s / v.x, s / v.y);
}

bool operator == (const Vec2 &u, const Vec2 &v)
{
	return (u.x == v.x && u.y == v.y);
}

/* --------------------------------------------------------------------------------- */
Vec3::Vec3(std::string serial)
{
    float aux[3];
	unsigned int init = 0, k = 0;

    for(UINT i=0; i < serial.size(); i++)
	{		
		if(serial[i] == ' ')
		{
			std::string auxNumber = serial;
			auxNumber.erase(auxNumber.begin(), auxNumber.begin() + init);
			auxNumber.erase(auxNumber.begin() + (i - init), auxNumber.end());
			aux[k] = (float)atof(auxNumber.c_str());
			k++;
			init = i + 1;
		}
	}

	std::string auxNumber = serial;
	auxNumber.erase(auxNumber.begin(), auxNumber.begin() + init);
	auxNumber.erase(auxNumber.begin() + (serial.size() - init), auxNumber.end());
	aux[k] = (float)atof(auxNumber.c_str());

    x = aux[0];
    y = aux[1];
    z = aux[2];
}

Vec3 Vec3::operator += (const float s)
{
	x += s;
	y += s;
	z += s;

	return *this;
}

Vec3 Vec3::operator += (const Vec3 &v)
{
	x += v.x;
	y += v.y;
	z += v.z;

	return *this;
}

Vec3 Vec3::operator -= (const float s)
{
	x -= s;
	y -= s;
	z -= s;

	return *this;
}

Vec3 Vec3::operator -= (const Vec3 &v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;

	return *this;
}

Vec3 Vec3::operator *= (const float s)
{
	x *= s;
	y *= s;
	z *= s;

	return *this;
}

Vec3 Vec3::operator *= (const Vec3 &v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;

	return *this;
}

Vec3 Vec3::operator /= (const float s)
{
	x /= s;
	y /= s;
	z /= s;

	return *this;
}

Vec3 Vec3::operator /= (const Vec3 &v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;

	return *this;
}

bool Vec3::operator< (const Vec3 &v) const
{
	if (x == v.x)
	{
		if (y == v.y)
		{
			return  z < v.z;
		}
		else
		{
			return y < v.y;
		}
	}

	return x < v.x;
}

Vec3 operator + (const Vec3 &u, const Vec3 &v)
{
	return Vec3(u.x + v.x, u.y + v.y, u.z + v.z);
}

Vec3 operator + (const Vec3 &v, const float s)
{
	return Vec3(v.x + s, v.y + s, v.z + s);
}

Vec3 operator + (const float s, const Vec3 &v)
{
	return Vec3(v.x + s, v.y + s, v.z + s);
}

Vec3 operator - (const Vec3 &u, const Vec3 &v)
{
	return Vec3(u.x - v.x, u.y - v.y, u.z - v.z);
}

Vec3 operator - (const Vec3 &v, const float s)
{
	return Vec3(v.x - s, v.y - s, v.z - s);
}

Vec3 operator - (const float s, const Vec3 &v)
{
	return Vec3(s - v.x, s - v.y, s - v.z);
}

Vec3 operator - (const Vec3 &v){
	return Vec3(-v.x, -v.y, -v.z);
}

Vec3 operator * (const Vec3 &u, const Vec3 &v)
{
	return Vec3(u.x * v.x, u.y * v.y, u.z * v.z);
}

Vec3 operator * (const float s, const Vec3 &v)
{
	return Vec3(v.x * s, v.y * s, v.z * s);
}

Vec3 operator * (const Vec3 &v, const float s)
{
	return Vec3(v.x * s, v.y * s, v.z * s);
}

Vec3 operator / (const Vec3 &u, const Vec3 &v)
{
	return Vec3(u.x / v.x, u.y / v.y, u.z / v.z);
}

Vec3 operator / (const Vec3 &v, const float s)
{
	return Vec3(v.x / s, v.y / s, v.z / s);
}

Vec3 operator / (const float s, const Vec3 &v)
{
	return Vec3(s / v.x, s / v.y, s / v.z);
}

bool operator == (const Vec3 &u, const Vec3 &v)
{
	return (u.x == v.x && u.y == v.y && u.z == v.z);
}

/* --------------------------------------------------------------------------------- */
Vec4::Vec4(std::string serial)
{
    float aux[4];
	unsigned int init = 0, k = 0;

	for(UINT i=0; i < serial.size(); i++)
	{		
		if(serial[i] == ' ')
		{
			std::string auxNumber = serial;
			auxNumber.erase(auxNumber.begin(), auxNumber.begin() + init);
			auxNumber.erase(auxNumber.begin() + (i - init), auxNumber.end());
			aux[k] = (float)atof(auxNumber.c_str());
			k++;
			init = i + 1;
		}
	}

	std::string auxNumber = serial;
	auxNumber.erase(auxNumber.begin(), auxNumber.begin() + init);
	auxNumber.erase(auxNumber.begin() + (serial.size() - init), auxNumber.end());
	aux[k] = (float)atof(auxNumber.c_str());

	x = aux[0];
    y = aux[1];
    z = aux[2];
    w = aux[3];
}

std::string Vec4::to_string()
{
	return std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z) + " " + std::to_string(w);
}

void Vec4::operator += (const float s)
{
	x += s;
	y += s;
	z += s;
	w += s;
}

void Vec4::operator += (const Vec4 &v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
}

void Vec4::operator -= (const float s)
{
	x -= s;
	y -= s;
	z -= s;
	w -= s;
}

void Vec4::operator -= (const Vec4 &v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
}

void Vec4::operator *= (const float s)
{
	x *= s;
	y *= s;
	z *= s;
	w *= s;
}

void Vec4::operator *= (const Vec4 &v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
	w *= v.w;
}

void Vec4::operator /= (const float s)
{
	x /= s;
	y /= s;
	z /= s;
	w /= s;
}

void Vec4::operator /= (const Vec4 &v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;
	w /= v.w;
}

bool Vec4::is_Vec4(std::string serial)
{
	if (serial[0] < '0' || serial[0] > '9')
		return false;

	return true;
}

Vec4 operator + (const Vec4 &u, const Vec4 &v)
{
	return Vec4(u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w);
}

Vec4 operator + (const Vec4 &v, const float s)
{
	return Vec4(v.x + s, v.y + s, v.z + s, v.w + s);
}

Vec4 operator + (const float s, const Vec4 &v)
{
	return Vec4(v.x + s, v.y + s, v.z + s, v.w + s);
}

Vec4 operator - (const Vec4 &u, const Vec4 &v)
{
	return Vec4(u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w);
}

Vec4 operator - (const Vec4 &v, const float s)
{
	return Vec4(v.x - s, v.y - s, v.z - s, v.w - s);
}

Vec4 operator - (const float s, const Vec4 &v)
{
	return Vec4(s - v.x, s - v.y, s - v.z, s - v.w);
}

Vec4 operator - (const Vec4 &v)
{
	return Vec4(-v.x, -v.y, -v.z, -v.w);
}

Vec4 operator * (const Vec4 &u, const Vec4 &v)
{
	return Vec4(u.x * v.x, u.y * v.y, u.z * v.z, u.w * v.w);
}

Vec4 operator * (const float s, const Vec4 &v)
{
	return Vec4(v.x * s, v.y * s, v.z * s, v.w * s);
}

Vec4 operator * (const Vec4 &v, const float s)
{
	return Vec4(v.x * s, v.y * s, v.z * s, v.w * s);
}

Vec4 operator / (const Vec4 &u, const Vec4 &v)
{
	return Vec4(u.x / v.x, u.y / v.y, u.z / v.z, u.w / v.w);
}

Vec4 operator / (const Vec4 &v, const float s)
{
	return Vec4(v.x / s, v.y / s, v.z / s, v.w / s);
}

Vec4 operator / (const float s, const Vec4 &v)
{
	return Vec4(s / v.x, s / v.y, s / v.z, s / v.w);
}

bool operator == (const Vec4 &u, const Vec4 &v)
{
	return (u.x == v.x && u.y == v.y && u.z == v.z && u.w && v.w);
}

/* --------------------------------------------------------------------------------- */

float dot(const Vec2 &u, const Vec2 &v)
{
	return u.x * v.x + u.y * v.y;
}

float dot(const Vec3 &u, const Vec3 &v)
{
	return u.x * v.x + u.y * v.y + u.z * v.z;
}

float dot(const Vec4 &u, const Vec4 &v)
{
	return u.x * v.x + u.y * v.y + u.z * v.z + u.w * v.w;
}

float lerp(const float u, const float v, const float x)
{
	return u + x * (v - u);
}

Vec2 lerp(const Vec2 &u, const Vec2 &v, const float x)
{
	return u + x * (v - u);
}

Vec3 lerp(const Vec3 &u, const Vec3 &v, const float x)
{
	return u + x * (v - u);
}

Vec4 lerp(const Vec4 &u, const Vec4 &v, const float x)
{
	return u + x * (v - u);
}

Vec2 lerp(const Vec2 &u, const Vec2 &v, const Vec2 &x)
{
	return u + x * (v - u);
}

Vec3 lerp(const Vec3 &u, const Vec3 &v, const Vec3 &x)
{
	return u + x * (v - u);
}

Vec4 lerp(const Vec4 &u, const Vec4 &v, const Vec4 &x)
{
	return u + x * (v - u);
}

float cerp(const float u0, const float u1, const float u2, const float u3, float x)
{
	float p = (u3 - u2) - (u0 - u1);
	float q = (u0 - u1) - p;
	float r = u2 - u0;
	return x * (x * (x * p + q) + r) + u1;
}

Vec2 cerp(const Vec2 &u0, const Vec2 &u1, const Vec2 &u2, const Vec2 &u3, float x)
{
	Vec2 p = (u3 - u2) - (u0 - u1);
	Vec2 q = (u0 - u1) - p;
	Vec2 r = u2 - u0;
	return x * (x * (x * p + q) + r) + u1;
}

Vec3 cerp(const Vec3 &u0, const Vec3 &u1, const Vec3 &u2, const Vec3 &u3, float x)
{
	Vec3 p = (u3 - u2) - (u0 - u1);
	Vec3 q = (u0 - u1) - p;
	Vec3 r = u2 - u0;
	return x * (x * (x * p + q) + r) + u1;
}

Vec4 cerp(const Vec4 &u0, const Vec4 &u1, const Vec4 &u2, const Vec4 &u3, float x)
{
	Vec4 p = (u3 - u2) - (u0 - u1);
	Vec4 q = (u0 - u1) - p;
	Vec4 r = u2 - u0;
	return x * (x * (x * p + q) + r) + u1;
}

float sign(const float v)
{
	return (v > 0)? 1.0f : (v < 0)? -1.0f : 0.0f;
}

Vec2 sign(const Vec2 &v)
{
	return Vec2(sign(v.x), sign(v.y));
}

Vec3 sign(const Vec3 &v)
{
	return Vec3(sign(v.x), sign(v.y), sign(v.z));
}

Vec4 sign(const Vec4 &v)
{
	return Vec4(sign(v.x), sign(v.y), sign(v.z), sign(v.w));
}

float clamp(const float v, const float c0, const float c1)
{
	return min(max(v, c0), c1);
}

Vec2 clamp(const Vec2 &v, const float c0, const float c1)
{
	return Vec2(min(max(v.x, c0), c1), min(max(v.y, c0), c1));
}

Vec2 clamp(const Vec2 &v, const Vec2 &c0, const Vec2 &c1)
{
	return Vec2(min(max(v.x, c0.x), c1.x), min(max(v.y, c0.y), c1.y));
}

Vec3 clamp(const Vec3 &v, const float c0, const float c1)
{
	return Vec3(min(max(v.x, c0), c1), min(max(v.y, c0), c1), min(max(v.z, c0), c1));
}

Vec3 clamp(const Vec3 &v, const Vec3 &c0, const Vec3 &c1)
{
	return Vec3(min(max(v.x, c0.x), c1.x), min(max(v.y, c0.y), c1.y), min(max(v.z, c0.z), c1.z));
}

Vec4 clamp(const Vec4 &v, const float c0, const float c1)
{
	return Vec4(min(max(v.x, c0), c1), min(max(v.y, c0), c1), min(max(v.z, c0), c1), min(max(v.z, c0), c1));
}

Vec4 clamp(const Vec4 &v, const Vec4 &c0, const Vec4 &c1)
{
	return Vec4(min(max(v.x, c0.x), c1.x), min(max(v.y, c0.y), c1.y), min(max(v.z, c0.z), c1.z), min(max(v.w, c0.w), c1.w));
}

Vec2 normalize(const Vec2 &v)
{
	float module = sqrtf(v.x * v.x + v.y * v.y);

	if(module == 0.0f)
		return v;

	float invLen = 1.0f / module;
	return v * invLen;
}

Vec3 normalize(const Vec3 &v)
{
	float module = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);

	if(module == 0.0f)
		return v;

	float invLen = 1.0f / module;
	return v * invLen;
}

Vec4 normalize(const Vec4 &v)
{
	float module = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);

	if(module == 0.0f)
		return v;

	float invLen = 1.0f / sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
	return v * invLen;
}

Vec2 fastNormalize(const Vec2 &v)
{
	float invLen = rsqrtf(v.x * v.x + v.y * v.y);
	return v * invLen;
}

Vec3 fastNormalize(const Vec3 &v)
{
	float invLen = rsqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	return v * invLen;
}

Vec4 fastNormalize(const Vec4 &v)
{
	float invLen = rsqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
	return v * invLen;
}

float length(const Vec2 &v)
{
	return sqrtf(v.x * v.x + v.y * v.y);
}

float length(const Vec3 &v)
{
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

float length(const Vec4 &v)
{
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

float squaredLength(const Vec2 &v)
{
	return v.x * v.x + v.y * v.y;
}

float squaredLength(const Vec3 &v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

float squaredLength(const Vec4 &v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
}

Vec3 reflect(const Vec3 &v, const Vec3 &normal)
{
	float n = dot(v, normal);
	return v - 2 * n * normal;
}

float distance(const Vec2 &u, const Vec2 &v)
{
    Vec2 d = u - v;
	return dot(d, d);
}

float distance(const Vec3 &u, const Vec3 &v)
{
    Vec3 d = u - v;
	return sqrtf(dot(d, d));
}

float distance(const Vec4 &u, const Vec4 &v)
{
    Vec4 d = u - v;
	return sqrtf(dot(d, d));
}

float planeDistance(const Vec3 &normal, const float offset, const Vec3 &point)
{
    return point.x * normal.x + point.y * normal.y + point.z * normal.z + offset;
}

float planeDistance(const Vec4 &plane, const Vec3 &point)
{
    return point.x * plane.x + point.y * plane.y + point.z * plane.z + plane.w;
}

float sCurve(const float t)
{
	return t * t * (3 - 2 * t);
}

Vec3 cross(const Vec3 &u, const Vec3 &v)
{
	return Vec3(u.y * v.z - v.y * u.z, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x);
}

Vec4 cross(const Vec4 &u, const Vec4 &v, const Vec4 &w)
{
	Vec4 out;

	out.x = u.y * (v.z * w.w - w.z * v.w) - u.z * (v.y * w.w - w.y * v.w) + u.w * (v.y * w.z - v.z *w.y);
	out.y = -(u.x * (v.z * w.w - w.z * v.w) - u.z * (v.x * w.w - w.x * v.w) + u.w * (v.x * w.z - w.x * v.z));
	out.z = u.x * (v.y * w.w - w.y * v.w) - u.y * (v.x *w.w - w.x * v.w) + u.w * (v.x * w.y - w.x * v.y);
	out.w = -(u.x * (v.y * w.z - w.y * v.z) - u.y * (v.x * w.z - w.x *v.z) + u.z * (v.x * w.y - w.x * v.y));
	return out;
}

Vec3 computeReflectionDirection (const Vec3& direction, const Vec3& normal)
{
	return direction - (2.0f * dot(direction, normal)) * normal;
}

Vec3 parallelComponent (const Vec3& direction, const Vec3& normal)
{
	float magnitude = dot(direction, normal);
	return normal * magnitude;
}

Vec3 perpindicularComponent (const Vec3& direction, const Vec3& normal)
{
	return direction - parallelComponent(direction, normal);
}

float lineProjection(const Vec3 &line0, const Vec3 &line1, const Vec3 &point)
{
	Vec3 v = line1 - line0;
	return dot(v, point - line0) / dot(v, v);
}

/* --------------------------------------------------------------------------------- */

void Mat4x4::translate(const Vec3 &v)
{
	rows[0].w += dot(rows[0].xyz(), v);
	rows[1].w += dot(rows[1].xyz(), v);
	rows[2].w += dot(rows[2].xyz(), v);
	rows[3].w += dot(rows[3].xyz(), v);
}

Mat4x4 operator + (const Mat4x4 &m, const Mat4x4 &n)
{
	return Mat4x4(m.rows[0] + n.rows[0], m.rows[1] + n.rows[1], m.rows[2] + n.rows[2], m.rows[3] + n.rows[3]);
}

Mat4x4 operator - (const Mat4x4 &m, const Mat4x4 &n)
{
	return Mat4x4(m.rows[0] - n.rows[0], m.rows[1] - n.rows[1], m.rows[2] - n.rows[2], m.rows[3] - n.rows[3]);
}

Mat4x4 operator - (const Mat4x4 &m)
{
	return Mat4x4(-m.rows[0], -m.rows[1], -m.rows[2], -m.rows[3]);
}

#define rcDot4(r, c) (m.rows[r].x * n.rows[0][c] + m.rows[r].y * n.rows[1][c] + m.rows[r].z * n.rows[2][c] + m.rows[r].w * n.rows[3][c])

Mat4x4 operator * (const Mat4x4 &m, const Mat4x4 &n)
{
	return Mat4x4(
		rcDot4(0, 0), rcDot4(0, 1), rcDot4(0, 2), rcDot4(0, 3),
		rcDot4(1, 0), rcDot4(1, 1), rcDot4(1, 2), rcDot4(1, 3),
		rcDot4(2, 0), rcDot4(2, 1), rcDot4(2, 2), rcDot4(2, 3),
		rcDot4(3, 0), rcDot4(3, 1), rcDot4(3, 2), rcDot4(3, 3));
}

Vec4 operator * (const Mat4x4 &m, const Vec4 &v)
{
	return Vec4(dot(m.rows[0], v), dot(m.rows[1], v), dot(m.rows[2], v), dot(m.rows[3], v));
}

Mat4x4 operator * (const Mat4x4 &m, const float x)
{
	return Mat4x4(m.rows[0] * x, m.rows[1] * x, m.rows[2] * x, m.rows[3] * x);
}

Mat4x4 transpose(const Mat4x4 &m)
{
	return Mat4x4(
		m.rows[0].x, m.rows[1].x, m.rows[2].x, m.rows[3].x,
		m.rows[0].y, m.rows[1].y, m.rows[2].y, m.rows[3].y,
		m.rows[0].z, m.rows[1].z, m.rows[2].z, m.rows[3].z,
		m.rows[0].w, m.rows[1].w, m.rows[2].w, m.rows[3].w);
}

Mat4x4 operator ! (const Mat4x4 &m)
{
	Mat4x4 mat;

	float p00 = m.rows[2][2] * m.rows[3][3];
	float p01 = m.rows[3][2] * m.rows[2][3];
	float p02 = m.rows[1][2] * m.rows[3][3];
	float p03 = m.rows[3][2] * m.rows[1][3];
	float p04 = m.rows[1][2] * m.rows[2][3];
	float p05 = m.rows[2][2] * m.rows[1][3];
	float p06 = m.rows[0][2] * m.rows[3][3];
	float p07 = m.rows[3][2] * m.rows[0][3];
	float p08 = m.rows[0][2] * m.rows[2][3];
	float p09 = m.rows[2][2] * m.rows[0][3];
	float p10 = m.rows[0][2] * m.rows[1][3];
	float p11 = m.rows[1][2] * m.rows[0][3];

	mat.rows[0][0] = (p00 * m.rows[1][1] + p03 * m.rows[2][1] + p04 * m.rows[3][1]) - (p01 * m.rows[1][1] + p02 * m.rows[2][1] + p05 * m.rows[3][1]);
	mat.rows[0][1] = (p01 * m.rows[0][1] + p06 * m.rows[2][1] + p09 * m.rows[3][1]) - (p00 * m.rows[0][1] + p07 * m.rows[2][1] + p08 * m.rows[3][1]);
	mat.rows[0][2] = (p02 * m.rows[0][1] + p07 * m.rows[1][1] + p10 * m.rows[3][1]) - (p03 * m.rows[0][1] + p06 * m.rows[1][1] + p11 * m.rows[3][1]);
	mat.rows[0][3] = (p05 * m.rows[0][1] + p08 * m.rows[1][1] + p11 * m.rows[2][1]) - (p04 * m.rows[0][1] + p09 * m.rows[1][1] + p10 * m.rows[2][1]);
	mat.rows[1][0] = (p01 * m.rows[1][0] + p02 * m.rows[2][0] + p05 * m.rows[3][0]) - (p00 * m.rows[1][0] + p03 * m.rows[2][0] + p04 * m.rows[3][0]);
	mat.rows[1][1] = (p00 * m.rows[0][0] + p07 * m.rows[2][0] + p08 * m.rows[3][0]) - (p01 * m.rows[0][0] + p06 * m.rows[2][0] + p09 * m.rows[3][0]);
	mat.rows[1][2] = (p03 * m.rows[0][0] + p06 * m.rows[1][0] + p11 * m.rows[3][0]) - (p02 * m.rows[0][0] + p07 * m.rows[1][0] + p10 * m.rows[3][0]);
	mat.rows[1][3] = (p04 * m.rows[0][0] + p09 * m.rows[1][0] + p10 * m.rows[2][0]) - (p05 * m.rows[0][0] + p08 * m.rows[1][0] + p11 * m.rows[2][0]);

	float q00 = m.rows[2][0] * m.rows[3][1];
	float q01 = m.rows[3][0] * m.rows[2][1];
	float q02 = m.rows[1][0] * m.rows[3][1];
	float q03 = m.rows[3][0] * m.rows[1][1];
	float q04 = m.rows[1][0] * m.rows[2][1];
	float q05 = m.rows[2][0] * m.rows[1][1];
	float q06 = m.rows[0][0] * m.rows[3][1];
	float q07 = m.rows[3][0] * m.rows[0][1];
	float q08 = m.rows[0][0] * m.rows[2][1];
	float q09 = m.rows[2][0] * m.rows[0][1];
	float q10 = m.rows[0][0] * m.rows[1][1];
	float q11 = m.rows[1][0] * m.rows[0][1];

	mat.rows[2][0] = (q00 * m.rows[1][3] + q03 * m.rows[2][3] + q04 * m.rows[3][3]) - (q01 * m.rows[1][3] + q02 * m.rows[2][3] + q05 * m.rows[3][3]);
	mat.rows[2][1] = (q01 * m.rows[0][3] + q06 * m.rows[2][3] + q09 * m.rows[3][3]) - (q00 * m.rows[0][3] + q07 * m.rows[2][3] + q08 * m.rows[3][3]);
	mat.rows[2][2] = (q02 * m.rows[0][3] + q07 * m.rows[1][3] + q10 * m.rows[3][3]) - (q03 * m.rows[0][3] + q06 * m.rows[1][3] + q11 * m.rows[3][3]);
	mat.rows[2][3] = (q05 * m.rows[0][3] + q08 * m.rows[1][3] + q11 * m.rows[2][3]) - (q04 * m.rows[0][3] + q09 * m.rows[1][3] + q10 * m.rows[2][3]);
	mat.rows[3][0] = (q02 * m.rows[2][2] + q05 * m.rows[3][2] + q01 * m.rows[1][2]) - (q04 * m.rows[3][2] + q00 * m.rows[1][2] + q03 * m.rows[2][2]);
	mat.rows[3][1] = (q08 * m.rows[3][2] + q00 * m.rows[0][2] + q07 * m.rows[2][2]) - (q06 * m.rows[2][2] + q09 * m.rows[3][2] + q01 * m.rows[0][2]);
	mat.rows[3][2] = (q06 * m.rows[1][2] + q11 * m.rows[3][2] + q03 * m.rows[0][2]) - (q10 * m.rows[3][2] + q02 * m.rows[0][2] + q07 * m.rows[1][2]);
	mat.rows[3][3] = (q10 * m.rows[2][2] + q04 * m.rows[0][2] + q09 * m.rows[1][2]) - (q08 * m.rows[1][2] + q11 * m.rows[2][2] + q05 * m.rows[0][2]);

	return mat * (1.0f / (m.rows[0][0] * mat.rows[0][0] + m.rows[1][0] * mat.rows[0][1] + m.rows[2][0] * mat.rows[0][2] + m.rows[3][0] * mat.rows[0][3]));
}

float Mat4x4::determinant() const
{
	float det;

	det = rows[0][0]*rows[1][1]*rows[2][2]*rows[3][3] - rows[0][0]*rows[1][1]*rows[2][3]*rows[3][2] +
	- rows[0][0]*rows[1][2]*rows[2][1]*rows[3][3] + rows[0][0]*rows[1][2]*rows[2][3]*rows[3][1] +
	+ rows[0][0]*rows[1][3]*rows[2][1]*rows[3][2] - rows[0][0]*rows[1][3]*rows[2][2]*rows[3][1] -
	- rows[0][1]*rows[1][0]*rows[2][2]*rows[3][3] + rows[0][1]*rows[1][0]*rows[2][3]*rows[3][2] +
	+ rows[0][1]*rows[1][2]*rows[2][0]*rows[3][3] - rows[0][1]*rows[1][2]*rows[2][3]*rows[3][0] -
	- rows[0][1]*rows[1][3]*rows[2][0]*rows[3][2] + rows[0][1]*rows[1][3]*rows[2][2]*rows[3][0] +
	+ rows[0][2]*rows[1][0]*rows[2][1]*rows[3][3] - rows[0][2]*rows[1][0]*rows[2][3]*rows[3][1] -
	- rows[0][2]*rows[1][1]*rows[2][0]*rows[3][3] + rows[0][2]*rows[1][1]*rows[2][3]*rows[3][0] +
	+ rows[0][2]*rows[1][3]*rows[2][0]*rows[3][1] - rows[0][2]*rows[1][3]*rows[2][1]*rows[3][0] -
	- rows[0][3]*rows[1][0]*rows[2][1]*rows[3][2] + rows[0][3]*rows[1][0]*rows[2][2]*rows[3][1] +
	+ rows[0][3]*rows[1][1]*rows[2][0]*rows[3][2] - rows[0][3]*rows[1][1]*rows[2][2]*rows[3][0] -
	- rows[0][3]*rows[1][2]*rows[2][0]*rows[3][1] + rows[0][3]*rows[1][2]*rows[2][1]*rows[3][0];

	return det;
}

Mat4x4 Mat4x4::inverse() const
{
    Mat4x4 Inverse(
        rows[1].data[2]*rows[2].data[3]*rows[3].data[1] - rows[1].data[3]*rows[2].data[2]*rows[3].data[1] + rows[1].data[3]*rows[2].data[1]*rows[3].data[2] - rows[1].data[1]*rows[2].data[3]*rows[3].data[2] - rows[1].data[2]*rows[2].data[1]*rows[3].data[3] + rows[1].data[1]*rows[2].data[2]*rows[3].data[3],
        rows[0].data[3]*rows[2].data[2]*rows[3].data[1] - rows[0].data[2]*rows[2].data[3]*rows[3].data[1] - rows[0].data[3]*rows[2].data[1]*rows[3].data[2] + rows[0].data[1]*rows[2].data[3]*rows[3].data[2] + rows[0].data[2]*rows[2].data[1]*rows[3].data[3] - rows[0].data[1]*rows[2].data[2]*rows[3].data[3],
        rows[0].data[2]*rows[1].data[3]*rows[3].data[1] - rows[0].data[3]*rows[1].data[2]*rows[3].data[1] + rows[0].data[3]*rows[1].data[1]*rows[3].data[2] - rows[0].data[1]*rows[1].data[3]*rows[3].data[2] - rows[0].data[2]*rows[1].data[1]*rows[3].data[3] + rows[0].data[1]*rows[1].data[2]*rows[3].data[3],
        rows[0].data[3]*rows[1].data[2]*rows[2].data[1] - rows[0].data[2]*rows[1].data[3]*rows[2].data[1] - rows[0].data[3]*rows[1].data[1]*rows[2].data[2] + rows[0].data[1]*rows[1].data[3]*rows[2].data[2] + rows[0].data[2]*rows[1].data[1]*rows[2].data[3] - rows[0].data[1]*rows[1].data[2]*rows[2].data[3],
        rows[1].data[3]*rows[2].data[2]*rows[3].data[0] - rows[1].data[2]*rows[2].data[3]*rows[3].data[0] - rows[1].data[3]*rows[2].data[0]*rows[3].data[2] + rows[1].data[0]*rows[2].data[3]*rows[3].data[2] + rows[1].data[2]*rows[2].data[0]*rows[3].data[3] - rows[1].data[0]*rows[2].data[2]*rows[3].data[3],
        rows[0].data[2]*rows[2].data[3]*rows[3].data[0] - rows[0].data[3]*rows[2].data[2]*rows[3].data[0] + rows[0].data[3]*rows[2].data[0]*rows[3].data[2] - rows[0].data[0]*rows[2].data[3]*rows[3].data[2] - rows[0].data[2]*rows[2].data[0]*rows[3].data[3] + rows[0].data[0]*rows[2].data[2]*rows[3].data[3],
        rows[0].data[3]*rows[1].data[2]*rows[3].data[0] - rows[0].data[2]*rows[1].data[3]*rows[3].data[0] - rows[0].data[3]*rows[1].data[0]*rows[3].data[2] + rows[0].data[0]*rows[1].data[3]*rows[3].data[2] + rows[0].data[2]*rows[1].data[0]*rows[3].data[3] - rows[0].data[0]*rows[1].data[2]*rows[3].data[3],
        rows[0].data[2]*rows[1].data[3]*rows[2].data[0] - rows[0].data[3]*rows[1].data[2]*rows[2].data[0] + rows[0].data[3]*rows[1].data[0]*rows[2].data[2] - rows[0].data[0]*rows[1].data[3]*rows[2].data[2] - rows[0].data[2]*rows[1].data[0]*rows[2].data[3] + rows[0].data[0]*rows[1].data[2]*rows[2].data[3],
        rows[1].data[1]*rows[2].data[3]*rows[3].data[0] - rows[1].data[3]*rows[2].data[1]*rows[3].data[0] + rows[1].data[3]*rows[2].data[0]*rows[3].data[1] - rows[1].data[0]*rows[2].data[3]*rows[3].data[1] - rows[1].data[1]*rows[2].data[0]*rows[3].data[3] + rows[1].data[0]*rows[2].data[1]*rows[3].data[3],
        rows[0].data[3]*rows[2].data[1]*rows[3].data[0] - rows[0].data[1]*rows[2].data[3]*rows[3].data[0] - rows[0].data[3]*rows[2].data[0]*rows[3].data[1] + rows[0].data[0]*rows[2].data[3]*rows[3].data[1] + rows[0].data[1]*rows[2].data[0]*rows[3].data[3] - rows[0].data[0]*rows[2].data[1]*rows[3].data[3],
        rows[0].data[1]*rows[1].data[3]*rows[3].data[0] - rows[0].data[3]*rows[1].data[1]*rows[3].data[0] + rows[0].data[3]*rows[1].data[0]*rows[3].data[1] - rows[0].data[0]*rows[1].data[3]*rows[3].data[1] - rows[0].data[1]*rows[1].data[0]*rows[3].data[3] + rows[0].data[0]*rows[1].data[1]*rows[3].data[3],
        rows[0].data[3]*rows[1].data[1]*rows[2].data[0] - rows[0].data[1]*rows[1].data[3]*rows[2].data[0] - rows[0].data[3]*rows[1].data[0]*rows[2].data[1] + rows[0].data[0]*rows[1].data[3]*rows[2].data[1] + rows[0].data[1]*rows[1].data[0]*rows[2].data[3] - rows[0].data[0]*rows[1].data[1]*rows[2].data[3],
        rows[1].data[2]*rows[2].data[1]*rows[3].data[0] - rows[1].data[1]*rows[2].data[2]*rows[3].data[0] - rows[1].data[2]*rows[2].data[0]*rows[3].data[1] + rows[1].data[0]*rows[2].data[2]*rows[3].data[1] + rows[1].data[1]*rows[2].data[0]*rows[3].data[2] - rows[1].data[0]*rows[2].data[1]*rows[3].data[2],
        rows[0].data[1]*rows[2].data[2]*rows[3].data[0] - rows[0].data[2]*rows[2].data[1]*rows[3].data[0] + rows[0].data[2]*rows[2].data[0]*rows[3].data[1] - rows[0].data[0]*rows[2].data[2]*rows[3].data[1] - rows[0].data[1]*rows[2].data[0]*rows[3].data[2] + rows[0].data[0]*rows[2].data[1]*rows[3].data[2],
        rows[0].data[2]*rows[1].data[1]*rows[3].data[0] - rows[0].data[1]*rows[1].data[2]*rows[3].data[0] - rows[0].data[2]*rows[1].data[0]*rows[3].data[1] + rows[0].data[0]*rows[1].data[2]*rows[3].data[1] + rows[0].data[1]*rows[1].data[0]*rows[3].data[2] - rows[0].data[0]*rows[1].data[1]*rows[3].data[2],
        rows[0].data[1]*rows[1].data[2]*rows[2].data[0] - rows[0].data[2]*rows[1].data[1]*rows[2].data[0] + rows[0].data[2]*rows[1].data[0]*rows[2].data[1] - rows[0].data[0]*rows[1].data[2]*rows[2].data[1] - rows[0].data[1]*rows[1].data[0]*rows[2].data[2] + rows[0].data[0]*rows[1].data[1]*rows[2].data[2]);
        
    float Determinant = 
        rows[0].data[3]*rows[1].data[2]*rows[2].data[1]*rows[3].data[0] - rows[0].data[2]*rows[1].data[3]*rows[2].data[1]*rows[3].data[0] - rows[0].data[3]*rows[1].data[1]*rows[2].data[2]*rows[3].data[0] + rows[0].data[1]*rows[1].data[3]*rows[2].data[2]*rows[3].data[0] +
        rows[0].data[2]*rows[1].data[1]*rows[2].data[3]*rows[3].data[0] - rows[0].data[1]*rows[1].data[2]*rows[2].data[3]*rows[3].data[0] - rows[0].data[3]*rows[1].data[2]*rows[2].data[0]*rows[3].data[1] + rows[0].data[2]*rows[1].data[3]*rows[2].data[0]*rows[3].data[1] +
        rows[0].data[3]*rows[1].data[0]*rows[2].data[2]*rows[3].data[1] - rows[0].data[0]*rows[1].data[3]*rows[2].data[2]*rows[3].data[1] - rows[0].data[2]*rows[1].data[0]*rows[2].data[3]*rows[3].data[1] + rows[0].data[0]*rows[1].data[2]*rows[2].data[3]*rows[3].data[1] +
        rows[0].data[3]*rows[1].data[1]*rows[2].data[0]*rows[3].data[2] - rows[0].data[1]*rows[1].data[3]*rows[2].data[0]*rows[3].data[2] - rows[0].data[3]*rows[1].data[0]*rows[2].data[1]*rows[3].data[2] + rows[0].data[0]*rows[1].data[3]*rows[2].data[1]*rows[3].data[2] +
        rows[0].data[1]*rows[1].data[0]*rows[2].data[3]*rows[3].data[2] - rows[0].data[0]*rows[1].data[1]*rows[2].data[3]*rows[3].data[2] - rows[0].data[2]*rows[1].data[1]*rows[2].data[0]*rows[3].data[3] + rows[0].data[1]*rows[1].data[2]*rows[2].data[0]*rows[3].data[3] +
        rows[0].data[2]*rows[1].data[0]*rows[2].data[1]*rows[3].data[3] - rows[0].data[0]*rows[1].data[2]*rows[2].data[1]*rows[3].data[3] - rows[0].data[1]*rows[1].data[0]*rows[2].data[2]*rows[3].data[3] + rows[0].data[0]*rows[1].data[1]*rows[2].data[2]*rows[3].data[3];

		Inverse /= Determinant;
		return Inverse;
}

Mat4x4 Mat4x4::transformInverse() const
{
	Mat4x4 rotation(
		rows[0].data[0], rows[1].data[0], rows[2].data[0], 0.0f,
		rows[0].data[1], rows[1].data[1], rows[2].data[1], 0.0f,
		rows[0].data[2], rows[1].data[2], rows[2].data[2], 0.0f,
		0.0f,			 0.0f,			  0.0f,			   1.0f);

	Mat4x4 translateMatrix(
		1.0f, 0.0f, 0.0f, -rows[0].data[3],
		0.0f, 1.0f, 0.0f, -rows[1].data[3],
		0.0f, 0.0f, 1.0f, -rows[2].data[3],
		0.0f, 0.0f, 0.0f, 1.0f);

	return rotation*translateMatrix;
}

void Mat4x4::operator *= (const float s)
{
    rows[0].x *= s;
    rows[0].y *= s;
    rows[0].z *= s;
    rows[0].w *= s;

    rows[1].x *= s;
    rows[1].y *= s;
    rows[1].z *= s;
    rows[1].w *= s;

    rows[2].x *= s;
    rows[2].y *= s;
    rows[2].z *= s;
    rows[2].w *= s;

    rows[3].x *= s;
    rows[3].y *= s;
    rows[3].z *= s;
    rows[3].w *= s;
}

void Mat4x4::operator /= (const float s)
{
    *this *= 1.0f/s;
}

/* --------------------------------------------------------------------------------- */

Mat4x4 rotateAxis(const Vec3 axis, const float angle)
{
	return Mat4x4(
		(1.0f - cos(angle)) * axis.x * axis.x + cos(angle),			 (1.0f - cos(angle)) * axis.y * axis.x - sin(angle) * axis.z, (1.0f - cos(angle)) * axis.z * axis.x + sin(angle) * axis.y, 0.0f,
		(1.0f - cos(angle)) * axis.y * axis.x + sin(angle) * axis.z, (1.0f - cos(angle)) * axis.y * axis.y + cos(angle),		  (1.0f - cos(angle)) * axis.y * axis.z - sin(angle) * axis.x, 0.0f,
		(1.0f - cos(angle)) * axis.z * axis.x - sin(angle) * axis.y, (1.0f - cos(angle)) * axis.z * axis.y + sin(angle) * axis.x, (1.0f - cos(angle)) * axis.z * axis.z + cos(angle),		   0.0f,
		0.0f,														 0.0f,														  0.0f,														   1.0f);
}

Mat4x4 rotateX(const float angle)
{
	float cosA = cosf(angle), sinA = sinf(angle);
	
	return Mat4x4(
		1, 0,     0,    0,
		0, cosA, -sinA, 0,
		0, sinA,  cosA, 0,
		0, 0,     0,    1);
}

Mat4x4 rotateY(const float angle)
{
	float cosA = cosf(angle), sinA = sinf(angle);

	return Mat4x4(
		cosA, 0, -sinA, 0,
		0,    1,  0,    0,
		sinA, 0,  cosA, 0,
		0,    0,  0,    1);
}

Mat4x4 rotateZ(const float angle)
{
	float cosA = cosf(angle), sinA = sinf(angle);

	return Mat4x4(
		cosA, -sinA, 0, 0,
		sinA,  cosA, 0, 0,
		0,     0,    1, 0,
		0,     0,    0, 1);
}

Mat4x4 rotateXY(const float angleX, const float angleY)
{
	float cosX = cosf(angleX), sinX = sinf(angleX), 
		  cosY = cosf(angleY), sinY = sinf(angleY);

	return Mat4x4(
		 cosY,        0,    -sinY,        0,
		-sinX * sinY, cosX, -sinX * cosY, 0,
		 cosX * sinY, sinX,  cosX * cosY, 0,
		 0,           0,     0,           1);
}

Mat4x4 rotateYX(const float angleX, const float angleY)
{
	float cosX = cosf(angleX), sinX = sinf(angleX), 
		  cosY = cosf(angleY), sinY = sinf(angleY);

	return Mat4x4(
		cosY, -sinX * sinY, -cosX * sinY, 0,
		0,     cosX,        -sinX,        0,
		sinY,  sinX * cosY,  cosX * cosY, 0,
		0,     0,            0,           1);
}

Mat4x4 rotateZXY(const float angleX, const float angleY, const float angleZ)
{
	float cosX = cosf(angleX), sinX = sinf(angleX), 
		  cosY = cosf(angleY), sinY = sinf(angleY),
		  cosZ = cosf(angleZ), sinZ = sinf(angleZ);

	return Mat4x4(
		cosY * cosZ + sinX * sinY * sinZ,   -cosX * sinZ,    sinX * cosY * sinZ - sinY * cosZ,  0,
		cosY * sinZ - sinX * sinY * cosZ,    cosX * cosZ,   -sinY * sinZ - sinX * cosY * cosZ,  0,
		cosX * sinY,                         sinX,           cosX * cosY,                       0,
		0,                                   0,              0,                                 1);
}

Mat4x4 rotateXYZ(const float angleX, const float angleY, const float angleZ)
{
	Mat4x4 rotX = rotateX(angleX);
	Mat4x4 rotY = rotateY(angleY);
	Mat4x4 rotZ = rotateZ(angleZ);

	return rotZ*rotY*rotX;
}

Mat4x4 translate(const Vec3 &v)
{
	return Mat4x4(1,0,0,v.x, 0,1,0,v.y, 0,0,1,v.z, 0,0,0,1);
}

Mat4x4 translate(const float x, const float y, const float z)
{
	return Mat4x4(1,0,0,x, 0,1,0,y, 0,0,1,z, 0,0,0,1);
}

Mat4x4 scale(const float x, const float y, const float z)
{
	return Mat4x4(x,0,0,0, 0,y,0,0, 0,0,z,0, 0,0,0,1);
}

Mat4x4 scale(const Vec3 scale)
{
    return Mat4x4(scale.x, 0, 0, 0, 0, scale.y, 0, 0, 0, 0, scale.z, 0, 0, 0, 0, 1);
}

Mat4x4 perspectiveMatrix(const float fov, const float aspect, const float zNear, const float zFar)
{
	float s = 1.0f / tanf(0.5f * fov);

	return Mat4x4(
		s / aspect, 0.0f,							  0.0f, 0.0f,
			  0.0f,	   s,							  0.0f, 0.0f,
			  0.0f,	0.0f,			 zFar / (zFar - zNear), 1.0f,
			  0.0f,	0.0f, (-zFar * zNear) / (zFar - zNear), 0.0f);
}

Mat4x4 perspectiveMatrixRH(const float fov, const float aspect, const float zNear, const float zFar)
{
	float s = cosf(0.5f * fov) / sinf(0.5f * fov);

	return Mat4x4(
		s / aspect, 0.0f,							  0.0f,  0.0f,
		0.0f,		   s,							  0.0f,	 0.0f,
		0.0f,		0.0f,			 zFar / (zNear - zFar), -1.0f,
		0.0f,		0.0f,	 zFar * zNear / (zNear - zFar),  0.0f);
}

Mat4x4 perspectiveMatrixX(const float fov, const int width, const int height, const float zNear, const float zFar)
{
	float w = cosf(0.5f * fov) / sinf(0.5f * fov);
	float h = (w * width) / height;

	return Mat4x4(
		w, 0, 0, 0,
		0, h, 0, 0,
		0, 0, (zFar + zNear) / (zFar - zNear), -(2 * zFar * zNear) / (zFar - zNear),
		0, 0, 1, 0);
}

Mat4x4 perspectiveMatrixY(const float fov, const int width, const int height, const float zNear, const float zFar)
{
	float h = cosf(0.5f * fov) / sinf(0.5f * fov);
	float w = (h * height) / width;

	return Mat4x4(
		w, 0, 0, 0,
		0, h, 0, 0,
		0, 0, (zFar + zNear) / (zFar - zNear), -(2 * zFar * zNear) / (zFar - zNear),
		0, 0, 1, 0);
}

Mat4x4 orthoMatrixX(const float left, const float right, const float top, const float bottom, const float zNear, const float zFar)
{
	float rl = right - left;
	float tb = top - bottom;
	float fn = zFar - zNear;

	return Mat4x4(
		2.0f / rl,	  0.0f,		 0.0f,						 -(right + left) / rl,
		0.0f,         2.0f / tb, 0.0f,						 -(top + bottom) / tb,
		0.0f,         0.0f,      -2.0f / fn,				 0.0f,
		0.0f,         0.0f,      -(zFar + zNear) / fn,       1.0f);
}

Mat4x4 orthoMatrixX(const float width, const float height, const float zNear, const float zFar)
{
	float rl = width;
	float tb = height;
	float fn = zFar - zNear;

	return Mat4x4(
   2.0f / rl,			 0.0f,					 0.0f, 0.0f,
		0.0f,		2.0f / tb,					 0.0f, 0.0f,
		0.0f,			 0.0f,				1.0f / fn, 0.0f,
		0.0f,			 0.0f,			  -zNear / fn, 1.0f);
}

Mat4x4 viewMatrix(const Vec3 pos, const Vec3 look, const Vec3 up)
{
	// Keep camera's axes orthogonal to each other:
	Vec3 _look = normalize(look - pos);
	Vec3 _up = normalize(up);
	Vec3 _right = cross(_up, _look);;
	_right = normalize(_right);
	_up = cross(_look, _right);
	// Build the view matrix:
	float x = -dot(_right, pos);
	float y = -dot(_up, pos);
	float z = -dot(_look, pos);

	return Mat4x4(
		_right.x, _up.x, _look.x, 0.0f,
		_right.y, _up.y, _look.y, 0.0f,
		_right.z, _up.z, _look.z, 0.0f,
		x,		  y,	 z,        1.0f);
}

Mat4x4 toD3DProjection(const Mat4x4 &m)
{
	Mat4x4 mat;

	mat.rows[0] = m.rows[0];
	mat.rows[1] = m.rows[1];
	mat.rows[2] = 0.5f * (m.rows[2] + m.rows[3]);
	mat.rows[3] = m.rows[3];

	return mat;
}

Mat4x4 toGLProjection(const Mat4x4 &m)
{
	Mat4x4 mat;

	mat.rows[0] = m.rows[0];
	mat.rows[1] = m.rows[1];
	mat.rows[2] = m.rows[2] * 2.0 - m.rows[3];
	mat.rows[3] = m.rows[3];

	return mat;
}

Mat4x4 pegToFarPlane(const Mat4x4 &m)
{
	Mat4x4 mat;

	mat.rows[0] = m.rows[0];
	mat.rows[1] = m.rows[1];
	mat.rows[2] = m.rows[3];
	mat.rows[3] = m.rows[3];

	return mat;
}

Mat4x4 cubeViewMatrix(const unsigned int side)
{
	switch(side)
	{
	case POSITIVE_X:
		return Mat4x4(
			0, 0, 1, 0,
			0, 1, 0, 0,
		   -1, 0, 0, 0,
			0, 0, 0, 1);
	case NEGATIVE_X:
		return Mat4x4(
			0, 0,-1, 0,
			0, 1, 0, 0,
			1, 0, 0, 0,
			0, 0, 0, 1);
	case POSITIVE_Y:
		return Mat4x4(
			1, 0, 0, 0,
			0, 0, 1, 0,
			0,-1, 0, 0,
			0, 0, 0, 1);
	case NEGATIVE_Y:
		return Mat4x4(
			1, 0, 0, 0,
			0, 0,-1, 0,
			0, 1, 0, 0,
			0, 0, 0, 1);
	case POSITIVE_Z:
		return Mat4x4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
		//case NEGATIVE_Z:
	default:
		return Mat4x4(
		   -1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0,-1, 0,
			0, 0, 0, 1);
	}
}

Mat4x4 cubeProjectionMatrixGL(const float zNear, const float zFar)
{
	return Mat4x4(
		1,  0, 0, 0,
		0, -1, 0, 0,
		0,  0, (zFar + zNear) / (zFar - zNear), -(2 * zFar * zNear) / (zFar - zNear),
		0,  0, 1, 0);
}

Mat4x4 cubeProjectionMatrixD3D(const float zNear, const float zFar)
{
	return Mat4x4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, zFar / (zFar - zNear), (zFar * zNear) / (zNear - zFar),
		0, 0, 1, 0);
}

Mat4x4 identity4()
{
	return Mat4x4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
}

void Aabb::SetFromVertexSet(const Vec4* pPoints, const int numPoints)
{
	vcMin = pPoints[0].xyz();
	vcMax = pPoints[0].xyz();

	for (int i = 0; i<numPoints; i++)
	{
		if (vcMin.x > pPoints[i].x)
		{
			vcMin.x = pPoints[i].x;
		}
		if (vcMin.y > pPoints[i].y)
		{
			vcMin.y = pPoints[i].y;
		}
		if (vcMin.z > pPoints[i].z)
		{
			vcMin.z = pPoints[i].z;
		}
		if (vcMax.x < pPoints[i].x)
		{
			vcMax.x = pPoints[i].x;
		}
		if (vcMax.y < pPoints[i].y)
		{
			vcMax.y = pPoints[i].y;
		}
		if (vcMax.z < pPoints[i].z)
		{
			vcMax.z = pPoints[i].z;
		}
	}

	//avoid precision error
	vcMin -= 0.05f;
	vcMax += 0.05f;


	vcCenter = (vcMin + vcMax)*0.5;
}

void Aabb::SetFromVertexSet(const Vec3* pPoints, const int numPoints)
{
    vcMin = pPoints[0];
	vcMax = pPoints[0];

	for(int i=0; i<numPoints; i++)
	{
		if(vcMin.x > pPoints[i].x)
		{
			vcMin.x = pPoints[i].x;
		}
		if(vcMin.y > pPoints[i].y)
		{
			vcMin.y = pPoints[i].y;
		}
		if(vcMin.z > pPoints[i].z)
		{
			vcMin.z = pPoints[i].z;
		}
		if(vcMax.x < pPoints[i].x)
		{
			vcMax.x = pPoints[i].x;
		}
		if(vcMax.y < pPoints[i].y)
		{
			vcMax.y = pPoints[i].y;
		}
		if(vcMax.z < pPoints[i].z)
		{
			vcMax.z = pPoints[i].z;
		}
	}

	//avoid precision error
	vcMin -= 0.05f;
	vcMax += 0.05f;


	vcCenter = (vcMin + vcMax)*0.5;
}

Aabb Aabb::Union(const Vec3 vPoint)
{
    Aabb result;

    if (isVoid)
    {
        result.vcMax = vPoint;
        result.vcMin = vPoint;

        isVoid = false;
    }
    else
    {
        if (result.vcMin.x > vPoint.x)
        {
            result.vcMin.x = vPoint.x;
        }
        if (result.vcMin.y > vPoint.y)
        {
            result.vcMin.y = vPoint.y;
        }
        if (result.vcMin.z > vPoint.z)
        {
            result.vcMin.z = vPoint.z;
        }
        if (result.vcMax.x < vPoint.x)
        {
            result.vcMax.x = vPoint.x;
        }
        if (result.vcMax.y < vPoint.y)
        {
            result.vcMax.y = vPoint.y;
        }
        if (result.vcMax.z < vPoint.z)
        {
            result.vcMax.z = vPoint.z;
        }
    }	

    result.vcCenter = (result.vcMin + result.vcMax)*0.5;

    return result;
}

Aabb Aabb::Union(const Aabb aabb)
{
	if (isVoid)
		return aabb;

    Aabb result;

    result.vcMax.x = (vcMax.x >= aabb.vcMax.x) ? vcMax.x : aabb.vcMax.x;
    result.vcMax.y = (vcMax.y >= aabb.vcMax.y) ? vcMax.y : aabb.vcMax.y;
    result.vcMax.z = (vcMax.z >= aabb.vcMax.z) ? vcMax.z : aabb.vcMax.z;

    result.vcMin.x = (vcMin.x <= aabb.vcMin.x) ? vcMin.x : aabb.vcMin.x;
    result.vcMin.y = (vcMin.y <= aabb.vcMin.y) ? vcMin.y : aabb.vcMin.y;
    result.vcMin.z = (vcMin.z <= aabb.vcMin.z) ? vcMin.z : aabb.vcMin.z;

    result.vcCenter = (result.vcMin + result.vcMax)/2.0;

    return result;
}

Aabb Aabb::Union(Vec3 vMin, Vec3 vMax)
{
    Aabb result;

    if (isVoid)
    {
        result.vcMax = vMax;
        result.vcMin = vMin;

		isVoid = false;
    }
    else
    {
        result.vcMax.x = (vcMax.x >= vMax.x) ? vcMax.x : vMax.x;
        result.vcMax.y = (vcMax.y >= vMax.y) ? vcMax.y : vMax.y;
        result.vcMax.z = (vcMax.z >= vMax.z) ? vcMax.z : vMax.z;

        result.vcMin.x = (vcMin.x <= vMin.x) ? vcMin.x : vMin.x;
        result.vcMin.y = (vcMin.y <= vMin.y) ? vcMin.y : vMin.y;
        result.vcMin.z = (vcMin.z <= vMin.z) ? vcMin.z : vMin.z;
    }    

    //result.vcCenter = (result.vcMin + result.vcMax) / 2.0;

    return result;
}

Aabb operator* (const Mat4x4 &m, const Aabb &aabb)
{
    Aabb result;

    result.vcMax = Vec4(dot(m.rows[0], Vec4(aabb.vcMax, 1.0f)), dot(m.rows[1], Vec4(aabb.vcMax, 1.0f)), dot(m.rows[2], Vec4(aabb.vcMax, 1.0f)), dot(m.rows[3], Vec4(aabb.vcMax, 1.0f))).xyz();
    result.vcMin = Vec4(dot(m.rows[0], Vec4(aabb.vcMin, 1.0f)), dot(m.rows[1], Vec4(aabb.vcMin, 1.0f)), dot(m.rows[2], Vec4(aabb.vcMin, 1.0f)), dot(m.rows[3], Vec4(aabb.vcMin, 1.0f))).xyz();
    result.vcCenter = Vec4(dot(m.rows[0], Vec4(aabb.vcCenter, 1.0f)), dot(m.rows[1], Vec4(aabb.vcCenter, 1.0f)), dot(m.rows[2], Vec4(aabb.vcCenter, 1.0f)), dot(m.rows[3], Vec4(aabb.vcCenter, 1.0f))).xyz();

    return result;
}

bool Aabb::Intersects(const Aabb &aabb) const
{
	if (aabb.isInfinite || isInfinite)
		return true;

	// Check for a separating axis .
	if(this->vcMin.x > aabb.vcMax.x)
		return false;
	if(this->vcMax.x < aabb.vcMin.x)
		return false;
	if(this->vcMin.y > aabb.vcMax.y)
		return false;
	if(this->vcMax.y < aabb.vcMin.y)
		return false;
	if(this->vcMin.z > aabb.vcMax.z)
		return false;
	if(this->vcMax.z < aabb.vcMin.z)
		return false;

	return true;	
}
bool Aabb::Intersects(const Vec3 &dir) const
{
	if(dir.x <= this->vcMax.x && dir.x >= this->vcMin.x)
	{
		if(dir.y <= this->vcMax.y && dir.y >= this->vcMin.y)
		{
			if(dir.z <= this->vcMax.z && dir.z >= this->vcMin.z)
			{
				return true;
			}
		}
	}

	return false;
}

Aabb Aabb::Transform(const Mat4x4 &mat) const
{
    Aabb aabbT;

    float a, b;
    float Amin[3], Amax[3];
    float Bmin[3], Bmax[3];

    /*Copy box A into a min array and a max array for easy reference.*/

    Amin[0] = vcMin.x; Amax[0] = vcMax.x;
    Amin[1] = vcMin.y; Amax[1] = vcMax.y;
    Amin[2] = vcMin.z; Amax[2] = vcMax.z;

    /* Take care of translation by beginning at T. */

    Bmin[0] = Bmax[0] = mat.rows[0].w;
    Bmin[1] = Bmax[1] = mat.rows[1].w;;
    Bmin[2] = Bmax[2] = mat.rows[2].w;;

    /* Now find the extreme points by considering the product of the */
    /* min and max with each component of M. */
                     
    for(int i = 0; i < 3; i++ )
    {
        a = mat.rows[i].x * Amin[0];
        b = mat.rows[i].x * Amax[0];
        if( a < b )
        {
            Bmin[i] += a;
            Bmax[i] += b;
        }
        else
        {
            Bmin[i] += b;
            Bmax[i] += a;
        }

        /////////////////////////////////////////////////////
        a = mat.rows[i].y * Amin[1];
        b = mat.rows[i].y * Amax[1];
        if( a < b )
        {
            Bmin[i] += a;
            Bmax[i] += b;
        }
        else
        {
            Bmin[i] += b;
            Bmax[i] += a;
        }

        /////////////////////////////////////////////////////
        a = mat.rows[i].z * Amin[2];
        b = mat.rows[i].z * Amax[2];
        if( a < b )
        {
            Bmin[i] += a;
            Bmax[i] += b;
        }
        else
        {
            Bmin[i] += b;
            Bmax[i] += a;
        }
    }

    /* Copy the result into the new box. */

    aabbT.vcMin.x = Bmin[0]; aabbT.vcMax.x = Bmax[0];
    aabbT.vcMin.y = Bmin[1]; aabbT.vcMax.y = Bmax[1];
    aabbT.vcMin.z = Bmin[2]; aabbT.vcMax.z = Bmax[2];

    aabbT.vcCenter = (aabbT.vcMin + aabbT.vcMax)*0.5;
    aabbT.isInfinite = isInfinite;

    return aabbT;
}

bool Plane::Intersects(Vec3 v0, Vec3 v1, Vec3 v2)
{
	int n = this->Classify(v0);

	if ( (n == this->Classify(v1)) && (n == this->Classify(v2)) )
		return false;
	
	return true;
}

bool Plane::Inside(const Vec3 &point, const float radius) const
{
	float fDistance;	// calculate our distances to each of the planes

	// find the distance to this plane
	fDistance = dot(planeNormal, point);		

	// if this distance is < -radius, we are outside
	return (fDistance >= -radius);
}

bool Plane::Inside(const Vec3 &point) const
{
	// Inside the plane is defined as the direction the normal is facing
	float result = dot(planeNormal, point);
	return (result >= 0.0f);
}

void Ray::DeTransform(const Mat4x4 &_m)
{
	Mat4x4 mInv;
	Mat4x4 m=_m;

	// inverse translation
	m_vcOrig.x -= m.rows[0].w;
	m_vcOrig.y -= m.rows[1].w;
	m_vcOrig.z -= m.rows[2].w;

	// delete translation in the matrix
	m.rows[0].w=0.0f; m.rows[1].w=0.0f; m.rows[2].w=0.0f;
	
	// invert matrix
	mInv = m.inverse();
	
	Vec4 _vcOrig, _vcDir;

	// apply inverse matrix
	_vcOrig = mInv * Vec4(m_vcOrig, 1.0f);
	m_vcOrig.x = _vcOrig.x;
	m_vcOrig.y = _vcOrig.y;
	m_vcOrig.z = _vcOrig.z;
	
	_vcDir = mInv * Vec4(m_vcDir, 0.0f);
	m_vcDir.x = _vcDir.x;
	m_vcDir.y = _vcDir.y;
	m_vcDir.z = _vcDir.z;
}

bool Ray::Intersects(const Vec3 &vc0, const Vec3 &vc1, const Vec3 &vc2, bool bCull, float *t)
{
	Vec3 pvec, tvec, qvec;
	Vec3 edge1 = vc1 - vc0;
	Vec3 edge2 = vc2 - vc0;
	pvec = cross(m_vcDir, edge2);
	
	// if close to 0 ray is parallel
	float det = dot(edge1, pvec);
	
	// to account for poor float precision use
	// so called epsilon value of 0.0001 instead
	// of comparing to exactly 0
	if ( (bCull) && (det < 0.0001f) )
		return false;
	else if ( (det < 0.0001f) && (det > -0.0001f) )
		return false;
	
	// distance to plane, < 0 means ray behind the plane
	tvec = m_vcOrig - vc0;
	float u = dot(tvec, pvec);
	if (u < 0.0f || u > det)
		return false;

	qvec = cross(tvec, edge1);
	float v = dot(m_vcDir, qvec);
	if (v < 0.0f || u+v > det)
		return false;
	if (t) 
	{
		*t = dot(edge2, qvec);
		float fInvDet = 1.0f / det;
		*t *= fInvDet;
	}
	return true;
}

bool Ray::Intersects(const Plane &plane, bool bCull, float *t, Vec3 *vcHit) const
{
	float Vd = dot(plane.planeNormal, m_vcDir);
	
	// ray parallel to the plane
	if (fabs(Vd) < 0.00001f)
		return false;
	
	// plane normal point away from ray direction
	// => intersection with back face if any
	if (bCull && (Vd > 0.0f))
		return false;
	
	float Vo = -( dot(plane.planeNormal, m_vcOrig) + plane.distance);
	float _t = Vo / Vd;
	
	// intersection before ray origin
	if (_t < 0.0f)
		return false;
	if (vcHit) 
	{
		(*vcHit) = m_vcOrig + (m_vcDir * _t);
	}
	if (t)
		(*t) = _t;
	
	return true;
}

bool Ray::IntersectsWithoutOrigin(const Plane &plane, bool bCull, float *t, Vec3 *vcHit)
{
	float Vd = dot(plane.planeNormal, m_vcDir);
	
	// ray parallel to the plane
	if (fabs(Vd) < 0.00001f)
		return false;
	
	// plane normal point away from ray direction
	// => intersection with back face if any
	if (bCull && (Vd > 0.0f))
		return false;
	
	float Vo = -( dot(plane.planeNormal, m_vcOrig) + plane.distance);
	float _t = Vo / Vd;
	
	if (vcHit) 
	{
		(*vcHit) = m_vcOrig + (m_vcDir * _t);
	}
	if (t)
		(*t) = _t;
	
	return true;
}

bool Ray::Intersects(const Plane &plane, bool bCull, float fL, float *t, Vec3 *vcHit) const
{
	float tTemp;
	bool Intersect;

	Intersect = Intersects(plane, bCull, &tTemp, vcHit);
	
	if(t)
		memcpy(t, &tTemp, sizeof(float));

	if(Intersect && tTemp <= fL)
	{
		return true;
	}

	return false;
}

bool Ray::Intersects(const Aabb &aabb, Vec3 *vcHit)
{
	bool bInside = true;
	Vec3 MaxT;
	MaxT.x = -1.0f; MaxT.y = -1.0f; MaxT.z = -1.0f;

	// find the x component
	if (m_vcOrig.x < aabb.vcMin.x) 
	{
		(*vcHit).x = aabb.vcMin.x;
		bInside = false;

		if (m_vcDir.x != 0.0f)
			MaxT.x = (aabb.vcMin.x - m_vcOrig.x) / m_vcDir.x;
	}
	else if (m_vcOrig.x > aabb.vcMax.x) 
	{
		(*vcHit).x = aabb.vcMax.x;
		bInside = false;
		
		if (m_vcDir.x != 0.0f)
			MaxT.x = (aabb.vcMax.x - m_vcOrig.x) / m_vcDir.x;
	}
	
	// find the y component
	if (m_vcOrig.y < aabb.vcMin.y) 
	{
		(*vcHit).y = aabb.vcMin.y;
		bInside = false;
		
		if (m_vcDir.y != 0.0f)
			MaxT.y = (aabb.vcMin.y - m_vcOrig.y) / m_vcDir.y;
	}
	else if (m_vcOrig.y > aabb.vcMax.y) 
	{
		(*vcHit).y = aabb.vcMax.y;
		bInside = false;

		if (m_vcDir.y != 0.0f)
			MaxT.y = (aabb.vcMax.y - m_vcOrig.y) / m_vcDir.y;
	}
	
	// find the z component
	if (m_vcOrig.z < aabb.vcMin.z) 
	{
		(*vcHit).z = aabb.vcMin.z;
		bInside = false;
		
		if (m_vcDir.z != 0.0f)
			MaxT.z = (aabb.vcMin.z - m_vcOrig.z) / m_vcDir.z;
	}
	else if (m_vcOrig.z > aabb.vcMax.z)	
	{
		(*vcHit).z = aabb.vcMax.z;
		bInside = false;

		if (m_vcDir.z != 0.0f)
			MaxT.z = (aabb.vcMax.z - m_vcOrig.z) / m_vcDir.z;
	}
	
	// ray origin inside the box
	if (bInside) 
	{
		(*vcHit) = m_vcOrig;
		return true;
	}
	
	// find maximum value for maxT
	int nPlane = 0;
	if (MaxT.y > ((float*)&MaxT)[nPlane]) 
		nPlane = 1;
	if (MaxT.z > ((float*)&MaxT)[nPlane]) 
		nPlane = 2;
	if ( ((float*)&MaxT)[nPlane] < 0.0f) 
		return false;
	if (nPlane == 0) 
	{
		(*vcHit).y = m_vcOrig.y + MaxT.x * m_vcDir.y;
		(*vcHit).z = m_vcOrig.z + MaxT.x * m_vcDir.z;

		if ( ((*vcHit).y < aabb.vcMin.y-0.00001f) || ((*vcHit).y > aabb.vcMax.y+0.00001f) )
			return false;

		if ( ((*vcHit).z < aabb.vcMin.z-0.00001f) || ((*vcHit).z > aabb.vcMax.z+0.00001f) )
			return false;
	}

	if (nPlane == 1)
	{
		(*vcHit).x = m_vcOrig.x + MaxT.y * m_vcDir.x;
		(*vcHit).z = m_vcOrig.z + MaxT.y * m_vcDir.z;
		
		if ( ((*vcHit).x < aabb.vcMin.x-0.00001f) || ((*vcHit).x > aabb.vcMax.x+0.00001f) )
			return false;
		
		if ( ((*vcHit).z < aabb.vcMin.z-0.00001f) || ((*vcHit).z > aabb.vcMax.z+0.00001f) )
			return false;
	}
	
	if (nPlane == 2) 
	{
		(*vcHit).x = m_vcOrig.x + MaxT.z * m_vcDir.x;
		(*vcHit).y = m_vcOrig.y + MaxT.z * m_vcDir.y;
		
		if ( ((*vcHit).x < aabb.vcMin.x-0.00001f) || ((*vcHit).x > aabb.vcMax.x+0.00001f) )
			return false;

		if ( ((*vcHit).y < aabb.vcMin.y-0.00001f) || ((*vcHit).y > aabb.vcMax.y+0.00001f) )
			return false;
	}
	
	return true;
}

bool Ray::Intersects(const Ray& ray, float fL, Vec3 *vcHit)
{
	float t, s, denominator1, denominator2, denominator3;
	float error = 0.0000001f;

	if(ray.m_vcDir == m_vcDir)
	{
		return false;
	}

	denominator1 = m_vcDir.x*(-ray.m_vcDir.y) - (-ray.m_vcDir.x)*m_vcDir.y;
	denominator2 = m_vcDir.y*(-ray.m_vcDir.z) - (-ray.m_vcDir.y)*m_vcDir.z;
	denominator3 = m_vcDir.x*(-ray.m_vcDir.z) - (-ray.m_vcDir.x)*m_vcDir.z;

	if(denominator1 < error && denominator1 > -error)
	{
		if(denominator2 < error && denominator2 > -error)
		{
			if(denominator3 < error && denominator3 > -error)
			{
				return false;
			}
			else
			{
				t = (-ray.m_vcDir.z*(ray.m_vcOrig.x - m_vcOrig.x) + ray.m_vcDir.x*(ray.m_vcOrig.z - m_vcOrig.z))/denominator3;
			}
		}
		else
		{
			t = (-ray.m_vcDir.z*(ray.m_vcOrig.y - m_vcOrig.y) + ray.m_vcDir.y*(ray.m_vcOrig.z - m_vcOrig.z))/denominator2;
		}
	}
	else
	{
		t = (-ray.m_vcDir.y*(ray.m_vcOrig.x - m_vcOrig.x) + ray.m_vcDir.x*(ray.m_vcOrig.y - m_vcOrig.y))/denominator1;
	}

	if(ray.m_vcDir.x > error || ray.m_vcDir.x < -error)
	{
		s = (ray.m_vcOrig.x - m_vcOrig.x - m_vcDir.x*t)/(-ray.m_vcDir.x);
	}
	else if(ray.m_vcDir.y > error || ray.m_vcDir.y < -error)
	{
		s = (ray.m_vcOrig.y - m_vcOrig.y - m_vcDir.y*t)/(-ray.m_vcDir.y);
	}
	else
	{
		s = (ray.m_vcOrig.z - m_vcOrig.z - m_vcDir.z*t)/(-ray.m_vcDir.z);
	}

	if(t < 0 || fL < t || s < 0)
		return false;

	if(vcHit)
	{
		*vcHit = m_vcDir*t + m_vcOrig;
	}

	return true;
}

Vec3 Ray::closestPointOnLine(Vec3 p, float fLength)
{
	// Determine t (the length of the vector from ‘a’ to ‘p’)

	Vec3 c = p - m_vcOrig;
	Vec3 b = fLength*m_vcDir + m_vcOrig;
	double t = dot(m_vcDir, c);

	// Check to see if ‘t’ is beyond the extents of the line segment

	if (t < 0) return m_vcOrig;
	if (t > fLength) return b;
 
	// Return the point between ‘a’ and ‘b’
	return m_vcOrig + m_vcDir*(float)t;
}

//
// Frustum::Frustum					
//
Frustum::Frustum()
{
	m_Fov = CHG_PI/4.0f;		// default field of view is 90 degrees
	m_Aspect = 1.0f;			// default aspect ratio is 1:1
	m_Near = 0.01f;				// default near clip plane is 1m away from the camera
	m_Far = 1000.0f;				// default near clip plane is 100m away from the camera
	m_Type = PERSPECTIVE_FRUSTUM;
}


//
// Frustum::Inside		//sphere			
//
bool Frustum::Inside(const Vec3 &point) const
{
	for (int i=0; i<NumPlanes; ++i)
	{
		if (!m_Planes[i].Inside(point))
			return false;
	}

	return true;
}

//
// Frustum::Inside		//point tranform frustum			
//
bool Frustum::Inside(const Vec3 &point, const Mat4x4 &transform) const
{
	for (int i=0; i<NumPlanes; ++i)
	{
		Vec3 planeNormalT = (transform * Vec4(m_Planes[i].planeNormal, 0.0f)).xyz();
		Vec3 planePointT = (transform * Vec4(m_Planes[i].planePoint, 1.0f)).xyz();
		float distanceT = dot(planeNormalT, planePointT);

		if ((dot(planeNormalT, point) - distanceT) < 0.0f)
			return false;
	}

	return true;
}

//
// Frustum::Inside		//Aabb
//
bool Frustum::Inside(const Aabb &aabb) const
{
	Vec3 vcMin, vcMax;
	bool bIntersects = false;

	if(aabb.isInfinite)
		return true;

	// build and test extreme points
	for (int i=0; i<6; i++) 
	{
		if (m_Planes[i].planeNormal.x >= 0.0f) 
		{
			vcMin.x = aabb.vcMin.x;
			vcMax.x = aabb.vcMax.x;
		}
		else 
		{
			vcMin.x = aabb.vcMax.x;
			vcMax.x = aabb.vcMin.x;
		}
		if (m_Planes[i].planeNormal.y >= 0.0f) 
		{
			vcMin.y = aabb.vcMin.y;
			vcMax.y = aabb.vcMax.y;
		}
		else 
		{
			vcMin.y = aabb.vcMax.y;
			vcMax.y = aabb.vcMin.y;
		}
		if (m_Planes[i].planeNormal.z >= 0.0f) 
		{
			vcMin.z = aabb.vcMin.z;
			vcMax.z = aabb.vcMax.z;
		}
		else 
		{
			vcMin.z = aabb.vcMax.z;
			vcMax.z = aabb.vcMin.z;
		}
		
		if ( (dot(m_Planes[i].planeNormal, vcMin) + m_Planes[i].distance) > 0.0f)
			return false;
	} // for
	
	return true;
}

//
// Frustum::Inside		//Aabb
//
bool Frustum::Inside(const Aabb &aabb, const Mat4x4 &transform) const
{
	Vec3 vcMin, vcMax;

	if(aabb.isInfinite)
		return true;

	// build and test extreme points
	for (int i=0; i<6; i++) 
	{
		Vec3 planeNormalT = (transform * Vec4(m_Planes[i].planeNormal, 0.0f)).xyz(); //Se supone que nunca será escalado -> Evita normalizacion
		Vec3 planePointT = (transform * Vec4(m_Planes[i].planePoint, 1.0f)).xyz();

		float distanceT = dot(planeNormalT, planePointT);

		/*if(i==0)
		{
			printf("Plane Distance: ( %f ) \n", distanceT);
			printf("Plane Point: ( %f, %f, %f ) \n", planePointT.x, planePointT.y, planePointT.z);
			printf("Plane Normal: ( %f, %f, %f ) \n", planeNormalT.x, planeNormalT.y, planeNormalT.z);
		}*/

		if (planeNormalT.x <= 0.0f) 
		{
			vcMin.x = aabb.vcMin.x;
			vcMax.x = aabb.vcMax.x;
		}
		else 
		{
			vcMin.x = aabb.vcMax.x;
			vcMax.x = aabb.vcMin.x;
		}
		if (planeNormalT.y <= 0.0f) 
		{
			vcMin.y = aabb.vcMin.y;
			vcMax.y = aabb.vcMax.y;
		}
		else 
		{
			vcMin.y = aabb.vcMax.y;
			vcMax.y = aabb.vcMin.y;
		}
		if (planeNormalT.z <= 0.0f) 
		{
			vcMin.z = aabb.vcMin.z;
			vcMax.z = aabb.vcMax.z;
		}
		else 
		{
			vcMin.z = aabb.vcMax.z;
			vcMax.z = aabb.vcMin.z;
		}
		
		if ( (dot(planeNormalT, vcMin) - distanceT) < 0.0f)
		{
			//printf("Plane: %d \n", i);
			return false;
		}
	} // for
	
	return true;
}

bool Frustum::Inside(const Aabb &aabb, const Mat4x4 &frustumMat, const Mat4x4 &aabbMat) const
{
	Vec3 vcMin, vcMax;

	if(aabb.isInfinite)
		return true;

	Mat4x4 transform = aabbMat*frustumMat;

	// build and test extreme points
	for (int i=0; i<6; i++) 
	{
		Vec3 planeNormalT = (transform * Vec4(m_Planes[i].planeNormal, 0.0f)).xyz(); //Se supone que nunca será escalado -> Evita normalizacion
		Vec3 planePointT = (transform * Vec4(m_Planes[i].planePoint, 1.0f)).xyz();

		float distanceT = dot(planeNormalT, planePointT);

		/*if(i==0)
		{
			printf("Plane Distance: ( %f ) \n", distanceT);
			printf("Plane Point: ( %f, %f, %f ) \n", planePointT.x, planePointT.y, planePointT.z);
			printf("Plane Normal: ( %f, %f, %f ) \n", planeNormalT.x, planeNormalT.y, planeNormalT.z);
		}*/

		if (planeNormalT.x <= 0.0f) 
		{
			vcMin.x = aabb.vcMin.x;
			vcMax.x = aabb.vcMax.x;
		}
		else 
		{
			vcMin.x = aabb.vcMax.x;
			vcMax.x = aabb.vcMin.x;
		}
		if (planeNormalT.y <= 0.0f) 
		{
			vcMin.y = aabb.vcMin.y;
			vcMax.y = aabb.vcMax.y;
		}
		else 
		{
			vcMin.y = aabb.vcMax.y;
			vcMax.y = aabb.vcMin.y;
		}
		if (planeNormalT.z <= 0.0f) 
		{
			vcMin.z = aabb.vcMin.z;
			vcMax.z = aabb.vcMax.z;
		}
		else 
		{
			vcMin.z = aabb.vcMax.z;
			vcMax.z = aabb.vcMin.z;
		}
		
		if ( (dot(planeNormalT, vcMin) - distanceT) < 0.0f)
		{
			//printf("Plane: %d \n", i);
			return false;
		}
	} // for
	
	return true;
}

bool Frustum::Intersects(const Mat4x4 &transform, const Ray &ray, const float length) const
{
    Vec3 pointA = ray.m_vcOrig;
    Vec3 pointB = ray.m_vcOrig + ray.m_vcDir*length;

    for(int i=0; i<6; i++)
    {
		Plane planeT;
		planeT.planeNormal = (transform * Vec4(m_Planes[i].planeNormal, 0.0f)).xyz();
		planeT.planePoint = (transform * Vec4(m_Planes[i].planePoint, 1.0f)).xyz();
		planeT.distance = -dot(planeT.planeNormal, planeT.planePoint);
        
        PointInPlane cA = planeT.Classify(pointA);
        PointInPlane cB = planeT.Classify(pointB);

        if (cA == BACK_PLANE && cA == BACK_PLANE) return false; // completely outside
        if (cA == FRONT_PLANE && cB == FRONT_PLANE)   continue; // both points are inside, no clip

        Vec3 intersectPoint;
        ray.Intersects(planeT, false, length, NULL, &intersectPoint);

        if (cA == BACK_PLANE)
        {
            pointA = intersectPoint;
        }
        else
        {
            pointB = intersectPoint;
        }
    }

	return true;
}

//
// Frustum::Inside		//Frustum
//
bool Frustum::Inside(const Mat4x4 &transform1, const Frustum &frustum, const Mat4x4 &transform2) const
{
	Vec3 frustumPointA[8];
	Vec3 frustumPointB[8];

    Ray frustumEdgeA[12];
    Ray frustumEdgeB[12];

	for(int i=0; i<4; i++)
	{
		frustumPointA[i] = (transform1 * Vec4(m_NearClip[i], 1.0f)).xyz();
		
		if(Inside(frustumPointA[i], transform2))
			return true;
	}

	for(int i=0; i<4; i++)
	{
		frustumPointA[4 + i] = (transform1 * Vec4(m_FarClip[i], 1.0f)).xyz();
		
		if(Inside(frustumPointA[4 + i], transform2))
			return true;
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////

	for(int i=0; i<4; i++)
	{
		frustumPointB[i] = (transform2 * Vec4(frustum.m_NearClip[i], 1.0f)).xyz();
		
		if(Inside(frustumPointB[i], transform1))
			return true;
	}

	for(int i=0; i<4; i++)
	{
		frustumPointB[4 + i] = (transform2 * Vec4(frustum.m_FarClip[i], 1.0f)).xyz();
		
		if(Inside(frustumPointB[4 + i], transform1))
			return true;
	}

	///////////////////////////////////////////////////////////////////////////////////////////

    frustumEdgeA[0].Set(frustumPointA[0], frustumPointA[1] - frustumPointA[0]);
    frustumEdgeA[1].Set(frustumPointA[1], frustumPointA[2] - frustumPointA[1]);
    frustumEdgeA[2].Set(frustumPointA[2], frustumPointA[3] - frustumPointA[2]);
    frustumEdgeA[3].Set(frustumPointA[3], frustumPointA[0] - frustumPointA[3]);
    frustumEdgeA[4].Set(frustumPointA[4], frustumPointA[5] - frustumPointA[4]);
    frustumEdgeA[5].Set(frustumPointA[5], frustumPointA[6] - frustumPointA[5]);
    frustumEdgeA[6].Set(frustumPointA[6], frustumPointA[7] - frustumPointA[6]);
    frustumEdgeA[7].Set(frustumPointA[7], frustumPointA[4] - frustumPointA[7]);
    frustumEdgeA[8].Set(frustumPointA[0], frustumPointA[4] - frustumPointA[0]);
    frustumEdgeA[9].Set(frustumPointA[1], frustumPointA[5] - frustumPointA[1]);
    frustumEdgeA[10].Set(frustumPointA[2], frustumPointA[6] - frustumPointA[2]);
    frustumEdgeA[11].Set(frustumPointA[3], frustumPointA[7] - frustumPointA[3]);

    for(int i=0; i<12; i++)
    {
        float edgeLength = length(frustumEdgeA[i].m_vcDir);
        frustumEdgeA[i].m_vcDir /= edgeLength;
        if(frustum.Intersects(transform2, frustumEdgeA[i], edgeLength))
            return true;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    frustumEdgeB[0].Set(frustumPointB[0], frustumPointB[1] - frustumPointB[0]);
    frustumEdgeB[1].Set(frustumPointB[1], frustumPointB[2] - frustumPointB[1]);
    frustumEdgeB[2].Set(frustumPointB[2], frustumPointB[3] - frustumPointB[2]);
    frustumEdgeB[3].Set(frustumPointB[3], frustumPointB[0] - frustumPointB[3]);
    frustumEdgeB[4].Set(frustumPointB[4], frustumPointB[5] - frustumPointB[4]);
    frustumEdgeB[5].Set(frustumPointB[5], frustumPointB[6] - frustumPointB[5]);
    frustumEdgeB[6].Set(frustumPointB[6], frustumPointB[7] - frustumPointB[6]);
    frustumEdgeB[7].Set(frustumPointB[7], frustumPointB[4] - frustumPointB[7]);
    frustumEdgeB[8].Set(frustumPointB[0], frustumPointB[4] - frustumPointB[0]);
    frustumEdgeB[9].Set(frustumPointB[1], frustumPointB[5] - frustumPointB[1]);
    frustumEdgeB[10].Set(frustumPointB[2], frustumPointB[6] - frustumPointB[2]);
    frustumEdgeB[11].Set(frustumPointB[3], frustumPointB[7] - frustumPointB[3]);

    for(int i=0; i<12; i++)
    {
        float edgeLength = length(frustumEdgeB[i].m_vcDir);
        frustumEdgeB[i].m_vcDir /= edgeLength;
        if(Intersects(transform1, frustumEdgeB[i], edgeLength))
            return true;
    }

    return false;
}

//
// Frustum::Inside					
//
bool Frustum::Inside(const Vec3 &point, const float radius) const
{
	for(int i = 0; i < NumPlanes; ++i) 
	{	
		if (!m_Planes[i].Inside(point, radius))
			return false;
	}	
	
	// otherwise we are fully in view
	return(true);
}

//
// Frustum::Init	//Perspective				
//
void Frustum::Init(const float fov, const float aspect, const float nearClip, const float farClip)
{
	m_Fov = fov;
	m_Aspect = aspect;
	m_Near = nearClip;
	m_Far = farClip;

	float tanFovOver2 = (float)tan(m_Fov/2.0f);
	Vec3 nearRight = (m_Near * tanFovOver2) * m_Aspect * g_Right;
	Vec3 farRight = (m_Far * tanFovOver2) * m_Aspect * g_Right;
	Vec3 nearUp = (m_Near * tanFovOver2 ) * g_Up;
	Vec3 farUp = (m_Far * tanFovOver2)  * g_Up;

	// points start in the upper right and go around clockwise
	m_NearClip[0] = (m_Near * g_Forward) - nearRight + nearUp;
	m_NearClip[1] = (m_Near * g_Forward) + nearRight + nearUp;
	m_NearClip[2] = (m_Near * g_Forward) + nearRight - nearUp;
	m_NearClip[3] = (m_Near * g_Forward) - nearRight - nearUp;

	m_FarClip[0] = (m_Far * g_Forward) - farRight + farUp;
	m_FarClip[1] = (m_Far * g_Forward) + farRight + farUp;
	m_FarClip[2] = (m_Far * g_Forward) + farRight - farUp;
	m_FarClip[3] = (m_Far * g_Forward) - farRight - farUp;

	// now we have all eight points. Time to construct 6 planes.
	// the normals point away from you if you use counter clockwise verts.

	Vec3 origin(0.0f, 0.0f, 0.0f);
	m_Planes[Near].Set(m_NearClip[2], m_NearClip[1], m_NearClip[0]);
	m_Planes[Far].Set(m_FarClip[0], m_FarClip[1], m_FarClip[2]);
	m_Planes[Right].Set(m_FarClip[2], m_FarClip[1], origin);
	m_Planes[Top].Set(m_FarClip[1], m_FarClip[0], origin);
	m_Planes[Left].Set(m_FarClip[0], m_FarClip[3], origin);
	m_Planes[Bottom].Set(m_FarClip[3], m_FarClip[2], origin);
}

//
// Frustum::Init	//Orthographic				
//
void Frustum::Init(const float height, const float width, const float aspect, const float nearClip, const float farClip)
{
	m_Height = height;
	m_Width = width;
	m_Aspect = aspect;
	m_Near = nearClip;
	m_Far = farClip;

	Vec3 nearRight = (m_Width/2.0f) * m_Aspect * g_Right;
	Vec3 farRight = (m_Width/2.0f) * m_Aspect * g_Right;
	Vec3 nearUp = (m_Height/2.0f) * g_Up;
	Vec3 farUp = (m_Height/2.0f)  * g_Up;

	// points start in the upper right and go around clockwise
	m_NearClip[0] = (m_Near * g_Forward) - nearRight + nearUp;
	m_NearClip[1] = (m_Near * g_Forward) + nearRight + nearUp;
	m_NearClip[2] = (m_Near * g_Forward) + nearRight - nearUp;
	m_NearClip[3] = (m_Near * g_Forward) - nearRight - nearUp;

	m_FarClip[0] = (m_Far * g_Forward) - farRight + farUp;
	m_FarClip[1] = (m_Far * g_Forward) + farRight + farUp;
	m_FarClip[2] = (m_Far * g_Forward) + farRight - farUp;
	m_FarClip[3] = (m_Far * g_Forward) - farRight - farUp;

	// now we have all eight points. Time to construct 6 planes.
	// the normals point away from you if you use counter clockwise verts.

	Vec3 origin(0.0f, 0.0f, 0.0f);
	m_Planes[Near].Set(m_NearClip[2], m_NearClip[1], m_NearClip[0]);
	m_Planes[Far].Set(m_FarClip[0], m_FarClip[1], m_FarClip[2]);
	m_Planes[Right].Set(m_FarClip[2], m_FarClip[1], m_NearClip[1]);
	m_Planes[Top].Set(m_FarClip[1], m_FarClip[0], m_NearClip[0]);
	m_Planes[Left].Set(m_FarClip[0], m_FarClip[3], m_NearClip[3]);
	m_Planes[Bottom].Set(m_FarClip[3], m_FarClip[2], m_NearClip[2]);
}

Aabb Frustum::GetFrustumAabb()
{
	Aabb frustumAabb;

	frustumAabb.vcMax = m_FarClip[1];
	frustumAabb.vcMin.z = m_NearClip[0].z;
	frustumAabb.vcMin.x = m_FarClip[3].x;
	frustumAabb.vcMin.y = m_FarClip[3].y;

	frustumAabb.vcCenter = (frustumAabb.vcMax + frustumAabb.vcMin)*0.5f;

	return frustumAabb;
}


