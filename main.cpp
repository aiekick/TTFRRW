// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "ttfrrw.h"

int main(int, char**argv)
{
	TTFRRW::TTFRRW ttfrrw;
	if (ttfrrw.OpenFontFile("testfont.ttf"))
	{
		printf("TTFRRW succeed to open testfont.ttf\n");
	}
	else
	{
		printf("TTFRRW failed to open testfont.ttf\n");
	}

    return 0;
}
