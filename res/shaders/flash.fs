#version 100

precision mediump float;

varying vec2 fragTexCoord;

uniform sampler2D texture0;
uniform vec3 color;
uniform float volume;

void main()
{
    vec4 texel = texture2D(texture0, fragTexCoord);

    vec3 finalRgb = mix(texel.rgb, color, volume);
    gl_FragColor = vec4(finalRgb, texel.a);
}
