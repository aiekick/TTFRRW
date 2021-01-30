// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "ttfrrw.h"

#include <3rdparty/tracy/Tracy.hpp>

int main(int, char**argv)
{
	(void)argv;

	ZoneScoped;

	TTFRRW::TTFRRW ttfrrw;
	static TTFRRW::ttfrrwProcessingFlags flags = TTFRRW::TTFRRW_PROCESSING_FLAG_VERBOSE_ONLY_ERRORS;

	if (ttfrrw.OpenFontFile("testfont2_colr.ttf", flags))
	{
		printf("TTFRRW succeed to open testfont.ttf\n");
	}
	else
	{
		printf("TTFRRW failed to open testfont.ttf\n");
	}

    return 0;
}
