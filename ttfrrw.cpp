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

ttfrrw::Curve::Curve()
{

}

ttfrrw::Curve::~Curve()
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
