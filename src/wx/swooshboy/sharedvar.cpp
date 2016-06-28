#include "swooshboy.h"
#include "sharedvar.h"


void vbaSharedObj::ref() {
    refcount++;
}

void vbaSharedObj::unref() {
    refcount--;

    if (!refcount)
        delete this;
}
