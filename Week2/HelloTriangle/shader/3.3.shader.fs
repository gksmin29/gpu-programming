#version 330 core
out vec4 FragColor;
in vec3 ourColor;
uniform float ourBrightness;

void main()
{
   FragColor = ourBrightness*vec4(ourColor, 1.0f);
}