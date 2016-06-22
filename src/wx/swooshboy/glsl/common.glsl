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
 * 24-bit. And thank GOD it's not 16-bit, because the jury-rigging you see below
 * would absolutely fall apart if not for the fact that these 24-bit integers
 * save me from having to deal with negative integers.
 *
 * The way I am emulating 32-bit integers is using an ivec2. That's two 16-bit
 * components stored in two 24-bit integers. Of course, this means I must deal
 * with overflows of the least significant component myself, and underflows of
 * the most significant component myself. Fortunately, this is easy and does not
 * have too much overhead, but I still find it silly that I need to write my own
 * function for addition, just to make sure the numbers come out straight.
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
 * I guess this is all just to say:
 * Man, I would love to know what they were smoking at Khronos when they came up
 * with this shit.
 *
 * Anyway, here's the common code shared between all the shaders in this
 * project. A significant amount of this code is here just to work around the
 * limitations of the OpenGL ES Shading Language, version 1.0, which is a part
 * of OpenGL ES, version 2.0. Fun stuff.
 */

precision highp float;
precision highp int;
precision highp sampler2D;

vec2 shiftR(vec2 val, float shift) {
    float divisor = exp2(shift);
    vec2 ret = floor(val / divisor);
    ret.y = mod(val.x, divisor) * exp2(16.0 - shift);
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
vec2 and(vec2 a, vec2 b) {
    return vec2(and(a.x, b.x), and(a.y, b.y));
}

// This should probably be a macro
vec2 and(vec2 a, vec2 b, vec2 c) {
    return vec2(and(a.x, b.x, c.x), and(a.y, b.y, c.y));
}

// This should probably be a macro
vec2 fix_overflow(vec2 val) {
    vec2 ret;
    ret.y = mod(val.y, 65536.0);
    ret.x = mod((val.x + floor((val.y - ret.y) / 65536.0)), 65536.0);
    return ret;
}

// Turn a vec4 color into one of our jury-rigged 32-bit ints
vec2 pack(vec4 color) {
    return vec2(
                color.r * 255.0 * 256.0 +
                color.g * 255.0,
                color.b * 255.0 * 256.0 +
                color.a * 255.0);
}

// Turn one of our jury-rigged 32-bit ints into a vec4 color
vec4 unpack(vec2 c) {
    vec4 ret;
    ret.r = floor(c.x / 256.0);
    ret.g = mod(c.x, 256.0);
    ret.b = floor(c.y / 256.0);
    ret.a = mod(c.y, 256.0);
    return ret/255.0;
}

vec4 c_black  = vec4(0.0, 0.0, 0.0, 1.0);
vec4 c_white  = vec4(1.0, 1.0, 1.0, 1.0);
vec4 c_red    = vec4(1.0, 0.0, 0.0, 1.0);
vec4 c_green  = vec4(0.0, 1.0, 0.0, 1.0);
vec4 c_blue   = vec4(0.0, 0.0, 1.0, 1.0);
vec4 c_yellow = vec4(1.0, 1.0, 0.0, 1.0);
vec4 c_purple = vec4(1.0, 0.0, 1.0, 1.0);
vec4 c_indigo = vec4(0.0, 1.0, 1.0, 1.0);

