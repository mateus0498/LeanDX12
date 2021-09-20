// Autor: Mateus Ferreira da Silva
// Ano  : 2021
// Descrição: LeanDX12 Phong Illumination Model Sample

#define LIGHT_POINT 1
#define LIGHT_SPOT 2
#define LIGHT_DIRECTIONAL 3

static const float PI = 3.141592654f;

cbuffer Light : register(b0)
{
	uint numLights;
}

struct PSIn
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float3 vertexPosition : POSITION;
	float3 cameraPosition : CAMERA_POSITION;
};

struct LightSource
{
	uint type;
	float3 diffuse;
	float3 specular;
	float3 ambient;
	float3 position;
	float pitch;
	float yaw;
	float range;
	float falloff;
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
	float theta;
	float phi;
};

StructuredBuffer<LightSource> lights : register(t0);

float CalculateAttenuation(LightSource light, float3 vertexPosition)
{
	float attenuation = 1.0f, d;

	if (light.type != LIGHT_DIRECTIONAL)
	{
		d = distance(light.position, vertexPosition);

		if (d > light.range)
			return 0.0f;

		attenuation = light.constantAttenuation + light.linearAttenuation * d + light.quadraticAttenuation * d * d;
		attenuation = (attenuation >= 1.0f) ? attenuation : 1.0f;
	}

	return 1 / attenuation;
}

float CalculateSpotlightFactor(LightSource light, float3 vertexPosition)
{
	if (light.type != LIGHT_SPOT)
		return 1.0f;

	float degreesToRad = PI / 180;

	float3 lightDirection = float3(cos(degreesToRad * light.pitch) * sin(degreesToRad * light.yaw), -sin(degreesToRad * light.pitch), cos(degreesToRad * light.pitch) * cos(degreesToRad * light.yaw));
	float3 lightVertexVector = vertexPosition - light.position;
	float alphaCosine = dot(lightDirection, lightVertexVector) / (length(lightDirection) * length(lightVertexVector));

	float halfThetaCosine = cos(degreesToRad * (light.theta / 2));
	float halfPhiCosine = cos(degreesToRad * (light.phi / 2));

	if (alphaCosine > halfThetaCosine)
		return 1.0f;

	else if (alphaCosine < halfPhiCosine)
		return 0.0f;

	float spotlightFactor = (alphaCosine - halfPhiCosine) / (halfThetaCosine - halfPhiCosine);
	spotlightFactor = pow(spotlightFactor, light.falloff);

	return spotlightFactor;
}

float4 CalculateDiffuse(LightSource light, float3 vertexPosition, float3 vertexNormal)
{
	float diffuse, attenuation, spotlightFactor;
	float3 vertexLightVector;

	float degreesToRad = PI / 180;
	float3 lightDirection = float3(cos(degreesToRad * light.pitch) * sin(degreesToRad * light.yaw), -sin(degreesToRad * light.pitch), cos(degreesToRad * light.pitch) * cos(degreesToRad * light.yaw));

	switch (lights[1].type)
	{
	case LIGHT_DIRECTIONAL:
		vertexLightVector = -lightDirection;
		break;
	default:
		vertexLightVector = light.position - vertexPosition;
		break;
	}

	attenuation = CalculateAttenuation(light, vertexPosition);
	spotlightFactor = CalculateSpotlightFactor(light, vertexPosition);

	diffuse = dot(vertexNormal, vertexLightVector) / (length(vertexNormal) * length(vertexLightVector));
	diffuse = saturate(diffuse);
	diffuse = attenuation * spotlightFactor * diffuse;

	return float4(diffuse * light.diffuse, 1.0f);
}

float4 CalculateSpecular(LightSource light, float3 vertexPosition, float3 vertexNormal, float3 cameraPosition, float specularPower)
{
	float specular, attenuation, spotlightFactor;;
	float3 vertexLightVector, vertexCameraVector, halfwayVector;

	float degreesToRad = PI / 180;
	float3 lightDirection = float3(cos(degreesToRad * light.pitch) * sin(degreesToRad * light.yaw), -sin(degreesToRad * light.pitch), cos(degreesToRad * light.pitch) * cos(degreesToRad * light.yaw));

	switch (lights[1].type)
	{
	case LIGHT_DIRECTIONAL:
		vertexLightVector = -lightDirection;
		break;
	default:
		vertexLightVector = light.position - vertexPosition;
		break;
	}

	attenuation = CalculateAttenuation(light, vertexPosition);
	spotlightFactor = CalculateSpotlightFactor(light, vertexPosition);

	vertexCameraVector = cameraPosition - vertexPosition;
	halfwayVector = normalize(vertexCameraVector) + normalize(vertexLightVector);
	halfwayVector = normalize(halfwayVector);
	specular = dot(vertexNormal, halfwayVector) / length(vertexNormal);
	specular = saturate(specular);

	specular = pow(specular, specularPower);
	specular = attenuation * spotlightFactor * specular;

	return float4(specular * light.specular, 1.0f);
}

float4 main(PSIn input) : SV_TARGET0
{
	if (numLights == 0)
		return float4(1.0f, 1.0f, 1.0f, 1.0f);

	else if (numLights == 15)
		return float4(input.normal, 1.0f);

	float4 diffuseColor, specularColor;
	diffuseColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	specularColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	for (uint i = 0; i < numLights; i++)
	{
		diffuseColor = diffuseColor + CalculateDiffuse(lights[i], input.vertexPosition, input.normal);
		specularColor = specularColor + CalculateSpecular(lights[i], input.vertexPosition, input.normal, input.cameraPosition, 250.0f);
	}

	return saturate(diffuseColor + specularColor);
}