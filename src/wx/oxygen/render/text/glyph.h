#ifndef VBA_GLYPH_H
#define VBA_GLYPH_H

#include "oxygen.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glib.h>

/* ftGlyph
 * Container for glyph data
 *
 * Needs constructors and such
 */
class ftGlyph {
public:
    gchar character;

    vbaSize sz_tex;
    vbaSize sz_glyph;

    guchar *data; //maybe make this a gpointer
    FT_Vector adv;
    /* Not putting ptsize in here, as each set of ftGlyph objects will only
     * contain glyphs of the same size from the same font.
     */
};

#endif
