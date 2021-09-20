// Autor: Mateus Ferreira da Silva
// Ano  : 2021
// Descrição: LeanDX12 Phong Illumination Model Sample

struct VSInput
{
	float3 position : POSITION;
	float3 normal : NORMAL;
};

struct VSOut
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float3 vertexPosition : POSITION;
	float3 cameraPosition : CAMERA_POSITION;
};

// row-major x column-major
cbuffer teste : register(b1)
{
	column_major float4x4 scaling;
	column_major float4x4 rotation;
	column_major float4x4 translation;
	column_major float4x4 cameraPosition;
	column_major float4x4 cameraRoll;
	column_major float4x4 cameraYaw;
	column_major float4x4 cameraPitch;
	column_major float4x4 projection;
};

VSOut main(VSInput input)
{
	VSOut output;
	
	output.position = float4(input.position, 1.0f);
	
	output.position = mul(scaling, output.position);
	output.position = mul(rotation, output.position);
	output.position = mul(translation, output.position);
	output.vertexPosition = output.position.xyz;
	output.position = mul(cameraPosition, output.position);
	output.position = mul(cameraRoll, output.position);
	output.position = mul(cameraYaw, output.position);
	output.position = mul(cameraPitch, output.position);
	

	output.position = mul(projection, output.position);
	output.cameraPosition = -mul(cameraPosition, float4(0.0f, 0.0f, 0.0f, 1.0f)).xyz;


	float4 modelSpaceOrigin = float4(0.0f, 0.0f, 0.0f, 1.0f);
	modelSpaceOrigin = mul(translation, modelSpaceOrigin);

	float4 modelSpaceNormal = float4(input.normal, 1.0f);
	modelSpaceNormal = mul(rotation, modelSpaceNormal);
	modelSpaceNormal = mul(translation, modelSpaceNormal);

	output.normal = modelSpaceNormal.xyz - modelSpaceOrigin.xyz;
	
	return output;
}