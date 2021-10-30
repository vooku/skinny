#version 440

const int n = 8;
const int nFx = 12;

const float eps = 0.0001f;
const float pi = 3.14159;

layout(binding = 0) uniform sampler2DRect layers[n];

in vec2[n] uvs;

uniform bool[n] playing;
uniform vec2[n] dimensions;
uniform int[n] blendingModes;
uniform float[n] alphas;

uniform float masterAlpha;
uniform float timeShift;

uniform int[nFx] fxTypes;
uniform bool[nFx] fxPlaying;
uniform float[nFx] fxParam; 

out vec4 outputColor;

//--------------------------------------------------------------
vec2 horizontalOffset(vec2 uv, float p, vec2 dimensions)
{
    const float A = 0.075 * p * dimensions.x;
    const float p2 = p * p;
    const float f = 2 * pi * 5 * p2 / dimensions.y;
    const float timeOffset = 10 * p2 * timeShift;
    vec2 offset = vec2(A * sin(f * uv.y + timeOffset), 0);
    return mod(uv + offset, dimensions);
}

//--------------------------------------------------------------
vec2 verticalOffset(vec2 uv, float p, vec2 dimensions)
{
    const float A = 0.1 * p * dimensions.y;
    const float p2 = p * p;
    const float f = 2 * pi * 10 * p2 / dimensions.x;
    const float timeOffset = 20 * p2 * timeShift;
    vec2 offset = vec2(0, A * sin(f * uv.x + timeOffset));
    return mod(uv + offset, dimensions);
}

//--------------------------------------------------------------
// inspired by www.github.com/FitzwilliamMuseum/fitz-kaleidoscope
vec2 kaleidoscope(vec2 uv, float p, vec2 dimensions)
{
    const float segments = p * 127.0f;
    uv /= dimensions;

    if (segments < 2.0f)
    {
        // mirror
//        if (uv.x > 0.5f)
//            uv.x = 1.0f - uv.x;
        if (uv.x < 0.5f)
            uv.x = 1.0f - uv.x;
    }
    else
    {
        // proper kaleidoscope
        uv = uv * 2.0f - 1.0f;

        float r = length(uv);
        float theta = atan(uv.y, uv.x);
        theta /= pi;
        theta *= segments;
    
//        if (mod(theta, 2.0f) >= 1.0f)
              theta = fract(theta);
//        else
//            theta = 1.0f - fract(theta);

        theta /= segments;
        theta *= pi * 2.0f;
        uv.x = r * cos(theta);
        uv.y = r * sin(theta);

        uv = fract(uv);
    }

    uv *= dimensions;

    return uv;
}

//--------------------------------------------------------------
vec2 distort(vec2 uv, vec2 dimensions)
{
    for (int i = 0; i < nFx; i++) {
        if (!fxPlaying[i])
            continue;
       
        switch (fxTypes[i]) {
            case 4: // Horizontal Offset
                uv = horizontalOffset(uv, fxParam[i], dimensions);
                break;
            case 5: // Vertical Offset
                uv = verticalOffset(uv, fxParam[i], dimensions);
                break;
            case 10: // Kaleidoscope
                uv = kaleidoscope(uv, fxParam[i], dimensions);
                break;
        }
    }

    return uv;
}

//--------------------------------------------------------------
vec3 solarize(vec3 c, float p)
{
    const vec3 lum = { 0.2126, 0.7152, 0.0722 };
    const bool invert = dot(c, lum) <= p;
    return invert ? 1 - c : c;
}

//--------------------------------------------------------------
vec3 posterize(vec3 c, float p)
{
    float levels = p * 127;
    return floor(c * levels) / (levels - 1);
}

//--------------------------------------------------------------
vec3 colorShift(vec3 c, float p)
{
    return int(p * 127) % 2 == 1 ? c.brg : c.gbr;
}

//--------------------------------------------------------------
vec3 overdrive(vec3 c, float p)
{
    return mod(c * max(1, p * 127.0f), 1.0f + eps);
}

//--------------------------------------------------------------
vec3 desaturate(vec3 c, float p)
{
    const vec3 weights = vec3(0.299f, 0.587f, 0.114f);
    float lum = dot(weights, c);
    return mix(vec3(lum), c, 1.f - p);
}

//--------------------------------------------------------------
void main()
{
    vec3 blended = vec3(0.0);

    bool anyPlaying = false;

    for (int i = 0; i < n; i++) {
        if (!playing[i])
            continue;

        vec2 uvDist = distort(uvs[i], dimensions[i]);
//        blended.xy = uvDist / dimensions[i]; break;
        vec3 color = texture(layers[i], uvDist).rgb; // texture pixel coords
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
            case 0: // Solarize
                blended = solarize(blended, fxParam[i]);
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
            case 6: // Desaturate
                blended = desaturate(blended,  fxParam[i]);
                break;
            default:
                // do nothing
                break;
        }
    }

   outputColor = vec4(blended, 1.0);
}
