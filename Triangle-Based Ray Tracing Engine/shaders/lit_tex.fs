#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 Tex;

uniform sampler2D diffuseMap;
uniform vec3      viewPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

/* ---------- Light Structures ---------- */
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
    // // Diffuse
    // float diff  = max(dot(norm, lightDir), 0.0);
    // vec3 diffuse  = diff * albedo * lightColor;

    // // Specular
    // vec3 reflectDir = reflect(-lightDir, norm);
    // float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
    // vec3 specular = spec * lightColor * 0.4;

    // // Ambient
    // vec3 ambient  = 0.05 * albedo * lightColor;

    // return ambient + diffuse + specular;

    // Ambient
    vec3 ambient  = albedo * material.ambient * lightColor;
    
    // Diffuse
    float diff    = max(dot(norm, lightDir), 0.0);
    vec3 diffuse  = albedo * material.diffuse * diff * lightColor;

    // Specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec    = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.specular * spec * lightColor;
    
    return ambient + diffuse + specular;
}

void main()
{
    vec3 albedo = texture(diffuseMap, Tex).rgb;
    vec3 norm   = normalize(Normal);
    vec3 viewDir= normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);

    /* ---- Directional ---- */
    vec3 dirLightDir = normalize(-dirLight.direction);
    result += phong(dirLightDir, dirLight.color, norm, viewDir, albedo);

    /* ---- Point lights ---- */
    for(int i = 0; i < 2; ++i)
    {
        vec3 lightDir = normalize(pointLights[i].position - FragPos);
        result += phong(lightDir, pointLights[i].color, norm, viewDir, albedo);
    }

    FragColor = vec4(result, 1.0);
}


// 
// #version 330 core
// out vec4 FragColor;

// in vec3 FragPos;
// in vec3 Normal;
// in vec2 Tex;

// uniform sampler2D diffuseMap;
// uniform vec3 lightPos;
// uniform vec3 viewPos;

// void main()
// {
//     vec3  albedo  = texture(diffuseMap, Tex).rgb;

//     // Ambient
//     vec3 ambient  = 0.1 * albedo;

//     // Diffuse
//     vec3 norm     = normalize(Normal);
//     vec3 lightDir = normalize(lightPos - FragPos);
//     float diff    = max(dot(norm, lightDir), 0.0);
//     vec3 diffuse  = diff * albedo;

//     // Specular
//     vec3 viewDir    = normalize(viewPos - FragPos);   //  ← kameraya göre vektör
//     vec3 reflectDir = reflect(-lightDir, norm);
//     float spec      = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
//     vec3 specular = spec * vec3(1.0);        // beyaz parlama

//     FragColor = vec4(ambient + diffuse + specular, 1.0);
// }