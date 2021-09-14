// Autor: Mateus Ferreira da Silva
// Ano  : 2021
// Descrição: LeanDX12 Virtual Camera Sample

struct VSInput
{
	float3 position : VERTEX_POSITION;
	//uint instanceid : SV_INSTANCEID;
	//float offset : INSTANCE_OFFSET;
	//float4 color : INSTANCE_COLOR;
};

struct VSOut
{
	float4 position : SV_POSITION;
	//float4 color : INSTANCE_COLOR;
};

// row-major x column-major
cbuffer teste
{
	column_major float4x4 scaling;
	//column_major float4x4 pitch;
	//column_major float4x4 yaw;
	//column_major float4x4 roll;
	column_major float4x4 rotation;
	column_major float4x4 translation;
	column_major float4x4 cameraPosition;
	column_major float4x4 cameraRotation;
	column_major float4x4 projection;
};

VSOut main(VSInput input)
{
	VSOut output;
	
	//input.position.y += translation._m31;
	//output.position = 
	output.position = float4(input.position, 1.0f);
	
	output.position = mul(scaling, output.position);
	//output.position = mul(pitch, output.position);
	//output.position = mul(roll, output.position);
	//output.position = mul(yaw, output.position);
	output.position = mul(rotation, output.position);
	output.position = mul(translation, output.position);
	output.position = mul(cameraPosition, output.position);
	output.position = mul(cameraRotation, output.position);
	output.position = mul(projection, output.position);
	//output.color = input.color;
	
	return output;
}