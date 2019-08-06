#version 440

const int n = 8;
const int nFx = 4;

layout(binding = 0) uniform sampler2DRect layers[n];

in vec2[n] uvs;

uniform bool[n] playing;
uniform ivec2[n] dimensions;
uniform int[n] blendingModes;
uniform float[n] alphas;

uniform float masterAlpha;

uniform int[nFx] fxTypes;
uniform bool[nFx] fxPlaying;

out vec4 outputColor;

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
                vec3 tmp = clamp(blended + color + 2.0 * blended * color, 0.0, 1.0);
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
            case 1: // ReducePalette
                blended = round(blended);
                break;
            case 2: // ColorShift
                blended = blended.brg;
                break;
            case 3: // ColorShift2
                blended = blended.gbr;
                break;
            default:
                // do nothing
                break;
        }
    }

   outputColor = vec4(blended, 1.0);
}