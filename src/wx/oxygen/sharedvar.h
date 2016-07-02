#ifndef VBA_SHAREDVAR_H
#define VBA_SHAREDVAR_H

/* vbaSharedObj
 * Base class for variables whose allocation and deallocation are managed by
 * other classes, but which might be shared between class instances. Ensures
 * shared variables are still accessible if still being used by an
 * object instance
 */

class vbaSharedObj {
public:
    void ref();
    void unref();
protected:
    uint refcount = 1;
};

#endif
