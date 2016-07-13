#ifndef VBA_FTLIB_H
#define VBA_FTLIB_H

#include "oxygen.h"
#include "sharedvar.h"

/* After much weeping and gnashing of teeth, I have come to the conclusion that
 * stb_truetype.h is not the right choice for this project.
 *
 * For the record, I have no idea what I'm doing. If someone wants to come
 * behind me and clean up this mess, you're more than welcome.
 */
#include <ft2build.h>
#include FT_FREETYPE_H

/* There should only ever be one of these, so the instance will likely end up
 * being a variable in whatever class vbaGL eventually ends up inside.
 */
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


#endif
