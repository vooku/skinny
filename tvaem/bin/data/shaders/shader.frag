#version 440

const int n = 8;

layout(binding = 0) uniform sampler2DRect layers[n];

in vec2[n] uvs;

uniform bool[n] playing;
uniform ivec2[n] dimensions;
uniform int[n] blendingModes;
uniform float[n] alphas;
uniform float masterAlpha;
uniform bool invert;
uniform bool reducePalette;
uniform bool colorShift;
uniform bool colorShift2;

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

   if (anyPlaying) {
       blended = mix(blended, vec3(1.0) - blended, invert);
       blended = mix(blended, round(blended), reducePalette);
       blended = mix(blended, blended.brg, colorShift);
       blended = mix(blended, blended.gbr, colorShift2);
   }

   outputColor = vec4(blended, 1.0);
}