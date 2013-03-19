// Debug stage shader program

#pragma anki start vertexShader
#pragma anki include "shaders/Pack.glsl"

layout(location = 0) in vec3 position;
layout(location = 1) in uint color;
layout(location = 2) in mat4 modelViewProjectionMat;

out vec3 vColor;

void main()
{
	vColor = unpackUnorm4x8(color).rgb;
	gl_Position = modelViewProjectionMat * vec4(position, 1.0);
}

#pragma anki start fragmentShader
#pragma anki include "shaders/CommonFrag.glsl"

in vec3 vColor;

out vec3 fColor;

void main()
{
	fColor = vColor;
}
