#include "oxygen.h"
#include "ttfres.h"

#include <glib.h>

resTTF::resTTF() {
    is_init = FALSE;
}

resTTF::resTTF(gchar *ttf_in, const guint len) {
    init(ttf_in, len);
}

gboolean resTTF::init(gchar *ttf_in, const guint len) {
    ttf_main = (guchar *)ttf_in;
    ttf_len = len;
    is_init = TRUE;

    return TRUE;
}

resTTF::~resTTF() {
    // Nothing to do here
}

resTTF::resTTF(const resTTF &other) {
    ttf_main = other.ttf_main;
    ttf_len = other.ttf_len;
}

resTTF &resTTF::operator = (const resTTF &other) {
    ttf_main = other.ttf_main;
    ttf_len = other.ttf_len;
    return *this;
}
