#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 Tex;

uniform sampler2D texture1; // first texture
uniform sampler2D texture2; // scond texture
uniform vec3 viewPos;

/* ---------- light Structures ---------- */
struct DirLight {
    vec3 direction;
    vec3 color;
};

struct PointLight {
    vec3 position;
    vec3 color;
};

uniform DirLight   dirLight;            // 1 directional light
uniform PointLight pointLights[2];      // 2 point lights

/* ---------- Helper ---------- */
vec3 phong(vec3 lightDir, vec3 lightColor,
           vec3 norm, vec3 viewDir, vec3 albedo)
{
    float diff  = max(dot(norm, lightDir), 0.0);
    vec3 diffuse  = diff * albedo * lightColor;

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
    vec3 specular = spec * lightColor * 0.4;

    vec3 ambient  = 0.05 * albedo * lightColor;

    return ambient + diffuse + specular;
}

void main()
{
    // Blend textures
    vec3 tex1 = texture(texture1, Tex).rgb;
    vec3 tex2 = texture(texture2, Tex).rgb;
    vec3 albedo = mix(tex1, tex2, 0.5);  // %50-%50 blend

    vec3 norm   = normalize(Normal);
    vec3 viewDir= normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);

    // Directional light
    vec3 dirLightDir = normalize(-dirLight.direction);
    result += phong(dirLightDir, dirLight.color, norm, viewDir, albedo);

    // Point lights
    for(int i = 0; i < 2; ++i)
    {
        vec3 lightDir = normalize(pointLights[i].position - FragPos);
        result += phong(lightDir, pointLights[i].color, norm, viewDir, albedo);
    }

    FragColor = vec4(result, 1.0);
}
