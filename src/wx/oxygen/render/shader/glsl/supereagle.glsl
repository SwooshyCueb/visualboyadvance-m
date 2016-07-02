#version 100
/******************************************************************************/
/* GLSL Fragment Shader implementation of the SuperEagle 2x upscaler filter   */
/******************************************************************************/
/* Copyright (C) 2016 Markus Kitsinger (SwooshyCueb) <root@swooshalicio.us>   */
/*                                                                            */
/* This implementation of the SuperEagle filter is subject to the terms of    */
/* Mozilla Public License, version 2.0. You can obtain a copy of this         */
/* license at http://mozilla.org/MPL/2.0/.                                    */
/******************************************************************************/
/* The original SuperEagle filter implementation was created by               */
/* Derek Liauw Kie Fa (Kreed). It was released under the GPL license in 1999. */
/* It can be found at https://vdnoort.home.xs4all.nl/emulation/2xsai/.        */
/******************************************************************************/

#ifndef IN_VBA
#define DEBUG
#include "common.glsl"
#endif

#define NOSMASH4

varying vec2 f_texcoord;
varying vec2 dst_inc;
varying vec2 src_inc;
varying float f_needs_flip;

#ifdef VERTEX

attribute vec4 v_pos;
attribute vec2 v_texcoord;

uniform vec2 v_src_sz;
uniform vec2 v_dst_sz;

uniform mediump int v_pass_idx;
uniform mediump int v_pass_qty;

uniform bool needs_flip;

void main() {
    vec4 pos = v_pos;
    vec2 tc = v_texcoord;

    if (needs_flip) {
        pos.xy = -pos.xy;
        tc.x = 1.0 - tc.x;
    }

    gl_Position = pos;
    f_texcoord = tc;

    dst_inc = vec2(1.0/v_dst_sz.x, 1.0/v_dst_sz.y);
    src_inc = vec2(1.0/v_src_sz.x, 1.0/v_src_sz.y);
    f_needs_flip = float(needs_flip);
}

#endif

#ifdef FRAGMENT

uniform sampler2D src_tex;

uniform vec2 f_src_sz;
uniform vec2 f_dst_sz;

uniform mediump int f_pass_idx;
uniform mediump int f_pass_qty;

vec2 dst_pos;
vec2 src_pos;
vec2 dst_coord;
vec2 src_coord;

vec2 m_color_d = vec2(float(0xF7), float(0xDE));
vec2 m_low_d = vec2(float(0x08), float(0x21));
vec2 qm_color_d = vec2(float(0xE7), float(0x9C));
vec2 qm_low_d = vec2(float(0x18), float(0x63));

// TODO: Make these tweakable?
u32 m_color;
u32 m_low;
u32 qm_color;
u32 qm_low;

int magicnumber(vec4 A, vec4 B, vec4 C, vec4 D) {
    ivec3 tmp = ivec3(0, 0, 0);
    
    if (A == C) {
        tmp.x = tmp.x + 1;
    } else if (B == C) {
        tmp.y = tmp.y + 1;
    }

    if (A == D) {
        tmp.x = tmp.x + 1;
    } else if (B == D) {
        tmp.y = tmp.y + 1;
    }

    if (tmp.x <= 1) {
        tmp.z = tmp.z + 1;
    }

    if (tmp.y <= 1) {
        tmp.z = tmp.z - 1;
    }

    return tmp.z;
}

#ifdef NOSMASH2
#define smash2(a, b) a
#else
vec4 smash2(vec4 A, vec4 B) {
    if (A == B) {
        return A;
    }
    u32 rA = pack(A);
    u32 rB = pack(B);

    u32 rAm = shiftR(and(rA, m_color), 1.0);
    u32 rBm = shiftR(and(rB, m_color), 1.0);
    u32 rLm = and(rA, rB, m_low);

    return unpack(fix_overflow(rAm + rBm + rLm));
}
#endif

#ifdef NOSMASH4
#define smash4(a, b, c, d) a
#else
vec4 smash4(vec4 A, vec4 B, vec4 C, vec4 D) {
    u32 rA = pack(A);
    u32 rB = pack(B);
    u32 rC = pack(C);
    u32 rD = pack(D);

    u32 rAm = shiftR(and(rA, qm_color), 2.0);
    u32 rBm = shiftR(and(rB, qm_color), 2.0);
    u32 rCm = shiftR(and(rC, qm_color), 2.0);
    u32 rDm = shiftR(and(rD, qm_color), 2.0);

    u32 rAl = and(rA, qm_low);
    u32 rBl = and(rB, qm_low);
    u32 rCl = and(rC, qm_low);
    u32 rDl = and(rD, qm_low);

    u32 rL = shiftR(fix_overflow(rAl + rBl + rCl + rDl), 2.0);
    u32 rLm = and(rL, qm_low);

    return unpack(fix_overflow(rAm + rBm + rCm + rDm + rLm));
}
#endif

vec4 getpx(float x, float y) {
    vec2 offset = vec2(vec2(x, y) * src_inc);
    return texture2D(src_tex, src_coord + offset);
}

void main() {
    dst_pos = floor(gl_FragCoord.xy);
    src_pos = floor(dst_pos / 2.0);
    dst_coord = gl_FragCoord.xy * dst_inc;
    src_coord = (src_pos + 0.5) * src_inc;

    m_color = u32(m_color_d, m_color_d);
    m_low = u32(m_low_d, m_low_d);
    qm_color = u32(qm_color_d, qm_color_d);
    qm_low = u32(qm_low_d, qm_low_d);

    vec2 skew = mod(dst_pos, 2.0);

    #ifdef DEBUG // Shows a few useful borders

    if ((dst_pos.x == 0.0) ||
        (dst_pos.y == 0.0)) {
        gl_FragColor = c_green;
        return;
    } else if ((dst_pos.x == (f_src_sz.x - 1.0)) ||
               (dst_pos.y == (f_src_sz.y - 1.0))) {
        gl_FragColor = c_red;
        return;
    } else if ((dst_pos.x == (f_dst_sz.x - 1.0)) ||
               (dst_pos.y == (f_dst_sz.y - 1.0))) {
        gl_FragColor = c_indigo;
        return;
    }

    #endif

    vec4 dst_px;
    vec4 dst_px_E;
    vec4 dst_px_N;
    vec4 dst_px_NE;

    
    // Fetch pixels from source texture
    vec4 src_px     = getpx( 0.0,  0.0);
    vec4 src_px_N   = getpx( 0.0,  1.0);
    vec4 src_px_E   = getpx( 1.0,  0.0);
    vec4 src_px_S   = getpx( 0.0, -1.0);
    vec4 src_px_W   = getpx(-1.0,  0.0);
    vec4 src_px_NE  = getpx( 1.0,  1.0);
    vec4 src_px_SE  = getpx( 1.0, -1.0);
    vec4 src_px_NW  = getpx(-1.0,  1.0);
    vec4 src_px_NN  = getpx( 0.0,  2.0);
    vec4 src_px_EE  = getpx( 2.0,  0.0);
    vec4 src_px_NNE = getpx( 1.0,  2.0);
    vec4 src_px_ENE = getpx( 2.0,  1.0);

    // Do magic
    // (Don't ask me how this works; I barely understand it myself)
    if ((src_px_N == src_px_E) && (src_px != src_px_NE)) {
        dst_px_E = dst_px_N = src_px_N;

        if ((src_px_NW == src_px_N) || (src_px_E == src_px_SE)) {
            dst_px = smash2(src_px_N, smash2(src_px_N, src_px));
        } else {
            dst_px = smash2(src_px, src_px_E);
        }

        if ((src_px_E == src_px_EE) || (src_px_N == src_px_NN)) {
            dst_px_NE = smash2(src_px_N, smash2(src_px_N, src_px_NE));
        } else {
            dst_px_NE = smash2 (src_px_N, src_px_NE);
        }

    } else if ((src_px == src_px_NE) && (src_px_N != src_px_E)) {
        dst_px_NE = dst_px = src_px;

        if ((src_px_S == src_px) || (src_px_NE == src_px_ENE)) {
            dst_px_E = smash2 (src_px, smash2 (src_px, src_px_E));
        } else {
            dst_px_E = smash2 (src_px, src_px_E);
        }

        if ((src_px_NE == src_px_NNE) || (src_px_W == src_px)) {
            dst_px_N = smash2 (src_px, smash2 (src_px, src_px_N));
        } else {
            dst_px_N = smash2 (src_px_N, src_px_NE);
        }

    } else if (src_px == src_px_NE && src_px_N == src_px_E) {
        int r = 0;

        r += magicnumber(src_px_E, src_px, src_px_NW,  src_px_NN);
        r += magicnumber(src_px_E, src_px, src_px_W,  src_px_S);
        r += magicnumber(src_px_E, src_px, src_px_NNE, src_px_ENE);
        r += magicnumber(src_px_E, src_px, src_px_SE, src_px_EE);

        if (r > 0) {
            dst_px_E = dst_px_N = src_px_N;
            dst_px = dst_px_NE = smash2(src_px, src_px_E);
        } else if (r < 0) {
            dst_px_NE = dst_px = src_px;
            dst_px_E = dst_px_N = smash2(src_px, src_px_E);
        } else {
            dst_px_NE = dst_px = src_px;
            dst_px_E = dst_px_N = src_px_N;
        }

    } else {
        dst_px_NE = dst_px = smash2(src_px_N, src_px_E);
        dst_px_NE = smash4(src_px_NE, src_px_NE, src_px_NE, dst_px_NE);
        dst_px = smash4(src_px, src_px, src_px, dst_px);

        dst_px_N = dst_px_E = smash2(src_px, src_px_NE);
        dst_px_N = smash4(src_px_N, src_px_N, src_px_N, dst_px_N);
        dst_px_E = smash4(src_px_E, src_px_E, src_px_E, dst_px_E);

    }

    if(skew == vec2(0.0, 0.0)) {
        gl_FragColor = dst_px;
    } else if(skew == vec2(1.0, 0.0)) {
        gl_FragColor = dst_px_E;
    } else if(skew == vec2(0.0, 1.0)) {
        gl_FragColor = dst_px_N;
    } else if(skew == vec2(1.0, 1.0)) {
        gl_FragColor = dst_px_NE;
    }

}

#endif
