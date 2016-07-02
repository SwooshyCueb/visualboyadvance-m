/* Before we begin, I'd like to take a moment to talk about the OpenGL ES
 * Shading Language, version 1.0, which is a part of OpenGL ES, version 2.0.
 *
 * Some of these shaders require 32-bit unsigned integers, and some require
 * bitwise operations to be performed with 32-bit unsigned integers.
 *
 * The OpenGL ES Shading Language, version 1.0, which is a part of OpenGL ES,
 * version 2.0, does not have 32-bit unsigned integers, nor does it have 32-bit
 * signed integers. It doesn't even have unsigned integers to begin with. And
 * even in high-percision mode, the highest percision integer we can get is
 * 24-bit. And thank GOD it's not 16-bit, because the jury-rigging you see
 * below would absolutely fall apart if not for the fact that these 24-bit
 * integers save me from having to deal with negative integers.
 *
 * I was originally planning on emulating 32-bit integers by using an ivec2.
 * That's two 16-bit components stored in two 24-bit integers. Of course, this
 * means I must deal with overflows of the least significant component myself,
 * and underflows of the most significant component myself. Fortunately, this
 * is easy and does not have too much overhead.
 *
 * The OpenGL ES Shading Language, version 1.0, which is a part of OpenGL ES,
 * version 2.0, also does not have bitwise operators.
 * Any of them.
 * At all.
 * This means that whenever I need to perform a bitwise operation, I have to
 * reimplement said operation using integer arithmetic. Said reimplementations
 * do not perform very well, compared to their bitwise counterparts.
 *
 * Furthermore, in the OpenGL ES Shading Language, version 1.0, which is a part
 * of OpenGL ES, version 2.0, there is no guarantee that integers are actually
 * integers. Depending on the implementation, they could be floats. Because of
 * this, I have to wrap every division operation that has the possibility of
 * having a remainder in a call to floor(), or risk having the result rounded
 * up. The only way this can be possible, is if turning a float into an int
 * performs a rounding operation, rather than truncating anything past the
 * decimal point. Isn't that lovely? So, of course, this means I need to floor
 * any float I'm turning into an int as well.
 *
 * But here's the fun part: in the OpenGL ES Shading Language, version 1.0,
 * which is a part of OpenGL ES, verison 2.0, there is no way to tell at shader
 * compile time whether or not ints are implemented as floats. And floor() can
 * only accept floats or float vectors as arguments. Therefore if ints are not
 * implemented as floats for a given GPU, any calls to floor() which pass ints
 * or int vectors as the argument will cause compilation to fail.
 *
 * Rather than do the correct thing and programatically recompile without these
 * calls to floor(), I'm going to do the lazy thing and use floats everywhere
 * instead of ints. At least this way the types of my vairables will not be
 * implementation-dependent. Hopefully this won't cause too many issues.
 *
 * Of course, this means my original plan of using an ivec2 to emulate 32-bit
 * unsigned integers is out. I tried using a vec2, but even with high
 * precision floats, I still ended up with odd colors coming out of my
 * interpolation functions. This means that colors, regardless of whether they
 * are represented as one of our fake unsigned integers, or the standard OpenGL
 * groups of four floating point numbers, they're still stored in a vec4. Even
 * though I'm giving up and using the types GLSL ES wants me to use, I still
 * have to deal with underflows and overflows myself, and I still have to
 * write my own bitwise functions.
 *
 * I guess this is all just to say:
 * Man, I would love to know what they were smoking at Khronos when they came
 * up with this shit.
 *
 * Anyway, here's the common code shared between all the shaders in this
 * project. A significant amount of this code is here just to work around the
 * limitations of the OpenGL ES Shading Language, version 1.0, which is a part
 * of OpenGL ES, version 2.0. Fun stuff.
 */

precision highp float;
precision highp int;
precision highp sampler2D;

// Fake 32-bit unsigned integer
#define u32 vec4

u32 shiftR(u32 val, float shift) {
    float divisor = exp2(shift);
    float oshift = exp2(8.0 - shift);
    u32 ret = floor(val / divisor);
    ret.g += mod(val.r, divisor) * oshift;
    ret.b += mod(val.g, divisor) * oshift;
    ret.a += mod(val.b, divisor) * oshift;
    return ret;
}

float and(float a, float b) {
    float ret;
    float n = 1.0;
    vec2 x = vec2(a, b);
    while((x.r > 0.0) &&
          (x.g > 0.0)) {
        if(mod(x, 2.0) == vec2(1.0, 1.0)) {
            ret += n;
        }
        x = floor(x / 2.0);
        n *= 2.0;
    }
    return ret;
}

float and(float a, float b, float c) {
    float ret;
    float n = 1.0;
    vec3 x = vec3(a, b, c);
    while((x.r > 0.0) &&
          (x.g > 0.0) &&
          (x.b > 0.0)) {
        if(mod(x, 2.0) == vec3(1.0, 1.0, 1.0)) {
            ret += n;
        }
        x = floor(x / 2.0);
        n *= 2.0;
    }
    return ret;
}

// This should probably be a macro
u32 and(u32 a, u32 b) {
    return u32(and(a.r, b.r),
               and(a.g, b.g),
               and(a.b, b.b),
               and(a.a, b.a));
}

// This should probably be a macro
u32 and(u32 a, u32 b, u32 c) {
    return u32(and(a.r, b.r, c.r),
               and(a.g, b.g, c.g),
               and(a.b, b.b, c.b),
               and(a.a, b.a, c.a));
}

// This should probably be a macro
u32 fix_overflow(u32 val) {
    u32 ret;
    ret.a = mod(val.a, 256.0);
    ret.bgr = mod((val.bgr + floor(val.abg / 256.0)), 256.0);
    return ret;
}

// Turn a vec4 color into one of our jury-rigged 32-bit ints
u32 pack(vec4 color) {
    return color * 255.0;
}

// Turn one of our jury-rigged 32-bit ints into a vec4 color
vec4 unpack(u32 c) {
    return c / 255.0;
}

vec4 c_black  = vec4(0.0, 0.0, 0.0, 1.0);
vec4 c_white  = vec4(1.0, 1.0, 1.0, 1.0);
vec4 c_red    = vec4(1.0, 0.0, 0.0, 1.0);
vec4 c_green  = vec4(0.0, 1.0, 0.0, 1.0);
vec4 c_blue   = vec4(0.0, 0.0, 1.0, 1.0);
vec4 c_yellow = vec4(1.0, 1.0, 0.0, 1.0);
vec4 c_purple = vec4(1.0, 0.0, 1.0, 1.0);
vec4 c_indigo = vec4(0.0, 1.0, 1.0, 1.0);

