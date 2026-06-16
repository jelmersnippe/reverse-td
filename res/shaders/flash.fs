#version 330

in vec2 fragTexCoord;

uniform sampler2D texture0;
uniform vec3 color;
uniform float volume; // 0 = normal, 1 = fully white

out vec4 finalColor;

void main()
{
    vec4 texel = texture(texture0, fragTexCoord);

    finalColor.rgb = mix(texel.rgb, color, volume);
    finalColor.a = texel.a;
}
