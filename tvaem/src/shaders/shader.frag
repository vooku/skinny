#version 440

const int n = 8;
const int nFx = 5;

layout(binding = 0) uniform sampler2DRect layers[n];

in vec2[n] uvs;

uniform bool[n] playing;
uniform vec2[n] dimensions;
uniform int[n] blendingModes;
uniform float[n] alphas;

uniform float masterAlpha;

uniform int[nFx] fxTypes;
uniform bool[nFx] fxPlaying;
uniform float[nFx] fxParam;

out vec4 outputColor;

vec3 posterize(vec3 c, float p)
{
    float levels = p * 127;
    return floor(c * levels) / (levels - 1);
}

vec3 colorShift(vec3 c, float p)
{
    return int(p * 127) % 2 == 1 ? c.brg : c.gbr;
}

vec3 overdrive(vec3 c, float p)
{
    return mod(c * max(1, p * 127.0f), 1.0f);
}

void main()
{
    vec3 blended = vec3(0.0);

    bool anyPlaying = false;

    for (int i = 0; i < n; i++) {
        if (!playing[i])
            continue;

        vec3 color = texture(layers[i], uvs[i]).rgb;
        anyPlaying = true;

        switch(blendingModes[i]) {
            case 1: {// Multiply
                vec3 tmp = blended * color;
                blended = mix(blended, tmp, alphas[i]);
                break;
            }
            case 2: {// Screen
                vec3 tmp = 1.0 - (1.0 - blended) * (1.0 - color);
                blended = mix(blended, tmp, alphas[i]);
                break;
            }
            case 3: {// Darken
                vec3 tmp = min(blended, color);
                blended = mix(blended, tmp, alphas[i]);
                break;
            }
            case 4: {// Lighten
                vec3 tmp = max(blended, color);
                blended = mix(blended, tmp, alphas[i]);
                break;
            }
            case 5: {// LinearDodge
                vec3 tmp = clamp(blended + color, 0.0, 1.0);
                blended = mix(blended, tmp, alphas[i]);
                break;
            }
            case 6: {// Difference
                vec3 tmp = abs(blended - color);
                blended = mix(blended, tmp, alphas[i]);
                break;
            }
            case 7: {// Exclusion
                vec3 tmp = clamp(blended + color - 2.0 * blended * color, 0.0, 1.0);
                blended = mix(blended, tmp, alphas[i]);
                break;
            }
            default: // Normal
                blended = mix(blended, color, alphas[i]);
                break;
        }
        blended = masterAlpha * clamp(blended, vec3(0.0), vec3(1.0));
    }

    for (int i = 0; i < nFx; i++) {
        if (!anyPlaying)
            break;

        if (!fxPlaying[i])
            continue;

        switch (fxTypes[i]) {
            case 0: // Inverse
                blended = vec3(1.0) - blended;
                break;
            case 1: // Posterize
                blended = posterize(blended, fxParam[i]);
                break;
            case 2: // ColorShift
                blended = colorShift(blended, fxParam[i]);
                break;
            case 3: // Overdrive
                blended = overdrive(blended,  fxParam[i]);
                break;
            default:
                // do nothing
                break;
        }
    }

   outputColor = vec4(blended, 1.0);
}
