#ifndef VBA_SIZE_H
#define VBA_SIZE_H

#include "oxygen.h"
#include <GL/glxew.h>
#include <GL/glut.h>

class vbaSize {
public:
    vbaSize();
    vbaSize(uint w, uint h);
    vbaSize(uint w,  int h);
    vbaSize( int w, uint h);
    vbaSize( int w,  int h);
    vbaSize(const vbaSize &other);

    vbaSize &operator  = (const vbaSize &other);
    vbaSize &operator += (const vbaSize &other);
    vbaSize &operator -= (const vbaSize &other);
    vbaSize &operator *= (const    uint &num);
    vbaSize &operator *= (const     int &num);
    vbaSize &operator *= (const   float &num);
    vbaSize &operator /= (const    uint &num);
    vbaSize &operator /= (const     int &num);
    vbaSize &operator /= (const   float &num);
    vbaSize &operator %= (const    uint &num);
    vbaSize &operator %= (const     int &num);

    GLuint x;
    GLuint y;
};

bool    operator == (const vbaSize &left,  const vbaSize &right);
bool    operator != (const vbaSize &left,  const vbaSize &right);
bool    operator >  (const vbaSize &left,  const vbaSize &right);
bool    operator <  (const vbaSize &left,  const vbaSize &right);
bool    operator >= (const vbaSize &left,  const vbaSize &right);
bool    operator <= (const vbaSize &left,  const vbaSize &right);
vbaSize operator +  (const vbaSize &left,  const vbaSize &right);
vbaSize operator -  (const vbaSize &left,  const vbaSize &right);
vbaSize operator *  (const vbaSize &sz,    const    uint &scale);
vbaSize operator *  (const vbaSize &sz,    const     int &scale);
vbaSize operator *  (const vbaSize &sz,    const   float &scale);
vbaSize operator *  (const    uint &scale, const vbaSize &sz   );
vbaSize operator *  (const     int &scale, const vbaSize &sz   );
vbaSize operator *  (const   float &scale, const vbaSize &sz   );
vbaSize operator /  (const vbaSize &sz,    const    uint &scale);
vbaSize operator /  (const vbaSize &sz,    const     int &scale);
vbaSize operator /  (const vbaSize &sz,    const   float &scale);
vbaSize operator %  (const vbaSize &sz,    const    uint &scale);
vbaSize operator %  (const vbaSize &sz,    const     int &scale);
vbaSize operator ^  (const vbaSize &sz,    const    uint &scale);
vbaSize operator ^  (const vbaSize &sz,    const     int &scale);

#endif
