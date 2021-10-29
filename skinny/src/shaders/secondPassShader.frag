#version 440

const int n = 8;
const int nFx = 8;

const float eps = 0.0001f;
const float pi = 3.14159;

layout(binding = 0) uniform sampler2DRect firstPass;

in vec2 texCoordVarying;

uniform int[nFx] fxTypes;
uniform bool[nFx] fxPlaying;
uniform float[nFx] fxParam; 

out vec4 outputColor;

//--------------------------------------------------------------
vec3 blur(float p)
{
    const float width = max(ceil(p * 127.0f), 1);
    const int l = -int(floor(width * 0.5f));
    const int r = int(ceil(width * 0.5f));
    vec3 c = vec3(0.0f);
    for (int x = l; x < r; x++)
    {
        for (int y = l; y < r; y++)        {
            c += texture(firstPass, texCoordVarying + vec2(x, y)).rgb;
        }
    }

    return c / (width * width);
}

//--------------------------------------------------------------
void main()
{
    vec3 blended = texture(firstPass, texCoordVarying).rgb;

    for (int i = 0; i < nFx; i++) {
        if (!fxPlaying[i])
            continue;

        switch (fxTypes[i]) {
            case 7: // Blur
                blended = blur(fxParam[i]);
                break;
            default:
                // do nothing
                break;
        }
    }

   outputColor = vec4(blended, 1.0);
}
