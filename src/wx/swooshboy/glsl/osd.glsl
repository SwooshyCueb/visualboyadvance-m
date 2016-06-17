/* Shader for text rendering
 *
 * Vertex and fragment
 */

precision highp float;
precision highp int;
precision highp sampler2D;

varying vec2 v_texcoord;

#ifdef VERTEX

attribute vec2 texcoord;

#endif



#ifdef FRAGMENT

uniform sampler2D src_tex;

#endif
