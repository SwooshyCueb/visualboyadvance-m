#ifndef VBA_SB_H
#define VBA_SB_H

#include <GL/glxew.h>
#include <GL/glut.h>
//#include <wx/glcanvas.h>

// Forward declarations to make life easier.
class vbaErr;
class vbaGL;
class vbaTex;
class vbaOSD;

//#define VBA_TRIANGLE_STRIP

#include "osd.h"
#include "vbaerr.h"
#include "size.h"
#include "vbagl.h"
#include "tex.h"
#include "glsl.h"

#include <cstdio>
#include <cstddef>
#include <deque>
#include <queue>
#include <stack>


#ifdef _DEBUG
#define dprintf(format, ...) fprintf(stderr, format, ##__VA_ARGS__)
#else
#define dprintf(format, ...)
#endif


#endif
