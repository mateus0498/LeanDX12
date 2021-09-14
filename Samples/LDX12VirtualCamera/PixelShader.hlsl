// Autor: Mateus Ferreira da Silva
// Ano  : 2021
// Descrição: LeanDX12 Virtual Camera Sample

struct PSIn
{
	float4 position : SV_POSITION;
	//float4 color : INSTANCE_COLOR;
};

float4 main(PSIn input) : SV_TARGET0
{
	//return input.color;
	return float4(0.0f, 1.0f, 0.0f, 1.0f);
}