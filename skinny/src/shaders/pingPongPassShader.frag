#version 440

const int n = 8;
const int nFx = 12;

const float eps = 0.0001f;
const float pi = 3.14159;

const float max_kernel_size = 500.f;

layout(binding = 0) uniform sampler2DRect previousPass;

in vec2 texCoordVarying;

uniform int[nFx] fxTypes;
uniform bool[nFx] fxPlaying;
uniform float[nFx] fxParam; 
uniform bool horizontal;

out vec4 outputColor;

//--------------------------------------------------------------
vec3 blur(float p)
{
    const float width = max(ceil(p * max_kernel_size), 1);
    const int l = -int(floor(width * 0.5f));
    const int r = int(ceil(width * 0.5f));
    vec3 c = vec3(0.0f);
    for (int i = l; i < r; i++)
    {
        vec2 offset = mix(vec2(0.0f, i), vec2(i, 0.0f), vec2(horizontal, horizontal));
        c += texture(previousPass, texCoordVarying + offset).rgb;
    }

    return c / width;
}

//--------------------------------------------------------------
void main()
{
    vec3 blended = texture(previousPass, texCoordVarying).rgb;

    for (int i = 0; i < nFx; i++) {
        if (!fxPlaying[i])
            continue;

        switch (fxTypes[i]) {
            case 7: // Blur
                blended = blur(fxParam[i]);
                break;
            case 8: // Horizontal Blur
                if (horizontal)
                    blended = blur(fxParam[i]);
                break;
            case 9: // Vertical Blur
                if (!horizontal)
                    blended = blur(fxParam[i]);
                break;
            default:
                // do nothing
                break;
        }
    }

   outputColor = vec4(blended, 1.0);
}
