// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "TTFRRW.h"

#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>

#include <Tracy.hpp>

#define VERBOSE_MODE

// will use stl classe (std::vector) instead of simple c array
//#define USE_STL_CLASSES

///////////////////////////////////////////////////////////////////////
//// LOGGING //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

inline static void LogInfos(const TTFRRW::ttfrrwProcessingFlags& vFlags, const char* fmt, ...)
{
	ZoneScoped;

#ifdef VERBOSE_MODE
	if (!(vFlags & TTFRRW::TTFRRW_PROCESSING_FLAG_VERBOSE_ONLY_ERRORS))
	{
		va_list args;
		va_start(args, fmt);
		vprintf(fmt, args);
		va_end(args);
	}
#endif
}

inline static void LogError(const TTFRRW::ttfrrwProcessingFlags& vFlags, const char* fmt, ...)
{
	ZoneScoped;

#ifdef VERBOSE_MODE
	if (!(vFlags & TTFRRW::TTFRRW_PROCESSING_FLAG_NO_ERRORS))
	{
		va_list args;
		va_start(args, fmt);
		vprintf(fmt, args);
		va_end(args);
	}
#endif
}

///////////////////////////////////////////////////////////////////////
//// PROFIER //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#ifdef USE_SIMPLE_PROFILER

void TTFRRW::cProfiler::start()
{
	firstTimeMark = std::chrono::duration_cast<std::chrono::milliseconds>
		(std::chrono::system_clock::now().time_since_epoch()).count();
}

void TTFRRW::cProfiler::end()
{
	const int64_t secondTimeMark = std::chrono::duration_cast<std::chrono::milliseconds>
		(std::chrono::system_clock::now().time_since_epoch()).count();

	value += (double)(secondTimeMark - firstTimeMark) / 1000.0;
	count++;
}

void TTFRRW::cProfiler::reset()
{
	value = 0.0;
	count = 0;
}

double TTFRRW::cProfiler::result_Average()
{
	if (count > 0)
		return value / (double)count;
	return 0;
}

double TTFRRW::cProfiler::result_Full()
{
	return value;
}

size_t TTFRRW::cProfiler::result_Count()
{
	return count;
}

void TTFRRW::cProfiler::print(ttfrrwProcessingFlags vFlags, const char* parent, const char* label)
{
	if (vFlags & TTFRRW_PROCESSING_FLAG_VERBOSE_PROFILER)
	{
		if (count == 1)
		{
			printf("%s%s Time : %.9f s\n", parent, label, value);
		}
		else
		{
			double Average = result_Average();
			printf("%s%s :\n%s\tAverage : %.9f s\n%s\tSum : %.9f s\n%s\tCount : %zu\n", parent, label, parent, Average, parent, value, parent, count);
		}
	}
}

// clear console then print
void TTFRRW::cProfiler::erasePrint(ttfrrwProcessingFlags vFlags, const char* parent, const char* label)
{
	if (vFlags & TTFRRW_PROCESSING_FLAG_VERBOSE_PROFILER)
	{
		if (count == 1)
		{
			printf("%s%s Time (%.9fs)\n", parent, label, value);
		}
		else
		{
			// https://stackoverflow.com/questions/1508490/erase-the-current-printed-console-line
			double Average = result_Average();
			printf("%s%s : Average (%.9fs) Sum (%.9fs) Count (%zu)\r", parent, label, Average, value, count);
		}
	}
}

#endif

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

TTFRRW::MemoryStream::MemoryStream()
{
	ZoneScoped;
}

TTFRRW::MemoryStream::MemoryStream(const uint8_t* vDatas, const size_t& vSize)
{
	ZoneScoped;

	SetDatas(vDatas, vSize);
}

TTFRRW::MemoryStream::~MemoryStream()
{
	ZoneScoped;
}

////////////////////////////////////////////////////////////////
//// WRITE /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

void TTFRRW::MemoryStream::AppendMemoryStream(const MemoryStream& vMem)
{
	if (vMem.GetSize())
	{
		WriteBytes(&vMem.m_Datas);
	}
}

void TTFRRW::MemoryStream::WriteByte(const uint8_t& b)
{
	ZoneScoped;

	m_Datas.push_back(b);
}

void TTFRRW::MemoryStream::WriteBytes(const std::vector<uint8_t>* vDatas)
{
	if (vDatas)
	{
		ZoneScoped;

		m_Datas.insert(m_Datas.end(), vDatas->begin(), vDatas->end());
	}
}

void TTFRRW::MemoryStream::WriteBytes(const uint8_t* vDatas, const size_t& vSize)
{
	if (vDatas && vSize)
	{
		ZoneScoped;

		std::vector<uint8_t> arr; 
		arr.resize(vSize);
		memcpy(arr.data(), vDatas, vSize);
		m_Datas.insert(m_Datas.end(), arr.begin(), arr.end());
	}
}

void TTFRRW::MemoryStream::WriteInt(const int32_t& i)
{
	ZoneScoped;

	WriteByte((uint8_t)((i >> 24) & 0xff));
	WriteByte((uint8_t)((i >> 16) & 0xff));
	WriteByte((uint8_t)((i >> 8) & 0xff));
	WriteByte((uint8_t)(i & 0xff));
}

void TTFRRW::MemoryStream::WriteUShort(const int32_t& us)
{
	ZoneScoped;

	WriteByte((uint8_t)((us >> 8) & 0xff));
	WriteByte((uint8_t)(us & 0xff));
}

void TTFRRW::MemoryStream::WriteFWord(const int32_t& us)
{
	ZoneScoped;

	WriteUShort(us);
}

void TTFRRW::MemoryStream::WriteUFWord(const int32_t& us)
{
	ZoneScoped;

	WriteFWord(us);
}

void TTFRRW::MemoryStream::WriteShort(const int32_t& s)
{
	ZoneScoped;

	WriteUShort(s);
}

void TTFRRW::MemoryStream::WriteUInt24(const int32_t& ui)
{
	ZoneScoped;

	WriteByte((uint8_t)(ui >> 16) & 0xff);
	WriteByte((uint8_t)(ui >> 8) & 0xff);
	WriteByte((uint8_t)ui & 0xff);
}

void TTFRRW::MemoryStream::WriteULong(const int64_t& ul)
{
	ZoneScoped;

	WriteByte((uint8_t)((ul >> 24) & 0xff));
	WriteByte((uint8_t)((ul >> 16) & 0xff));
	WriteByte((uint8_t)((ul >> 8) & 0xff));
	WriteByte((uint8_t)(ul & 0xff));
}

void TTFRRW::MemoryStream::WriteLong(const int64_t& l)
{
	ZoneScoped;

	WriteULong(l);
}

void TTFRRW::MemoryStream::WriteFixed(const MemoryStream::Fixed& f)
{
	ZoneScoped;

	WriteByte((uint8_t)((f.high >> 24) & 0xff));
	WriteByte((uint8_t)((f.high >> 16) & 0xff));
	WriteByte((uint8_t)((f.low >> 8) & 0xff));
	WriteByte((uint8_t)(f.low & 0xff));
}

void TTFRRW::MemoryStream::WriteF2DOT14(const MemoryStream::F2DOT14& f)
{
	ZoneScoped;

	WriteShort(f.value);
}

void TTFRRW::MemoryStream::WriteDateTime(const longDateTime& date)
{
	ZoneScoped;

	WriteULong((date >> 32) & 0xffffffff); //-V112
	WriteULong(date & 0xffffffff); //-V112
}

const uint32_t TTFRRW::MemoryStream::GetTag(const uint8_t& a, const uint8_t& b, const uint8_t& c, const uint8_t& d)
{
	return (d) | (c << 8) | (b << 16) | (a << 24);
}

void TTFRRW::MemoryStream::WriteTag(const std::string& vTag)
{
	if (vTag.size() >= 4) // only the 4 frist char will be used btw
	{
		WriteULong(GetTag(vTag[3], vTag[2], vTag[1], vTag[0]));
	}
}

void TTFRRW::MemoryStream::WriteString(const std::string& vString)
{
	for (const auto& s : vString)
	{
		WriteByte(s);
	}
}

const uint8_t* TTFRRW::MemoryStream::GetDatas() const
{
	ZoneScoped;

	return m_Datas.data();
}

const size_t TTFRRW::MemoryStream::GetSize() const
{
	ZoneScoped;

	return m_Datas.size();
}

const size_t TTFRRW::MemoryStream::GetPos() const
{
	ZoneScoped;

	return m_ReadPos;
}

void TTFRRW::MemoryStream::SetPos(const size_t& vPos)
{
	ZoneScoped;

	m_ReadPos = vPos;
}

void TTFRRW::MemoryStream::SetDatas(const uint8_t* vDatas, const size_t& vSize)
{
	ZoneScoped;

	if (vDatas && vSize)
	{
		m_Datas.clear();
		m_Datas.resize(vSize);

		memcpy(m_Datas.data(), vDatas, vSize);
	}
}

////////////////////////////////////////////////////////////////
//// READ //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

const uint8_t TTFRRW::MemoryStream::ReadByte(const size_t& vOffset)
{
	ZoneScoped;

	if (vOffset + m_ReadPos < m_Datas.size())
		return m_Datas[vOffset + m_ReadPos++];

	return 0;
}

const std::vector<uint8_t> TTFRRW::MemoryStream::ReadBytes(const size_t& vLen, const size_t& vOffset)
{
	ZoneScoped;

	std::vector<uint8_t> res;

	if (vOffset + m_ReadPos + vLen < m_Datas.size())
	{
		auto start = m_Datas.begin() + vOffset + m_ReadPos;
		auto end = start + vLen;
		res = std::vector<uint8_t>(start, end);
		m_ReadPos += vLen;
	}

	return res;

}

const int32_t TTFRRW::MemoryStream::ReadUShort(const size_t& vOffset)
{
	ZoneScoped;

	return 0xffff & (ReadByte(vOffset) << 8 | ReadByte(vOffset));
}

const int32_t TTFRRW::MemoryStream::ReadShort(const size_t& vOffset)
{
	ZoneScoped;

	return ((ReadByte(vOffset) << 8 | ReadByte(vOffset)) << 16) >> 16;
}

const TTFRRW::MemoryStream::FWord TTFRRW::MemoryStream::ReadFWord(const size_t& vOffset)
{
	ZoneScoped;

	return (int16_t)ReadShort(vOffset);
}

const TTFRRW::MemoryStream::UFWord TTFRRW::MemoryStream::ReadUFWord(const size_t& vOffset)
{
	ZoneScoped;

	return (uint16_t)ReadUShort(vOffset);
}

const uint32_t TTFRRW::MemoryStream::ReadUInt24(const size_t& vOffset)
{
	ZoneScoped;

	return 0xffffff & (ReadByte(vOffset) << 16 | ReadByte(vOffset) << 8 | ReadByte(vOffset));
}

const uint64_t TTFRRW::MemoryStream::ReadULong(const size_t& vOffset)
{
	ZoneScoped;

	return 0xffffffffL & ReadLong(vOffset); //-V112
}

const uint32_t TTFRRW::MemoryStream::ReadULongAsInt(const size_t& vOffset)
{
	ZoneScoped;

	const int64_t ulong = ReadULong(vOffset);
	return ((int32_t)ulong) & ~0x80000000; //-V112
}

const int32_t TTFRRW::MemoryStream::ReadLong(const size_t& vOffset)
{
	ZoneScoped;

	return
		ReadByte(vOffset) << 24 |
		ReadByte(vOffset) << 16 |
		ReadByte(vOffset) << 8 |
		ReadByte(vOffset);
}

const TTFRRW::MemoryStream::Fixed TTFRRW::MemoryStream::ReadFixed(const size_t& vOffset)
{
	ZoneScoped;

	Fixed res;
	const int32_t f = ReadLong(vOffset);
	res.high = (int16_t)((f >> 16) & 0xff);
	res.low = (int16_t)(f & 0xff);
	return res;
}

const TTFRRW::MemoryStream::F2DOT14 TTFRRW::MemoryStream::ReadF2DOT14(const size_t& vOffset)
{
	ZoneScoped;

	F2DOT14 res;
	res.value = (int16_t)ReadShort(vOffset);
	return res;
}

const TTFRRW::MemoryStream::longDateTime TTFRRW::MemoryStream::ReadDateTime(const size_t& vOffset)
{
	ZoneScoped;

	return (int64_t)ReadULong(vOffset) << 32 | ReadULong(vOffset); //-V112
}

const std::string TTFRRW::MemoryStream::ReadString(const size_t& vLen, const size_t& vOffset)
{
	if (vOffset + m_ReadPos + vLen < m_Datas.size())
	{
		ZoneScoped;

		const std::string res = std::string((char*)(m_Datas.data() + vOffset + m_ReadPos), vLen);
		m_ReadPos += vLen;
		return res;
	}
	return "";
}

const std::string TTFRRW::MemoryStream::ReadTag(const size_t& vOffset)
{
	ZoneScoped;

	std::string res = "OOOO"; // 4
	uint32_t _tag = (uint32_t)ReadULong(vOffset);
	res[0] = (uint8_t)((_tag >> 24) & 0xff);
	res[1] = (uint8_t)((_tag >> 16) & 0xff);
	res[2] = (uint8_t)((_tag >> 8) & 0xff);
	res[3] = (uint8_t)(_tag & 0xff);
	return res;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#define ATOMIC_RETURN_IF_STOP_WORKING(v) if (vWorking) if (!vWorking->load()) return v;
#define ATOMIC_PROGRESS_ADD(v) if (vProgress) vProgress->store(vProgress->load() + v)
#define ATOMIC_OBJECTS_COUNT_INC if (vObjectCount) vObjectCount->store(vObjectCount->load() + 1U)

TTFRRW::TTFRRW::TTFRRW()
{
	ZoneScoped;
}

TTFRRW::TTFRRW::~TTFRRW()
{
	ZoneScoped;
}

void TTFRRW::TTFRRW::Clear(TTFRRW_ATOMIC_PARAMS)
{
	ZoneScoped;

	(void)vWorking;
	if (vProgress && vObjectCount)
	{
		vProgress->store(0.0f);
		vObjectCount->store(0U);
	}

	m_TTFInfos = TTFInfos();
	m_TTFProfiler.Reset();

	m_IsValid_For_Rasterize = false;
	m_IsValid_For_GlyphTreatment = false;

	m_Glyphs.clear();
	m_GlyphNames.clear();
	m_CodePoint_To_GlyphIndex.clear();
	m_GlyphIndex_To_CodePoints.clear();
	m_Names.clear();
	m_Tables.clear();
	m_IndexToLocFormat = 0;
	m_GlyphsOffsets.clear();
	m_Palettes.clear();
	m_MumOfLongHorMetrics = 0;
}

bool TTFRRW::TTFRRW::OpenFontFile(
	const std::string& vFontFilePathName,
	ttfrrwProcessingFlags vFlags,
	const char* vDebugInfos,
	TTFRRW_ATOMIC_PARAMS)
{
	ZoneScoped;

	bool res = false;
#ifdef USE_SIMPLE_PROFILER
	cProfiler mainProfiler;
	mainProfiler.start();
#endif
	MemoryStream mem;

	int error = 0;
	res = LoadFileToMemory(vFontFilePathName, &mem, &error);
	if (res)
	{
		res = Parse_Font_File(&mem, vFlags, TTFRRW_ATOMIC_PARAMS_BY_REF);
	}
#ifdef USE_SIMPLE_PROFILER
	mainProfiler.end();
	mainProfiler.print(vFlags, "OpenFontFile ", vDebugInfos);
#else
	(void)vDebugInfos;
#endif
	return res;
}

bool TTFRRW::TTFRRW::OpenFontStream(
	uint8_t* vStream, const size_t& vStreamSize,
	ttfrrwProcessingFlags vFlags,
	const char* vDebugInfos,
	TTFRRW_ATOMIC_PARAMS)
{
	ZoneScoped;

	bool res = false;
#ifdef USE_SIMPLE_PROFILER
	cProfiler mainProfiler;
	mainProfiler.start();
#endif
	if (vStream && vStreamSize)
	{
		MemoryStream mem;
		mem.SetDatas(vStream, vStreamSize);
		res = Parse_Font_File(&mem, vFlags, TTFRRW_ATOMIC_PARAMS_BY_REF);
	}
#ifdef USE_SIMPLE_PROFILER
	mainProfiler.end();
	mainProfiler.print(vFlags, "OpenFontStream ", vDebugInfos);
#else
	(void)vDebugInfos;
#endif
	return res;
}

// finalize the job
// put codepoint in each glyphs
void TTFRRW::TTFRRW::ConsolidateGlyphs()
{
	for (size_t idx = 0; idx < m_Glyphs.size(); idx++)
	{
		GlyphIndex gi = (GlyphIndex)idx;
		if (m_GlyphIndex_To_CodePoints.find(gi) != m_GlyphIndex_To_CodePoints.end())
		{
			auto cdps = m_GlyphIndex_To_CodePoints[gi];
			if (!cdps.empty())
			{
				m_Glyphs[idx].m_CodePoint = *cdps.begin();
			}
		}
	}
}

std::vector<TTFRRW::Glyph>* TTFRRW::TTFRRW::GetGlyphs()
{
	if (!m_Glyphs.empty())
	{
		ZoneScoped;

		return &m_Glyphs;
	}

	return nullptr;
}

TTFRRW::Glyph* TTFRRW::TTFRRW::GetGlyphWithGlyphIndex(const GlyphIndex& vGlyphIndex)
{
	if (vGlyphIndex)
	{
		if (vGlyphIndex < m_Glyphs.size())
		{
			ZoneScoped;

			return &m_Glyphs[vGlyphIndex];
		}
	}

	return nullptr;
}

bool TTFRRW::TTFRRW::WriteFontFile(const std::string& vFontFilePathName)
{
	ZoneScoped;

	(void)vFontFilePathName;

	return false;
}

///////////////////////////////////////////////////////////////////////
//// PUBLIC METHOD'S //////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

void TTFRRW::TTFRRW::AddGlyph(const Glyph& vGlyph, const CodePoint& vCodePoint)
{
	ZoneScoped;

	(void)vGlyph;
	(void)vCodePoint;
}

TTFRRW::Glyph* TTFRRW::TTFRRW::GetGlyphWithCodePoint(const CodePoint& vCodePoint)
{
	ZoneScoped;

	(void)vCodePoint;

	return nullptr;
}

TTFRRW::GlyphIndex TTFRRW::TTFRRW::GetGlyphIndexFromCodePoint(const CodePoint& vCodePoint)
{
	ZoneScoped;

	if (m_CodePoint_To_GlyphIndex.find(vCodePoint) != m_CodePoint_To_GlyphIndex.end())
	{
		return m_CodePoint_To_GlyphIndex[vCodePoint];
	}

	return 0;
}

std::set<TTFRRW::CodePoint>* TTFRRW::TTFRRW::GetCodePointsFromGlyphIndex(const GlyphIndex& vGlyphIndex)
{
	ZoneScoped;

	if (m_GlyphIndex_To_CodePoints.find(vGlyphIndex) != m_GlyphIndex_To_CodePoints.end())
	{
		return &m_GlyphIndex_To_CodePoints[vGlyphIndex];
	}

	return nullptr;
}

TTFRRW::TTFInfos TTFRRW::TTFRRW::GetFontInfos()
{
	ZoneScoped;

	return m_TTFInfos;
}

bool TTFRRW::TTFRRW::IsValidForRasterize()
{
	ZoneScoped;

	return m_IsValid_For_Rasterize;
}

bool TTFRRW::TTFRRW::IsValidFotGlyppTreatment()
{
	ZoneScoped;

	return m_IsValid_For_GlyphTreatment;
}

///////////////////////////////////////////////////////////////////////
//// PRIVATE FILE / STREAM ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

bool TTFRRW::TTFRRW::LoadFileToMemory(
	const std::string& vFilePathName,
	MemoryStream* vOutMem,
	int* vError)
{
	ZoneScoped;

	bool res = false;

	if (vOutMem)
	{
		FILE* intput_file = NULL;
#if defined(MSVC)
		errno_t returnValue = fopen_s(&intput_file, vFilePathName.c_str(), "rb");
		if (vError)
			*vError = returnValue;
#else
		intput_file = fopen(vFilePathName.c_str(), "rb");
		if (vError)
			*vError = errno;
#endif
		if (intput_file != reinterpret_cast<FILE*>(NULL))
		{
			// obtain file size:
			fseek(intput_file, 0, SEEK_END); //-V303
			const size_t fileSize = (size_t)ftell(intput_file); //-V303
			rewind(intput_file);

			if (fileSize)
			{
				// copy the file into the buffer and close
				std::vector<uint8_t> bytes;
				bytes.resize(fileSize);
				fread(bytes.data(), 1, fileSize, intput_file);
				vOutMem->SetDatas(bytes.data(), bytes.size());
			}

			fclose(intput_file);

			res = true;
		}
	}

	return res;
}

bool TTFRRW::TTFRRW::WriteMemoryToFile(
	const std::string& vFilePathName,
	const MemoryStream& vInMem,
	int* vError)
{
	ZoneScoped;

	bool res = false;

	if (vInMem.GetSize())
	{
		FILE* output_file = nullptr;
#if defined(MSVC)
		fopen_s(&output_file, vFilePathName.c_str(), "wb");
#else
		output_file = fopen(vFilePathName.c_str(), "wb");
#endif
		if (vError)
			*vError = errno;
		if (output_file != reinterpret_cast<FILE*>(NULL))
		{
			fwrite(vInMem.GetDatas(), 1, vInMem.GetSize(), output_file);
			fflush(output_file);
			fclose(output_file);
			res = true;
		}
	}

	return res;
}

///////////////////////////////////////////////////////////////////////
//// PRIVATE PARSER ///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

bool TTFRRW::TTFRRW::Parse_Font_File(MemoryStream* vMem, const ttfrrwProcessingFlags& vFlags, TTFRRW_ATOMIC_PARAMS)
{
	ZoneScoped;

	// la loca contient la position des glyphs et leur taille
	// la maxp contient le nombre de glyphs
	// la cmap contient la correspondance entre les codepoint et les index de glyphs
	// la glyf contient les glyf adressé par la loca
	// la head contient les infos font tel que UnitEmPm qui peut faire foier freetype
	// la header contient le nombre de table
	// la name qui contient les infos de license ou nom etc..
	// la hmtx et hhea qui contient les infos ascent, descent, advance x etc
	// la post qui contient les noms de glyphs

	// on veut recuperer tous les glyphs
	// alors on va extraire les infos de la loca et de la glyf pour commencer

	bool res = false;

	if (vMem)
	{
		Clear(TTFRRW_ATOMIC_PARAMS_BY_REF);

		if (vFlags & TTFRRW_PROCESSING_FLAG_VERBOSE_PROFILER)
		{
			printf("Profiler Started\n");
		}

		if (Parse_Table_Header(vMem, vFlags, TTFRRW_ATOMIC_PARAMS_BY_REF))
		{
			const bool maxpOK = Parse_MAXP_Table(vMem, vFlags, TTFRRW_ATOMIC_PARAMS_BY_REF);
			if (maxpOK) // dependencies
			{
				/*bool cmapOK =*/ Parse_CMAP_Table(vMem, vFlags, TTFRRW_ATOMIC_PARAMS_BY_REF);
				ATOMIC_RETURN_IF_STOP_WORKING(false);
				const bool headOK = Parse_HEAD_Table(vMem, vFlags, TTFRRW_ATOMIC_PARAMS_BY_REF);
				ATOMIC_RETURN_IF_STOP_WORKING(false);
				const bool locaOK = Parse_LOCA_Table(vMem, vFlags, TTFRRW_ATOMIC_PARAMS_BY_REF);
				ATOMIC_RETURN_IF_STOP_WORKING(false);
				const bool hheaOK = Parse_HHEA_Table(vMem, vFlags, TTFRRW_ATOMIC_PARAMS_BY_REF);
				ATOMIC_RETURN_IF_STOP_WORKING(false);
				const bool nameOK = Parse_NAME_Table(vMem, vFlags, TTFRRW_ATOMIC_PARAMS_BY_REF);
				ATOMIC_RETURN_IF_STOP_WORKING(false);
				bool glyfOK = false;
				//bool postOK = false;
				bool hmtxOK = false;
				bool colrOK = false;
				bool cpalOK = false;

				// on fait post avant glyf comme ca 
				// on pourra mettre le nom directement dans le glyph
				/*postOK = */Parse_POST_Table(vMem, vFlags, TTFRRW_ATOMIC_PARAMS_BY_REF);
				ATOMIC_RETURN_IF_STOP_WORKING(false);

				if (headOK && locaOK) // dependencies
					glyfOK = Parse_GLYF_Table(vMem, vFlags, TTFRRW_ATOMIC_PARAMS_BY_REF);
				ATOMIC_RETURN_IF_STOP_WORKING(false);

				// on fait ca apres glyph comme ca on pourra remplir
				// les metrics dans le glyph
				if (hheaOK) // dependencies
				{
					hmtxOK = Parse_HMTX_Table(vMem, vFlags, TTFRRW_ATOMIC_PARAMS_BY_REF);
				}
				ATOMIC_RETURN_IF_STOP_WORKING(false);

				cpalOK = Parse_CPAL_Table(vMem, vFlags, TTFRRW_ATOMIC_PARAMS_BY_REF);
				ATOMIC_RETURN_IF_STOP_WORKING(false);

				if (cpalOK) // dependencies
					colrOK = Parse_COLR_Table(vMem, vFlags, TTFRRW_ATOMIC_PARAMS_BY_REF);
				ATOMIC_RETURN_IF_STOP_WORKING(false);

				// tres permissif, le minimum est d'avoir des glyphs
				// le reste au besoin on le fera nous meme
				res = glyfOK;

				m_IsValid_For_GlyphTreatment = res;
				m_IsValid_For_Rasterize = (glyfOK && hmtxOK);
			}
		}
		else
		{
			LogError(vFlags, "ERR : Corrupted Header Table\n");
		}

		m_TTFProfiler.Print(vFlags);
	}

	return res;
}

bool TTFRRW::TTFRRW::Parse_Table_Header(MemoryStream* vMem, const ttfrrwProcessingFlags& vFlags, TTFRRW_ATOMIC_PARAMS)
{
	(void)vProgress;

	ZoneScoped;

	// header
	const std::string scalerType = vMem->ReadString(4); //-V112
	if (scalerType[0] == 1)  m_FontType = "TrueType 1"; // TrueType 1
	if (scalerType[1] == 1)  m_FontType = "OpenType 1"; // TrueType 1
	const uint16_t numTables = (uint16_t)vMem->ReadUShort();
	/*uint16_t searchRange =*/ //(uint16_t)vMem->ReadUShort();
	/*uint16_t entrySelector =*/ //(uint16_t)vMem->ReadUShort();
	/*uint16_t rangeShift =*/ //(uint16_t)vMem->ReadUShort();

	// tables
	for (int i = 0; i < numTables; i++)
	{
		TableStruct tbl;

		tbl.tag = vMem->ReadTag(6);
		tbl.checkSum = (uint32_t)vMem->ReadULong(6);
		tbl.offset = (size_t)vMem->ReadULong(6);
		tbl.length = (size_t)vMem->ReadULong(6);

		m_Tables[tbl.tag] = tbl;
		LogInfos(vFlags, "Table %s found\n", tbl.tag.c_str()); //-V111

		ATOMIC_OBJECTS_COUNT_INC;
		ATOMIC_RETURN_IF_STOP_WORKING(false);
	}

	return (!m_Tables.empty());
}

bool TTFRRW::TTFRRW::Parse_CMAP_Table(MemoryStream* vMem, const ttfrrwProcessingFlags& vFlags, TTFRRW_ATOMIC_PARAMS)
{
	(void)vProgress;

	ZoneScoped;

	if (!vMem) return false;

	if (m_Tables.find("cmap") != m_Tables.end())
	{
		ATOMIC_OBJECTS_COUNT_INC;

		const auto tbl = m_Tables["cmap"];
		vMem->SetPos(tbl.offset);
		//uint32_t len = tbl.length;

		/*uint16_t version =*/ //(uint16_t)vMem->ReadUShort();
		const uint16_t numEncodingRecords = (uint16_t)vMem->ReadUShort(2);

		size_t sizeOfEncodingRecord = 8U;
		for (size_t encodingRecordID = 0; encodingRecordID < (size_t)numEncodingRecords; encodingRecordID++)
		{
			ATOMIC_RETURN_IF_STOP_WORKING(false);

			vMem->SetPos(tbl.offset + (size_t)4U + sizeOfEncodingRecord * encodingRecordID); //-V112

			/*uint16_t platformID =*/ //(uint16_t)vMem->ReadUShort();
			/*uint16_t encodingID =*/ //(uint16_t)vMem->ReadUShort();
			const size_t offset = (size_t)vMem->ReadULong(4);

			vMem->SetPos(tbl.offset + offset);

			const uint16_t format = (uint16_t)vMem->ReadUShort();
			
			if (format == 0U)
			{
				/*uint16_t length =*/ //(uint16_t)vMem->ReadUShort();
				/*uint16_t language =*/ //(uint16_t)vMem->ReadUShort();
				for (GlyphIndex glyphIndex = 0; glyphIndex < 256; glyphIndex++)
				{
					ATOMIC_RETURN_IF_STOP_WORKING(false);

					const uint8_t codePoint = vMem->ReadByte(4);
					m_CodePoint_To_GlyphIndex[codePoint] = glyphIndex;
					m_GlyphIndex_To_CodePoints[glyphIndex].emplace(codePoint);
				}
			} //-V112
			else if (format == 4U) //-V112
			{
				/*uint16_t length =*/ //(uint16_t)vMem->ReadUShort();
				/*uint16_t language =*/ //(uint16_t)vMem->ReadUShort();
				const uint16_t segCountX2 = (uint16_t)vMem->ReadUShort(4);
				/*uint16_t searchRange =*/ //(uint16_t)vMem->ReadUShort();
				/*uint16_t entrySelector =*/ //(uint16_t)vMem->ReadUShort();
				/*uint16_t rangeShift =*/ //(uint16_t)vMem->ReadUShort();

				std::vector<uint16_t> endCode;
				std::vector<uint16_t> startCode;
				std::vector<int16_t> idDelta;
				std::vector<uint16_t> idRangeOffset;

				int segCount = segCountX2 / 2;
				for (int i = 0; i < segCount; i++)
				{
					ATOMIC_RETURN_IF_STOP_WORKING(false);

					endCode.push_back((uint16_t)vMem->ReadUShort(10));
				}
				/*uint16_t reservedPad =*/ //(uint16_t)vMem->ReadUShort();
				for (int i = 0; i < segCount; i++)
				{
					ATOMIC_RETURN_IF_STOP_WORKING(false);

					startCode.push_back((uint16_t)vMem->ReadUShort(12));
				}
				for (int i = 0; i < segCount; i++)
				{
					ATOMIC_RETURN_IF_STOP_WORKING(false);

					idDelta.push_back((uint16_t)vMem->ReadShort(12));
				}
				const size_t idRangeOffsetAddress = vMem->GetPos() + 12;
				for (int i = 0; i < segCount; i++)
				{
					ATOMIC_RETURN_IF_STOP_WORKING(false);

					idRangeOffset.push_back((uint16_t)vMem->ReadUShort(12));
				}

				for (uint16_t codePoint = 0; codePoint < 0xFFFF; codePoint++) //-V112
				{
					ATOMIC_RETURN_IF_STOP_WORKING(false);

					// d'abord on va localiser le segment
					bool found = false;
					size_t segment = 0;
					size_t start = 0;
					size_t end = segCount;
					while (start != end)
					{
						segment = (start + end) / 2;
						if (codePoint < startCode[segment])  // location is below current location
							end = segment;
						else  // is key below the upper bound?
						{
							if (codePoint <= endCode[segment])
							{
								found = true;
								break;
							}
							// location is above the current location
							start = segment + 1;
						}
					}
					if (found)
					{
						start = startCode[segment];
						if (codePoint >= start)
						{
							GlyphIndex foundGlyphIndex = 0;

							const size_t id_range_offset = (size_t)idRangeOffset[segment];
							if (id_range_offset == 0)
							{
								foundGlyphIndex = (codePoint + idDelta[segment]) % 0x10000; // 0x10000 is 65536
							}
							else
							{
								const size_t idRangeOffsetLocation = idRangeOffsetAddress + segment * sizeof(uint16_t);
								vMem->SetPos(id_range_offset + idRangeOffsetLocation + ((size_t)codePoint - start) * 2U);
								foundGlyphIndex = (uint16_t)vMem->ReadUShort();
							}

							if (foundGlyphIndex < 0xFFFF)
							{
								m_CodePoint_To_GlyphIndex[codePoint] = foundGlyphIndex;
								m_GlyphIndex_To_CodePoints[foundGlyphIndex].emplace(codePoint);
								LogInfos(vFlags, "CodePoint %u => GlyphIndex %u\n", codePoint, foundGlyphIndex);
							}
							else
							{
								LogError(vFlags, "ERR : CodePoint %u => Error\n", codePoint);
							}
						}
						else
						{
							//notdef;
						}
					}
				}
			}
			else if (format == 6U)
			{
				/*uint16_t length =*/ //(uint16_t)vMem->ReadUShort();
				/*uint16_t language =*/ //(uint16_t)vMem->ReadUShort();
				const uint16_t firstCode = (uint16_t)vMem->ReadUShort(4);
				const uint16_t entryCount = (uint16_t)vMem->ReadUShort(4);
				for (GlyphIndex glyphIndex = 0; glyphIndex < entryCount; glyphIndex++)
				{
					ATOMIC_RETURN_IF_STOP_WORKING(false);

					const uint16_t codePoint = (uint16_t)vMem->ReadUShort(4);
					m_CodePoint_To_GlyphIndex[codePoint] = glyphIndex;
					m_GlyphIndex_To_CodePoints[glyphIndex].emplace(codePoint);
				}
			}
			else if (format == 12)
			{
				/*uint16_t reserved =*/ //(uint16_t)vMem->ReadUShort();
				/*uint32_t length =*/ //(uint32_t)vMem->ReadULong();
				/*uint32_t language =*/ //(uint32_t)vMem->ReadULong();
				const uint32_t nGroups = (uint32_t)vMem->ReadULong(10);
				
				for (uint32_t groupID = 0; groupID < nGroups; groupID++)
				{
					ATOMIC_RETURN_IF_STOP_WORKING(false);

					const uint32_t startCharCode = (uint32_t)vMem->ReadULong(10);
					const uint32_t endCharCode = (uint32_t)vMem->ReadULong(10);
					const uint32_t startGlyphID = (uint32_t)vMem->ReadULong(10);

					const uint32_t count = endCharCode - startCharCode;
					if (count)
					{
						for (uint32_t charCodeID = 0; charCodeID < count; charCodeID++)
						{
							ATOMIC_RETURN_IF_STOP_WORKING(false);

							const CodePoint codePoint = (CodePoint)(startCharCode + charCodeID);
							const GlyphIndex glyphIndex = (GlyphIndex)(startGlyphID + charCodeID);
							m_CodePoint_To_GlyphIndex[codePoint] = glyphIndex;
							m_GlyphIndex_To_CodePoints[glyphIndex].emplace(codePoint);
						}
					}
					else
					{
						const CodePoint codePoint = (CodePoint)startCharCode;
						const GlyphIndex glyphIndex = (GlyphIndex)startGlyphID;
						m_CodePoint_To_GlyphIndex[codePoint] = glyphIndex;
						m_GlyphIndex_To_CodePoints[glyphIndex].emplace(codePoint);
					}
				}
			}
			else
			{
				LogError(vFlags, "ERR : CMAP Format %u not supported for the moment\n", format);
			}
		}

		return true;
	}
	else
	{
		LogError(vFlags, "ERR : CMAP Table not found\n");
	}

	return false;
}

bool TTFRRW::TTFRRW::Parse_HEAD_Table(MemoryStream* vMem, const ttfrrwProcessingFlags& vFlags, TTFRRW_ATOMIC_PARAMS)
{
	(void)vProgress;

	ZoneScoped;

	if (m_Tables.find("head") != m_Tables.end())
	{
		ATOMIC_OBJECTS_COUNT_INC;
		ATOMIC_RETURN_IF_STOP_WORKING(false);

		auto tbl = m_Tables["head"];
		vMem->SetPos(tbl.offset);
		//uint32_t len = tbl.length;

		/*MemoryStream::Fixed version =*/ //vMem->ReadFixed();				//4
		/*MemoryStream::Fixed fontRevision =*/ //vMem->ReadFixed();			//4
		/*uint32_t checkSumAdjustment =*/ //(uint32_t)vMem->ReadULong();	//4
		/*uint32_t magicNumber =*/ //(uint32_t)vMem->ReadULong();			//4
		/*uint16_t flags =*/ //(uint16_t)vMem->ReadUShort(); // bitset		//2
		/*uint16_t unitsPerEm =*/ //(uint16_t)vMem->ReadUShort();			//2
		/*MemoryStream::longDateTime created =*/ //vMem->ReadDateTime();	//8
		/*MemoryStream::longDateTime modified */ //vMem->ReadDateTime();	//8 => offset 36
		m_TTFInfos.m_GlobalBBox.lowerBound.x = vMem->ReadFWord(36);			//2
		m_TTFInfos.m_GlobalBBox.lowerBound.y = vMem->ReadFWord(36);			//2
		m_TTFInfos.m_GlobalBBox.upperBound.x = vMem->ReadFWord(36);			//2
		m_TTFInfos.m_GlobalBBox.upperBound.y = vMem->ReadFWord(36);			//2
		/*uint16_t macStyle =*/ //(uint16_t)vMem->ReadUShort(); // bitset	//2
		/*uint16_t lowestRecPPEM =*/ //(uint16_t)vMem->ReadUShort();		//2
		/*uint16_t fontDirectionHint =*/ //(int16_t)vMem->ReadShort();		//2 => offset 36 + 6
		m_IndexToLocFormat = (int16_t)vMem->ReadShort(42);					//2
		/*uint16_t glyphDataFormat =*/// (int16_t)vMem->ReadShort();

		return true;
	}
	else
	{
		LogError(vFlags, "ERR : HEAD Table not found\n");
	}

	return false;
}

bool TTFRRW::TTFRRW::Parse_MAXP_Table(MemoryStream* vMem, const ttfrrwProcessingFlags& vFlags, TTFRRW_ATOMIC_PARAMS)
{
	(void)vProgress;

	ZoneScoped;

	if (m_Tables.find("maxp") != m_Tables.end())
	{
		ATOMIC_OBJECTS_COUNT_INC;
		ATOMIC_RETURN_IF_STOP_WORKING(false);

		auto tbl = m_Tables["maxp"];
		vMem->SetPos(tbl.offset);
		//uint32_t len = tbl.length;

		/*MemoryStream::Fixed version =*/ //vMem->ReadFixed();
		m_TTFInfos.m_GlyphCount = (uint16_t)vMem->ReadUShort(4);
		/*uint16_t maxPoints = (uint16_t)vMem->ReadUShort();
		uint16_t maxContours = (uint16_t)vMem->ReadUShort();
		uint16_t maxComponentPoints = (uint16_t)vMem->ReadUShort();
		uint16_t maxComponentContours = (uint16_t)vMem->ReadUShort();
		uint16_t maxZones = (uint16_t)vMem->ReadUShort();
		uint16_t maxTwilightPoints = (uint16_t)vMem->ReadUShort();
		uint16_t maxStorage = (uint16_t)vMem->ReadUShort();
		uint16_t maxFunctionDefs = (uint16_t)vMem->ReadUShort();
		uint16_t maxInstructionDefs = (uint16_t)vMem->ReadUShort();
		uint16_t maxStackElements = (uint16_t)vMem->ReadUShort();
		uint16_t maxSizeOfInstructions = (uint16_t)vMem->ReadUShort();
		uint16_t maxComponentElements = (uint16_t)vMem->ReadUShort();
		uint16_t maxComponentDepth = (uint16_t)vMem->ReadUShort();*/

		return true;
	}
	else
	{
		LogError(vFlags, "ERR : MAXP Table not found\n");
	}

	return false;
}

bool TTFRRW::TTFRRW::Parse_LOCA_Table(MemoryStream* vMem, const ttfrrwProcessingFlags& vFlags, TTFRRW_ATOMIC_PARAMS)
{
	(void)vProgress;

	ZoneScoped;

	if (m_Tables.find("loca") != m_Tables.end())
	{
		ATOMIC_OBJECTS_COUNT_INC;
		ATOMIC_RETURN_IF_STOP_WORKING(false);

		auto tbl = m_Tables["loca"];
		vMem->SetPos(tbl.offset);
		//uint32_t len = tbl.length;

		m_GlyphsOffsets.resize((size_t)m_TTFInfos.m_GlyphCount);

		if (m_IndexToLocFormat == 0) // short format
		{
			for (uint16_t i = 0; i < m_TTFInfos.m_GlyphCount; i++)
			{
				ATOMIC_RETURN_IF_STOP_WORKING(false);

				const size_t offset = (size_t)((uint32_t)vMem->ReadUShort()) * 2;
				m_GlyphsOffsets[i] = offset;
			}
		}
		else if (m_IndexToLocFormat == 1) // long format
		{
			for (uint16_t i = 0; i < m_TTFInfos.m_GlyphCount; i++)
			{
				ATOMIC_RETURN_IF_STOP_WORKING(false);

				const size_t offset = (size_t)((uint32_t)vMem->ReadULong());
				m_GlyphsOffsets[i] = offset;
			}
		}

		return true;
	}
	else
	{
		LogError(vFlags, "ERR : LOCA Table not found\n");
	}

	return false;
}

bool TTFRRW::TTFRRW::Parse_GLYF_Table(MemoryStream* vMem, const ttfrrwProcessingFlags& vFlags, TTFRRW_ATOMIC_PARAMS)
{
	(void)vProgress;

	ZoneScoped;

	if (m_Tables.find("glyf") != m_Tables.end())
	{
		ATOMIC_OBJECTS_COUNT_INC;
		ATOMIC_RETURN_IF_STOP_WORKING(false);

		auto tbl = m_Tables["glyf"];
		vMem->SetPos(tbl.offset);
		//uint32_t len = tbl.length;

		//size_t length = 0;
		for (size_t glyphID = 0; glyphID < (size_t)m_TTFInfos.m_GlyphCount; glyphID++)
		{
			if (vProgress)
				vProgress->store((float)glyphID / (float)m_TTFInfos.m_GlyphCount);

			ATOMIC_OBJECTS_COUNT_INC;
			ATOMIC_RETURN_IF_STOP_WORKING(false);

			const size_t glyphOffset = tbl.offset + m_GlyphsOffsets[glyphID];
			vMem->SetPos(glyphOffset);

			const int16_t numberOfContours = (int16_t)vMem->ReadShort();
			const MemoryStream::FWord xMin = vMem->ReadFWord();
			const MemoryStream::FWord yMin = vMem->ReadFWord();
			const MemoryStream::FWord xMax = vMem->ReadFWord();
			const MemoryStream::FWord yMax = vMem->ReadFWord();

			LogInfos(vFlags, "-----------------------\n");

			Glyph glyph;
			glyph.m_LocalBBox.lowerBound.x = xMin;
			glyph.m_LocalBBox.lowerBound.y = yMin;
			glyph.m_LocalBBox.upperBound.x = xMax;
			glyph.m_LocalBBox.upperBound.y = yMax;

			if (glyphID < m_GlyphNames.size())
				glyph.m_Name = m_GlyphNames[glyphID];

			LogInfos(vFlags, "BBox : %i,%i > %i,%i\n", xMin, yMin, xMax, yMax);

			if (numberOfContours >= 0) // simple glyf
			{
				LogInfos(vFlags, "Glyph %u : Simple Glyph\n", (uint32_t)glyphID);

				glyph.m_IsSimple = true;

				if (!(vFlags & TTFRRW_PROCESSING_FLAG_NO_GLYPH_PARSING))
				{
					const auto g = Parse_Simple_Glyf(vMem, (GlyphIndex)glyphID, numberOfContours, vFlags, TTFRRW_ATOMIC_PARAMS_BY_REF);
					glyph.m_Contours = g.m_Contours;
					glyph.m_AdvanceX = g.m_AdvanceX;
					glyph.m_LeftSideBearing = g.m_LeftSideBearing;
				}
			}
			else // composite glyf
			{
				LogInfos(vFlags, "Glyph %u : Composite Glyph\n", (uint32_t)glyphID);

				glyph.m_IsSimple = false;
			}

			m_Glyphs.push_back(glyph);

			LogInfos(vFlags, "-----------------------\n");

		//	length = glyphOffset;
		}

		return true;
	}
	else
	{
		LogError(vFlags, "ERR : GLYF Table not found\n");
	}

	return false;
}

TTFRRW::Glyph TTFRRW::TTFRRW::Parse_Simple_Glyf(MemoryStream* vMem, const GlyphIndex& vGlyphIndex, const int16_t& vCountContour, const ttfrrwProcessingFlags& vFlags, TTFRRW_ATOMIC_PARAMS)
{
	(void)vProgress;
	(void)vObjectCount;

	ZoneScoped;

	Glyph glyph;

	if (vMem)
	{
#ifdef USE_SIMPLE_PROFILER
		m_TTFProfiler.simpleGlyfProfiler.start();
#endif
		if (vCountContour >= 0) // this is well simple glyph
		{
			const size_t countContours = (size_t)vCountContour;

#ifdef USE_STL_CLASSES
			std::vector<uint16_t> endPtsOfContours; // todo: to use a std::deque say PVS => to check
			endPtsOfContours.resize(countContours);
#else
			uint16_t* endPtsOfContours = new uint16_t[countContours];
			memset(endPtsOfContours, 0, sizeof(uint16_t) * countContours);
#endif
			for (size_t contourID = 0; contourID < countContours; contourID++)
			{
				endPtsOfContours[contourID] = (uint16_t)vMem->ReadShort() + 1U;
			}

			const size_t instructionLength = (size_t)vMem->ReadUShort();
			if (instructionLength)
			{	
#ifdef USE_STL_CLASSES
				std::vector<uint8_t> instructions;
				instructions.resize(instructionLength);
#else
				uint8_t* instructions = new uint8_t[instructionLength];
				memset(instructions, 0, sizeof(uint8_t) * instructionLength);
#endif
				for (size_t instructionID = 0; instructionID < instructionLength; instructionID++)
				{
					instructions[instructionID] = (uint8_t)vMem->ReadByte();
				}
				delete[] instructions;
			}

#ifdef USE_STL_CLASSES
			std::vector<uint8_t> flags;
			std::vector<bool> onCurves;
			std::vector<int16_t> xCoordinates;
			std::vector<int16_t> yCoordinates;
#else
			uint8_t* flags = nullptr;
			bool* onCurves = nullptr;
			int16_t* xCoordinates = nullptr;
			int16_t* yCoordinates = nullptr;
#endif
			const size_t maxPoints = (size_t)endPtsOfContours[countContours - 1];
			if (maxPoints)
			{
#ifdef USE_STL_CLASSES
				xCoordinates.resize(maxPoints);
				yCoordinates.resize(maxPoints);
				onCurves.resize(maxPoints);
				flags.resize(maxPoints);
#else
				xCoordinates = new int16_t[maxPoints];
				memset(xCoordinates, 0, sizeof(int16_t) * maxPoints);
				yCoordinates = new int16_t[maxPoints];
				memset(yCoordinates, 0, sizeof(int16_t) * maxPoints);
				onCurves = new bool[maxPoints];
				memset(onCurves, 0, sizeof(bool) * maxPoints);
				flags = new uint8_t[maxPoints];
				memset(flags, 0, sizeof(uint8_t) * maxPoints);
#endif
				enum SimpleFlags
				{
					SimpleFlagOnCurve = 1 << 0,
					SimpleFlagOnXShort = 1 << 1,
					SimpleFlagOnYShort = 1 << 2,
					SimpleFlagOnRepeat = 1 << 3,
					SimpleFlagOnXRepeatSign = 1 << 4,
					SimpleFlagOnYRepeatSign = 1 << 5,
				};

				uint32_t flag_repeat = 0;
				int32_t flag = 0;
				for (size_t pointID = 0; pointID < maxPoints; pointID++)
				{
					ATOMIC_RETURN_IF_STOP_WORKING(glyph);

					if (flag_repeat == 0)
					{
						flag = vMem->ReadByte();
						if ((flag & SimpleFlagOnRepeat) == SimpleFlagOnRepeat)
						{
							flag_repeat = vMem->ReadByte();
						}
					}
					else
					{
						flag_repeat--;
					}

					const uint8_t u8Flag = (uint8_t)flag;
					flags[pointID] = u8Flag;
					onCurves[pointID] = ((u8Flag & SimpleFlagOnCurve) == SimpleFlagOnCurve);
				}

				for (size_t pointID = 0; pointID < maxPoints; pointID++)
				{
					ATOMIC_RETURN_IF_STOP_WORKING(glyph);

					flag = flags[pointID];

					if ((flag & SimpleFlagOnXShort) == SimpleFlagOnXShort)
					{
						int16_t coord = (int16_t)vMem->ReadByte();
						coord *= ((flag & SimpleFlagOnXRepeatSign) == SimpleFlagOnXRepeatSign) ? 1 : -1;
						xCoordinates[pointID] = coord;
					}
					else if (!((flag & SimpleFlagOnXRepeatSign) == SimpleFlagOnXRepeatSign))
					{
						xCoordinates[pointID] = (int16_t)vMem->ReadShort();
					}
					if (pointID)
					{
						xCoordinates[pointID] += xCoordinates[pointID - 1U];
					}
				}

				for (size_t pointID = 0; pointID < maxPoints; pointID++)
				{
					ATOMIC_RETURN_IF_STOP_WORKING(glyph);

					flag = flags[pointID];

					if ((flag & SimpleFlagOnYShort) == SimpleFlagOnYShort)
					{
						int16_t coord = (int16_t)vMem->ReadByte();
						coord *= ((flag & SimpleFlagOnYRepeatSign) == SimpleFlagOnYRepeatSign) ? 1 : -1;
						yCoordinates[pointID] = coord;
					}
					else if (!((flag & SimpleFlagOnYRepeatSign) == SimpleFlagOnYRepeatSign))
					{
						yCoordinates[pointID] = (int16_t)vMem->ReadShort();
					}
					if (pointID)
					{
						yCoordinates[pointID] += yCoordinates[pointID - 1U];
					}
				}
			}

			// convert in final glyph
			size_t lastCount = 0;
			glyph.m_Contours.resize(countContours);
			for (size_t contourID = 0; contourID < countContours; contourID++)
			{
				ATOMIC_RETURN_IF_STOP_WORKING(glyph);

				const auto contour = &glyph.m_Contours[contourID];
				const size_t pmax = endPtsOfContours[contourID] - lastCount;
				if (pmax)
				{
					contour->m_Points.resize(pmax);
					contour->m_OnCurve.resize(pmax);

					for (size_t p = 0; p < pmax; p++)
					{
						ATOMIC_RETURN_IF_STOP_WORKING(glyph);

						contour->m_Points[p].x = (int32_t)xCoordinates[lastCount + p];
						contour->m_Points[p].y = (int32_t)yCoordinates[lastCount + p];
						contour->m_OnCurve[p] = onCurves[lastCount + p];
					}
				}
				else
				{
					LogError(vFlags, "ERR : Point count is null in Contour %u of glyph %u\n", (uint32_t)contourID, vGlyphIndex);
				}

				lastCount = endPtsOfContours[contourID];
			}
#ifndef USE_STL_CLASSES
			delete[] flags; flags = nullptr;
			delete[] onCurves; onCurves = nullptr;
			delete[] xCoordinates; xCoordinates = nullptr;
			delete[] yCoordinates; yCoordinates = nullptr;
			delete[] endPtsOfContours; endPtsOfContours = nullptr;
#endif
		}
#ifdef USE_SIMPLE_PROFILER
		m_TTFProfiler.simpleGlyfProfiler.end();
#endif
	}

	return glyph;
}

#define STANDARD_MAC_NAMES_COUNT 258
static const char* standardMacNames[STANDARD_MAC_NAMES_COUNT] =
{ ".notdef", ".null", "nonmarkingreturn", "space", "exclam", "quotedbl", "numbersign", "dollar", "percent",
"ampersand", "quotesingle", "parenleft", "parenright", "asterisk", "plus", "comma", "hyphen", "period",
"slash", "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "colon",
"semicolon", "less", "equal", "greater", "question", "at", "A", "B", "C", "D", "E", "F", "G", "H",
"I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "bracketleft",
"backslash", "bracketright", "asciicircum", "underscore", "grave", "a", "b", "c", "d", "e", "f", "g", "h",
"i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "braceleft", "bar",
"braceright", "asciitilde", "Adieresis", "Aring", "Ccedilla", "Eacute", "Ntilde", "Odieresis", "Udieresis",
"aacute", "agrave", "acircumflex", "adieresis", "atilde", "aring", "ccedilla", "eacute", "egrave", "ecircumflex",
"edieresis", "iacute", "igrave", "icircumflex", "idieresis", "ntilde", "oacute", "ograve", "ocircumflex", "odieresis",
"otilde", "uacute", "ugrave", "ucircumflex", "udieresis", "dagger", "degree", "cent", "sterling", "section", "bullet",
"paragraph", "germandbls", "registered", "copyright", "trademark", "acute", "dieresis", "notequal", "AE", "Oslash",
"infinity", "plusminus", "lessequal", "greaterequal", "yen", "mu", "partialdiff", "summation", "product", "pi",
"integral", "ordfeminine", "ordmasculine", "Omega", "ae", "oslash", "questiondown", "exclamdown", "logicalnot",
"radical", "florin", "approxequal", "Delta", "guillemotleft", "guillemotright", "ellipsis", "nonbreakingspace",
"Agrave", "Atilde", "Otilde", "OE", "oe", "endash", "emdash", "quotedblleft", "quotedblright", "quoteleft",
"quoteright", "divide", "lozenge", "ydieresis", "Ydieresis", "fraction", "currency", "guilsinglleft", "guilsinglright",
"fi", "fl", "daggerdbl", "periodcentered", "quotesinglbase", "quotedblbase", "perthousand", "Acircumflex", "Ecircumflex",
"Aacute", "Edieresis", "Egrave", "Iacute", "Icircumflex", "Idieresis", "Igrave", "Oacute", "Ocircumflex", "apple", "Ograve",
"Uacute", "Ucircumflex", "Ugrave", "dotlessi", "circumflex", "tilde", "macron", "breve", "dotaccent", "ring", "cedilla",
"hungarumlaut", "ogonek", "caron", "Lslash", "lslash", "Scaron", "scaron", "Zcaron", "zcaron", "brokenbar", "Eth", "eth",
"Yacute", "yacute", "Thorn", "thorn", "minus", "multiply", "onesuperior", "twosuperior", "threesuperior", "onehalf",
"onequarter", "threequarters", "franc", "Gbreve", "gbreve", "Idotaccent", "Scedilla", "scedilla", "Cacute", "cacute",
"Ccaron", "ccaron", "dcroat" };

bool TTFRRW::TTFRRW::Parse_POST_Table(MemoryStream* vMem, const ttfrrwProcessingFlags& vFlags, TTFRRW_ATOMIC_PARAMS)
{
	(void)vProgress;

	ZoneScoped;

	if (m_Tables.find("post") != m_Tables.end())
	{
		ATOMIC_RETURN_IF_STOP_WORKING(false);

		auto tbl = m_Tables["post"];
		vMem->SetPos(tbl.offset);
		//uint32_t len = tbl.length;

		m_GlyphNames.clear();

		const MemoryStream::Fixed format = vMem->ReadFixed();
		/*MemoryStream::Fixed italicAngle =*/ //vMem->ReadFixed();//4
		/*MemoryStream::FWord underlinePosition =*/ //(MemoryStream::FWord)vMem->ReadFWord();//2
		/*MemoryStream::FWord underlineThickness =*/ //(MemoryStream::FWord)vMem->ReadFWord();//2
		/*uint32_t isFixedPitch =*/ //(uint32_t)vMem->ReadULong();//4
		/*uint32_t minMemType42 =*/// (uint32_t)vMem->ReadULong();//4
		/*uint32_t maxMemType42 =*/ //(uint32_t)vMem->ReadULong();//4
		/*uint32_t minMemType1 =*/ //(uint32_t)vMem->ReadULong();//4
		/*uint32_t maxMemType1 =*/// (uint32_t)vMem->ReadULong();//4 => offset of 28

		if (format.high == 1)
		{
			for (size_t idx = 0; idx < STANDARD_MAC_NAMES_COUNT; idx++)
			{
				ATOMIC_OBJECTS_COUNT_INC;
				ATOMIC_RETURN_IF_STOP_WORKING(false);

				m_GlyphNames.push_back(standardMacNames[idx]);
			}
		}
		else if (format.high == 2)
		{
			const uint16_t numberOfGlyphs = (uint16_t)vMem->ReadUShort(28);
			if (numberOfGlyphs == m_TTFInfos.m_GlyphCount)
			{
				ATOMIC_OBJECTS_COUNT_INC;
				ATOMIC_RETURN_IF_STOP_WORKING(false);

				std::vector<uint16_t> glyphNameIndex;
				for (int i = 0; i < numberOfGlyphs; i++)
				{
					ATOMIC_OBJECTS_COUNT_INC;
					ATOMIC_RETURN_IF_STOP_WORKING(false);

					glyphNameIndex.push_back((uint16_t)vMem->ReadUShort(28));
				}

				const size_t endPos = (size_t)tbl.offset + (size_t)tbl.length;

				std::vector<std::string> pendingNames;
				while (vMem->GetPos() < endPos)
				{
					ATOMIC_OBJECTS_COUNT_INC;
					ATOMIC_RETURN_IF_STOP_WORKING(false);

					const uint8_t len = vMem->ReadByte(28);
					const std::string str = vMem->ReadString(len, 28);
					pendingNames.push_back(str);
				}

				for (int i = 0; i < numberOfGlyphs; i++)
				{
					ATOMIC_OBJECTS_COUNT_INC;
					ATOMIC_RETURN_IF_STOP_WORKING(false);

					const uint16_t mapIdx = glyphNameIndex[i];
					if (mapIdx >= 258)
					{
						const uint16_t idx = mapIdx - 258;
						if (idx < pendingNames.size())
							m_GlyphNames.push_back(pendingNames[idx]);
					}
					else
					{
						m_GlyphNames.emplace_back(standardMacNames[mapIdx]);
					}
				}
			}
			else
			{
				LogError(vFlags, "ERR : POST Glyph count mismatch the Glyph count in MAXP\n");
			}
		}
		else if (format.high == 3)
		{
			// not an error
			// just post is wanted in any ttf as a base table
			// but can be empty, and in this case the format is 3
			// so no names but no error too
			// https://docs.microsoft.com/en-us/typography/opentype/spec/post#version-30
		}
		else
		{
			LogError(vFlags, "ERR : POST Format %u not supported for the moment\n", format.high);
		}

		return true;
	}
	else
	{
		LogError(vFlags, "ERR : POST Table not found\n");
	}

	return false;
}

bool TTFRRW::TTFRRW::Parse_CPAL_Table(MemoryStream* vMem, const ttfrrwProcessingFlags& vFlags, TTFRRW_ATOMIC_PARAMS)
{
	(void)vProgress;

	ZoneScoped;

	if (m_Tables.find("CPAL") != m_Tables.end())
	{
		ATOMIC_OBJECTS_COUNT_INC;

		auto tbl = m_Tables["CPAL"];
		vMem->SetPos(tbl.offset);
		//uint32_t len = tbl.length;

		const uint16_t version = (uint16_t)vMem->ReadUShort();
		if (version == 0)
		{
			const uint16_t numPaletteEntries = (uint16_t)vMem->ReadUShort();
			const uint16_t numPalettes = (uint16_t)vMem->ReadUShort();
			/*uint16_t numColorRecords =*/ //(uint16_t)vMem->ReadUShort();
			const uint32_t colorRecordsArrayOffset = (uint32_t)vMem->ReadULong(2);

			std::vector<uint16_t> colorRecordIndices; // numPalettes
			colorRecordIndices.resize(numPalettes);
			for (int paletteIndex = 0; paletteIndex < numPalettes; paletteIndex++)
			{
				ATOMIC_OBJECTS_COUNT_INC;
				ATOMIC_RETURN_IF_STOP_WORKING(false);

				colorRecordIndices[paletteIndex] = (uint16_t)vMem->ReadUShort(2);
			}

			m_Palettes.resize(numPalettes);
			for (size_t paletteIndex = 0; paletteIndex < numPalettes; paletteIndex++)
			{
				ATOMIC_OBJECTS_COUNT_INC;
				ATOMIC_RETURN_IF_STOP_WORKING(false);

				for (size_t paletteEntryIndex = 0; paletteEntryIndex < numPaletteEntries; paletteEntryIndex++)
				{
					ATOMIC_OBJECTS_COUNT_INC;
					ATOMIC_RETURN_IF_STOP_WORKING(false);

					const size_t colorRecordIndex = (size_t)colorRecordIndices[paletteIndex] + paletteEntryIndex;

					vMem->SetPos(tbl.offset + (size_t)colorRecordsArrayOffset + colorRecordIndex * 4U); //-V112

					fvec4 col; // BGRA
					col.z = (float)(vMem->ReadByte()) / 255.0f; // B
					col.y = (float)(vMem->ReadByte()) / 255.0f; // G
					col.x = (float)(vMem->ReadByte()) / 255.0f; // R
					col.w = (float)(vMem->ReadByte()) / 255.0f; // A

					m_Palettes[paletteIndex].push_back(col);
				}
			}
		}
		else
		{
			LogError(vFlags, "ERR : CPAL Format %u not supported for the moment\n", version);
		}

		return true;
	}
	else
	{
		LogError(vFlags, "ERR : CPAL Table not found\n");
	}

	return false;
}

bool TTFRRW::TTFRRW::Parse_COLR_Table(MemoryStream* vMem, const ttfrrwProcessingFlags& vFlags, TTFRRW_ATOMIC_PARAMS)
{
	(void)vProgress;

	ZoneScoped;

	if (m_Tables.find("COLR") != m_Tables.end())
	{
		ATOMIC_RETURN_IF_STOP_WORKING(false);

		auto tbl = m_Tables["COLR"];
		vMem->SetPos(tbl.offset);
		//uint32_t len = tbl.length;

		/*uint16_t version =*/ //(uint16_t)vMem->ReadUShort();
		const uint16_t numBaseGlyphRecords = (uint16_t)vMem->ReadUShort(2);
		const uint32_t baseGlyphRecordsOffset = (uint32_t)vMem->ReadULong(2);
		const uint32_t layerRecordsOffset = (uint32_t)vMem->ReadULong(2);
		/*uint16_t numLayerRecords =*/ //(uint16_t)vMem->ReadUShort();

		for (size_t glyphRecordID = 0; glyphRecordID < (size_t)numBaseGlyphRecords; glyphRecordID++)
		{
			ATOMIC_OBJECTS_COUNT_INC;
			ATOMIC_RETURN_IF_STOP_WORKING(false);

			vMem->SetPos(tbl.offset + (size_t)baseGlyphRecordsOffset + glyphRecordID * 6U);
			const uint16_t baseGlyphID = (uint16_t)vMem->ReadUShort();
			const uint16_t firstLayerIndex = (uint16_t)vMem->ReadUShort();
			const uint16_t numLayers = (uint16_t)vMem->ReadUShort();

			if (baseGlyphID < m_Glyphs.size())
			{
				for (size_t layerID = 0; layerID < (size_t)numLayers; layerID++)
				{
					ATOMIC_OBJECTS_COUNT_INC;
					ATOMIC_RETURN_IF_STOP_WORKING(false);

					vMem->SetPos(tbl.offset + (size_t)layerRecordsOffset + ((size_t)firstLayerIndex + layerID) * 4U); //-V112
					const uint16_t glyphID = (uint16_t)vMem->ReadUShort();
					const uint16_t paletteID = (uint16_t)vMem->ReadUShort();

					// if CPAL would have been parsed before, i could directly write the palette color
					// we not select the palette (its is not defiend by font but by app, so by design)
					if (!m_Palettes.empty())
					{
						if (paletteID < m_Palettes[0].size())
						{
							auto color = m_Palettes[0][paletteID];
							if (glyphID < m_Glyphs.size())
							{
								m_Glyphs[glyphID].m_Color[baseGlyphID] = color;
								m_Glyphs[glyphID].m_PaletteIndex[baseGlyphID] = paletteID;
								m_Glyphs[glyphID].m_IsLayer = true;
								m_Glyphs[glyphID].m_Parents.emplace(baseGlyphID);
							}
							else
								LogError(vFlags, "ERR : COLR Layer.GlyphId >= than glyph count\n");
						}
						else
							LogError(vFlags, "ERR : COLR paletteID > than palette entry count\n");
					}

					m_Glyphs[baseGlyphID].m_Layers.push_back(glyphID);
				}
			}
		}

		return true;
	}
	else
	{
		LogError(vFlags, "ERR : COLR Table not found\n");
	}

	return false;
}

bool TTFRRW::TTFRRW::Parse_HHEA_Table(MemoryStream* vMem, const ttfrrwProcessingFlags& vFlags, TTFRRW_ATOMIC_PARAMS)
{
	(void)vProgress;

	ZoneScoped;

	if (m_Tables.find("hhea") != m_Tables.end())
	{
		ATOMIC_OBJECTS_COUNT_INC;
		ATOMIC_RETURN_IF_STOP_WORKING(false);

		auto tbl = m_Tables["hhea"];
		vMem->SetPos(tbl.offset);
		//uint32_t len = tbl.length;

		/*MemoryStream::Fixed version =*/ //vMem->ReadFixed();
		m_TTFInfos.m_Ascent = (int16_t)vMem->ReadShort(4);
		m_TTFInfos.m_Descent = (int16_t)vMem->ReadShort(4);
		m_TTFInfos.m_LineGap = (int16_t)vMem->ReadShort(4);
		m_TTFInfos.m_AdvanceWidthMax = (uint16_t)vMem->ReadUShort(4);
		m_TTFInfos.m_MinLeftSideBearing = (int16_t)vMem->ReadShort(4);
		m_TTFInfos.m_MinRightSideBearing = (int16_t)vMem->ReadShort(4);
		m_TTFInfos.m_XMaxExtent = (int16_t)vMem->ReadShort(4);
		/*
		int16_t caretSlopeRise = (int16_t)vMem->ReadShort(); // 2
		int16_t caretSlopeRun = (int16_t)vMem->ReadShort(); // 2
		MemoryStream::FWord caretOffset = (int16_t)vMem->ReadFWord(); // 2
		int16_t reserved1 = (int16_t)vMem->ReadShort(); // 2
		int16_t reserved2 = (int16_t)vMem->ReadShort(); // 2
		int16_t reserved3 = (int16_t)vMem->ReadShort(); // 2
		int16_t reserved4 = (int16_t)vMem->ReadShort(); // 2
		int16_t metricDataFormat = (int16_t)vMem->ReadShort(); // 2
		*/ // total 16 bytes to offset + 4
		m_MumOfLongHorMetrics = (int16_t)vMem->ReadShort(20);

		return true;
	}
	else
	{
		LogError(vFlags, "ERR : HHEA Table not found\n");
	}

	return false;
}

bool TTFRRW::TTFRRW::Parse_HMTX_Table(MemoryStream* vMem, const ttfrrwProcessingFlags& vFlags, TTFRRW_ATOMIC_PARAMS)
{
	(void)vProgress;

	ZoneScoped;

	if (m_Tables.find("hmtx") != m_Tables.end())
	{
		ATOMIC_OBJECTS_COUNT_INC;
		ATOMIC_RETURN_IF_STOP_WORKING(false);

		auto tbl = m_Tables["hmtx"];
		vMem->SetPos(tbl.offset);
		//uint32_t len = tbl.length;

		if (m_MumOfLongHorMetrics > 0 && m_TTFInfos.m_GlyphCount > 0)
		{
			struct longHorMetric
			{
				uint16_t advanceWidth = 0;
				int16_t leftSideBearing = 0;
			};

			std::vector<longHorMetric> hMetrics;
			hMetrics.resize(m_MumOfLongHorMetrics);
			for (GlyphIndex glyphID = 0; glyphID < m_MumOfLongHorMetrics; glyphID++)
			{
				ATOMIC_OBJECTS_COUNT_INC;
				ATOMIC_RETURN_IF_STOP_WORKING(false);

				longHorMetric lhm;
				lhm.advanceWidth = (uint16_t)vMem->ReadUShort();
				lhm.leftSideBearing = (int16_t)vMem->ReadShort();
				hMetrics[glyphID] = lhm;
				if (glyphID < m_Glyphs.size())
				{
					m_Glyphs[glyphID].m_AdvanceX = lhm.advanceWidth;
					m_Glyphs[glyphID].m_LeftSideBearing = lhm.leftSideBearing;
					m_Glyphs[glyphID].m_RightSideBearing = lhm.advanceWidth -
						(lhm.leftSideBearing + m_Glyphs[glyphID].m_LocalBBox.upperBound.x - m_Glyphs[glyphID].m_LocalBBox.lowerBound.x);
				}
			}

			if (m_TTFInfos.m_GlyphCount > (uint32_t)m_MumOfLongHorMetrics)
			{
				std::vector<MemoryStream::FWord> leftSideBearings;
				const size_t leftSideBearingCount = (size_t)m_TTFInfos.m_GlyphCount - (size_t)m_MumOfLongHorMetrics;
				leftSideBearings.resize(leftSideBearingCount);
				for (GlyphIndex idx = 0; idx < leftSideBearingCount; idx++)
				{
					ATOMIC_OBJECTS_COUNT_INC;
					ATOMIC_RETURN_IF_STOP_WORKING(false);

					leftSideBearings[idx] = vMem->ReadFWord();
					const GlyphIndex glyphID = m_MumOfLongHorMetrics + idx;
					if (glyphID < m_Glyphs.size())
					{
						const size_t lastMetric = (size_t)m_MumOfLongHorMetrics - 1U;
						m_Glyphs[glyphID].m_AdvanceX = hMetrics[lastMetric].advanceWidth;
						m_Glyphs[glyphID].m_LeftSideBearing = leftSideBearings[idx];
						m_Glyphs[glyphID].m_RightSideBearing = hMetrics[lastMetric].advanceWidth -
							(hMetrics[lastMetric].leftSideBearing + m_Glyphs[glyphID].m_LocalBBox.upperBound.x - m_Glyphs[glyphID].m_LocalBBox.lowerBound.x);
					}
				}
			}
			else
			{
				if (m_TTFInfos.m_GlyphCount < (uint32_t)m_MumOfLongHorMetrics)
					LogError(vFlags, "ERR : HMTX Glyph Count < to Long HMetrics Count\n");
			}

			return true;
		}
		else
		{
			if (m_MumOfLongHorMetrics <= 0)
				LogError(vFlags, "ERR : HMTX Mum Of Long HMetrics must be > to 0\n");
			if (m_TTFInfos.m_GlyphCount <= 0)
				LogError(vFlags, "ERR : HMTX Count Glyphs must be > to 0\n");
		}
	}
	else
	{
		LogError(vFlags, "ERR : HMTX Table not found\n");
	}

	return false;
}

bool TTFRRW::TTFRRW::Parse_NAME_Table(MemoryStream* vMem, const ttfrrwProcessingFlags& vFlags, TTFRRW_ATOMIC_PARAMS)
{
	(void)vProgress;

	ZoneScoped;

	if (m_Tables.find("name") != m_Tables.end())
	{
		ATOMIC_OBJECTS_COUNT_INC;
		ATOMIC_RETURN_IF_STOP_WORKING(false);

		auto tbl = m_Tables["name"];
		vMem->SetPos(tbl.offset);
		//uint32_t len = tbl.length;

		const uint16_t version = (uint16_t)vMem->ReadUShort();
		if (version == 0)
		{
			const uint16_t count = (uint16_t)vMem->ReadUShort();
			const uint16_t storageOffset = (uint16_t)vMem->ReadUShort();
		
			for (size_t i = 0; i < count; i++)
			{
				vMem->SetPos(tbl.offset + 6U + 12U * i);

				//12 bytes :
				//const uint16_t platformID = (uint16_t)vMem->ReadUShort();		// 2 bytes
				//const uint16_t encodingID = (uint16_t)vMem->ReadUShort();		// 2 bytes
				//const uint16_t languageID = (uint16_t)vMem->ReadUShort();		// 2 bytes
				const uint16_t nameID = (uint16_t)vMem->ReadUShort(6);			// 2 bytes
				const uint16_t length = (uint16_t)vMem->ReadUShort(6);			// 2 bytes
				const uint16_t stringOffset = (uint16_t)vMem->ReadUShort(6);	// 2 bytes

				vMem->SetPos(tbl.offset + storageOffset + stringOffset);
				const std::string name = vMem->ReadString(length);
				LogInfos(vFlags, "NameID %u => %s", nameID, name.c_str());
				m_Names.emplace(std::pair<uint16_t, std::string>(nameID, name));
			}
		}
		else
		{
			LogError(vFlags, "ERR : NAME Format %u not supported for the moment\n", version);
		}
	}
	else
	{
		LogError(vFlags, "ERR : NAME Table not found\n");
	}

	return false;
}

///////////////////////////////////////////////////////////////////////
//// PRIVATE TABLES ///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

TTFRRW::MemoryStream TTFRRW::TTFRRW::Assemble_GLYF_Table()
{
	ZoneScoped;

	MemoryStream mem;

	return mem;
}

TTFRRW::MemoryStream TTFRRW::TTFRRW::Assemble_LOCA_Table()
{
	ZoneScoped;

	MemoryStream mem;

	return mem;
}

TTFRRW::MemoryStream TTFRRW::TTFRRW::Assemble_MAXP_Table()
{
	ZoneScoped;

	MemoryStream mem;

	return mem;
}

TTFRRW::MemoryStream TTFRRW::TTFRRW::Assemble_CMAP_Table()
{
	ZoneScoped;

	MemoryStream mem;

	uint16_t version = 0; // Version number(Set to zero)
	mem.WriteUShort(version);
	uint16_t numberSubtables = 1; // Number of encoding subtables
	mem.WriteUShort(numberSubtables);

	// 0 	Unicode 	Indicates Unicode version.
	// 1 	Macintosh 	Script Manager code.
	// 2 	(reserved; do not use)
	// 3 	Microsoft 	Microsoft encoding.
	uint16_t platformID = 0; // Platform identifier
	mem.WriteUShort(platformID);

	// 0 	Version 1.0 semantics
	// 1 	Version 1.1 semantics
	// 2 	ISO 10646 1993 semantics (deprecated)
	// 3 	Unicode 2.0 or later semantics (BMP only)
	// 4 	Unicode 2.0 or later semantics (non-BMP characters allowed)
	// 5 	Unicode Variation Sequences
	// 6 	Last Resort
	uint16_t platformSpecificID = 0; // Platform - specific encoding identifier
	mem.WriteUShort(platformSpecificID);
	uint32_t offset = 0; // Offset of the mapping table from start of the cmap table
	mem.WriteUShort(offset);

	uint16_t format = 6; // Format number is set to 6
	uint16_t length; // Length of subtable in bytes
	uint16_t language; // Language code(see above)
	uint16_t segCountX2; // 2 * segCount
	uint16_t searchRange; // 2 * (2 * *FLOOR(log2(segCount)))
	uint16_t entrySelector; // log2(searchRange / 2)
	uint16_t rangeShift; // (2 * segCount) - searchRange
	uint16_t endCode;// [segCount] ; // Ending character code for each segment, last = 0xFFFF.
	uint16_t reservedPad; // This value should be zero
	uint16_t startCode;// [segCount]; // Starting character code for each segment
	uint16_t idDelta;// [segCount]; // Delta for all character codes in segment
	uint16_t idRangeOffset;// [segCount]; // Offset in bytes to glyph indexArray, or 0
	uint16_t glyphIndexArray;// [variable] ; // Glyph index array
	

	return mem;
}

TTFRRW::MemoryStream TTFRRW::TTFRRW::Assemble_HMTX_Table()
{
	ZoneScoped;

	MemoryStream mem;

	return mem;
}

TTFRRW::MemoryStream TTFRRW::TTFRRW::Assemble_HHEA_Table()
{
	ZoneScoped;

	MemoryStream mem;

	const int16_t caretSlopeRise = 0; // used to calculate the slope of the caret(rise / run) set to 1 for vertical caret
	const int16_t caretSlopeRun = 0; // 0 for vertical
	const MemoryStream::FWord caretOffset = 0; // set value to 0 for non - slanted fonts
	const int16_t metricDataFormat = 0; // 0 for current format

	MemoryStream::Fixed version; version.high = 1;
	mem.WriteFixed(version);
	mem.WriteFWord(m_TTFInfos.m_Ascent);
	mem.WriteFWord(m_TTFInfos.m_Descent);
	mem.WriteFWord(m_TTFInfos.m_LineGap);
	mem.WriteUFWord(m_TTFInfos.m_AdvanceWidthMax);
	mem.WriteFWord(m_TTFInfos.m_MinLeftSideBearing);
	mem.WriteFWord(m_TTFInfos.m_MinRightSideBearing);
	mem.WriteFWord(m_TTFInfos.m_XMaxExtent);
	mem.WriteShort(caretSlopeRise);
	mem.WriteShort(caretSlopeRun);
	mem.WriteFWord(caretOffset);
	mem.WriteShort(0); // reserved
	mem.WriteShort(0); // reserved
	mem.WriteShort(0); // reserved
	mem.WriteShort(0); // reserved
	mem.WriteShort(metricDataFormat);
	mem.WriteUShort(m_MumOfLongHorMetrics);

	return mem;
}

TTFRRW::MemoryStream TTFRRW::TTFRRW::Assemble_POST_Table()
{
	ZoneScoped;

	MemoryStream mem;

	std::unordered_map<std::string, int32_t> invertedStandardNames;
	for (int32_t i = 0; i < STANDARD_MAC_NAMES_COUNT; ++i)
	{
		invertedStandardNames[standardMacNames[i]] = i;
	}

	MemoryStream::Fixed table_Version; table_Version.high = 2;
	mem.WriteFixed(table_Version); // version
	MemoryStream::Fixed italicAngle; italicAngle.high = 0;
	mem.WriteFixed(italicAngle); // italic
	mem.WriteShort(0); // underlinePosition
	mem.WriteShort(0); // underlineThickness
	mem.WriteULong(0); // isFixedPitch
	mem.WriteULong(0); // minMemType42
	mem.WriteULong(0); // maxMemType42
	mem.WriteULong(0); // minMemType1
	mem.WriteULong(0); // maxMemType1
	mem.WriteUShort((int32_t)m_Glyphs.size());

	MemoryStream names;
	int32_t tableIndex = STANDARD_MAC_NAMES_COUNT;
	for (size_t idx = 0; idx < m_Glyphs.size(); idx++)
	{
		const auto& name = m_Glyphs[idx].m_Name;
		if (!name.empty())
		{
			int32_t glyphNameIndex = 0;
			if (invertedStandardNames.find(name) != invertedStandardNames.end())
			{
				glyphNameIndex = invertedStandardNames.at(name);
			}
			else
			{
				glyphNameIndex = tableIndex++;
				names.WriteString(name);
			}
			mem.WriteUShort(glyphNameIndex);
		}
	}
	
	mem.AppendMemoryStream(names);
	
	return mem;
}

TTFRRW::MemoryStream TTFRRW::TTFRRW::Assemble_NAME_Table()
{
	ZoneScoped;

	MemoryStream mem;

	return mem;
}

TTFRRW::MemoryStream TTFRRW::TTFRRW::Assemble_HEAD_Table()
{
	ZoneScoped;

	MemoryStream mem;

	// 0x00010000 if (version 1.0)
	MemoryStream::Fixed version; version.high = 1;
	mem.WriteFixed(version);

	// set by font manufacturer
	MemoryStream::Fixed fontRevision; fontRevision.high = 0;
	mem.WriteFixed(fontRevision);

	// To compute: set it to 0, calculate the checksum for the 'head' table 
	// and put it in the table directory, sum the entire font as a uint32_t, 
	// then store 0xB1B0AFBA - sum. (The checksum for the 'head' table will 
	// be wrong as a result. That is OK; do not reset it.)
	uint32_t checkSumAdjustment = 0;
	mem.WriteULong(checkSumAdjustment);

	// set to 0x5F0F3CF5
	mem.WriteULong(0x5F0F3CF5);

	// bit 0 - y value of 0 specifies baseline
	// bit 1 - x position of left most black bit is LSB
	// bit 2 - scaled point size and actual point size will differ (i.e. 24 point glyph differs from 12 point glyph scaled by factor of 2)
	// bit 3 - use integer scaling instead of fractional
	// bit 4 - (used by the Microsoft implementation of the TrueType scaler)
	// bit 5 - This bit should be set in fonts that are intended to e laid out vertically, and in which the glyphs have been drawn such that an x-coordinate of 0 corresponds to the desired vertical baseline.
	// bit 6 - This bit must be set to zero.
	// bit 7 - This bit should be set if the font requires layout for correct linguistic rendering (e.g. Arabic fonts).
	// bit 8 - This bit should be set for an AAT font which has one or more metamorphosis effects designated as happening by default.
	// bit 9 - This bit should be set if the font contains any strong right-to-left glyphs.
	// bit 10 - This bit should be set if the font contains Indic-style rearrangement effects.
	// bits 11-13 - Defined by Adobe.
	// bit 14 - This bit should be set if the glyphs in the font are simply generic symbols for code point ranges, such as for a last resort font.
	uint16_t flags = 0;
	mem.WriteUShort(flags);

	// range from 64 to 16384
	uint16_t unitsPerEm = m_TTFInfos.m_GlobalBBox.upperBound.x - m_TTFInfos.m_GlobalBBox.lowerBound.x;
	mem.WriteUShort(unitsPerEm);

	// international dates
	MemoryStream::longDateTime created = 0;
	mem.WriteDateTime(created);
	MemoryStream::longDateTime modified = 0;
	mem.WriteDateTime(modified);

	// for all glyph bounding boxes
	mem.WriteFWord(m_TTFInfos.m_GlobalBBox.lowerBound.x);
	mem.WriteFWord(m_TTFInfos.m_GlobalBBox.lowerBound.y);
	mem.WriteFWord(m_TTFInfos.m_GlobalBBox.upperBound.x);
	mem.WriteFWord(m_TTFInfos.m_GlobalBBox.upperBound.y);
	
	// bit 0 bold
	// bit 1 italic
	// bit 2 underline
	// bit 3 outline
	// bit 4 shadow
	// bit 5 condensed(narrow)
	// bit 6 extended
	uint16_t macStyle = 0;
	mem.WriteUShort(macStyle);

	// smallest readable size in pixels
	uint16_t lowestRecPPEM = 0;
	mem.WriteUShort(lowestRecPPEM);

	// 0 Mixed directional glyphs
	// 1 Only strongly left to right glyphs
	// 2 Like 1 but also contains neutrals
	// - 1 Only strongly right to left glyphs
	// - 2 Like - 1 but also contains neutrals
	uint16_t fontDirectionHint = 0;
	mem.WriteShort(fontDirectionHint);

	// Loc table format : 0 for short offsets, 1 for long
	m_IndexToLocFormat = 0;
	mem.WriteShort(m_IndexToLocFormat);

	// 0 for current format
	uint16_t glyphDataFormat = 0;
	mem.WriteShort(glyphDataFormat);

	return mem;
}
