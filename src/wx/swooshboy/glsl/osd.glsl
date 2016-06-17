/* Shader for text rendering
 *
 * Vertex and fragment
 * Not finished yet (obviously)
 */

precision highp float;
precision highp int;
precision highp sampler2D;

varying vec2 v_texcoord;

#ifdef VERTEX

attribute vec2 texcoord;

#endif



#ifdef FRAGMENT

uniform sampler2D glyph;
uniform vec4 fg;
uniform vec4 bg;

#endif
