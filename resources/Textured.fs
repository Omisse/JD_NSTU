#version 330 core

layout(location=0) out vec4 vFragColor;

in vec2 UV;

uniform sampler2D texture_diffuse1;

void main()
{    
    //vFragColor = vec4(UV, 0,1);
    vFragColor = texture(texture_diffuse1, UV);
}