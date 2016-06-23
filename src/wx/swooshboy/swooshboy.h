/* I'm just going to keep some notes here for now. I'll likely end up making a
 * readme for this branch at some point and moving all this information over
 * there.
 *
 * This is my personal build of VBA-M. Right now it's not really fit for public
 * consumption. At the moment I'm working with the WX version, as I've had the
 * best experience with the WX version on Linux, which is my primary operating
 * system. I will eventually move on to butchering the sdl version, or creating
 * an entirely new sdl version, in the process of working towards my goals for
 * this branch.
 *
 * Current goals:
 * Move as much CPU-handled graphics over to the GPU as possible
 * GLES 2.0
 * Build for Steam Link
 * Rewrite upscaling filters in GLSL
 *
 *
 * Below are notes for my own reference:
 *
 * https://github.com/vurtun/nuklear (potential GUI for Steam Link version)
 * https://github.com/gingerBill/gb/blob/master/gb_gl.h (font rendering stuff in here)
 * https://github.com/nothings/stb/blob/master/stb_truetype.h#L238 (maybe use this instead of freetype?)
 * https://github.com/nothings/stb/blob/master/stb_image.h (might be useful when writing SL gui)
 * https://github.com/nothings/stb/blob/master/stb_leakcheck.h (!!!!)
 * https://github.com/nothings/stb/blob/master/stb_easy_font.h (might be useful for testing things?)
 *
 * FT ver is 2.5.5
 * glib is 2.43.0, might want to look into using this
 * json-c is 0.12, might use this if we implement an update checker or overhaul the cfg system
 * libatomic_ops is 7.2, probably don't need this
 *
 * Might have to use SDL for audio.
 * It would appear that pulseaudio is available (v 6.0/8.0?)
 * alsa-lib is 1.0.28
 * Maybe look into statically linking OpenAL?
 * Or just compiling it in?
 * http://kcat.strangesoft.net/openal.html
 *
 * https://github.com/janezz55/lualite/ (could be useful if we do rr stuff) (MIT)
 * http://scumways.com/happyhttp/happyhttp.html (update checking? surely we have curl) (zlib)
 */

#ifndef VBA_SB_H
#define VBA_SB_H

#include <cstdint>

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
#include "shaders.h"

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

#define DECLARE_RES(name) \
    extern char *res_##name __attribute__((nocommon)); \
    extern void *_res_##name##_len __attribute__((nocommon)); \
    uint res_##name##_len  __attribute__((common)) = \
            (uint)(uintptr_t)&_res_##name##_len

#endif
