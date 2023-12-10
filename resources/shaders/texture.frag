#version 330 core

// UV coordinate in variable
in vec2 uvCoords;

// A sampler2D uniform
uniform sampler2D m_texture;

// bools for whether or not to filter the texture
uniform bool m_doFilterPerPixel;
uniform bool m_doKernelFilter;
uniform bool m_doExtraCredit1;

uniform int m_scene_width;
uniform int m_scene_height;

out vec4 fragColor;

void main()
{

    // Set fragColor using the sampler2D at the UV coordinate
    fragColor = texture(m_texture, uvCoords);

    // Invert fragColor's r, g, and b color channels if per pixel filter bool is true
    if(m_doFilterPerPixel) fragColor = 1.0f - fragColor;

    // Implement a 5x5 box blur filter if kernel filter is true
    if(m_doKernelFilter){
        vec3 resultColor = vec3(0.f);

            // Loop over a 5x5 neighborhood
            for (int i = -2; i <= 2; ++i) {
                for (int j = -2; j <= 2; ++j) {
                    // Calculate the adjusted UV coordinate
                    vec2 adjustedUV = uvCoords + vec2(float(i) / m_scene_width,
                                                      float(j) / m_scene_height);

                    // Sample the texture at the adjusted UV coordinate
                    vec3 sampledColor = texture(m_texture, adjustedUV).rgb;

                    // Accumulate the color
                    resultColor += sampledColor;
                }
            }

        // Average the accumulated color
        resultColor /= 25.f;

        // Output the result color
        fragColor = vec4(resultColor, 1.f);
    }

    if(m_doExtraCredit1){ // grayscale using the luma method
        float gray = 0.299f * fragColor[0] + 0.587 * fragColor[1] + 0.114 * fragColor[2];
        fragColor = vec4(gray, gray, gray, 1.f);
    }
}
