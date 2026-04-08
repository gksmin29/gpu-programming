#version 330 core
out vec4 FragColor;

in vec2 texCoord;
uniform sampler2D texture1; // texture sampler1
uniform sampler2D texture2; // texture sampler2
uniform float alpha;

void main()
{
    FragColor = mix(texture(texture1, texCoord), texture(texture2, texCoord), alpha);
}