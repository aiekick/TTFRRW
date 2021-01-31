// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "ttfrrw.h"

#include <Tracy.hpp>

#ifdef TRACY_ENABLE
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
#endif

static TTFRRW::ttfrrwProcessingFlags flags = 
	TTFRRW::TTFRRW_PROCESSING_FLAG_VERBOSE_ONLY_ERRORS | 
	TTFRRW::TTFRRW_PROCESSING_FLAG_VERBOSE_PROFILER;

int main(int, char**argv)
{
	(void)argv;

	ZoneScoped;

	TTFRRW::TTFRRW ttfrrw;
	
	if (ttfrrw.OpenFontFile("testfont.ttf", flags))
	{
		printf("TTFRRW succeed to open testfont.ttf\n");

		if (ttfrrw.WriteFontFile("testfont_rewrite.ttf"))
		{
			printf("TTFRRW succeed to write testfont_rewrite.ttf\n");
		}
		else
		{
			printf("TTFRRW failed to write testfont_rewrite.ttf\n");
		}
	}
	else
	{
		printf("TTFRRW failed to open testfont.ttf\n");
	}

    return 0;
}
