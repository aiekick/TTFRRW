#include "ttfrrw.h"

#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

ttfrrw::MemoryStream::MemoryStream()
{

}

ttfrrw::MemoryStream::MemoryStream(uint8_t * vDatas, size_t vSize)
{
	Set(vDatas, vSize);
}

ttfrrw::MemoryStream::~MemoryStream()
{

}

void ttfrrw::MemoryStream::WriteByte(uint8_t b)
{
	m_Datas.push_back(b);
}

void ttfrrw::MemoryStream::WriteBytes(std::vector<uint8_t> *buffer)
{
	if (buffer)
	{
		m_Datas.insert(m_Datas.end(), buffer->begin(), buffer->end());
	}
}

void ttfrrw::MemoryStream::WriteInt(int32_t i)
{
	WriteByte((uint8_t)((i >> 24) & 0xff));
	WriteByte((uint8_t)((i >> 16) & 0xff));
	WriteByte((uint8_t)((i >> 8) & 0xff));
	WriteByte((uint8_t)(i & 0xff));
}

void ttfrrw::MemoryStream::WriteUShort(int32_t us)
{
	WriteByte((uint8_t)((us >> 8) & 0xff));
	WriteByte((uint8_t)(us & 0xff));
}

void ttfrrw::MemoryStream::WriteFWord(int32_t us)
{
	WriteUShort(us);
}

void ttfrrw::MemoryStream::WriteShort(int32_t s)
{
	WriteUShort(s);
}

void ttfrrw::MemoryStream::WriteUInt24(int32_t ui)
{
	WriteByte((uint8_t)(ui >> 16) & 0xff);
	WriteByte((uint8_t)(ui >> 8) & 0xff);
	WriteByte((uint8_t)ui & 0xff);
}

void ttfrrw::MemoryStream::WriteULong(int64_t ul)
{
	WriteByte((uint8_t)((ul >> 24) & 0xff));
	WriteByte((uint8_t)((ul >> 16) & 0xff));
	WriteByte((uint8_t)((ul >> 8) & 0xff));
	WriteByte((uint8_t)(ul & 0xff));
}

void ttfrrw::MemoryStream::WriteLong(int64_t l)
{
	WriteULong(l);
}

void ttfrrw::MemoryStream::WriteFixed(MemoryStream::Fixed f)
{
	WriteByte((uint8_t)((f.high >> 24) & 0xff));
	WriteByte((uint8_t)((f.high >> 16) & 0xff));
	WriteByte((uint8_t)((f.low >> 8) & 0xff));
	WriteByte((uint8_t)(f.low & 0xff));
}

void ttfrrw::MemoryStream::WriteF2DOT14(MemoryStream::F2DOT14 f)
{
	WriteShort(f.value);
}

void ttfrrw::MemoryStream::WriteDateTime(longDateTime date)
{
	WriteULong((date >> 32) & 0xffffffff);
	WriteULong(date & 0xffffffff);
}

uint8_t* ttfrrw::MemoryStream::Get()
{
	return m_Datas.data();
}

size_t ttfrrw::MemoryStream::Size()
{
	return m_Datas.size();
}

size_t ttfrrw::MemoryStream::GetPos()
{
	return m_ReadPos;
}

void ttfrrw::MemoryStream::SetPos(size_t vPos)
{
	m_ReadPos = vPos;
}

void ttfrrw::MemoryStream::Set(uint8_t * vDatas, size_t vSize)
{
	if (vDatas && vSize)
	{
		m_Datas.clear();
		m_Datas.resize(vSize);

		memcpy(m_Datas.data(), vDatas, vSize);
	}
}

uint8_t ttfrrw::MemoryStream::ReadByte()
{
	if (m_ReadPos < m_Datas.size())
		return m_Datas[m_ReadPos++];
	return 0;
}

int32_t ttfrrw::MemoryStream::ReadUShort()
{
	return 0xffff & (ReadByte() << 8 | ReadByte());
}

int32_t ttfrrw::MemoryStream::ReadShort()
{
	return ((ReadByte() << 8 | ReadByte()) << 16) >> 16;
}

ttfrrw::MemoryStream::FWord ttfrrw::MemoryStream::ReadFWord()
{
	return (int16_t)ReadShort();
}

uint32_t ttfrrw::MemoryStream::ReadUInt24()
{
	return 0xffffff & (ReadByte() << 16 | ReadByte() << 8 | ReadByte());
}

uint64_t ttfrrw::MemoryStream::ReadULong()
{
	return 0xffffffffL & ReadLong();
}

uint32_t ttfrrw::MemoryStream::ReadULongAsInt()
{
	int64_t ulong = ReadULong();
	return ((int32_t)ulong) & ~0x80000000;
}

int32_t ttfrrw::MemoryStream::ReadLong()
{
	return ReadByte() << 24 | ReadByte() << 16 | ReadByte() << 8 | ReadByte();
}

ttfrrw::MemoryStream::Fixed ttfrrw::MemoryStream::ReadFixed()
{
	Fixed res;
	int32_t f = ReadLong();
	res.high = (int16_t)((f >> 16) & 0xff);
	res.low = (int16_t)(f & 0xff);
	return res;
}

ttfrrw::MemoryStream::F2DOT14 ttfrrw::MemoryStream::ReadF2DOT14()
{
	F2DOT14 res;
	res.value = (int16_t)ReadShort();
	return res;
}

ttfrrw::MemoryStream::longDateTime ttfrrw::MemoryStream::ReadDateTime()
{
	return (int64_t)ReadULong() << 32 | ReadULong();
}

std::string ttfrrw::MemoryStream::ReadString(size_t vLen)
{
	std::string res = std::string((char*)(m_Datas.data() + m_ReadPos), vLen);
	m_ReadPos += vLen;
	return res;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

ttfrrw::Contour::Contour()
{

}

ttfrrw::Contour::~Contour()
{

}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

ttfrrw::AffineGlyph::AffineGlyph()
{

}

ttfrrw::AffineGlyph::~AffineGlyph()
{

}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

ttfrrw::Glyph::Glyph()
{

}

ttfrrw::Glyph::~Glyph()
{

}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

ttfrrw::ttfrrw::ttfrrw()
{

}

ttfrrw::ttfrrw::~ttfrrw()
{

}

bool ttfrrw::ttfrrw::OpenFontFile(const std::string& vFontFilePathName)
{
	bool res = false;

	MemoryStream mem;

	int error = 0;
	res = LoadFileToMemory(vFontFilePathName, &mem, &error);
	if (res)
	{
		ParseFontFile(&mem);
	}
	return res;
}

bool ttfrrw::ttfrrw::WriteFontFile(const std::string& vFontFilePathName)
{
	return "";
}

///////////////////////////////////////////////////////////////////////
//// PUBLIC METHOD'S //////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

void ttfrrw::ttfrrw::AddGlyph(const Glyph& vGlyph, const CodePoint& vCodePoint)
{
	(void)vGlyph;
	(void)vCodePoint;
}

ttfrrw::Glyph ttfrrw::ttfrrw::GetGlyph(const CodePoint& vCodePoint)
{
	(void)vCodePoint;

	Glyph res;
	return res;
}

///////////////////////////////////////////////////////////////////////
//// PRIVATE FILE / STREAM ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

bool ttfrrw::ttfrrw::LoadFileToMemory(
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

bool ttfrrw::ttfrrw::WriteMemoryToFile(
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

void ttfrrw::ttfrrw::ParseFontFile(MemoryStream* vMem)
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

			printf("Table %s found\n", tagString.c_str());
		}

		uint16_t indexToLocFormat = 0; // head table : loca format
		uint16_t numGlyphs = 0; // maxp table : count glyphs

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
					for (int i = 0; i < numGlyphs; i++)
					{
						glyphsOffsets.push_back(((uint32_t)vMem->ReadUShort()) * 2);
					}
				}
				else if (indexToLocFormat == 1) // long format
				{
					for (int i = 0; i < numGlyphs; i++)
					{
						glyphsOffsets.push_back((uint32_t)vMem->ReadULong());
					}
				}
			}

			// GLYF
			if (tables.find("glyf") != tables.end())
			{
				auto tbl = tables["glyf"];
				vMem->SetPos(tbl.offset);
				uint32_t len = tbl.length;

				int length = 0;
				for (auto& offset : glyphsOffsets) // each glyph
				{
					Glyph glyph;

					int16_t numberOfContours = (int16_t)vMem->ReadShort();
					MemoryStream::FWord xMin = vMem->ReadFWord();
					MemoryStream::FWord yMin = vMem->ReadFWord();
					MemoryStream::FWord xMax = vMem->ReadFWord();
					MemoryStream::FWord yMax = vMem->ReadFWord();

					if (numberOfContours >= 0) // simple glyf
					{
						vMem->SetPos(tbl.offset + offset);

						glyph = ParseSimpleGlyf(vMem, numberOfContours);
					}
					else // composite glyf
					{
						//compositeGlyph.parse(vMem, vMem->GetPos(), vLength - vMem->GetPos(), numberOfContours);
					}

					AddGlyph(glyph, 0);

					//glyfs.push_back(glyf);

					length = offset;
				}
			}
		}
	}
}

ttfrrw::Glyph ttfrrw::ttfrrw::ParseSimpleGlyf(MemoryStream* vMem, int16_t vCountContour)
{
	if (vMem)
	{
		std::vector<uint16_t> endPtsOfContours;
		uint16_t instructionLength;
		std::vector<uint8_t> instructions;
		std::vector<uint8_t> flags;
		std::vector<int16_t> xCoordinates;
		std::vector<int16_t> yCoordinates;

		if (vCountContour)
		{
			for (int i = 0; i < vCountContour; i++)
				endPtsOfContours.push_back((uint16_t)vMem->ReadShort());

			instructionLength = (uint16_t)vMem->ReadUShort();

			for (int i = 0; i < instructionLength; i++)
				instructions.push_back(vMem->ReadByte());

			if (!endPtsOfContours.empty())
			{
				int countPoints = endPtsOfContours[endPtsOfContours.size() - 1];
				if (countPoints > 0)
				{
					uint32_t flag_repeat = 0;
					int flag = 0;
					for (int i = 0; i < countPoints; i++)
					{
						if (flag_repeat == 0)
						{
							flag = vMem->ReadByte();
							if ((flag & (1 << 3)) == (1 << 3))
							{
								flag_repeat = vMem->ReadByte();
							}
						}
						else
						{
							flag_repeat--;
						}
						flags.push_back((uint8_t)flag);
					}
					int shortX = (1 << 1);
					int shortY = (1 << 2);
					for (auto& it : flags)
					{
						if (it & shortX)
							xCoordinates.push_back((int16_t)vMem->ReadByte());
						else
							xCoordinates.push_back((int16_t)vMem->ReadShort());

						if (it & shortY)
							yCoordinates.push_back((int16_t)vMem->ReadByte());
						else
							yCoordinates.push_back((int16_t)vMem->ReadShort());
					}
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////
//// PRIVATE TABLES ///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

bool ttfrrw::ttfrrw::Assemble_GLYF_Table()
{
	return false;
}

bool ttfrrw::ttfrrw::Assemble_LOCA_Table()
{
	return false;
}

bool ttfrrw::ttfrrw::Assemble_MAXP_Table()
{
	return false;
}

bool ttfrrw::ttfrrw::Assemble_CMAP_Table()
{
	return false;
}

bool ttfrrw::ttfrrw::Assemble_HMTX_Table()
{
	return false;
}

bool ttfrrw::ttfrrw::Assemble_HHEA_Table()
{
	return false;
}

bool ttfrrw::ttfrrw::Assemble_POST_Table()
{
	return false;
}

bool ttfrrw::ttfrrw::Assemble_NAME_Table()
{
	return false;
}

bool ttfrrw::ttfrrw::Assemble_HEAD_Table()
{
	return false;
}
