// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "ttfrrw.h"

#include <3rdparty/tracy/Tracy.hpp>

void* operator new(std::size_t count)
{
	auto ptr = malloc(count);
	TracyAlloc(ptr, count);
	return ptr;
}
void* operator new[](std::size_t count)
{
	auto ptr = malloc(count);
	TracyAlloc(ptr, count);
	return ptr;
}
void operator delete(void* ptr) noexcept
{
	TracyFree(ptr);
	free(ptr);
}
void operator delete[](void* ptr) noexcept
{
	TracyFree(ptr);
	free(ptr);
}

static TTFRRW::ttfrrwProcessingFlags flags = 
	TTFRRW::TTFRRW_PROCESSING_FLAG_VERBOSE_ONLY_ERRORS | 
	TTFRRW::TTFRRW_PROCESSING_FLAG_VERBOSE_PROFILER;

int main(int, char**argv)
{
	(void)argv;

	ZoneScoped;

	TTFRRW::TTFRRW ttfrrw;
	
	if (ttfrrw.OpenFontFile("testfont2_colr.ttf", flags))
	{
		printf("TTFRRW succeed to open testfont2_colr.ttf\n");
	}
	else
	{
		printf("TTFRRW failed to open testfont2_colr.ttf\n");
	}

    return 0;
}
