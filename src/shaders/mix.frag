#version 330 core

uniform sampler2D texture;
uniform bool isHorizontal;
float weight = 0.05;

void main() {
  vec2 texOffset = 1. / textureSize(texture, 0);
  vec2 texCoord = gl_TexCoord[0].xy;
  vec3 col = texture2D(texture, texCoord).rgb;

  if (isHorizontal)
    for (int i = 1; i < 5; i++) {
      col += texture2D(texture, texCoord + vec2(texOffset.x * i, 0.)).rgb * weight;
      col += texture2D(texture, texCoord - vec2(texOffset.x * i, 0.)).rgb * weight;
    }
  else
    for (int i = 1; i < 5; i++) {
      col += texture2D(texture, texCoord + vec2(0., texOffset.y * i)).rgb * weight;
      col += texture2D(texture, texCoord - vec2(0., texOffset.y * i)).rgb * weight;
    }

  gl_FragColor = vec4(col, 1.0);
}

