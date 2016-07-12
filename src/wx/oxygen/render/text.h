#ifndef VBA_TEXT_H
#define VBA_TEXT_H

#include "oxygen.h"

/* Since a lot of the data this stage works with is stage-independent, we're
 * going to want to cache it outside of the class, or in static members. However
 * at the moment I'm just focused on getting some text on the screen.
 *
 * Text rendering is weird and is a lot more complicated than it needs to be.
 */

// Maybe each of these classes should be their own file?

/* After much weeping and gnashing of teeth, I have come to the conclusion that
 * stb_truetype.h is not the right choice for this project.
 *
 * For the record, I have no idea what I'm doing. If someone wants to come
 * behind me and clean up this mess, you're more than welcome.
 */
#include <ft2build.h>
#include FT_FREETYPE_H

/* Both GHashTable* and std::unordered_map seem to be ideal candidates for
 * containing our cached glyphs. Since we're using glib anyway, let's use
 * GHashTable*
 */
#include <glib.h>

/* The above mentioned container will hold a bunch of these. */
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

// Ideally, there should only ever be one of these
class ftLib : public vbaSharedObj {
    friend class ftFont;
public:
    ftLib();
    ~ftLib();
    void ref();
    void unref();
private:


    FT_Memory mmgmt;
    FT_Library lib;
};

// This is essentially just glslSrc over again. Maybe make a single resource class?
class resTTF {
    friend class ftFont;
    friend class vbaGL;
public:
    resTTF(); //dummy constructor
    resTTF(gchar *ttf_in, const guint len);
    gboolean init(gchar *ttf_in, const guint len);
    ~resTTF();

    resTTF(const resTTF &other);
    resTTF &operator  = (const resTTF &other);

protected:
    gboolean is_init = FALSE;
    // The glsl source
    guchar *ttf_main;
    // Length of the array
    guint ttf_len;
};
#define CREATE_TTF_RES_OBJ(obj, ttf) \
    DECLARE_RES(ttf_##ttf); \
    resTTF obj(res_ttf_##ttf, res_ttf_##ttf##_len)


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

    GHashTable *glyphSets;
    ftLib *ctx;
    resTTF ttf;
    FT_Face face;

    gboolean is_init = FALSE;
    gboolean has_ttf = FALSE;

    gunichar undf = (gunichar)0xfffd;


};

#endif
