#version 330 core

#define MAX_POINT_LIGHTS 32
#define MAX_SPOTLIGHTS 32

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
in vec3 LightPosition;

struct Material 
{
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
    sampler2D texture_specular3;
    sampler2D emission;
    float shininess;
};

struct DirectionalLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;

    float cutOff;
    float outerCutoff;
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct CamInfo
{
    float near;
    float far;
};

uniform Material material;

uniform DirectionalLight directionalLight;

uniform int numOfPointLights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform int numOfSpotLights;
uniform SpotLight spotLights[MAX_SPOTLIGHTS];

uniform CamInfo camInfo;

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection);
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPosition, vec3 viewDirection);
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPosition, vec3 viewDirection);
float LinearizeDepth(float depth);

void main()
{ 
    vec3 normal = normalize(Normal);

    vec3 totalLight = vec3(0.0f);
    for (int i = 0; i < numOfPointLights; ++i)
    {
        totalLight += CalculatePointLight(pointLights[i], normal, FragPos, normalize(-FragPos));
    }

    totalLight += CalculateDirectionalLight(directionalLight, normal, normalize(-FragPos));

    for (int j = 0; j < numOfSpotLights; ++j)
    {
        totalLight += CalculateSpotLight(spotLights[j], normal, FragPos, normalize(-FragPos));
    }

    FragColor = vec4(totalLight, 1.0f);
}

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection)
{   
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));

    vec3 lightDirection = normalize(-light.direction);
    vec3 diffuse = max(dot(normal, lightDirection), 0.0) * light.diffuse * vec3(texture(material.texture_diffuse1, TexCoords));

    vec3 reflectDirection = reflect(-lightDirection, normal);
    vec3 specular = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess) * light.specular * vec3(texture(material.texture_specular1, TexCoords));

    return (ambient + diffuse + specular);
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPosition, vec3 viewDirection)
{
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));

    vec3 lightDirection = normalize(light.position - fragPosition);
    vec3 diffuse = max(dot(normal, lightDirection), 0.0) * light.diffuse * vec3(texture(material.texture_diffuse1, TexCoords));

    vec3 reflectDirection = reflect(-lightDirection, normal);
    vec3 specular = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess) * light.specular * vec3(texture(material.texture_specular1, TexCoords));

    float distance = length(light.position - fragPosition);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    return attenuation * (ambient + diffuse + specular);
}

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPosition, vec3 viewDirection)
{
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));

    float epsilon = light.cutOff - light.outerCutoff;

    vec3 lightDirection = normalize(light.position - fragPosition);
    float cosTheta = dot(-lightDirection, light.direction);

    float intensity = clamp((cosTheta - light.outerCutoff) / epsilon, 0.0f, 1.0f);

    vec3 diffuse = max(dot(normal, lightDirection), 0.0) * light.diffuse * vec3(texture(material.texture_diffuse1, TexCoords));

    vec3 reflectDirection = reflect(-lightDirection, normal);
    vec3 specular = pow(max(dot(lightDirection, reflectDirection), 0.0), material.shininess) * light.specular * vec3(texture(material.texture_specular1, TexCoords));

    float distance = length(light.position - fragPosition);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    return intensity * attenuation * (ambient + diffuse + specular);
}

float LinearizeDepth(float depth)
{
    float z = depth * 2.0f - 1.0f;
    return (2.0f * camInfo.near * camInfo.far) / (camInfo.far + camInfo.near - z * (camInfo.far - camInfo.near));
}