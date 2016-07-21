#ifndef VBA_SIZE_H
#define VBA_SIZE_H

#include "oxygen.h"
#include <glib.h>
#include <cmath>

class vbaSize {
public:
    vbaSize() { _x = _y = 0.0; }
    vbaSize(const vbaSize &other) { _x = other._x; _y = other._y; }

    vbaSize(gdouble w, gdouble h) { _x =          w; _y =          h; }
    vbaSize(gdouble w,  gfloat h) { _x =          w; _y = (gdouble)h; }
    vbaSize(gdouble w,   guint h) { _x =          w; _y = (gdouble)h; }
    vbaSize(gdouble w,    gint h) { _x =          w; _y = (gdouble)h; }
    vbaSize( gfloat w, gdouble h) { _x = (gdouble)w; _y =          h; }
    vbaSize( gfloat w,  gfloat h) { _x = (gdouble)w; _y = (gdouble)h; }
    vbaSize( gfloat w,   guint h) { _x = (gdouble)w; _y = (gdouble)h; }
    vbaSize( gfloat w,    gint h) { _x = (gdouble)w; _y = (gdouble)h; }
    vbaSize(  guint w, gdouble h) { _x = (gdouble)w; _y =          h; }
    vbaSize(  guint w,  gfloat h) { _x = (gdouble)w; _y = (gdouble)h; }
    vbaSize(  guint w,   guint h) { _x = (gdouble)w; _y = (gdouble)h; }
    vbaSize(  guint w,    gint h) { _x = (gdouble)w; _y = (gdouble)h; }
    vbaSize(   gint w, gdouble h) { _x = (gdouble)w; _y =          h; }
    vbaSize(   gint w,  gfloat h) { _x = (gdouble)w; _y = (gdouble)h; }
    vbaSize(   gint w,   guint h) { _x = (gdouble)w; _y = (gdouble)h; }
    vbaSize(   gint w,    gint h) { _x = (gdouble)w; _y = (gdouble)h; }

    inline vbaSize &operator  = (const vbaSize &other) {
        _x  = other._x; _y  = other._y; return *this;
    }
    inline vbaSize &operator += (const vbaSize &other) {
        _x += other._x; _y += other._y; return *this;
    }
    inline vbaSize &operator -= (const vbaSize &other) {
        _x -= other._x; _y -= other._y; return *this;
    }

    inline vbaSize &operator *= (const gdouble &num) {
        _x *=          num; _y *=          num; return *this;
    }
    inline vbaSize &operator *= (const  gfloat &num) {
        _x *= (gdouble)num; _y *= (gdouble)num; return *this;
    }
    inline vbaSize &operator *= (const   guint &num) {
        _x *= (gdouble)num; _y *= (gdouble)num; return *this;
    }
    inline vbaSize &operator *= (const    gint &num) {
        _x *= (gdouble)num; _y *= (gdouble)num; return *this;
    }

    inline vbaSize &operator /= (const gdouble &num) {
        _x /=          num; _y /=          num; return *this;
    }
    inline vbaSize &operator /= (const  gfloat &num) {
        _x /= (gdouble)num; _y /= (gdouble)num; return *this;
    }
    inline vbaSize &operator /= (const   guint &num) {
        _x /= (gdouble)num; _y /= (gdouble)num; return *this;
    }
    inline vbaSize &operator /= (const    gint &num) {
        _x /= (gdouble)num; _y /= (gdouble)num; return *this;
    }

    inline vbaSize &operator %= (const gdouble &num) {
        _x = fmod(_x,          num); _y = fmod(_y,          num); return *this;
    }
    inline vbaSize &operator %= (const  gfloat &num) {
        _x = fmod(_x, (gdouble)num); _y = fmod(_y, (gdouble)num); return *this;
    }
    inline vbaSize &operator %= (const   guint &num) {
        _x = fmod(_x, (gdouble)num); _y = fmod(_y, (gdouble)num); return *this;
    }
    inline vbaSize &operator %= (const    gint &num) {
        _x = fmod(_x, (gdouble)num); _y = fmod(_y, (gdouble)num); return *this;
    }

    inline gdouble xd() const { return         _x; }
    inline  gfloat xf() const { return (gfloat)_x; }
    inline   guint xu() const { return  (guint)_x; }
    inline    gint xi() const { return   (gint)_x; }
    inline gdouble yd() const { return         _y; }
    inline  gfloat yf() const { return (gfloat)_y; }
    inline   guint yu() const { return  (guint)_y; }
    inline    gint yi() const { return   (gint)_y; }

    inline void x(gdouble w) { _x =          w; }
    inline void x( gfloat w) { _x = (gdouble)w; }
    inline void x(  guint w) { _x = (gdouble)w; }
    inline void x(   gint w) { _x = (gdouble)w; }
    inline void y(gdouble h) { _y =          h; }
    inline void y( gfloat h) { _y = (gdouble)h; }
    inline void y(  guint h) { _y = (gdouble)h; }
    inline void y(   gint h) { _y = (gdouble)h; }

private:
    gdouble _x;
    gdouble _y;
};

/* Note:
 * As is currently implemented, the == operator will return true if the
 * dimensions are equal in both instances. The <, >, =<, and => operators
 * make comparisons based on the area.
 *
 * Maybe we should make operators for approximate equality?
 */

inline bool    operator == (const vbaSize &left,  const vbaSize &right) {
    return ((left.xd() == right.xd()) && (left.yd() == right.yd()));
}
inline bool    operator != (const vbaSize &left,  const vbaSize &right) {
    return ((left.xd() != right.xd()) || (left.yd() != right.yd()));
}

inline bool    operator >  (const vbaSize &left,  const vbaSize &right) {
    return ((left.xd() * left.yd()) >  (right.xd() * right.yd()));
}
inline bool    operator <  (const vbaSize &left,  const vbaSize &right) {
    return ((left.xd() * left.yd()) <  (right.xd() * right.yd()));
}
inline bool    operator >= (const vbaSize &left,  const vbaSize &right) {
    return ((left.xd() * left.yd()) >= (right.xd() * right.yd()));
}
inline bool    operator <= (const vbaSize &left,  const vbaSize &right) {
    return ((left.xd() * left.yd()) <= (right.xd() * right.yd()));
}

inline vbaSize operator +  (const vbaSize &left,  const vbaSize &right) {
    return vbaSize(left.xd() + right.xd(), left.yd() + right.yd());
}
inline vbaSize operator -  (const vbaSize &left,  const vbaSize &right) {
    return vbaSize(left.xd() - right.xd(), left.yd() - right.yd());
}

inline vbaSize operator *  (const vbaSize &sz,    const gdouble &num) {
    return vbaSize(sz.xd() *          num, sz.yd() *          num);
}
inline vbaSize operator *  (const vbaSize &sz,    const  gfloat &num) {
    return vbaSize(sz.xd() * (gdouble)num, sz.yd() * (gdouble)num);
}
inline vbaSize operator *  (const vbaSize &sz,    const   guint &num) {
    return vbaSize(sz.xd() * (gdouble)num, sz.yd() * (gdouble)num);
}
inline vbaSize operator *  (const vbaSize &sz,    const    gint &num) {
    return vbaSize(sz.xd() * (gdouble)num, sz.yd() * (gdouble)num);
}

inline vbaSize operator *  (const gdouble &num,   const vbaSize &sz ) {
    return vbaSize(         num * sz.xd(),          num * sz.yd());
}
inline vbaSize operator *  (const  gfloat &num,   const vbaSize &sz ) {
    return vbaSize((gdouble)num * sz.xd(), (gdouble)num * sz.yd());
}
inline vbaSize operator *  (const   guint &num,   const vbaSize &sz ) {
    return vbaSize((gdouble)num * sz.xd(), (gdouble)num * sz.yd());
}
inline vbaSize operator *  (const    gint &num,   const vbaSize &sz ) {
    return vbaSize((gdouble)num * sz.xd(), (gdouble)num * sz.yd());
}

inline vbaSize operator /  (const vbaSize &sz,    const gdouble &num) {
    return vbaSize(sz.xd() /          num, sz.yd() /          num);
}
inline vbaSize operator /  (const vbaSize &sz,    const  gfloat &num) {
    return vbaSize(sz.xd() / (gdouble)num, sz.yd() / (gdouble)num);
}
inline vbaSize operator /  (const vbaSize &sz,    const   guint &num) {
    return vbaSize(sz.xd() / (gdouble)num, sz.yd() / (gdouble)num);
}
inline vbaSize operator /  (const vbaSize &sz,    const    gint &num) {
    return vbaSize(sz.xd() / (gdouble)num, sz.yd() / (gdouble)num);
}

inline vbaSize operator /  (const gdouble &num,   const vbaSize &sz ) {
    return vbaSize(         num / sz.xd(),          num / sz.yd());
}
inline vbaSize operator /  (const  gfloat &num,   const vbaSize &sz ) {
    return vbaSize((gdouble)num / sz.xd(), (gdouble)num / sz.yd());
}
inline vbaSize operator /  (const   guint &num,   const vbaSize &sz ) {
    return vbaSize((gdouble)num / sz.xd(), (gdouble)num / sz.yd());
}
inline vbaSize operator /  (const    gint &num,   const vbaSize &sz ) {
    return vbaSize((gdouble)num / sz.xd(), (gdouble)num / sz.yd());
}

inline vbaSize operator %  (const vbaSize &sz,    const gdouble &num) {
    return vbaSize(fmod(sz.xd(),          num), fmod(sz.yd(),          num));
}
inline vbaSize operator %  (const vbaSize &sz,    const  gfloat &num) {
    return vbaSize(fmod(sz.xd(), (gdouble)num), fmod(sz.yd(), (gdouble)num));
}
inline vbaSize operator %  (const vbaSize &sz,    const   guint &num) {
    return vbaSize(fmod(sz.xd(), (gdouble)num), fmod(sz.yd(), (gdouble)num));
}
inline vbaSize operator %  (const vbaSize &sz,    const    gint &num) {
    return vbaSize(fmod(sz.xd(), (gdouble)num), fmod(sz.yd(), (gdouble)num));
}

inline vbaSize operator %  (const gdouble &num,   const vbaSize &sz ) {
    return vbaSize(fmod(         num, sz.xd()), fmod(         num, sz.yd()));
}
inline vbaSize operator %  (const  gfloat &num,   const vbaSize &sz ) {
    return vbaSize(fmod((gdouble)num, sz.xd()), fmod((gdouble)num, sz.yd()));
}
inline vbaSize operator %  (const   guint &num,   const vbaSize &sz ) {
    return vbaSize(fmod((gdouble)num, sz.xd()), fmod((gdouble)num, sz.yd()));
}
inline vbaSize operator %  (const    gint &num,   const vbaSize &sz ) {
    return vbaSize(fmod((gdouble)num, sz.xd()), fmod((gdouble)num, sz.yd()));
}

#endif
