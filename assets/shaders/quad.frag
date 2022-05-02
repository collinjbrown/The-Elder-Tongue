#version 330 core
in vec4 rgbaColor;
in vec2 texCoords;
in float texIndex;
in float mapIndex;
out vec4 color;

// The size of this array is hard-coded,
// and must be manually changed if the QuadRenderer's
// corresponding constant is changed.
uniform sampler2D batchQuadTextures[32];

void main()
{
    int tIndex = int(texIndex);
    int mIndex = int(mapIndex);
    
    vec2 mapSize = textureSize(batchQuadTextures[mIndex], 2);

    vec4 sourceColor = texture(batchQuadTextures[tIndex], texCoords);
    vec2 mapCoord;
    mapCoord.x = sourceColor.r * mapSize.x;
    mapCoord.y = sourceColor.g * mapSize.y;

    color = rgbaColor * texture(batchQuadTextures[mIndex], mapCoord);
    // color = vec4(rgbaColor);
}
