#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <utility> // pair
#include <cmath>

namespace ttfrrw
{
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
		bool ContainsPoint(const vec2<T>& pt) const { bool result = true; result = result && lowerBound.x <= pt.x; result = result && lowerBound.y <= pt.y; result = result && pt.x <= upperBound.x; result = result && pt.y <= upperBound.y; return result; }
		bool Intersects(const AABB<T>& other) { bool result = true; result = result || lowerBound.x <= other.lowerBound.x; result = result || lowerBound.y <= other.lowerBound.y; result = result || other.upperBound.x <= upperBound.x; result = result || other.upperBound.y <= upperBound.y; return result; }
		vec2<T> Size() { return vec2<T>(upperBound - lowerBound); }
	};
	typedef AABB<int> iAABB;
	typedef AABB<double> dAABB;
	typedef AABB<float> fAABB;

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
			F2DOT14 operator = (int16_t v) { value = v; }
			void SetFloat(float vValue) { value = (int16_t)roundf(vValue * 16384.f); }
			float GetFloat() { return (float)(value >> 14) + (float)(value & 0x3FFF) / 16384.0f; }
		};
		typedef uint16_t UFWord;
		typedef int16_t FWord;
		typedef int64_t longDateTime;

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

		uint8_t ReadByte();
		int32_t ReadUShort();
		int32_t ReadShort();
		FWord ReadFWord();
		uint32_t ReadUInt24();
		uint64_t ReadULong();
		uint32_t ReadULongAsInt();
		int32_t ReadLong();
		Fixed ReadFixed();
		F2DOT14 ReadF2DOT14();
		longDateTime ReadDateTime();
		std::string ReadString(size_t vLen);

	private:
		std::vector<uint8_t> m_Datas;
		size_t m_ReadPos = 0;
	};

	///////////////////////////////////////////////////////////////////////
	///// CURVE ///////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////

	class Curve
	{
	public:
		std::vector<fvec2> m_Points;

	public:
		Curve();
		~Curve();
	};

	///////////////////////////////////////////////////////////////////////
	///// GLYPH ///////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////

	class BaseGlyph
	{
	public:
		std::vector<Curve> m_Curves;
		iAABB m_LocalBBox;
		int32_t m_AdvanceX = 0;
		int32_t m_LeftSideBearing = 0;
	};

	class AffineGlyph : public BaseGlyph
	{
	public:
		fvec2 m_Translation;
		fvec2 m_Scale;

	public:
		AffineGlyph();
		~AffineGlyph();
	};

	class Glyph : public BaseGlyph
	{
	public:
		std::vector<AffineGlyph> m_AffineGlyphs; // composite or color layers
		
	public:
		Glyph();
		~Glyph();
	};

	///////////////////////////////////////////////////////////////////////
	///// MAIN CLASS TTFRRW ///////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////

	typedef uint32_t CodePoint;
	typedef uint32_t GlyphIndex;
	
	class ttfrrw
	{
	private: // will be computed by ttfrrw
		iAABB m_GlobalBBox;
		int32_t m_AdvanceWidthMax = 0;
		int32_t m_MinLeftSideBearing = 0;
		int32_t m_MinRightSideBearing = 0;

	private: // must be defined by user
		std::vector<Glyph> m_Glyphs; // bd des glyph
		std::map<CodePoint, GlyphIndex> m_CodePoints;
		
	public: // must be defined by user
		int32_t m_Ascent = 0;
		int32_t m_Descent = 0;
		int32_t m_LineGap = 0;
		
		std::string m_FontCopyright;
		std::string m_FontFamily;
		std::string m_FontSubFamily;
		std::string m_FontFullName;
		std::string m_FontTrademark;
		std::string m_FontDesignerName;
		std::string m_FontManufacturerName;
		std::string m_UrlFontDesigner;
		std::string m_UrlFontVendor;

	public:
		ttfrrw();
		~ttfrrw();

	public:
		bool OpenFontFile(const std::string& vFontFilePathName);

	public:
		bool WriteFontFile(const std::string& vFontFilePathName);

	public:
		void AddGlyph(const Glyph& vGlyph, const CodePoint& vCodePoint);
		Glyph GetGlyph(const CodePoint& vCodePoint);

	private:
		bool LoadFileToMemory(const std::string& vFilePathName, MemoryStream* vInMem, int* vError);
		bool WriteMemoryToFile(const std::string& vFilePathName, MemoryStream* vOutMem, int* vError);

	private: // write table
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