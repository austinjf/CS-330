#version 440 core 

in vec3 vertexNormal;
in vec3 vertexFragmentPos;
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPosition;
uniform sampler2D uTexture;

void main()
{

    float ambientStrength = 0.5f;
    vec3 ambient = ambientStrength * lightColor;


    vec3 norm = normalize(vertexNormal);
    vec3 lightDirection = normalize(lightPos - vertexFragmentPos);
    float lightImpact = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = lightImpact * lightColor;


    float specularIntensity = 2.0f;
    float highlightSize = 8.0f;
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos);
    vec3 reflectDir = reflect(-lightDirection, norm);
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 specular = specularIntensity * specularComponent * lightColor;


    vec4 textureColor = texture(uTexture, vertexTextureCoordinate);

    vec3 phong = (ambient + diffuse + specular) * textureColor.xyz;

    fragmentColor = vec4(phong, 1.0);
}