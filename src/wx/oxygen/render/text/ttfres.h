#ifndef VBA_TTFRES_H
#define VBA_TTFRES_H

#include <glib.h>

// This is essentially just glslSrc over again.
// Maybe make a single resource class?
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

#endif
