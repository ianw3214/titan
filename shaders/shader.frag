#version 400 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D uTexture1;
uniform sampler2D uTexture2;

void main() {
    // FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    // FragColor = vec4(vertexColour, 1.0);
    // FragColor = texture(uTexture, texCoord);
    // FragColor = texture(uTexture, texCoord) * vec4(vertexColour, 1.0);
    FragColor = mix(texture(uTexture1, texCoord), texture(uTexture2, texCoord), 0.2);
}