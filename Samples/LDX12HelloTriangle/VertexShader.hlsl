// Autor: Mateus Ferreira da Silva
// Ano  : 2021
// Descrição: LeanDX12 Hello Triangle Sample

struct VSInput
{
	float3 position : VERTEX_POSITION;
	uint instanceid : SV_INSTANCEID;
	float offset : INSTANCE_OFFSET;
	float4 color : INSTANCE_COLOR;
};

struct VSOut
{
	float4 position : SV_POSITION;
	float4 color : INSTANCE_COLOR;
};

VSOut main(VSInput input)
{
	VSOut output;

	if (input.instanceid == 1)
	{
		// A instância 1 é o triângulo original verticalmente invertido.
		input.position.y = -input.position.y;
	}

	input.position.y += input.offset;
	
	output.position = float4(input.position, 1.0f);
	output.color = input.color;
	
	return output;
}