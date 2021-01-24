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

inline static void LogStr(const char* fmt, ...)
{
#ifdef VERBOSE_MODE
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
#endif
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

TTFRRW::MemoryStream::MemoryStream()
{

}

TTFRRW::MemoryStream::MemoryStream(uint8_t * vDatas, size_t vSize)
{
	Set(vDatas, vSize);
}

TTFRRW::MemoryStream::~MemoryStream()
{

}

void TTFRRW::MemoryStream::WriteByte(uint8_t b)
{
	m_Datas.push_back(b);
}

void TTFRRW::MemoryStream::WriteBytes(std::vector<uint8_t> *buffer)
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
	WriteULong((date >> 32) & 0xffffffff);
	WriteULong(date & 0xffffffff);
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

void TTFRRW::MemoryStream::Set(uint8_t * vDatas, size_t vSize)
{
	if (vDatas && vSize)
	{
		m_Datas.clear();
		m_Datas.resize(vSize);

		memcpy(m_Datas.data(), vDatas, vSize);
	}
}

uint8_t TTFRRW::MemoryStream::ReadByte()
{
	if (m_ReadPos < m_Datas.size())
		return m_Datas[m_ReadPos++];
	return 0;
}

int32_t TTFRRW::MemoryStream::ReadUShort()
{
	return 0xffff & (ReadByte() << 8 | ReadByte());
}

int32_t TTFRRW::MemoryStream::ReadShort()
{
	return ((ReadByte() << 8 | ReadByte()) << 16) >> 16;
}

TTFRRW::MemoryStream::FWord TTFRRW::MemoryStream::ReadFWord()
{
	return (int16_t)ReadShort();
}

uint32_t TTFRRW::MemoryStream::ReadUInt24()
{
	return 0xffffff & (ReadByte() << 16 | ReadByte() << 8 | ReadByte());
}

uint64_t TTFRRW::MemoryStream::ReadULong()
{
	return 0xffffffffL & ReadLong();
}

uint32_t TTFRRW::MemoryStream::ReadULongAsInt()
{
	int64_t ulong = ReadULong();
	return ((int32_t)ulong) & ~0x80000000;
}

int32_t TTFRRW::MemoryStream::ReadLong()
{
	return ReadByte() << 24 | ReadByte() << 16 | ReadByte() << 8 | ReadByte();
}

TTFRRW::MemoryStream::Fixed TTFRRW::MemoryStream::ReadFixed()
{
	Fixed res;
	int32_t f = ReadLong();
	res.high = (int16_t)((f >> 16) & 0xff);
	res.low = (int16_t)(f & 0xff);
	return res;
}

TTFRRW::MemoryStream::F2DOT14 TTFRRW::MemoryStream::ReadF2DOT14()
{
	F2DOT14 res;
	res.value = (int16_t)ReadShort();
	return res;
}

TTFRRW::MemoryStream::longDateTime TTFRRW::MemoryStream::ReadDateTime()
{
	return (int64_t)ReadULong() << 32 | ReadULong();
}

std::string TTFRRW::MemoryStream::ReadString(size_t vLen)
{
	std::string res = std::string((char*)(m_Datas.data() + m_ReadPos), vLen);
	m_ReadPos += vLen;
	return res;
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

TTFRRW::TTFRRW::TTFRRW()
{

}

TTFRRW::TTFRRW::~TTFRRW()
{

}

bool TTFRRW::TTFRRW::OpenFontFile(const std::string& vFontFilePathName, ttfrrwProcessingFlags vFlags)
{
	bool res = false;

	MemoryStream mem;

	int error = 0;
	res = LoadFileToMemory(vFontFilePathName, &mem, &error);
	if (res)
	{
		ParseFontFile(&mem, vFlags);
	}
	return res;
}

bool TTFRRW::TTFRRW::OpenFontStream(uint8_t* vStream, size_t vStreamSize, ttfrrwProcessingFlags vFlags)
{
	bool res = false;

	if (vStream && vStreamSize)
	{
		MemoryStream mem;

		mem.Set(vStream, vStreamSize);

		ParseFontFile(&mem, vFlags);

		res = true;
	}

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
	return "";
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
			long fileSize = 0;
			// obtain file size:
			fseek(intput_file, 0, SEEK_END);
			fileSize = ftell(intput_file);
			rewind(intput_file);

			// copy the file into the buffer and close
			bytes.resize(fileSize);
			fread(bytes.data(), 1, fileSize, intput_file);
			vInMem->Set(bytes.data(), bytes.size());
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
		if (vOutMem->Get() && vOutMem->Size())
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

void TTFRRW::TTFRRW::ParseFontFile(MemoryStream* vMem, ttfrrwProcessingFlags vFlags)
{
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

	if (vMem)
	{
		// header
		std::string scalerType = vMem->ReadString(4);
		if (scalerType[0] == 1)  scalerType = "TrueType 1"; // TrueType 1
		if (scalerType[1] == 1)  scalerType = "OpenType 1"; // TrueType 1
		uint16_t numTables = (uint16_t)vMem->ReadUShort();
		uint16_t searchRange = (uint16_t)vMem->ReadUShort();
		uint16_t entrySelector = (uint16_t)vMem->ReadUShort();
		uint16_t rangeShift = (uint16_t)vMem->ReadUShort();

		// tables
		struct TableStruct
		{
			uint8_t tag[5] = {};
			uint32_t checkSum = 0;
			uint32_t offset = 0;
			uint32_t length = 0;
		};
		std::unordered_map<std::string, TableStruct> tables;
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
			tbl.offset = (uint32_t)vMem->ReadULong();
			tbl.length = (uint32_t)vMem->ReadULong();
			
			std::string tagString = std::string((char*)tbl.tag);
			tables[tagString] = tbl;

			LogStr("Table %s found\n", tagString.c_str());
		}

		uint16_t indexToLocFormat = 0; // head table : loca format
		uint16_t numGlyphs = 0; // maxp table : count glyphs

		// CMAP
		if (tables.find("cmap") != tables.end())
		{
			auto tbl = tables["cmap"];
			vMem->SetPos(tbl.offset);
			uint32_t len = tbl.length;

			uint16_t version = (uint16_t)vMem->ReadUShort();
			uint16_t numEncodingRecords = (uint16_t)vMem->ReadUShort();

			for (size_t encodingRecord = 0; encodingRecord < (size_t)numEncodingRecords; encodingRecord++)
			{
				vMem->SetPos(tbl.offset + 4U);

				uint16_t platformID = (uint16_t)vMem->ReadUShort();
				uint16_t encodingID = (uint16_t)vMem->ReadUShort();
				uint32_t offset = (uint16_t)vMem->ReadULong();

				vMem->SetPos(tbl.offset + offset);

				uint16_t format = (uint16_t)vMem->ReadUShort();
				uint16_t length = (uint16_t)vMem->ReadUShort();

				if (format == 0)
				{
					uint16_t language = (uint16_t)vMem->ReadUShort();
					for (int glyphindex = 0; glyphindex < 256; glyphindex++)
					{
						uint8_t codePoint = vMem->ReadByte();
						m_CodePoints[codePoint] = glyphindex;
					}
				}
				else if (format == 4)
				{
					uint16_t language = (uint16_t)vMem->ReadUShort();
					uint16_t segCountX2 = (uint16_t)vMem->ReadUShort();
					uint16_t searchRange = (uint16_t)vMem->ReadUShort();
					uint16_t entrySelector = (uint16_t)vMem->ReadUShort();
					uint16_t rangeShift = (uint16_t)vMem->ReadUShort();

					std::vector<uint16_t> endCode;
					uint16_t reservedPad = 0;
					std::vector<uint16_t> startCode;
					std::vector<int16_t> idDelta;
					std::vector<uint16_t> idRangeOffset;
					std::vector<uint16_t> glyphIdArray;

					int segCount = segCountX2 / 2;
					for (int i = 0; i < segCount; i++)
					{
						endCode.push_back((uint16_t)vMem->ReadUShort());
					}
					reservedPad = (uint16_t)vMem->ReadUShort();
					for (int i = 0; i < segCount; i++)
					{
						startCode.push_back((uint16_t)vMem->ReadUShort());
					}
					for (int i = 0; i < segCount; i++)
					{
						idDelta.push_back((uint16_t)vMem->ReadShort());
					}
					for (int i = 0; i < segCount; i++)
					{
						idRangeOffset.push_back((uint16_t)vMem->ReadUShort());
					}

					for (uint16_t codePoint = 0x20; codePoint < 0xFFFF; codePoint++)
					{
						// d'abord on va localiser le segment
						int segment = 0;;
						for (segment = 0; segment < segCount; segment++)
						{
							if (codePoint > startCode[segment] && codePoint < endCode[segment])
								continue;
							else
								break;
						}
						if (codePoint > startCode[segment] && codePoint < endCode[segment])
						{
							uint16_t glyphId = *(idRangeOffset[segment] / 2 + (codePoint - startCode[segment]) + &idRangeOffset[segment]);
							m_CodePoints[codePoint] = glyphId;
						}
					}
				}
				else
				{
					LogStr("CMAP Format %u not supported\n", format);
				}
			}
		}

		if (tables.find("head") != tables.end() &&
			tables.find("maxp") != tables.end() &&
			tables.find("loca") != tables.end())
		{
			// HEAD
			if (tables.find("head") != tables.end())
			{
				auto tbl = tables["head"];
				vMem->SetPos(tbl.offset);
				uint32_t len = tbl.length;

				MemoryStream::Fixed version = vMem->ReadFixed();
				MemoryStream::Fixed fontRevision = vMem->ReadFixed();
				uint32_t checkSumAdjustment = (uint32_t)vMem->ReadULong();
				uint32_t magicNumber = (uint32_t)vMem->ReadULong();
				uint16_t flags = (uint16_t)vMem->ReadUShort(); // bitset
				uint16_t unitsPerEm = (uint16_t)vMem->ReadUShort();
				MemoryStream::longDateTime created = vMem->ReadDateTime();
				MemoryStream::longDateTime modified = vMem->ReadDateTime();
				MemoryStream::FWord xMin = vMem->ReadFWord();
				MemoryStream::FWord yMin = vMem->ReadFWord();
				MemoryStream::FWord xMax = vMem->ReadFWord();
				MemoryStream::FWord yMax = vMem->ReadFWord();
				uint16_t macStyle = (uint16_t)vMem->ReadUShort(); // bitset
				uint16_t lowestRecPPEM = (uint16_t)vMem->ReadUShort();
				uint16_t fontDirectionHint = (int16_t)vMem->ReadShort();
				indexToLocFormat = (int16_t)vMem->ReadShort();
				uint16_t glyphDataFormat = (int16_t)vMem->ReadShort();
			}

			// MAXP
			if (tables.find("maxp") != tables.end())
			{
				auto tbl = tables["maxp"];
				vMem->SetPos(tbl.offset);
				uint32_t len = tbl.length;

				MemoryStream::Fixed version = vMem->ReadFixed();
				numGlyphs = (uint16_t)vMem->ReadUShort();
				uint16_t maxPoints = (uint16_t)vMem->ReadUShort();
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
				uint16_t maxComponentDepth = (uint16_t)vMem->ReadUShort();
			}

			// LOCA
			std::vector<uint32_t> glyphsOffsets;
			if (tables.find("loca") != tables.end())
			{
				auto tbl = tables["loca"];
				vMem->SetPos(tbl.offset);
				uint32_t len = tbl.length;

				if (indexToLocFormat == 0) // short format
				{
					for (uint16_t i = 0; i < numGlyphs; i++)
					{
						uint32_t offset = ((uint32_t)vMem->ReadUShort()) * 2;
						glyphsOffsets.push_back(offset);
					}
				}
				else if (indexToLocFormat == 1) // long format
				{
					for (uint16_t i = 0; i < numGlyphs; i++)
					{
						uint32_t offset = ((uint32_t)vMem->ReadULong());
						glyphsOffsets.push_back(offset);
					}
				}
			}

			// GLYF
			if (tables.find("glyf") != tables.end())
			{
				auto tbl = tables["glyf"];
				size_t tblOffset = tbl.offset;
				vMem->SetPos(tblOffset);
				uint32_t len = tbl.length;

				int length = 0;
				for (size_t i = 0; i < (size_t)numGlyphs; i++)
				{
					uint32_t offset = glyphsOffsets[i];

					vMem->SetPos(tblOffset + offset);

					int16_t numberOfContours = (int16_t)vMem->ReadShort();
					MemoryStream::FWord xMin = vMem->ReadFWord();
					MemoryStream::FWord yMin = vMem->ReadFWord();
					MemoryStream::FWord xMax = vMem->ReadFWord();
					MemoryStream::FWord yMax = vMem->ReadFWord();

					LogStr("-----------------------\n");
					
					Glyph glyph;
					glyph.m_LocalBBox.lowerBound.x = xMin;
					glyph.m_LocalBBox.lowerBound.y = yMin;
					glyph.m_LocalBBox.upperBound.x = xMax;
					glyph.m_LocalBBox.upperBound.y = yMax;
					
					LogStr("BBox : %i,%i > %i,%i\n", xMin, yMin, xMax, yMax);
					
					if (numberOfContours >= 0) // simple glyf
					{
						LogStr("Glyph %u : Simple Glyph\n", (uint32_t)i);
						
						glyph.m_IsSimpleGlyph = true;

						if (!(vFlags & TTFRRW_PROCESSING_FLAG_NO_GLYPH_PARSING))
						{
							auto g = ParseSimpleGlyf(vMem, numberOfContours);
							glyph.m_Contours = g.m_Contours;
							glyph.m_AdvanceX = g.m_AdvanceX;
							glyph.m_LeftSideBearing = g.m_LeftSideBearing;
						}
					}
					else // composite glyf
					{
						LogStr("Glyph %u : Composite Glyph\n", (uint32_t)i);

						glyph.m_IsSimpleGlyph = false;
					}

					m_Glyphs.push_back(glyph);

					LogStr("-----------------------\n");

					length = offset;
				}
			}
		}
	}
}

TTFRRW::Glyph TTFRRW::TTFRRW::ParseSimpleGlyf(MemoryStream* vMem, int16_t vCountContour)
{
	Glyph glyph;
	
	if (vMem)
	{
		std::vector<uint16_t> endPtsOfContours;
		uint16_t instructionLength;
		std::vector<uint8_t> instructions;
		std::vector<uint8_t> flags;
		std::vector<bool> onCurves;
		std::vector<int16_t> xCoordinates;
		std::vector<int16_t> yCoordinates;

		if (vCountContour >= 0) // this is well simple glyph
		{
			for (int i = 0; i < vCountContour; i++)
				endPtsOfContours.push_back((uint16_t)vMem->ReadShort());

			instructionLength = (uint16_t)vMem->ReadUShort();

			for (int i = 0; i < instructionLength; i++)
				instructions.push_back((uint8_t)vMem->ReadByte());

			const int32_t SimpleFlagOnCurve = 1;
			const int32_t SimpleFlagOnXShort = 1 << 1;
			const int32_t SimpleFlagOnYShort = 1 << 2;
			const int32_t SimpleFlagOnRepeat = 1 << 3;
			const int32_t SimpleFlagOnXRepeatSign = 1 << 4;
			const int32_t SimpleFlagOnYRepeatSign = 1 << 5;

			if (!endPtsOfContours.empty())
			{
				int countPoints = endPtsOfContours[vCountContour - 1] + 1;
				if (countPoints > 0)
				{
					xCoordinates.resize(countPoints);
					yCoordinates.resize(countPoints);
					onCurves.resize(countPoints);

					uint32_t flag_repeat = 0;
					int32_t flag = 0;
					for (size_t i = 0; i < countPoints; i++)
					{
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
						flags.push_back(u8Flag);

						onCurves[i] = ((u8Flag & SimpleFlagOnCurve) == SimpleFlagOnCurve);
					}

					for (size_t i = 0; i < countPoints; i++)
					{
						auto flag = flags[i];

						if ((flag & SimpleFlagOnXShort) == SimpleFlagOnXShort)
						{
							int16_t coord = (int16_t)vMem->ReadByte();
							coord *= ((flag & SimpleFlagOnXRepeatSign) == SimpleFlagOnXRepeatSign) ? 1 : -1;
							xCoordinates[i] = coord;
						}
						else if (!((flag & SimpleFlagOnXRepeatSign) == SimpleFlagOnXRepeatSign))
						{
							xCoordinates[i] = (int16_t)vMem->ReadShort();
						}
						if (i > 0)
						{
							xCoordinates[i] += xCoordinates[i - 1U];
						}
					}

					for (size_t i = 0; i < countPoints; i++)
					{
						auto flag = flags[i];

						if ((flag & SimpleFlagOnYShort) == SimpleFlagOnYShort)
						{
							int16_t coord = (int16_t)vMem->ReadByte();
							coord *= ((flag & SimpleFlagOnYRepeatSign) == SimpleFlagOnYRepeatSign) ? 1 : -1;
							yCoordinates[i] = coord;
						}
						else if (!((flag & SimpleFlagOnYRepeatSign) == SimpleFlagOnYRepeatSign))
						{
							yCoordinates[i] = (int16_t)vMem->ReadShort();
						}
						if (i > 0)
						{
							yCoordinates[i] += yCoordinates[i - 1U];
						}
					}
				}
			}

			// convert in final glyph

			endPtsOfContours.insert(endPtsOfContours.begin(), 0);
			LogStr("Contours : %i\n", vCountContour);
			for (size_t c = 0; c < vCountContour; c++)
			{
				Contour contour;

				size_t pmax = endPtsOfContours[c+1] - endPtsOfContours[c];
				LogStr("Contour %u, Count Points : %u\n", (uint32_t)c, (uint32_t)pmax);
				for (size_t p = 0; p < pmax; p++)
				{
					ivec2 pt;
					pt.x = (int32_t)xCoordinates[endPtsOfContours[c] + p];
					pt.y = (int32_t)yCoordinates[endPtsOfContours[c] + p];
					contour.m_Points.push_back(pt);
					bool oc = onCurves[endPtsOfContours[c] + p];
					contour.m_OnCurve.push_back(oc);
					LogStr("Point %u %s : %i,%i\n", (uint32_t)p, (oc ? "on curve" : ""), pt.x, pt.y);
				}

				glyph.m_Contours.push_back(contour);
			}
		}
	}

	return glyph;
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
