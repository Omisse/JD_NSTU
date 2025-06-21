#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUV;
layout (location = 3) in vec4 vColor;

out vec4 vVertexColor;

uniform mat4 mModel;
uniform mat4 mView;
uniform mat4 mProjection;

void main()
{
    vVertexColor = vec4(vColor.r, vColor.g, vColor.b, vColor.a);
    gl_Position = mProjection * mView * mModel * vec4(vPos,1.0);
}