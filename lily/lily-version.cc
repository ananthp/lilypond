#include "version.hh"
#include <stdio.h>

static char *s = "LilyPond " MAJOR_VERSION "." MINOR_VERSION "."  PATCH_LEVEL MY_PATCH_LEVEL " #%d";

static const int build=
#include ".build"
;

char const * 
lily_version_sz()
{
    static char v[1024];
    sprintf(v, s, build);
    return v;
}
