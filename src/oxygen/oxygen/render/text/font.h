#ifndef VBA_FONT_H
#define VBA_FONT_H

#include "oxygen.h"
#include "ftlib.h"
#include "glyph.h"
#include "ttfres.h"

/* Since a lot of the data this interface works with is stage-independent, we're
 * going to want to cache it outside of the stages, or in static members.
 * However, at the moment I'm just focused on getting some text on the screen.
 *
 * Text rendering is weird and is a lot more complicated than it needs to be.
 */

#include <ft2build.h>
#include FT_FREETYPE_H

/* Both GHashTable* and std::unordered_map seem to be ideal candidates for
 * containing our cached glyphs. Since we're using glib anyway, let's use
 * GHashTable*
 */
#include <glib.h>

class ftFont {
public:
    ftFont(); // dummy constructor

    ftFont(ftLib *lib); // Standard constructor
    gboolean init(ftLib *lib); // Standard initializer

    ftFont(ftLib *lib, resTTF ttf);
    gboolean init(ftLib *lib, resTTF ttf);
    gboolean setTTF(resTTF ttf);
    gboolean setUndefinedIndex(guint32 idx);

    gboolean cacheGlyphs(guint size);

    ftGlyph *getGlyph(gunichar character, guint size);

    ~ftFont();
private:
    gboolean cacheGlyph(gunichar character, guint size);
    gboolean cacheGlyph(gunichar character, GHashTable *gset);

    /* Right now, this hash table contains more hash tables. These subtables
     * contain all the cached glyphs for this font for a given size, which is
     * the subtable's key in this table.
     *
     * Potential TODO:
     * Create an object for these subtables that will make placeholder character
     * substitution easier.
     */
    GHashTable *glyphSets;

    ftLib *ctx;
    resTTF ttf;
    FT_Face face;

    gboolean is_init = FALSE;
    gboolean has_ttf = FALSE;

    gunichar undf = (gunichar)0xfffd;

};


#endif
