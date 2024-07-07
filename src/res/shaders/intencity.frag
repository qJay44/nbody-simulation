#version 330 core

uniform sampler2D texture;
uniform vec3 colormap[256];

void main() {
  vec4 tex = texture2D(texture, gl_TexCoord[0].xy);

  float brightness = 255.f / 3.f; // 256 is wrong though look cool
  float sum = tex.r + tex.g + tex.b;

  vec3 col = colormap[int(sum * brightness)];

  gl_FragColor = vec4(col, 1.0);
}

