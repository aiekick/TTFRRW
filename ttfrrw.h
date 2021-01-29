#pragma once

#include <cstdint>
#include <vector>
#include <deque>
#include <string>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <stdarg.h> // variadic
#include <utility> // pair
#include <cmath>
#include <chrono> // profiler
#include <thread>
#include <atomic>

namespace TTFRRW
{
	typedef uint16_t CodePoint;
	typedef uint16_t GlyphIndex;
	typedef uint16_t PaletteIndex;

	typedef int ttfrrwProcessingFlags;
	enum ttfrrwProcessingFlags_
	{
		TTFRRW_PROCESSING_FLAG_NONE = 0,
		TTFRRW_PROCESSING_FLAG_NO_GLYPH_PARSING = (1 << 0), // on ne parse pas les points, on prend juste des stats de bases
		TTFRRW_PROCESSING_FLAG_VERBOSE_ONLY_ERRORS = (1 << 1), // print only the errors to the console
		TTFRRW_PROCESSING_FLAG_VERBOSE_PROFILER = (1 << 2), // print profiler
		TTFRRW_PROCESSING_FLAG_NO_ERRORS = (1 << 3), // print no erros
	};

	///////////////////////////////////////////////////////////////////////
	///// PROFILER ////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////

	class cProfiler
	{
	private:
		int64_t firstTimeMark = 0;
		double value = 0.0;
		size_t count = 0;

	public:
		void start();
		void end();
		void reset();
		double result_Average();
		double result_Full();
		size_t result_Count();
		void print(ttfrrwProcessingFlags vFlags, const char* parent, const char* label);
		void erasePrint(ttfrrwProcessingFlags vFlags, const char* parent, const char* label); // clear console then print
	};

	///////////////////////////////////////////////////////////////////////
	///// COMMON///////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////

	template <typename T> inline T round(const T& v) { return (T)(::std::round((double)v)); }
	template <typename T> inline T floor(const T& v) { return (T)(::std::floor((double)v)); }
	template <typename T> inline T ceil(const T& v) { return (T)(::std::ceil((double)v)); }
	template <typename T> inline T fract(const T& v) { return v - floor<T>(v); }
	template <typename T> inline T cos(const T& v) { return ::std::cos(v); }
	template <typename T> inline T sin(const T& v) { return ::std::sin(v); }
	template <typename T> inline T tan(const T& v) { return ::std::tan(v); }
	template <typename T> inline T atan(const T& v) { return ::std::atan(v); }
	template <typename T> inline T mini(const T& a, const T& b) { return a < b ? a : b; }
	template <typename T> inline T maxi(const T& a, const T& b) { return a > b ? a : b; }
	template <typename T> inline T clamp(const T& n) { return n >= T(0) && n <= T(1) ? n : T(n > T(0)); } // clamp n => 0 to 1
	template <typename T> inline T clamp(const T& n, const T& b) { return n >= T(0) && n <= b ? n : T(n > T(0)) * b; } // clamp n => 0 to b
	template <typename T> inline T clamp(const T& n, const T& a, const T& b) { return n >= a && n <= b ? n : n < a ? a : b; } // clamp n => a to b
	template <typename T> inline T abs(const T& a) { return a < 0 ? a * (T)-1 : a; }
	template <typename T> inline T sign(const T& a) { return a < 0 ? (T)-1 : (T)1; }
	template <typename T> inline T step(const T& a, const T& b) { return b < a ? (T)0 : (T)1; }
	template <typename T> inline T mod(const T& v, const T& l) { return ::std::modf(v, l); }
	template <typename T> inline T invMix(const T& i, const T& s, const T& r) { return (r - i) / (s - i); }
	template <typename T> inline T lerp(const T& a, const T& b, const T& t) { return a * (1.0f - t) + b * t; }
	template <typename T> inline T eerp(const T& a, const T& b, const T& t) { if (a == (T)0) return T(0); return pow(a * (b / a), t); }
	template <typename T> inline T mix(const T& a, const T& b, const T& t) { return lerp(a, b, t); }

	///////////////////////////////////////////////////////////////////////
	///// VEC2 ////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////

	template <typename T>
	struct vec2
	{
		T x, y;
		vec2<T>() { x = (T)0; y = (T)0; }
		template <typename U> vec2<T>(vec2<U> a) { x = (T)a.x; y = (T)a.y; }
		vec2<T>(T a) { x = a; y = a; }
		vec2<T>(T a, T b) { x = a; y = b; }
		T operator () (size_t i) const { return (&x)[i]; }
		T operator [] (size_t i) const { return (&x)[i]; }
		T& operator () (size_t i) { return (&x)[i]; }
		void operator ++ () { x++; y++; }
		void operator -- () { x--; y--; }
		void operator += (const vec2<T>& v) { x += v.x; y += v.y; }
		void operator -= (const vec2<T>& v) { x -= v.x; y -= v.y; }
		bool operator == (const vec2<T>& v) { return (x == v.x) && (y == v.y); }
		bool operator != (const vec2<T>& v) { return (x != v.x) || (y != v.y); }
		void operator *= (T a) { x *= a; y *= a; }
		void operator *= (vec2<T> v) { x *= v.x; y *= v.y; }
		void operator /= (T a) { x /= a; y /= a; }
		void operator /= (vec2<T> v) { x /= v.x; y /= v.y; }
		T length() { return sqrt(lengthSquared()); }
		T lengthSquared() { return x * x + y * y; }
		T normalize() { T _length = length(); if (_length < (T)1e-5) return (T)0.0; T _invLength = (T)1.0 / _length; x *= _invLength; y *= _invLength; return _length; } // return length
		vec2<T> GetNormalized() { vec2<T> n = vec2<T>(x, y); n.normalize(); return n; }
		T sum() { return x + y; }
		T sumAbs() { return abs<T>(x) + abs<T>(y); }
		bool emptyAND() { return x == (T)0 && y == (T)0; }
		bool emptyOR() { return x == (T)0 || y == (T)0; }
		std::string string(char c = ';') { return toStr(x) + c + toStr(y); }
		T ratioXY() { if (y > (T)0) return x / y; return (T)0; }
		T ratioYX() { if (x > (T)0) return y / x; return (T)0; }
	};
	template <typename T> inline vec2<T> operator + (vec2<T> v, T f) { return vec2<T>(v.x + f, v.y + f); }
	template <typename T> inline vec2<T> operator + (T f, vec2<T> v) { return vec2<T>(v.x + f, v.y + f); }
	template <typename T> inline vec2<T> operator + (vec2<T> v, vec2<T> f) { return vec2<T>(v.x + f.x, v.y + f.y); }
	template <typename T> inline vec2<T> operator - (vec2<T> v, T f) { return vec2<T>(v.x - f, v.y - f); }
	template <typename T> inline vec2<T> operator - (T f, vec2<T> v) { return vec2<T>(f - v.x, f - v.y); }
	template <typename T> inline vec2<T> operator - (vec2<T> v, vec2<T> f) { return vec2<T>(v.x - f.x, v.y - f.y); }
	template <typename T> inline vec2<T> operator * (vec2<T> v, T f) { return vec2<T>(v.x * f, v.y * f); }
	template <typename T> inline vec2<T> operator * (T f, vec2<T> v) { return vec2<T>(v.x * f, v.y * f); }
	template <typename T> inline vec2<T> operator * (vec2<T> v, vec2<T> f) { return vec2<T>(v.x * f.x, v.y * f.y); }
	template <typename T> inline vec2<T> operator / (vec2<T> v, T f) { return vec2<T>(v.x / f, v.y / f); }
	template <typename T> inline vec2<T> operator / (T f, vec2<T> v) { return vec2<T>(f / v.x, f / v.y); }
	template <typename T> inline vec2<T> operator / (vec2<T> v, vec2<T> f) { return vec2<T>(v.x / f.x, v.y / f.y); }
	template <typename T> inline bool operator < (vec2<T> v, vec2<T> f) { return v.x < f.x&& v.y < f.y; }
	template <typename T> inline bool operator < (vec2<T> v, T f) { return v.x < f&& v.y < f; }
	template <typename T> inline bool operator > (vec2<T> v, vec2<T> f) { return v.x > f.x && v.y > f.y; }
	template <typename T> inline bool operator > (vec2<T> v, T f) { return v.x > f && v.y > f; }
	template <typename T> inline bool operator <= (vec2<T> v, vec2<T> f) { return v.x <= f.x && v.y <= f.y; }
	template <typename T> inline bool operator <= (vec2<T> v, T f) { return v.x <= f && v.y <= f; }
	template <typename T> inline bool operator >= (vec2<T> v, vec2<T> f) { return v.x >= f.x && v.y >= f.y; }
	template <typename T> inline bool operator >= (vec2<T> v, T f) { return v.x >= f && v.y >= f; }
	template <typename T> inline bool operator != (vec2<T> v, vec2<T> f) { return f.x != v.x || f.y != v.y; }
	template <typename T> inline bool operator == (vec2<T> v, vec2<T> f) { return f.x == v.x && f.y == v.y; }
	template <typename T> inline vec2<T> floor(vec2<T> a) { return vec2<T>(floor(a.x), floor(a.y)); }
	template <typename T> inline vec2<T> fract(vec2<T> a) { return vec2<T>(fract(a.x), fract(a.y)); }
	template <typename T> inline vec2<T> ceil(vec2<T> a) { return vec2<T>(ceil(a.x), ceil(a.y)); }
	template <typename T> inline vec2<T> mini(vec2<T> a, vec2<T> b) { return vec2<T>(mini(a.x, b.x), mini(a.y, b.y)); }
	template <typename T> inline vec2<T> maxi(vec2<T> a, vec2<T> b) { return vec2<T>(maxi(a.x, b.x), maxi(a.y, b.y)); }
	template <typename T> inline T dot(vec2<T> a, vec2<T> b) { return a.x * b.x + a.y * b.y; }
	template <typename T> inline T det(vec2<T> a, vec2<T> b) { return a.x * b.y - a.y * b.x; }
	template <typename T> inline vec2<T> cross(vec2<T> a, vec2<T> b) { return vec2<T>(a.x * b.y - a.y * b.x, a.y * b.x - a.x * b.y); }
	template <typename T> inline vec2<T> reflect(vec2<T> I, vec2<T> N) { return I - (T)2 * dotS(N, I) * N; }
	template <typename T> inline vec2<T> sign(vec2<T> a) { return vec2<T>(a.x < (T)0 ? -(T)1 : (T)1, a.y < (T)0 ? (T)-1 : (T)1); }
	template <typename T> inline vec2<T> sin(vec2<T> a) { return vec2<T>(sin<T>(a.x), sin<T>(a.y)); }
	template <typename T> inline vec2<T> cos(vec2<T> a) { return vec2<T>(cos<T>(a.x), cos<T>(a.y)); }
	template <typename T> inline vec2<T> tan(vec2<T> a) { return vec2<T>(tan<T>(a.x), tan<T>(a.y)); }
	template <typename T> inline vec2<T> atan(vec2<T> a) { return vec2<T>(atan<T>(a.x), atan<T>(a.y)); }
	typedef vec2<double> dvec2;
	typedef vec2<float> fvec2;
	typedef vec2<bool> bvec2;
	typedef vec2<int8_t> i8vec2;
	typedef vec2<int16_t> i16vec2;
	typedef vec2<int32_t> ivec2;
	typedef vec2<int32_t> i32vec2;
	typedef vec2<int64_t> i64vec2;
	typedef vec2<uint8_t> u8vec2;
	typedef vec2<uint16_t> u16vec2;
	typedef vec2<uint32_t> uvec2;
	typedef vec2<uint32_t> u32vec2;
	typedef vec2<uint64_t> u64vec2;
	
	///////////////////////////////////////////////////////////////////////
	///// AABB ////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////

	template <typename T>
	struct AABB // copy of b2AABB struct
	{
		vec2<T> lowerBound;	///< the lower left vertex
		vec2<T> upperBound;	///< the upper right vertex
		AABB() : lowerBound((T)0), upperBound((T)0) {}
		AABB(vec2<T> vlowerBound, vec2<T> vUpperBound) : lowerBound(vlowerBound), upperBound(vUpperBound) {}
		//may be in format "0.2f,0.3f,0.4f,0.8f" left, bottom, right, top
		AABB(::std::string vec, char c) { ::std::vector<float> result = StringToNumberVector<float>(vec, c); size_t s = result.size(); if (s > 0) lowerBound.x = result[0]; if (s > 1) lowerBound.y = result[1]; if (s > 2) upperBound.x = result[2]; if (s > 3) upperBound.y = result[3]; }
		/// Add a vector to this vector.
		void operator += (const vec2<T>& v) { lowerBound += v; upperBound += v; }
		/// Subtract a vector from this vector.
		void operator -= (const vec2<T>& v) { lowerBound -= v; upperBound -= v; }
		/// Multiply this vector by a scalar.
		void operator *= (T a) { lowerBound *= a; upperBound *= a; }
		/// Divide this vector by a scalar.
		void operator /= (T a) { lowerBound /= a; upperBound /= a; }
		/// Get the center of the AABB.
		vec2<T> GetCenter() const { return (lowerBound + upperBound) / (T)2; }
		/// Get the extents of the AABB (half-widths).
		vec2<T> GetExtents() const { return (upperBound - lowerBound) / (T)2; }
		/// Get the perimeter length
		T GetPerimeter() const { float wx = upperBound.x - lowerBound.x; float wy = upperBound.y - lowerBound.y; return (T)2 * (wx + wy); }
		/// Combine an AABB into this one.
		void Combine(const AABB<T>& aabb) { lowerBound = mini(lowerBound, aabb.lowerBound); upperBound = maxi(upperBound, aabb.upperBound); }
		/// Combine two AABBs into this one.
		void Combine(const AABB<T>& aabb1, const AABB<T>& aabb2) { lowerBound = mini(aabb1.lowerBound, aabb2.lowerBound); upperBound = maxi(aabb1.upperBound, aabb2.upperBound); }
		/// Combine a point into this one.
		void Combine(const vec2<T>& pt) { lowerBound.x = mini(lowerBound.x, pt.x); lowerBound.y = mini(lowerBound.y, pt.y); upperBound.x = maxi(upperBound.x, pt.x); upperBound.y = maxi(upperBound.y, pt.y); }
		/// Set vlowerBound at mini and vUpperBound at maxi
		void Set(vec2<T> vlowerBound, vec2<T> vUpperBound) { lowerBound.x = mini(vlowerBound.x, vUpperBound.x); lowerBound.y = mini(vlowerBound.y, vUpperBound.y); upperBound.x = maxi(vlowerBound.x, vUpperBound.x); upperBound.y = maxi(vlowerBound.y, vUpperBound.y); }
		/// Does this aabb contain the provided AABB.
		bool Contains(const AABB<T>& aabb) const { bool result = true; result = result && lowerBound.x <= aabb.lowerBound.x; result = result && lowerBound.y <= aabb.lowerBound.y; result = result && aabb.upperBound.x <= upperBound.x; result = result && aabb.upperBound.y <= upperBound.y; return result; }
		/// Does this aabb contain the provided vec2<T>.
		bool ContainsPoint(const vec2<T>& pt) { return lowerBound.x <= pt.x && lowerBound.y <= pt.y && pt.x <= upperBound.x && pt.y <= upperBound.y; }
		bool Intersects(const AABB<T>& other) { return lowerBound.x <= other.lowerBound.x || lowerBound.y <= other.lowerBound.y || other.upperBound.x <= upperBound.x || other.upperBound.y <= upperBound.y; }
		vec2<T> Size() { return vec2<T>(upperBound - lowerBound); }
	};
	typedef AABB<int> iAABB;
	typedef AABB<double> dAABB;
	typedef AABB<float> fAABB;

	///////////////////////////////////////////////////////////////////////
	///// VEc4 ////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////

	template <typename T>
	struct vec4
	{
		T x, y, z, w;
		vec4() : x((T)0), y((T)0), z((T)0), w((T)0) {}
		template <typename U> vec4<T>(vec4<U> a) { x = (T)a.x; y = (T)a.y; z = (T)a.z; w = (T)a.w; }
		vec4(vec2<T> xy, vec2<T> zw) : x(xy.x), y(xy.y), z(zw.x), w(zw.y) {}
		vec4(T xyzw) : x(xyzw), y(xyzw), z(xyzw), w(xyzw) {}
		vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
		void Set(T vX, T vY, T vZ, T vW) { x = vX; y = vY; z = vZ; w = vW; }
		vec4 operator -() const { return vec4(-x, -y, -z, -w); }
		vec2<T> SizeLBRT() { return vec2<T>(z - x, w - y); }// Considere vec4 as a rect with LBRT for have size LBRT Mean => x = Left, y = Bottom, z = Right, w = Top
		vec2<T> xy() { return vec2<T>(x, y); }
		vec2<T> zw() { return vec2<T>(z, w); }
		vec2<T> pos() { return xy(); }
		vec2<T> size() { return zw(); }
		T operator [] (const size_t& i)
		{
			switch (i)
			{
			case 0: return x;
			case 1: return y;
			case 2: return z;
			case 3: return w;
			}
			assert(false);
			return 0;
		}
		void Set(const size_t& i, T v)
		{
			switch (i)
			{
			case 0: x = v; break;
			case 1: y = v; break;
			case 2: z = v; break;
			case 3: w = v; break;
			}
		}
		void operator += (const vec4& v) { x += v.x; y += v.y; z += v.z; w += v.w; }
		bool operator == (const vec4& v) { return (x == v.x && y == v.y && z == v.z && w == v.w); }
		bool operator != (const vec4& v) { return (x != v.x || y != v.y || z != v.z || w != v.w); }
		void operator -= (const vec4& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; }
		void operator *= (T a) { x *= a; y *= a; z *= a; w *= a; }
		void operator /= (T a) { x /= a; y /= a; z /= a; w /= a; }
		T length() { return sqrtf(lengthSquared()); }
		T lengthSquared() { return x * x + y * y + z * z + w * w; }
		T normalize() { T _length = length(); if (_length < (T)1e-5)return (T)0; T _invLength = (T)1 / _length; x *= _invLength; y *= _invLength; z *= _invLength; w *= _invLength; return _length; }
		vec4<T> GetNormalized() { vec4<T> n = vec4<T>(x, y, z, w); n.normalize(); return n; }
		bool emptyAND() { return x == (T)0 && y == (T)0 && z == (T)0 && w == (T)0; }
		bool emptyOR() { return x == (T)0 || y == (T)0 || z == (T)0 || w == (T)0; }
		T sum() { return x + y + z + w; }
		T sumAbs() { return abs<T>(x) + abs<T>(y) + abs<T>(z) + abs<T>(w); }
		std::string string(char c = ';') { return toStr(x) + c + toStr(y) + c + toStr(z) + c + toStr(w); }
	};
	template <typename T> inline vec4<T> operator + (vec4<T> v, T f) { return vec4<T>(v.x + f, v.y + f, v.z + f, v.w + f); }
	template <typename T> inline vec4<T> operator + (vec4<T> v, vec4<T> f) { return vec4<T>(v.x + f.x, v.y + f.y, v.z + f.z, v.w + f.w); }
	template <typename T> inline vec4<T> operator - (vec4<T> v, T f) { return vec4<T>(v.x - f, v.y - f, v.z - f, v.w - f); }
	template <typename T> inline vec4<T> operator - (T f, vec4<T> v) { return vec4<T>(f - v.x, f - v.y, f - v.z, f - v.w); }
	template <typename T> inline vec4<T> operator - (vec4<T> v, vec4<T> f) { return vec4<T>(v.x - f.x, v.y - f.y, v.z - f.z, v.w - f.w); }
	template <typename T> inline vec4<T> operator * (vec4<T> v, T f) { return vec4<T>(v.x * f, v.y * f, v.z * f, v.w * f); }
	template <typename T> inline vec4<T> operator * (vec4<T> v, vec4<T> f) { return vec4<T>(v.x * f.x, v.y * f.y, v.z * f.z, v.w * f.w); }
	template <typename T> inline vec4<T> operator / (vec4<T> v, T f) { return vec4<T>(v.x / f, v.y / f, v.z / f, v.w / f); }
	template <typename T> inline vec4<T> operator / (vec4<T> v, vec4<T> f) { return vec4<T>(v.x / f.x, v.y / f.y, v.z / f.z, v.w / f.w); }
	template <typename T> inline bool operator < (vec4<T> v, vec4<T> f) { return v.x < f.x&& v.y < f.y&& v.z < f.z&& v.w < f.w; }
	template <typename T> inline bool operator < (vec4<T> v, T f) { return v.x < f&& v.y < f&& v.z < f&& v.w < f; }
	template <typename T> inline bool operator > (vec4<T> v, vec4<T> f) { return v.x > f.x && v.y > f.y && v.z > f.z && v.w > f.w; }
	template <typename T> inline bool operator > (vec4<T> v, T f) { return v.x > f && v.y > f && v.z > f && v.w > f; }
	template <typename T> inline bool operator <= (vec4<T> v, vec4<T> f) { return v.x <= f.x && v.y <= f.y && v.z <= f.z && v.w <= f.w; }
	template <typename T> inline bool operator <= (vec4<T> v, T f) { return v.x <= f && v.y <= f && v.z <= f && v.w <= f; }
	template <typename T> inline bool operator >= (vec4<T> v, vec4<T> f) { return v.x >= f.x && v.y >= f.y && v.z >= f.z && v.w >= f.w; }
	template <typename T> inline bool operator >= (vec4<T> v, T f) { return v.x >= f && v.y >= f && v.z >= f && v.w >= f; }
	template <typename T> inline bool operator == (vec4<T> v, vec4<T> f) { return f.x == v.x && f.y == v.y && f.z == v.z && f.w == v.w; }
	template <typename T> inline bool operator != (vec4<T> v, vec4<T> f) { return f.x != v.x || f.y != v.y || f.z != v.z || f.w != v.w; }
	template <typename T> inline vec4<T> mini(vec4<T> a, vec4<T> b) { return vec4<T>(mini<T>(a.x, b.x), mini<T>(a.y, b.y), mini<T>(a.z, b.z), mini<T>(a.w, b.w)); }
	template <typename T> inline vec4<T> maxi(vec4<T> a, vec4<T> b) { return vec4<T>(maxi<T>(a.x, b.x), maxi<T>(a.y, b.y), maxi<T>(a.z, b.z), maxi<T>(a.w, b.w)); }
	template <typename T> inline vec4<T> floor(vec4<T> a) { return vec4<T>(floor<T>(a.x), floor<T>(a.y), floor<T>(a.z), floor<T>(a.w)); }
	template <typename T> inline vec4<T> ceil(vec4<T> a) { return vec4<T>(ceil<T>(a.x), ceil<T>(a.y), ceil<T>(a.z), ceil<T>(a.w)); }
	template <typename T> inline vec4<T> abs(vec4<T> a) { return vec4<T>(abs<T>(a.x), abs<T>(a.y), abs<T>(a.z), abs<T>(a.w)); }
	template <typename T> inline vec4<T> sign(vec4<T> a) { return vec4<T>(a.x < (T)0 ? (T)-1 : (T)1, a.y < (T)0 ? (T)-1 : 1, a.z < (T)0 ? (T)-1 : (T)1, a.w < (T)0 ? (T)-1 : (T)1); }
	template <typename T> inline vec4<T> sin(vec4<T> a) { return vec4<T>(sin<T>(a.x), sin<T>(a.y), sin<T>(a.z), sin<T>(a.w)); }
	template <typename T> inline vec4<T> cos(vec4<T> a) { return vec4<T>(cos<T>(a.x), cos<T>(a.y), cos<T>(a.z), cos<T>(a.w)); }
	template <typename T> inline vec4<T> tan(vec4<T> a) { return vec4<T>(tan<T>(a.x), tan<T>(a.y), tan<T>(a.z), tan<T>(a.w)); }
	typedef vec4<double> dvec4;
	typedef vec4<float> fvec4;
	typedef vec4<bool> bvec4;
	typedef vec4<int> ivec4;
	typedef vec4<int8_t> i8vec4;
	typedef vec4<int16_t> i16vec4;
	typedef vec4<int32_t> ivec4;
	typedef vec4<int32_t> i32vec4;
	typedef vec4<int64_t> i64vec4;
	typedef vec4<uint8_t> u8vec4;
	typedef vec4<uint16_t> u16vec4;
	typedef vec4<uint32_t> uvec4;
	typedef vec4<uint32_t> u32vec4;
	typedef vec4<uint64_t> u64vec4;

	///////////////////////////////////////////////////////////////////////
	///// MEMORY STREAM ///////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////

	class MemoryStream
	{
	public:
		struct Fixed // its like a float with int32 : high.low ex : 1 high 0 low => 1.0
		{
			int16_t high = 0;
			int16_t low = 0;
		};
		struct F2DOT14
		{
			int16_t value = 0;
			F2DOT14() { value = 0; }
			F2DOT14(int16_t v) { value = v; }
			void operator = (int16_t v) { value = v; }
			void SetFloat(float vValue) { value = (int16_t)roundf(vValue * 16384.f); }
			float GetFloat() { return (float)(value >> 14) + (float)(value & 0x3FFF) / 16384.0f; }
		};
		struct bitfield24 
		{
			uint32_t value : 24;
			bitfield24()
			{
				value = 0;
			}
			bitfield24(uint32_t vValue)
			{
				value = vValue;
			}
		};
		typedef uint16_t UFWord;
		typedef int16_t FWord;
		typedef int64_t longDateTime;
		typedef bitfield24 uint24_t;

	public:
		MemoryStream();
		MemoryStream(uint8_t* vDatas, size_t vSize);
		~MemoryStream();

		void WriteByte(uint8_t b);
		void WriteBytes(std::vector<uint8_t>* buffer);
		void WriteShort(int32_t i);
		void WriteUShort(int32_t us);
		void WriteFWord(int32_t us);
		void WriteInt(int32_t i);
		void WriteUInt24(int32_t ui);
		void WriteULong(int64_t ul);
		void WriteLong(int64_t l);
		void WriteFixed(Fixed f);
		void WriteF2DOT14(F2DOT14 f);
		void WriteDateTime(longDateTime date);

		uint8_t* Get();
		void Set(uint8_t* vDatas, size_t vSize);

		size_t Size();

		size_t GetPos();
		void SetPos(size_t vPos);

		uint8_t ReadByte(size_t vOffset = 0);
		int32_t ReadUShort(size_t vOffset = 0);
		int32_t ReadShort(size_t vOffset = 0);
		FWord ReadFWord(size_t vOffset = 0);
		UFWord ReadUFWord(size_t vOffset = 0);
		uint32_t ReadUInt24(size_t vOffset = 0);
		uint64_t ReadULong(size_t vOffset = 0);
		uint32_t ReadULongAsInt(size_t vOffset = 0);
		int32_t ReadLong(size_t vOffset = 0);
		Fixed ReadFixed(size_t vOffset = 0);
		F2DOT14 ReadF2DOT14(size_t vOffset = 0);
		longDateTime ReadDateTime(size_t vOffset = 0);
		std::string ReadString(size_t vLen, size_t vOffset = 0);

	private:
		std::vector<uint8_t> m_Datas;
		size_t m_ReadPos = 0;
	};

	///////////////////////////////////////////////////////////////////////
	///// GLYPH ///////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////

	class Contour
	{
	public:
		std::vector<ivec2> m_Points;
		std::vector<bool> m_OnCurve;

	public:
		bool IsValid() { return !m_Points.empty() && !m_OnCurve.empty(); }
		bool IsOnCurve(size_t vIdx)
		{
			size_t count = m_OnCurve.size();
			if (count)
				return m_OnCurve[vIdx % count];
			return false;
		}
		fvec2 GetCoords(size_t vIdx, fvec2 vScale, fvec2 vTranslation)
		{
			size_t count = m_Points.size();
			if (count)
			{
				const auto p = m_Points[vIdx % count];
				return fvec2((float)p.x, (float)p.y) * vScale + vTranslation;
			}
				
			return fvec2();
		}
	};

	class ComposedGlyph
	{
	public:
		fvec2 m_Translation; // if composite
		fvec2 m_Scale; // if composite
		GlyphIndex m_GlyphIndex = 0;
	};

	class Glyph
	{
	public:
		std::vector<Contour> m_Contours;
		iAABB m_LocalBBox;
		int32_t m_AdvanceX = 0;
		int32_t m_LeftSideBearing = 0;
		int32_t m_RightSideBearing = 0;
		std::string m_Name;
		bool m_IsSimple = true; // simple or composite

		// layer
		fvec4 m_Color = 1.0f; // color if is a layer
		PaletteIndex m_PaletteIndex = 0; // palette if is a layer
		bool m_IsLayer = false; // layer

		std::vector<GlyphIndex> m_Layers; // color layers
		std::vector<ComposedGlyph> m_ComposedGlyph; // for composite

	public:
		bool IsValid()
		{
			for (auto& c : m_Contours)
			{
				bool v = c.IsValid();
				if (v) // au moins un a dessiner on stop la
					return v;
			}
			return false;
		}
	};

	///////////////////////////////////////////////////////////////////////
	///// MAIN CLASS TTFRRW ///////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////

	class TTFInfos
	{
	public:
		uint32_t m_GlyphCount = 0;
		iAABB m_GlobalBBox;
		int16_t m_Ascent = 0;
		int16_t m_Descent = 0;
		int16_t m_LineGap = 0;
		uint16_t m_AdvanceWidthMax = 0;
		int16_t m_MinLeftSideBearing = 0;
		int16_t m_MinRightSideBearing = 0;
		int16_t m_XMaxExtent = 0;
	};

	class TTFProfiler
	{
	public:
		cProfiler simpleGlyfProfiler;

	public:
		void Reset()
		{
			simpleGlyfProfiler.reset();
		}

		void Print(ttfrrwProcessingFlags vFlags)
		{
			simpleGlyfProfiler.print(vFlags, "Parse_Simple_Glyf", "");
		}
	};

#define TTFRRW_ATOMIC_PARAMS std::atomic<bool>* vWorking, std::atomic<float>* vProgress, std::atomic<uint32_t>* vObjectCount
#define TTFRRW_ATOMIC_PARAMS_DEFAULT std::atomic<bool>* vWorking = 0, std::atomic<float>* vProgress = 0, std::atomic<uint32_t>* vObjectCount = 0
#define TTFRRW_ATOMIC_PARAMS_BY_REF std::ref(vWorking), std::ref(vProgress), std::ref(vObjectCount)

	class TTFRRW
	{
	private:
		TTFInfos m_TTFInfos;
		TTFProfiler m_TTFProfiler;
		bool m_IsValid_For_Rasterize = false;
		bool m_IsValid_For_GlyphTreatment = false;
		std::string m_FontType;

	private: // must be defined by user
		std::vector<Glyph> m_Glyphs; // bd des glyphs
		std::vector<std::string> m_GlyphNames; // bd des noms
		// 1 codePoint => 1 glyphIndex
		std::map<CodePoint, GlyphIndex> m_CodePoint_To_GlyphIndex;
		// 1 glyphIndex => can be many codePoint's
		std::map<GlyphIndex, std::set<CodePoint>> m_GlyphIndex_To_CodePoints;
		// nameId => names
		std::set<std::pair<uint16_t, std::string>> m_Names; // bd des noms depuis la table NAME

	public:
		TTFRRW();
		~TTFRRW();

		bool OpenFontFile(const std::string& vFontFilePathName, 
			ttfrrwProcessingFlags vFlags = 0, 
			const char* vDebugInfos = "", 
			TTFRRW_ATOMIC_PARAMS_DEFAULT);
		bool OpenFontStream(uint8_t* vStream, size_t vStreamSize, 
			ttfrrwProcessingFlags vFlags = 0, 
			const char* vDebugInfos = "",
			TTFRRW_ATOMIC_PARAMS_DEFAULT);
		std::vector<Glyph>* GetGlyphs();
		Glyph* GetGlyphWithGlyphIndex(const GlyphIndex& vGlyphIndex);
		Glyph* GetGlyphWithCodePoint(const CodePoint& vCodePoint);
		GlyphIndex GetGlyphIndexFromCodePoint(const CodePoint& vCodePoint);
		std::set<CodePoint>* GetCodePointsFromGlyphIndex(const GlyphIndex& vGlyphIndex);
		TTFInfos GetFontInfos();

		bool IsValidForRasterize();
		bool IsValidFotGlyppTreatment();

		bool WriteFontFile(const std::string& vFontFilePathName);
		void AddGlyph(const Glyph& vGlyph, const CodePoint& vCodePoint);
		
	//////////////////////////////////////////////////////////////////////////////
	//// READ TABLE //////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	public: // read table
		struct TableStruct
		{
			uint8_t tag[5] = {};
			uint32_t checkSum = 0;
			size_t offset = 0;
			size_t length = 0;
		};

	private: // read table
		std::unordered_map<std::string, TableStruct> m_Tables;
		uint16_t m_IndexToLocFormat = 0; // head table : loca format
		std::vector<size_t> m_GlyphsOffsets; // loca table : glyphs address
		std::vector<std::vector<fvec4>> m_Palettes; // palette > colors > color
		int16_t m_MumOfLongHorMetrics = 0; // fromm hhea for hmtx

		void Clear(TTFRRW_ATOMIC_PARAMS);
		bool LoadFileToMemory(const std::string& vFilePathName, MemoryStream* vInMem, int* vError);
		bool Parse_Font_File(MemoryStream* vInMem, ttfrrwProcessingFlags vFlags, TTFRRW_ATOMIC_PARAMS);
		bool Parse_Table_Header(MemoryStream* vInMem, ttfrrwProcessingFlags vFlags, TTFRRW_ATOMIC_PARAMS);
		bool Parse_CMAP_Table(MemoryStream* vInMem, ttfrrwProcessingFlags vFlags, TTFRRW_ATOMIC_PARAMS);
		bool Parse_HEAD_Table(MemoryStream* vInMem, ttfrrwProcessingFlags vFlags, TTFRRW_ATOMIC_PARAMS);
		bool Parse_LOCA_Table(MemoryStream* vInMem, ttfrrwProcessingFlags vFlags, TTFRRW_ATOMIC_PARAMS);
		bool Parse_MAXP_Table(MemoryStream* vInMem, ttfrrwProcessingFlags vFlags, TTFRRW_ATOMIC_PARAMS);
		bool Parse_GLYF_Table(MemoryStream* vInMem, ttfrrwProcessingFlags vFlags, TTFRRW_ATOMIC_PARAMS);
		Glyph Parse_Simple_Glyf(MemoryStream* vInMem, GlyphIndex vGlyphIndex, int16_t vCountContour, ttfrrwProcessingFlags vFlags, TTFRRW_ATOMIC_PARAMS);
		bool Parse_POST_Table(MemoryStream* vInMem, ttfrrwProcessingFlags vFlags, TTFRRW_ATOMIC_PARAMS);
		bool Parse_CPAL_Table(MemoryStream* vInMem, ttfrrwProcessingFlags vFlags, TTFRRW_ATOMIC_PARAMS);
		bool Parse_COLR_Table(MemoryStream* vInMem, ttfrrwProcessingFlags vFlags, TTFRRW_ATOMIC_PARAMS);
		bool Parse_HHEA_Table(MemoryStream* vInMem, ttfrrwProcessingFlags vFlags, TTFRRW_ATOMIC_PARAMS);
		bool Parse_HMTX_Table(MemoryStream* vInMem, ttfrrwProcessingFlags vFlags, TTFRRW_ATOMIC_PARAMS);
		bool Parse_NAME_Table(MemoryStream* vInMem, ttfrrwProcessingFlags vFlags, TTFRRW_ATOMIC_PARAMS);

	//////////////////////////////////////////////////////////////////////////////
	//// WRITE TABLE /////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	private: // write table
		bool WriteMemoryToFile(const std::string& vFilePathName, MemoryStream* vOutMem, int* vError);
		bool Assemble_GLYF_Table();
		bool Assemble_LOCA_Table();
		bool Assemble_MAXP_Table();
		bool Assemble_CMAP_Table();
		bool Assemble_HMTX_Table();
		bool Assemble_HHEA_Table();
		bool Assemble_POST_Table();
		bool Assemble_NAME_Table();
		bool Assemble_HEAD_Table();
	};
}