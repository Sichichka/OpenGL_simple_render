#version 330 core

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shiniess;
};

struct Light
{
	int type;
	float cutOff;

	vec3 position;
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
};

in vec3 vertColor;
in vec2 texCoords;
in vec3 vertNormal;
in vec3 FragPos;
in vec4 FragPosLightSpace;

out vec4 outColor;

uniform sampler2D ourTexture;
uniform bool wireframeMode;
uniform sampler2D shadowMap;

uniform vec3 viewPos;
uniform Material material;

#define MAX_LIGHTS 4
uniform int lights_count;
uniform Light light[MAX_LIGHTS];

float getAtten(int i)
{
	float dist = distance(light[i].position, FragPos);
	float attenuation = 1.0 / (light[i].constant + light[i].linear*dist + light[i].quadratic * dist * dist);
	return attenuation;
}

/*float CalcDiffuse(int i, vec3 lightDir)
{
	//diffuse
	vec3 norm = normalize(vertNormal);
	float diff_koef = max(dot(norm, -lightDir), 0.0f);
	vec3 diffuse = light[i].diffuse * (diff_koef * material.diffuse) * getAtten(i);

	//specular
	vec3 reflectDir = reflect(lightDir, norm);
	vec3 viewDir = normalize(FragPos - viewPos);
	float spec_koef = pow(max(dot(viewDir, reflectDir), 0.0f), material.shiniess);
	vec3 specular = light[i].specular * (spec_koef * material.specular) * getAtten(i);
}*/

float ShadowCalculation(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float shadow = currentDepth > closestDepth + 0.005 ? 1.0 : 0.0;
    return shadow;
}

void main()
{	
	if (wireframeMode)
		outColor = vec4(vertColor, 1.0f);
	else
	{
		//float shadow = ShadowCalculation(FragPosLightSpace);
		//outColor = vec4(shadow, shadow, shadow, shadow);
		outColor = vec4(0, 0, 0, 0);
		vec3 lresult;
		for (int i = 0; i < lights_count; i++)
		{
			if (light[i].type == 1) // Directional light
			{
				float shadow = ShadowCalculation(FragPosLightSpace);
				vec3 ambient = light[i].ambient * material.ambient;
				vec3 norm = normalize(vertNormal);
				vec3 lightDir = -light[i].direction;
				float diff_koef = max(dot(norm, -lightDir), 0.0f);
				vec3 diffuse = light[i].diffuse * (diff_koef * material.diffuse);

				vec3 reflectDir = reflect(-lightDir, norm);
				vec3 viewDir = normalize(FragPos - viewPos);
				float spec_koef = pow(max(dot(viewDir, reflectDir), 0.0f), material.shiniess);

				vec3 specular = light[i].specular * spec_koef * material.specular; // vec3(0.1f, 0.1f, 0.1f);
				lresult = 1.5*ambient + diffuse + specular - shadow;
			}
			else if (light[i].type == 2) // Point Light
			{

				vec3 ambient = light[i].ambient * material.ambient * getAtten(i);

				//diffuse
				vec3 norm = normalize(vertNormal);
				vec3 lightDir = -normalize(FragPos - light[i].position);
				float diff_koef = max(dot(norm, -lightDir), 0.0f);
				vec3 diffuse = light[i].diffuse * (diff_koef * material.diffuse) * getAtten(i);

				//specular
				vec3 reflectDir = reflect(lightDir, norm);
				vec3 viewDir = normalize(FragPos - viewPos);
				float spec_koef = pow(max(dot(viewDir, reflectDir), 0.0f), material.shiniess);

				vec3 specular = light[i].specular * (spec_koef * material.specular) * getAtten(i);
				lresult = ambient + diffuse + specular;
			}
			else if (light[i].type == 3) //Spotlight
			{
				vec3 lightDir = -normalize(FragPos - light[i].position);
				float angle = dot(lightDir, normalize(-light[i].direction));

				if(acos(angle) < light[i].cutOff)
				{

					vec3 ambient = light[i].ambient * material.ambient * getAtten(i);

					//diffuse
					vec3 norm = normalize(vertNormal);
					float diff_koef = max(dot(norm, -lightDir), 0.0f);
					vec3 diffuse = light[i].diffuse * (diff_koef * material.diffuse) * getAtten(i);

					//specular
					vec3 reflectDir = reflect(lightDir, norm);
					vec3 viewDir = normalize(FragPos - viewPos);
					float spec_koef = pow(max(dot(viewDir, reflectDir), 0.0f), material.shiniess);
					vec3 specular = light[i].specular * (spec_koef * material.specular) * getAtten(i);

					diffuse *= spec_koef;

					lresult = ambient + diffuse + specular;
				}
				else
				{
					lresult = material.ambient * light[i].ambient;
				}
			}
			outColor += texture(ourTexture, texCoords) * vec4(lresult, 1.0f);
		}// end of for
	}
}