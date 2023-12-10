#version 330 core

in vec3 m_world_position;
in vec3 m_world_normal;

out vec4 fragColor;

// declare relevant uniforms for ambient and diffuse lighting
uniform float m_ka;
uniform float m_kd;

// declare relevant uniforms for specular lighting
uniform float m_ks;
uniform vec4 m_cameraWorldPos;

// declare relevant uniforms for material's properties
uniform vec4 material_cAmbient;
uniform vec4 material_cDiffuse;
uniform vec4 material_cSpecular;
uniform float material_shininess;
uniform vec4 material_cReflective;

// light related data as uniforms
uniform int num_lights;

struct LightData {
    int type;
    vec4 color;
    vec3 attenuation;
    vec4 pos;
    vec4 dir;

    float penumbra;
    float angle;
};

uniform LightData lights[8];

void main() {
    vec4 normWorld = vec4(normalize(m_world_normal), 0.f);
    float returnR = 0.0f;
    float returnG = 0.0f;
    float returnB = 0.0f;

    // calculate ambient term
    float ambientTermR = material_cAmbient[0] * m_ka;
    float ambientTermG = material_cAmbient[1] * m_ka;
    float ambientTermB = material_cAmbient[2] * m_ka;

    // add the ambient term
    returnR += ambientTermR;
    returnG += ambientTermG;
    returnB += ambientTermB;

    float diffuseWeightR = m_kd * material_cDiffuse[0];
    float diffuseWeightG = m_kd * material_cDiffuse[1];
    float diffuseWeightB = m_kd * material_cDiffuse[2];

    float specualarWeightR = m_ks * material_cSpecular[0];
    float specualarWeightG = m_ks * material_cSpecular[1];
    float specualarWeightB = m_ks * material_cSpecular[2];

    for(int i = 0; i < num_lights; i++) {
        vec4 surfaceToLight = (lights[i].type == 1) ? vec4(normalize(-(lights[i].dir))) : normalize(lights[i].pos - vec4(m_world_position, 1.0f));
        float n_lDot = dot(normWorld, surfaceToLight);
        n_lDot = clamp(n_lDot, 0.0f, 1.0f);

        vec4 reflLight = normalize(reflect(-surfaceToLight, normWorld));
        vec4 surfaceToCamera = normalize(m_cameraWorldPos - vec4(m_world_position, 1.0f));
        float r_eDot = dot(reflLight, surfaceToCamera);
        r_eDot = clamp(r_eDot, 0.0f, 1.0f);

        float diffuseTermR = diffuseWeightR * n_lDot;
        float diffuseTermG = diffuseWeightG * n_lDot;
        float diffuseTermB = diffuseWeightB * n_lDot;

        float powSpecular = pow(r_eDot, material_shininess);
        powSpecular = clamp(powSpecular, 0.0f, 1.0f);
        if(material_shininess == 0.f) powSpecular = 0.f;

        float specularTermR = specualarWeightR * powSpecular;
        float specularTermG = specualarWeightG * powSpecular;
        float specularTermB = specualarWeightB * powSpecular;

        float attenuation = 1.0f;
        if (lights[i].type != 1) {
            float distance = length(vec3(lights[i].pos) - m_world_position); // dis to light
            float attenuationQuotient = 1.0f/(lights[i].attenuation[0] + distance * lights[i].attenuation[1]
                                                + pow(distance, 2.0f) * lights[i].attenuation[2]);
            attenuation = min(1.0f, attenuationQuotient);
        }

        vec3 lightIntensity = vec3(lights[i].color);

        if(lights[i].type == 2) {
            vec3 currDir = vec3(-surfaceToLight); // direction from the spot light to the current point of intersection
            float dotProduct = dot(currDir, vec3(lights[i].dir));
            float magnitudeA = length(currDir);
            float magnitudeB = length(vec3(lights[i].dir));
            float angleXRadians = acos(dotProduct / (magnitudeA * magnitudeB)); // angle between current direction and spotlight direction

            float innerAngle = lights[i].angle - lights[i].penumbra;

            if(angleXRadians <= innerAngle){
                lightIntensity = vec3(lights[i].color);
            }else if (angleXRadians > innerAngle && angleXRadians <= lights[i].angle) {
                float angleQuotient = (angleXRadians - innerAngle) / lights[i].penumbra;
                float falloff = -2.f * pow(angleQuotient, 3.f) + 3.f * pow(angleQuotient, 2.f);
                lightIntensity = (1.f - falloff) * vec3(lights[i].color);
            } else if (angleXRadians > lights[i].angle) {
                lightIntensity = vec3(0.0f);
            }

        }

        float sumR = (attenuation * lightIntensity[0])*(diffuseTermR + specularTermR);
        float sumG = (attenuation * lightIntensity[1])*(diffuseTermG + specularTermG);
        float sumB = (attenuation * lightIntensity[2])*(diffuseTermB + specularTermB);

        returnR += sumR;
        returnG += sumG;
        returnB += sumB;

    }

    fragColor = vec4(returnR, returnG, returnB, 1.f);
}
