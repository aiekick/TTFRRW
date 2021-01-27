// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "TTFRRW.h"

#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>

#define VERBOSE_MODE

///////////////////////////////////////////////////////////////////////
//// LOGGING //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

inline static void LogInfos(TTFRRW::ttfrrwProcessingFlags vFlags, const char* fmt, ...)
{
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

inline static void LogError(TTFRRW::ttfrrwProcessingFlags vFlags, const char* fmt, ...)
{
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

void TTFRRW::cProfiler::start()
{
	firstTimeMark = std::chrono::duration_cast<std::chrono::milliseconds>
		(std::chrono::system_clock::now().time_since_epoch()).count();
}

void TTFRRW::cProfiler::end()
{
	int64_t secondTimeMark = std::chrono::duration_cast<std::chrono::milliseconds>
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

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

TTFRRW::MemoryStream::MemoryStream()
{

}

TTFRRW::MemoryStream::MemoryStream(uint8_t* vDatas, size_t vSize)
{
	Set(vDatas, vSize);
}

TTFRRW::MemoryStream::~MemoryStream()
{

}

////////////////////////////////////////////////////////////////
//// WRITE /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

void TTFRRW::MemoryStream::WriteByte(uint8_t b)
{
	m_Datas.push_back(b);
}

void TTFRRW::MemoryStream::WriteBytes(std::vector<uint8_t>* buffer)
{
	if (buffer)
	{
		m_Datas.insert(m_Datas.end(), buffer->begin(), buffer->end());
	}
}

void TTFRRW::MemoryStream::WriteInt(int32_t i)
{
	WriteByte((uint8_t)((i >> 24) & 0xff));
	WriteByte((uint8_t)((i >> 16) & 0xff));
	WriteByte((uint8_t)((i >> 8) & 0xff));
	WriteByte((uint8_t)(i & 0xff));
}

void TTFRRW::MemoryStream::WriteUShort(int32_t us)
{
	WriteByte((uint8_t)((us >> 8) & 0xff));
	WriteByte((uint8_t)(us & 0xff));
}

void TTFRRW::MemoryStream::WriteFWord(int32_t us)
{
	WriteUShort(us);
}

void TTFRRW::MemoryStream::WriteShort(int32_t s)
{
	WriteUShort(s);
}

void TTFRRW::MemoryStream::WriteUInt24(int32_t ui)
{
	WriteByte((uint8_t)(ui >> 16) & 0xff);
	WriteByte((uint8_t)(ui >> 8) & 0xff);
	WriteByte((uint8_t)ui & 0xff);
}

void TTFRRW::MemoryStream::WriteULong(int64_t ul)
{
	WriteByte((uint8_t)((ul >> 24) & 0xff));
	WriteByte((uint8_t)((ul >> 16) & 0xff));
	WriteByte((uint8_t)((ul >> 8) & 0xff));
	WriteByte((uint8_t)(ul & 0xff));
}

void TTFRRW::MemoryStream::WriteLong(int64_t l)
{
	WriteULong(l);
}

void TTFRRW::MemoryStream::WriteFixed(MemoryStream::Fixed f)
{
	WriteByte((uint8_t)((f.high >> 24) & 0xff));
	WriteByte((uint8_t)((f.high >> 16) & 0xff));
	WriteByte((uint8_t)((f.low >> 8) & 0xff));
	WriteByte((uint8_t)(f.low & 0xff));
}

void TTFRRW::MemoryStream::WriteF2DOT14(MemoryStream::F2DOT14 f)
{
	WriteShort(f.value);
}

void TTFRRW::MemoryStream::WriteDateTime(longDateTime date)
{
	WriteULong((date >> 32) & 0xffffffff); //-V112
	WriteULong(date & 0xffffffff); //-V112
}

uint8_t* TTFRRW::MemoryStream::Get()
{
	return m_Datas.data();
}

size_t TTFRRW::MemoryStream::Size()
{
	return m_Datas.size();
}

size_t TTFRRW::MemoryStream::GetPos()
{
	return m_ReadPos;
}

void TTFRRW::MemoryStream::SetPos(size_t vPos)
{
	m_ReadPos = vPos;
}

void TTFRRW::MemoryStream::Set(uint8_t* vDatas, size_t vSize)
{
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

uint8_t TTFRRW::MemoryStream::ReadByte(size_t vOffset)
{
	if (vOffset + m_ReadPos < m_Datas.size())
		return m_Datas[vOffset + m_ReadPos++];
	return 0;
}

int32_t TTFRRW::MemoryStream::ReadUShort(size_t vOffset)
{
	return 0xffff & (ReadByte(vOffset) << 8 | ReadByte(vOffset));
}

int32_t TTFRRW::MemoryStream::ReadShort(size_t vOffset)
{
	return ((ReadByte(vOffset) << 8 | ReadByte(vOffset)) << 16) >> 16;
}

TTFRRW::MemoryStream::FWord TTFRRW::MemoryStream::ReadFWord(size_t vOffset)
{
	return (int16_t)ReadShort(vOffset);
}

TTFRRW::MemoryStream::UFWord TTFRRW::MemoryStream::ReadUFWord(size_t vOffset)
{
	return (uint16_t)ReadUShort(vOffset);
}

uint32_t TTFRRW::MemoryStream::ReadUInt24(size_t vOffset)
{
	return 0xffffff & (ReadByte(vOffset) << 16 | ReadByte(vOffset) << 8 | ReadByte(vOffset));
}

uint64_t TTFRRW::MemoryStream::ReadULong(size_t vOffset)
{
	return 0xffffffffL & ReadLong(vOffset); //-V112
}

uint32_t TTFRRW::MemoryStream::ReadULongAsInt(size_t vOffset)
{
	int64_t ulong = ReadULong(vOffset);
	return ((int32_t)ulong) & ~0x80000000; //-V112
}

int32_t TTFRRW::MemoryStream::ReadLong(size_t vOffset)
{
	return
		ReadByte(vOffset) << 24 |
		ReadByte(vOffset) << 16 |
		ReadByte(vOffset) << 8 |
		ReadByte(vOffset);
}

TTFRRW::MemoryStream::Fixed TTFRRW::MemoryStream::ReadFixed(size_t vOffset)
{
	Fixed res;
	int32_t f = ReadLong(vOffset);
	res.high = (int16_t)((f >> 16) & 0xff);
	res.low = (int16_t)(f & 0xff);
	return res;
}

TTFRRW::MemoryStream::F2DOT14 TTFRRW::MemoryStream::ReadF2DOT14(size_t vOffset)
{
	F2DOT14 res;
	res.value = (int16_t)ReadShort(vOffset);
	return res;
}

TTFRRW::MemoryStream::longDateTime TTFRRW::MemoryStream::ReadDateTime(size_t vOffset)
{
	return (int64_t)ReadULong(vOffset) << 32 | ReadULong(vOffset); //-V112
}

std::string TTFRRW::MemoryStream::ReadString(size_t vLen, size_t vOffset)
{
	if (vOffset + m_ReadPos + vLen < m_Datas.size())
	{
		std::string res = std::string((char*)(m_Datas.data() + vOffset + m_ReadPos), vLen);
		m_ReadPos += vLen;
		return res;
	}
	return "";
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

TTFRRW::Contour::Contour()
{

}

TTFRRW::Contour::~Contour()
{

}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

TTFRRW::AffineGlyph::AffineGlyph()
{

}

TTFRRW::AffineGlyph::~AffineGlyph()
{

}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

TTFRRW::Glyph::Glyph()
{

}

TTFRRW::Glyph::~Glyph()
{

}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#define ATOMIC_RETURN_IF_STOP_WORKING(v) if (vWorking) if (!vWorking->load()) return v;
#define ATOMIC_PROGRESS_ADD(v) if (vProgress) vProgress->store(vProgress->load() + v)
#define ATOMIC_OBJECTS_COUNT_INC if (vObjectCount) vObjectCount->store(vObjectCount->load() + 1U)

TTFRRW::TTFRRW::TTFRRW()
{

}

TTFRRW::TTFRRW::~TTFRRW()
{

}

void TTFRRW::TTFRRW::Clear(TTFRRW_ATOMIC_PARAMS)
{
	vProgress->store(0.0f);
	vObjectCount->store(0U);

	m_TTFInfos = TTFInfos();
	m_TTFProfiler.Reset();

	m_IsValid_For_Rasterize = false;
	m_IsValid_For_GlyphTreatment = false;

	m_Glyphs.clear();
	m_GlyphNames.clear();
	m_CodePoint_To_GlyphIndex.clear();
	m_GlyphIndex_To_CodePoints.clear();

	m_FontCopyright.clear();
	m_FontFamily.clear();
	m_FontSubFamily.clear();
	m_FontFullName.clear();
	m_FontTrademark.clear();
	m_FontDesignerName.clear();
	m_FontManufacturerName.clear();
	m_UrlFontDesigner.clear();
	m_UrlFontVendor.clear();

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
	bool res = false;

	cProfiler mainProfiler;
	mainProfiler.start();

	MemoryStream mem;

	int error = 0;
	res = LoadFileToMemory(vFontFilePathName, &mem, &error);
	if (res)
	{
		res = Parse_Font_File(&mem, vFlags, TTFRRW_ATOMIC_PARAMS_BY_REF);
	}

	mainProfiler.end();
	mainProfiler.print(vFlags, "OpenFontFile ", vDebugInfos);

	return res;
}

bool TTFRRW::TTFRRW::OpenFontStream(
	uint8_t* vStream, size_t vStreamSize,
	ttfrrwProcessingFlags vFlags,
	const char* vDebugInfos,
	TTFRRW_ATOMIC_PARAMS)
{
	bool res = false;

	cProfiler mainProfiler;
	mainProfiler.start();

	if (vStream && vStreamSize)
	{
		MemoryStream mem;

		mem.Set(vStream, vStreamSize);

		res = Parse_Font_File(&mem, vFlags, TTFRRW_ATOMIC_PARAMS_BY_REF);
	}

	mainProfiler.end();
	mainProfiler.print(vFlags, "OpenFontStream ", vDebugInfos);

	return res;
}

std::vector<TTFRRW::Glyph>* TTFRRW::TTFRRW::GetGlyphs()
{
	if (!m_Glyphs.empty())
	{
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
			return &m_Glyphs[vGlyphIndex];
		}
	}

	return nullptr;
}

bool TTFRRW::TTFRRW::WriteFontFile(const std::string& vFontFilePathName)
{
	(void)vFontFilePathName;

	return false;
}

///////////////////////////////////////////////////////////////////////
//// PUBLIC METHOD'S //////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

void TTFRRW::TTFRRW::AddGlyph(const Glyph& vGlyph, const CodePoint& vCodePoint)
{
	(void)vGlyph;
	(void)vCodePoint;
}

TTFRRW::Glyph* TTFRRW::TTFRRW::GetGlyphWithCodePoint(const CodePoint& vCodePoint)
{
	(void)vCodePoint;

	return nullptr;
}

TTFRRW::GlyphIndex TTFRRW::TTFRRW::GetGlyphIndexFromCodePoint(const CodePoint& vCodePoint)
{
	if (m_CodePoint_To_GlyphIndex.find(vCodePoint) != m_CodePoint_To_GlyphIndex.end())
	{
		return m_CodePoint_To_GlyphIndex[vCodePoint];
	}

	return 0;
}

std::set<TTFRRW::CodePoint>* TTFRRW::TTFRRW::GetCodePointsFromGlyphIndex(const GlyphIndex& vGlyphIndex)
{
	if (m_GlyphIndex_To_CodePoints.find(vGlyphIndex) != m_GlyphIndex_To_CodePoints.end())
	{
		return &m_GlyphIndex_To_CodePoints[vGlyphIndex];
	}

	return nullptr;
}

TTFRRW::TTFInfos TTFRRW::TTFRRW::GetFontInfos()
{
	return m_TTFInfos;
}

bool TTFRRW::TTFRRW::IsValidForRasterize()
{
	return m_IsValid_For_Rasterize;
}

bool TTFRRW::TTFRRW::IsValidFotGlyppTreatment()
{
	return m_IsValid_For_GlyphTreatment;
}

///////////////////////////////////////////////////////////////////////
//// PRIVATE FILE / STREAM ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

bool TTFRRW::TTFRRW::LoadFileToMemory(
	const std::string& vFilePathName,
	MemoryStream* vInMem,
	int* vError)
{
	bool res = false;

	if (vInMem)
	{
		std::vector<uint8_t> bytes;

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
			size_t fileSize = 0;

			// obtain file size:
			fseek(intput_file, 0, SEEK_END); //-V303
			fileSize = (size_t)ftell(intput_file); //-V303
			rewind(intput_file);

			if (fileSize)
			{
				// copy the file into the buffer and close
				bytes.resize(fileSize);
				fread(bytes.data(), 1, fileSize, intput_file);
				vInMem->Set(bytes.data(), bytes.size());
			}

			fclose(intput_file);

			res = true;
		}
	}

	return res;
}

bool TTFRRW::TTFRRW::WriteMemoryToFile(
	const std::string& vFilePathName,
	MemoryStream* vOutMem,
	int* vError)
{
	bool res = false;

	if (vOutMem)
	{
		if (vOutMem->Size())
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
				fwrite(vOutMem->Get(), 1, vOutMem->Size(), output_file);
				fflush(output_file);
				fclose(output_file);
				res = true;
			}
		}
	}

	return res;
}

///////////////////////////////////////////////////////////////////////
//// PRIVATE PARSER ///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

bool TTFRRW::TTFRRW::Parse_Font_File(MemoryStream* vMem, ttfrrwProcessingFlags vFlags, TTFRRW_ATOMIC_PARAMS)
{
	// la loca contient la position des glyphs et leur taille
	// la maxp contient le nombre de glyphs
	// la cmap contient la correspondance entre les codepoint et les index de glyphs
	// la glyf contient les glyf adress� par la loca
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
			bool maxpOK = Parse_MAXP_Table(vMem, vFlags, TTFRRW_ATOMIC_PARAMS_BY_REF);
			if (maxpOK) // dependencies
			{
				/*bool cmapOK =*/ Parse_CMAP_Table(vMem, vFlags, TTFRRW_ATOMIC_PARAMS_BY_REF);
				//ATOMIC_PROGRESS_ADD(0.01f);	// 0.1
				ATOMIC_RETURN_IF_STOP_WORKING(false);
				bool headOK = Parse_HEAD_Table(vMem, vFlags, TTFRRW_ATOMIC_PARAMS_BY_REF);
				//ATOMIC_PROGRESS_ADD(0.02f);	// 0.2
				ATOMIC_RETURN_IF_STOP_WORKING(false);
				//ATOMIC_PROGRESS_ADD(0.03f);	// 0.3
				ATOMIC_RETURN_IF_STOP_WORKING(false);
				bool locaOK = Parse_LOCA_Table(vMem, vFlags, TTFRRW_ATOMIC_PARAMS_BY_REF);
				//ATOMIC_PROGRESS_ADD(0.04f);	// 0.4
				ATOMIC_RETURN_IF_STOP_WORKING(false);
				bool hheaOK = Parse_HHEA_Table(vMem, vFlags, TTFRRW_ATOMIC_PARAMS_BY_REF);
				//ATOMIC_PROGRESS_ADD(0.1f);	// 0.5
				ATOMIC_RETURN_IF_STOP_WORKING(false);
				bool glyfOK = false;
				//bool postOK = false;
				bool hmtxOK = false;
				bool colrOK = false;
				bool cpalOK = false;

				// on fait post avant glyf comme ca 
				// on pourra mettre le nom directement dans le glyph
				/*postOK = */Parse_POST_Table(vMem, vFlags, TTFRRW_ATOMIC_PARAMS_BY_REF);
				//ATOMIC_PROGRESS_ADD(0.1f);	// 0.6
				ATOMIC_RETURN_IF_STOP_WORKING(false);

				if (headOK && locaOK) // dependencies
					glyfOK = Parse_GLYF_Table(vMem, vFlags, TTFRRW_ATOMIC_PARAMS_BY_REF);
				//ATOMIC_PROGRESS_ADD(0.1f);	// 0.7
				ATOMIC_RETURN_IF_STOP_WORKING(false);

				// on fait ca apres glyph comme ca on pourra remplir
				// les metrics dans le glyph
				if (hheaOK) // dependencies
				{
					hmtxOK = Parse_HMTX_Table(vMem, vFlags, TTFRRW_ATOMIC_PARAMS_BY_REF);
				}
				//ATOMIC_PROGRESS_ADD(0.1f);	// 0.8
				ATOMIC_RETURN_IF_STOP_WORKING(false);

				cpalOK = Parse_CPAL_Table(vMem, vFlags, TTFRRW_ATOMIC_PARAMS_BY_REF);
				//ATOMIC_PROGRESS_ADD(0.1f);	// 0.9
				ATOMIC_RETURN_IF_STOP_WORKING(false);

				if (cpalOK) // dependencies
					colrOK = Parse_COLR_Table(vMem, vFlags, TTFRRW_ATOMIC_PARAMS_BY_REF);
				//ATOMIC_PROGRESS_ADD(0.1f);	// 1.0
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

bool TTFRRW::TTFRRW::Parse_Table_Header(MemoryStream* vMem, ttfrrwProcessingFlags vFlags, TTFRRW_ATOMIC_PARAMS)
{
	// header
	std::string scalerType = vMem->ReadString(4); //-V112
	if (scalerType[0] == 1)  scalerType = "TrueType 1"; // TrueType 1
	if (scalerType[1] == 1)  scalerType = "OpenType 1"; // TrueType 1
	uint16_t numTables = (uint16_t)vMem->ReadUShort();
	/*uint16_t searchRange =*/ (uint16_t)vMem->ReadUShort();
	/*uint16_t entrySelector =*/ (uint16_t)vMem->ReadUShort();
	/*uint16_t rangeShift =*/ (uint16_t)vMem->ReadUShort();

	// tables
	for (int i = 0; i < numTables; i++)
	{
		TableStruct tbl;

		uint32_t _tag = (uint32_t)vMem->ReadULong();
		tbl.tag[0] = (uint8_t)((_tag >> 24) & 0xff);
		tbl.tag[1] = (uint8_t)((_tag >> 16) & 0xff);
		tbl.tag[2] = (uint8_t)((_tag >> 8) & 0xff);
		tbl.tag[3] = (uint8_t)(_tag & 0xff);
		tbl.tag[4] = '\0';

		tbl.checkSum = (uint32_t)vMem->ReadULong();
		tbl.offset = (size_t)vMem->ReadULong();
		tbl.length = (size_t)vMem->ReadULong();

		std::string tagString = std::string((char*)tbl.tag); //-V112
		m_Tables[tagString] = tbl;
		LogInfos(vFlags, "Table %s found\n", tagString.c_str()); //-V111

		ATOMIC_OBJECTS_COUNT_INC;
		ATOMIC_RETURN_IF_STOP_WORKING(false);
	}

	return (!m_Tables.empty());
}

bool TTFRRW::TTFRRW::Parse_CMAP_Table(MemoryStream* vMem, ttfrrwProcessingFlags vFlags, TTFRRW_ATOMIC_PARAMS)
{
	if (!vMem) return false;

	if (m_Tables.find("cmap") != m_Tables.end())
	{
		ATOMIC_OBJECTS_COUNT_INC;

		const auto tbl = m_Tables["cmap"];
		vMem->SetPos(tbl.offset);
		//uint32_t len = tbl.length;

		/*uint16_t version =*/ (uint16_t)vMem->ReadUShort();
		uint16_t numEncodingRecords = (uint16_t)vMem->ReadUShort();

		size_t sizeOfEncodingRecord = 8U;
		for (size_t encodingRecordID = 0; encodingRecordID < (size_t)numEncodingRecords; encodingRecordID++)
		{
			ATOMIC_RETURN_IF_STOP_WORKING(false);

			vMem->SetPos(tbl.offset + (size_t)4U + sizeOfEncodingRecord * encodingRecordID); //-V112

			/*uint16_t platformID =*/ (uint16_t)vMem->ReadUShort();
			/*uint16_t encodingID =*/ (uint16_t)vMem->ReadUShort();
			size_t offset = (size_t)vMem->ReadULong();

			vMem->SetPos(tbl.offset + offset);

			uint16_t format = (uint16_t)vMem->ReadUShort();
			
			if (format == 0U)
			{
				/*uint16_t length =*/ (uint16_t)vMem->ReadUShort();
				/*uint16_t language =*/ (uint16_t)vMem->ReadUShort();
				for (GlyphIndex glyphIndex = 0; glyphIndex < 256; glyphIndex++)
				{
					ATOMIC_RETURN_IF_STOP_WORKING(false);

					uint8_t codePoint = vMem->ReadByte();
					m_CodePoint_To_GlyphIndex[codePoint] = glyphIndex;
					m_GlyphIndex_To_CodePoints[glyphIndex].emplace(codePoint);
				}
			} //-V112
			else if (format == 4U) //-V112
			{
				/*uint16_t length =*/ (uint16_t)vMem->ReadUShort();
				/*uint16_t language =*/ (uint16_t)vMem->ReadUShort();
				uint16_t segCountX2 = (uint16_t)vMem->ReadUShort();
				/*uint16_t searchRange =*/ (uint16_t)vMem->ReadUShort();
				/*uint16_t entrySelector =*/ (uint16_t)vMem->ReadUShort();
				/*uint16_t rangeShift =*/ (uint16_t)vMem->ReadUShort();

				std::vector<uint16_t> endCode;
				std::vector<uint16_t> startCode;
				std::vector<int16_t> idDelta;
				std::vector<uint16_t> idRangeOffset;

				int segCount = segCountX2 / 2;
				for (int i = 0; i < segCount; i++)
				{
					ATOMIC_RETURN_IF_STOP_WORKING(false);

					endCode.push_back((uint16_t)vMem->ReadUShort());
				}
				/*uint16_t reservedPad =*/ (uint16_t)vMem->ReadUShort();
				for (int i = 0; i < segCount; i++)
				{
					ATOMIC_RETURN_IF_STOP_WORKING(false);

					startCode.push_back((uint16_t)vMem->ReadUShort());
				}
				for (int i = 0; i < segCount; i++)
				{
					ATOMIC_RETURN_IF_STOP_WORKING(false);

					idDelta.push_back((uint16_t)vMem->ReadShort());
				}
				size_t idRangeOffsetAddress = vMem->GetPos();
				for (int i = 0; i < segCount; i++)
				{
					ATOMIC_RETURN_IF_STOP_WORKING(false);

					idRangeOffset.push_back((uint16_t)vMem->ReadUShort());
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

							size_t id_range_offset = (size_t)idRangeOffset[segment];
							if (id_range_offset == 0)
							{
								foundGlyphIndex = (codePoint + idDelta[segment]) % 0x10000; // 0x10000 is 65536
							}
							else
							{
								size_t idRangeOffsetLocation = idRangeOffsetAddress + segment * sizeof(uint16_t);
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
				/*uint16_t length =*/ (uint16_t)vMem->ReadUShort();
				/*uint16_t language =*/ (uint16_t)vMem->ReadUShort();
				uint16_t firstCode = (uint16_t)vMem->ReadUShort();
				uint16_t entryCount = (uint16_t)vMem->ReadUShort();
				for (GlyphIndex glyphIndex = 0; glyphIndex < entryCount; glyphIndex++)
				{
					ATOMIC_RETURN_IF_STOP_WORKING(false);

					uint16_t codePoint = (uint16_t)vMem->ReadUShort();
					m_CodePoint_To_GlyphIndex[codePoint] = glyphIndex;
					m_GlyphIndex_To_CodePoints[glyphIndex].emplace(codePoint);
				}
			}
			else if (format == 12)
			{
				/*uint16_t reserved =*/ (uint16_t)vMem->ReadUShort();
				/*uint32_t length =*/ (uint32_t)vMem->ReadULong();
				/*uint32_t language =*/ (uint32_t)vMem->ReadULong();
				uint32_t nGroups = (uint32_t)vMem->ReadULong();
				
				for (uint32_t groupID = 0; groupID < nGroups; groupID++)
				{
					ATOMIC_RETURN_IF_STOP_WORKING(false);

					uint32_t startCharCode = (uint32_t)vMem->ReadULong();
					uint32_t endCharCode = (uint32_t)vMem->ReadULong();
					uint32_t startGlyphID = (uint32_t)vMem->ReadULong();

					uint32_t count = endCharCode - startCharCode;
					if (count)
					{
						for (uint32_t charCodeID = 0; charCodeID < count; charCodeID++)
						{
							ATOMIC_RETURN_IF_STOP_WORKING(false);

							CodePoint codePoint = startCharCode + charCodeID;
							GlyphIndex glyphIndex = startGlyphID + charCodeID;
							m_CodePoint_To_GlyphIndex[codePoint] = glyphIndex;
							m_GlyphIndex_To_CodePoints[glyphIndex].emplace(codePoint);
						}
					}
					else
					{
						CodePoint codePoint = startCharCode;
						GlyphIndex glyphIndex = startGlyphID;
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

bool TTFRRW::TTFRRW::Parse_HEAD_Table(MemoryStream* vMem, ttfrrwProcessingFlags vFlags, TTFRRW_ATOMIC_PARAMS)
{
	if (m_Tables.find("head") != m_Tables.end())
	{
		ATOMIC_OBJECTS_COUNT_INC;
		ATOMIC_RETURN_IF_STOP_WORKING(false);

		auto tbl = m_Tables["head"];
		vMem->SetPos(tbl.offset);
		//uint32_t len = tbl.length;

		/*MemoryStream::Fixed version =*/ vMem->ReadFixed();
		/*MemoryStream::Fixed fontRevision =*/ vMem->ReadFixed();
		/*uint32_t checkSumAdjustment =*/ (uint32_t)vMem->ReadULong();
		/*uint32_t magicNumber =*/ (uint32_t)vMem->ReadULong();
		/*uint16_t flags =*/ (uint16_t)vMem->ReadUShort(); // bitset
		/*uint16_t unitsPerEm =*/ (uint16_t)vMem->ReadUShort();
		/*MemoryStream::longDateTime created =*/ vMem->ReadDateTime();
		/*MemoryStream::longDateTime modified */ vMem->ReadDateTime();
		m_TTFInfos.m_GlobalBBox.lowerBound.x = vMem->ReadFWord();
		m_TTFInfos.m_GlobalBBox.lowerBound.y = vMem->ReadFWord();
		m_TTFInfos.m_GlobalBBox.upperBound.x = vMem->ReadFWord();
		m_TTFInfos.m_GlobalBBox.upperBound.y = vMem->ReadFWord();
		/*uint16_t macStyle =*/ (uint16_t)vMem->ReadUShort(); // bitset
		/*uint16_t lowestRecPPEM =*/ (uint16_t)vMem->ReadUShort();
		/*uint16_t fontDirectionHint =*/ (int16_t)vMem->ReadShort();
		m_IndexToLocFormat = (int16_t)vMem->ReadShort();
		/*uint16_t glyphDataFormat =*/ (int16_t)vMem->ReadShort();

		return true;
	}
	else
	{
		LogError(vFlags, "ERR : HEAD Table not found\n");
	}

	return false;
}

bool TTFRRW::TTFRRW::Parse_MAXP_Table(MemoryStream* vMem, ttfrrwProcessingFlags vFlags, TTFRRW_ATOMIC_PARAMS)
{
	if (m_Tables.find("maxp") != m_Tables.end())
	{
		ATOMIC_OBJECTS_COUNT_INC;
		ATOMIC_RETURN_IF_STOP_WORKING(false);

		auto tbl = m_Tables["maxp"];
		vMem->SetPos(tbl.offset);
		//uint32_t len = tbl.length;

		/*MemoryStream::Fixed version =*/ vMem->ReadFixed();
		m_TTFInfos.m_GlyphCount = (uint16_t)vMem->ReadUShort();
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

bool TTFRRW::TTFRRW::Parse_LOCA_Table(MemoryStream* vMem, ttfrrwProcessingFlags vFlags, TTFRRW_ATOMIC_PARAMS)
{
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

				size_t offset = (size_t)((uint32_t)vMem->ReadUShort()) * 2;
				m_GlyphsOffsets[i] = offset;
			}
		}
		else if (m_IndexToLocFormat == 1) // long format
		{
			for (uint16_t i = 0; i < m_TTFInfos.m_GlyphCount; i++)
			{
				ATOMIC_RETURN_IF_STOP_WORKING(false);

				size_t offset = (size_t)((uint32_t)vMem->ReadULong());
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

bool TTFRRW::TTFRRW::Parse_GLYF_Table(MemoryStream* vMem, ttfrrwProcessingFlags vFlags, TTFRRW_ATOMIC_PARAMS)
{
	if (m_Tables.find("glyf") != m_Tables.end())
	{
		ATOMIC_OBJECTS_COUNT_INC;
		ATOMIC_RETURN_IF_STOP_WORKING(false);

		auto tbl = m_Tables["glyf"];
		vMem->SetPos(tbl.offset);
		//uint32_t len = tbl.length;

		size_t length = 0;
		for (size_t glyphID = 0; glyphID < (size_t)m_TTFInfos.m_GlyphCount; glyphID++)
		{
			if (vProgress)
				vProgress->store((double)glyphID / (double)m_TTFInfos.m_GlyphCount);

			ATOMIC_OBJECTS_COUNT_INC;
			ATOMIC_RETURN_IF_STOP_WORKING(false);

			size_t glyphOffset = tbl.offset + m_GlyphsOffsets[glyphID];
			vMem->SetPos(glyphOffset);

			int16_t numberOfContours = (int16_t)vMem->ReadShort();
			MemoryStream::FWord xMin = vMem->ReadFWord();
			MemoryStream::FWord yMin = vMem->ReadFWord();
			MemoryStream::FWord xMax = vMem->ReadFWord();
			MemoryStream::FWord yMax = vMem->ReadFWord();

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
					auto g = Parse_Simple_Glyf(vMem, numberOfContours, vFlags, TTFRRW_ATOMIC_PARAMS_BY_REF);
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

			length = glyphOffset;
		}

		return true;
	}
	else
	{
		LogError(vFlags, "ERR : GLYF Table not found\n");
	}

	return false;
}

TTFRRW::Glyph TTFRRW::TTFRRW::Parse_Simple_Glyf(MemoryStream* vMem, int16_t vCountContour, ttfrrwProcessingFlags vFlags, TTFRRW_ATOMIC_PARAMS)
{
	Glyph glyph;

	if (vMem)
	{
		m_TTFProfiler.simpleGlyfProfiler.start();

		std::vector<uint16_t> endPtsOfContours; // todo: to use a std::deque say PVS => to check
		size_t instructionLength;
		std::vector<uint8_t> instructions;
		std::vector<uint8_t> flags;
		std::vector<bool> onCurves;
		std::vector<int16_t> xCoordinates;
		std::vector<int16_t> yCoordinates;

		if (vCountContour >= 0) // this is well simple glyph
		{
			size_t countContours = (size_t)vCountContour;

			endPtsOfContours.resize(countContours + 1);
			endPtsOfContours[0] = 0U;
			for (size_t contourID = 0; contourID < countContours; contourID++)
			{
				ATOMIC_RETURN_IF_STOP_WORKING(glyph);

				endPtsOfContours[contourID + 1] = (uint16_t)vMem->ReadShort();
			}

			instructionLength = (size_t)vMem->ReadUShort();
			if (instructionLength)
			{
				ATOMIC_RETURN_IF_STOP_WORKING(glyph);

				instructions.resize(instructionLength);
				for (size_t instructionID = 0; instructionID < instructionLength; instructionID++)
					instructions[instructionID] = (uint8_t)vMem->ReadByte();
			}

			const int32_t SimpleFlagOnCurve = 1;
			const int32_t SimpleFlagOnXShort = 1 << 1;
			const int32_t SimpleFlagOnYShort = 1 << 2;
			const int32_t SimpleFlagOnRepeat = 1 << 3;
			const int32_t SimpleFlagOnXRepeatSign = 1 << 4;
			const int32_t SimpleFlagOnYRepeatSign = 1 << 5;

			if (endPtsOfContours.size() > 1)
			{
				size_t countPoints = (size_t)endPtsOfContours[countContours] + 1; // always sup to 0 with the +1

				xCoordinates.resize(countPoints);
				yCoordinates.resize(countPoints);
				onCurves.resize(countPoints);
				flags.resize(countPoints);

				uint32_t flag_repeat = 0;
				int32_t flag = 0;
				for (size_t pointID = 0; pointID < countPoints; pointID++)
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

					uint8_t u8Flag = (uint8_t)flag;
					flags[pointID] = u8Flag;
					onCurves[pointID] = ((u8Flag & SimpleFlagOnCurve) == SimpleFlagOnCurve);
				}

				for (size_t pointID = 0; pointID < countPoints; pointID++)
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

				for (size_t pointID = 0; pointID < countPoints; pointID++)
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

			//endPtsOfContours.insert(endPtsOfContours.begin(), 0);
			//LogInfos(vFlags, "Contours : %u\n", (uint32_t)countContours);

			glyph.m_Contours.resize(countContours);
			for (size_t contourID = 0; contourID < countContours; contourID++)
			{
				ATOMIC_RETURN_IF_STOP_WORKING(glyph);

				auto contour = &glyph.m_Contours[contourID];

				const size_t pmax = endPtsOfContours[contourID + 1] - endPtsOfContours[contourID];
				//LogInfos(vFlags, "Contour %u, Count Points : %u\n", (uint32_t)contourID, (uint32_t)pmax);

				contour->m_Points.resize(pmax);
				contour->m_OnCurve.resize(pmax);

				for (size_t p = 0; p < pmax; p++)
				{
					ATOMIC_RETURN_IF_STOP_WORKING(glyph);

					contour->m_Points[p].x = (int32_t)xCoordinates[endPtsOfContours[contourID] + p];
					contour->m_Points[p].y = (int32_t)yCoordinates[endPtsOfContours[contourID] + p];
					contour->m_OnCurve[p] = onCurves[endPtsOfContours[contourID] + p];

					//	LogInfos(vFlags, "Point %u %s : %i,%i\n", (uint32_t)p, (oc ? "on curve" : ""), pt.x, pt.y);
				}
			}
		}

		m_TTFProfiler.simpleGlyfProfiler.end();
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

bool TTFRRW::TTFRRW::Parse_POST_Table(MemoryStream* vMem, ttfrrwProcessingFlags vFlags, TTFRRW_ATOMIC_PARAMS)
{
	if (m_Tables.find("post") != m_Tables.end())
	{
		ATOMIC_RETURN_IF_STOP_WORKING(false);

		auto tbl = m_Tables["post"];
		vMem->SetPos(tbl.offset);
		//uint32_t len = tbl.length;

		m_GlyphNames.clear();

		MemoryStream::Fixed format = vMem->ReadFixed();;
		/*MemoryStream::Fixed italicAngle =*/ vMem->ReadFixed();
		/*MemoryStream::FWord underlinePosition =*/ (MemoryStream::FWord)vMem->ReadFWord();
		/*MemoryStream::FWord underlineThickness =*/ (MemoryStream::FWord)vMem->ReadFWord();
		/*uint32_t isFixedPitch =*/ (uint32_t)vMem->ReadULong();
		/*uint32_t minMemType42 =*/ (uint32_t)vMem->ReadULong();
		/*uint32_t maxMemType42 =*/ (uint32_t)vMem->ReadULong();
		/*uint32_t minMemType1 =*/ (uint32_t)vMem->ReadULong();
		/*uint32_t maxMemType1 =*/ (uint32_t)vMem->ReadULong();

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
			uint16_t numberOfGlyphs = (uint16_t)vMem->ReadUShort();
			if (numberOfGlyphs == m_TTFInfos.m_GlyphCount)
			{
				ATOMIC_OBJECTS_COUNT_INC;
				ATOMIC_RETURN_IF_STOP_WORKING(false);

				std::vector<uint16_t> glyphNameIndex;
				for (int i = 0; i < numberOfGlyphs; i++)
				{
					ATOMIC_OBJECTS_COUNT_INC;
					ATOMIC_RETURN_IF_STOP_WORKING(false);

					glyphNameIndex.push_back((uint16_t)vMem->ReadUShort());
				}

				size_t endPos = (size_t)tbl.offset + (size_t)tbl.length;

				std::vector<std::string> pendingNames;
				while (vMem->GetPos() < endPos)
				{
					ATOMIC_OBJECTS_COUNT_INC;
					ATOMIC_RETURN_IF_STOP_WORKING(false);

					uint8_t len = vMem->ReadByte();
					std::string str = vMem->ReadString(len);
					pendingNames.push_back(str);
				}

				for (int i = 0; i < numberOfGlyphs; i++)
				{
					ATOMIC_OBJECTS_COUNT_INC;
					ATOMIC_RETURN_IF_STOP_WORKING(false);

					uint16_t mapIdx = glyphNameIndex[i];
					if (mapIdx >= 258)
					{
						uint16_t idx = mapIdx - 258;
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

bool TTFRRW::TTFRRW::Parse_CPAL_Table(MemoryStream* vMem, ttfrrwProcessingFlags vFlags, TTFRRW_ATOMIC_PARAMS)
{
	if (m_Tables.find("CPAL") != m_Tables.end())
	{
		ATOMIC_OBJECTS_COUNT_INC;

		auto tbl = m_Tables["CPAL"];
		vMem->SetPos(tbl.offset);
		//uint32_t len = tbl.length;

		uint16_t version = (uint16_t)vMem->ReadUShort();

		if (version == 0)
		{
			uint16_t numPaletteEntries = (uint16_t)vMem->ReadUShort();
			uint16_t numPalettes = (uint16_t)vMem->ReadUShort();
			/*uint16_t numColorRecords =*/ (uint16_t)vMem->ReadUShort();
			uint32_t colorRecordsArrayOffset = (uint32_t)vMem->ReadULong();

			std::vector<uint16_t> colorRecordIndices; // numPalettes
			colorRecordIndices.resize(numPalettes);
			for (int paletteIndex = 0; paletteIndex < numPalettes; paletteIndex++)
			{
				ATOMIC_OBJECTS_COUNT_INC;
				ATOMIC_RETURN_IF_STOP_WORKING(false);

				colorRecordIndices[paletteIndex] = (uint16_t)vMem->ReadUShort();
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

					size_t colorRecordIndex = (size_t)colorRecordIndices[paletteIndex] + paletteEntryIndex;

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

bool TTFRRW::TTFRRW::Parse_COLR_Table(MemoryStream* vMem, ttfrrwProcessingFlags vFlags, TTFRRW_ATOMIC_PARAMS)
{
	if (m_Tables.find("COLR") != m_Tables.end())
	{
		ATOMIC_RETURN_IF_STOP_WORKING(false);

		auto tbl = m_Tables["COLR"];
		vMem->SetPos(tbl.offset);
		//uint32_t len = tbl.length;

		/*uint16_t version =*/ (uint16_t)vMem->ReadUShort();
		uint16_t numBaseGlyphRecords = (uint16_t)vMem->ReadUShort();
		uint32_t baseGlyphRecordsOffset = (uint32_t)vMem->ReadULong();
		uint32_t layerRecordsOffset = (uint32_t)vMem->ReadULong();
		/*uint16_t numLayerRecords =*/ (uint16_t)vMem->ReadUShort();

		for (size_t glyphRecordID = 0; glyphRecordID < (size_t)numBaseGlyphRecords; glyphRecordID++)
		{
			ATOMIC_OBJECTS_COUNT_INC;
			ATOMIC_RETURN_IF_STOP_WORKING(false);

			vMem->SetPos(tbl.offset + (size_t)baseGlyphRecordsOffset + glyphRecordID * 6U);
			uint16_t baseGlyphID = (uint16_t)vMem->ReadUShort();
			uint16_t firstLayerIndex = (uint16_t)vMem->ReadUShort();
			uint16_t numLayers = (uint16_t)vMem->ReadUShort();

			if (baseGlyphID < m_Glyphs.size())
			{
				for (size_t layerID = 0; layerID < (size_t)numLayers; layerID++)
				{
					ATOMIC_OBJECTS_COUNT_INC;
					ATOMIC_RETURN_IF_STOP_WORKING(false);

					vMem->SetPos(tbl.offset + (size_t)layerRecordsOffset + ((size_t)firstLayerIndex + layerID) * 4U); //-V112
					LayerGlyph lg;
					lg.glyphID = (uint16_t)vMem->ReadUShort();
					lg.paletteID = (uint16_t)vMem->ReadUShort();
					// if CPAL would have been parsed before, i could directly write the palette color
					// we not select the palette (its is not defiend by font but by app, so by design)
					if (!m_Palettes.empty())
					{
						if (lg.paletteID < m_Palettes[0].size())
						{
							lg.color = m_Palettes[0][lg.paletteID];
							if (lg.glyphID < m_Glyphs.size())
							{
								m_Glyphs[lg.glyphID].m_Color = lg.color;
								m_Glyphs[lg.glyphID].m_IsLayer = true;
							}
							else
								LogError(vFlags, "ERR : COLR Layer.GlyphId >= than glyph count\n");
						}
						else
							LogError(vFlags, "ERR : COLR paletteID > than palette entry count\n");
					}
					m_Glyphs[baseGlyphID].m_LayerGlyphs.push_back(lg);
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

bool TTFRRW::TTFRRW::Parse_HHEA_Table(MemoryStream* vMem, ttfrrwProcessingFlags vFlags, TTFRRW_ATOMIC_PARAMS)
{
	if (m_Tables.find("hhea") != m_Tables.end())
	{
		ATOMIC_OBJECTS_COUNT_INC;
		ATOMIC_RETURN_IF_STOP_WORKING(false);

		auto tbl = m_Tables["hhea"];
		vMem->SetPos(tbl.offset);
		//uint32_t len = tbl.length;

		/*MemoryStream::Fixed version =*/ vMem->ReadFixed();
		m_TTFInfos.m_Ascent = (int16_t)vMem->ReadShort();
		m_TTFInfos.m_Descent = (int16_t)vMem->ReadShort();
		m_TTFInfos.m_LineGap = (int16_t)vMem->ReadShort();
		m_TTFInfos.m_AdvanceWidthMax = (uint16_t)vMem->ReadUShort();
		m_TTFInfos.m_MinLeftSideBearing = (int16_t)vMem->ReadShort();
		m_TTFInfos.m_MinRightSideBearing = (int16_t)vMem->ReadShort();
		m_TTFInfos.m_XMaxExtent = (int16_t)vMem->ReadShort();
		/*
		int16_t caretSlopeRise = (int16_t)vMem->ReadShort(); // 2
		int16_t caretSlopeRun = (int16_t)vMem->ReadShort(); // 2
		MemoryStream::FWord caretOffset = (int16_t)vMem->ReadFWord(); // 2
		int16_t reserved1 = (int16_t)vMem->ReadShort(); // 2
		int16_t reserved2 = (int16_t)vMem->ReadShort(); // 2
		int16_t reserved3 = (int16_t)vMem->ReadShort(); // 2
		int16_t reserved4 = (int16_t)vMem->ReadShort(); // 2
		int16_t metricDataFormat = (int16_t)vMem->ReadShort(); // 2
		*/ // total 16 bytes to offset
		m_MumOfLongHorMetrics = (int16_t)vMem->ReadShort(16);

		return true;
	}
	else
	{
		LogError(vFlags, "ERR : HHEA Table not found\n");
	}

	return false;
}

bool TTFRRW::TTFRRW::Parse_HMTX_Table(MemoryStream* vMem, ttfrrwProcessingFlags vFlags, TTFRRW_ATOMIC_PARAMS)
{
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
				size_t leftSideBearingCount = (size_t)m_TTFInfos.m_GlyphCount - (size_t)m_MumOfLongHorMetrics;
				leftSideBearings.resize(leftSideBearingCount);
				for (GlyphIndex idx = 0; idx < leftSideBearingCount; idx++)
				{
					ATOMIC_OBJECTS_COUNT_INC;
					ATOMIC_RETURN_IF_STOP_WORKING(false);

					leftSideBearings[idx] = vMem->ReadFWord();
					GlyphIndex glyphID = m_MumOfLongHorMetrics + idx;
					if (glyphID < m_Glyphs.size())
					{
						size_t lastMetric = (size_t)m_MumOfLongHorMetrics - 1U;
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

///////////////////////////////////////////////////////////////////////
//// PRIVATE TABLES ///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

bool TTFRRW::TTFRRW::Assemble_GLYF_Table()
{
	return false;
}

bool TTFRRW::TTFRRW::Assemble_LOCA_Table()
{
	return false;
}

bool TTFRRW::TTFRRW::Assemble_MAXP_Table()
{
	return false;
}

bool TTFRRW::TTFRRW::Assemble_CMAP_Table()
{
	return false;
}

bool TTFRRW::TTFRRW::Assemble_HMTX_Table()
{
	return false;
}

bool TTFRRW::TTFRRW::Assemble_HHEA_Table()
{
	return false;
}

bool TTFRRW::TTFRRW::Assemble_POST_Table()
{
	return false;
}

bool TTFRRW::TTFRRW::Assemble_NAME_Table()
{
	return false;
}

bool TTFRRW::TTFRRW::Assemble_HEAD_Table()
{
	return false;
}
