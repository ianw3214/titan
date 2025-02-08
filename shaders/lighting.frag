#version 400 core

struct Material {
    // Multiple diffuse/specular maps are used for when the mesh is composed of many textures
    //  - See Mesh::Draw for how these are set in code
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
    sampler2D texture_specular3;
    float mShininess;
};

struct DirectionalLight {
    vec3 mDirection;

    vec3 mAmbient;
    vec3 mDiffuse;
    vec3 mSpecular;
};

struct PointLight {
    vec3 mPosition;

    float mConstant;
    float mLinear;
    float mQuadratic;

    vec3 mAmbient;
    vec3 mDiffuse;
    vec3 mSpecular;
};

struct SpotLight {
    vec3 mPosition;
    vec3 mDirection;
    float mCutoff;      // This is represented as the cosine of the angle
    float mOuterCutoff; // Same as above
    
    vec3 mAmbient;
    vec3 mDiffuse;
    vec3 mSpecular;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

#define NUM_POINT_LIGHTS 4
uniform vec3 viewPos;
uniform Material material;
uniform DirectionalLight directionalLight;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform SpotLight spotLight;

// ====================================================
vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDirection = normalize(-light.mDirection);
    // Diffuse shading
    float diff = max(dot(normal, lightDirection), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.mShininess);
    // Combine results
    vec3 ambient = light.mAmbient * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse = light.mDiffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.mSpecular * spec * vec3(texture(material.texture_specular1, TexCoords));
    return ambient + diffuse + specular;
}

// ====================================================
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDirection = normalize(light.mPosition - fragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDirection), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.mShininess);
    // Attenuation
    float distance = length(light.mPosition - fragPos);
    float attenuation = 1.0 / (light.mConstant + light.mLinear * distance + light.mQuadratic * distance * distance);
    // Combine results
    vec3 ambient = light.mAmbient * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse = light.mDiffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.mSpecular * spec * vec3(texture(material.texture_specular1, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return ambient + diffuse + specular;
}

// ====================================================
vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDirection = normalize(light.mPosition - fragPos);
    float theta = dot(lightDirection, normalize(-light.mDirection));
    if (theta > light.mOuterCutoff)
    {
        float epsilon = light.mCutoff - light.mOuterCutoff;
        float intensity = clamp((theta - light.mOuterCutoff) / epsilon, 0.f, 1.f);

        // Diffuse shading
        float diff = max(dot(normal, lightDirection), 0.0);
        // Specular shading
        vec3 reflectDir = reflect(-lightDirection, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.mShininess);
        // Combine results
        vec3 ambient = light.mAmbient * vec3(texture(material.texture_diffuse1, TexCoords));
        vec3 diffuse = light.mDiffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords)) * intensity;
        vec3 specular = light.mSpecular * spec * vec3(texture(material.texture_specular1, TexCoords)) * intensity;
        return ambient + diffuse + specular;
    }
    else
    {
        return vec3(0.0);
    }
}

float near = 0.1; 
float far  = 100.0; 
  
// ====================================================
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

// ====================================================
void main() {

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = calculateDirectionalLight(directionalLight, norm, viewDir);
    for (int i = 0; i < NUM_POINT_LIGHTS; ++i)
    {
        result += calculatePointLight(pointLights[i], norm, FragPos, viewDir);
    }
    result += calculateSpotLight(spotLight, norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0);

    // Visualizing depth
    // float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
    // FragColor = vec4(vec3(depth), 1.0);
}