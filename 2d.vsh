#version 120

uniform mat3 modelTransform;
attribute vec2 pos;
attribute vec4 color;

vec2 pos_out;
varying vec4 color_out;

void main() {
	pos_out = (modelTransform * vec3(pos,1)).xy;
	gl_Position = vec4(pos_out, 0, 1);
	color_out = color;
}
