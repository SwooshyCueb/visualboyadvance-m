#include "oxygen.h"
#include "size.h"

vbaSize::vbaSize() {
    x = y = 0;
}

vbaSize::vbaSize(uint w, uint h) {
    x = w;
    y = h;
}

vbaSize::vbaSize(uint w, int h) {
    x = w;
    y = uint(h);
}

vbaSize::vbaSize( int w, uint h) {
    x = uint(w);
    y = h;
}

vbaSize::vbaSize( int w, int h) {
    x = uint(w);
    y = uint(h);
}

vbaSize::vbaSize(const vbaSize &other) {
    x = other.x;
    y = other.y;
}

vbaSize &vbaSize::operator = (const vbaSize &other) {
    x = other.x;
    y = other.y;
    return *this;
}

/* Note:
 * As is currently implemented, the == operator will return true if the
 * dimensions are equal in both instances. The <, >, =<, and => operators
 * make comparisons based on the area.
 */

inline bool operator == (const vbaSize &left, const vbaSize &right) {
    return ((left.x == right.x) && (left.y == right.y));
}
inline bool operator != (const vbaSize &left, const vbaSize &right) {
    return ((left.x != right.x) || (left.y != right.y));
}

inline bool operator > (const vbaSize &left, const vbaSize &right) {
    return ((left.x * left.y) > (right.x * right.y));
}

inline bool operator < (const vbaSize &left, const vbaSize &right) {
    return ((left.x * left.y) < (right.x * right.y));
}

inline bool operator >= (const vbaSize &left, const vbaSize &right) {
    return ((left.x * left.y) >= (right.x * right.y));
}

inline bool operator <= (const vbaSize &left, const vbaSize &right) {
    return ((left.x * left.y) <= (right.x * right.y));
}

vbaSize operator + (const vbaSize &left, const vbaSize &right) {
    return vbaSize(left.x + right.x, left.y + right.y);
}

vbaSize &vbaSize::operator += (const vbaSize &other) {
    x += other.x;
    y += other.y;
    return *this;
}

vbaSize operator - (const vbaSize &left, const vbaSize &right) {
    return vbaSize(left.x - right.x, left.y - right.y);
}

vbaSize &vbaSize::operator -= (const vbaSize &other) {
    x -= other.x;
    y -= other.y;
    return *this;
}

vbaSize operator * (const vbaSize &sz, const uint &scale) {
    return vbaSize(sz.x * scale, sz.y * scale);
}

vbaSize operator * (const vbaSize &sz, const int &scale) {
    return vbaSize(sz.x * scale, sz.y * scale);
}

vbaSize operator * (const vbaSize &sz, const float &scale) {
    return vbaSize(uint(float(sz.x) * scale),
                   uint(float(sz.y) * scale));
}

vbaSize operator * (const uint &scale, const vbaSize &sz) {
    return vbaSize(sz.x * scale, sz.y * scale);
}

vbaSize operator * (const int &scale, const vbaSize &sz) {
    return vbaSize(sz.x * scale, sz.y * scale);
}

vbaSize operator * (const float &scale, const vbaSize &sz) {
    return vbaSize(uint(float(sz.x) * scale),
                   uint(float(sz.y) * scale));
}

vbaSize &vbaSize::operator *= (const uint &num) {
    x *= num;
    y *= num;
    return *this;
}

vbaSize &vbaSize::operator *= (const int &num) {
    x *= num;
    y *= num;
    return *this;
}

vbaSize &vbaSize::operator *= (const float &num) {
    x = uint(float(x) * num);
    y = uint(float(y) * num);
    return *this;
}

vbaSize operator / (const vbaSize &sz, const uint &scale) {
    return vbaSize(sz.x / scale, sz.y / scale);
}

vbaSize operator / (const vbaSize &sz, const int &scale) {
    return vbaSize(sz.x / scale, sz.y / scale);
}

vbaSize operator / (const vbaSize &sz, const float &scale) {
    return vbaSize(uint(float(sz.x) / scale),
                   uint(float(sz.y) / scale));
}

vbaSize &vbaSize::operator /= (const uint &num) {
    x /= num;
    y /= num;
    return *this;
}

vbaSize &vbaSize::operator /= (const int &num) {
    x /= num;
    y /= num;
    return *this;
}

vbaSize &vbaSize::operator /= (const float &num) {
    x = uint(float(x) / num);
    y = uint(float(y) / num);
    return *this;
}

vbaSize operator % (const vbaSize &sz, const uint &scale) {
    return vbaSize(sz.x % scale, sz.y % scale);
}

vbaSize operator % (const vbaSize &sz, const int &scale) {
    return vbaSize(sz.x % scale, sz.y % scale);
}

vbaSize &vbaSize::operator %= (const uint &num) {
    x %= num;
    y %= num;
    return *this;
}

vbaSize &vbaSize::operator %= (const int &num) {
    x %= num;
    y %= num;
    return *this;
}

vbaSize operator ^ (const vbaSize &sz, const uint &scale) {
    return vbaSize(sz.x ^ scale, sz.y ^ scale);
}

vbaSize operator ^ (const vbaSize &sz, const int &scale) {
    return vbaSize(sz.x ^ scale, sz.y ^ scale);
}
