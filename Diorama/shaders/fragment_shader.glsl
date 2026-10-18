#version 330 core

in vec4 vColor;
in vec2 vTex;

uniform sampler2D uTex;

out vec4 FragColor;

void main()
{
    // La textura da el color visible; vColor solo aporta el alfa
    // (asi cada triangulo puede tener su propia transparencia).
    vec4 texColor = texture(uTex, vTex);
    FragColor = vec4(texColor.rgb, texColor.a) * vColor;
}
