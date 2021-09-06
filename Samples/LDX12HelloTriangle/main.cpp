// Autor: Mateus Ferreira da Silva
// Ano  : 2021
// Descrição: LeanDX12 Hello Triangle Sample

#include <iostream>
#include "LeanDX12.h"

#define WIDTH 1024
#define HEIGHT 720
#define RENDER_TARGET_FORMAT RESOURCE_FORMAT_R8G8B8A8_UNORM

void NormalizeRGBA(unsigned char R, unsigned char G, unsigned char B, unsigned char A, float color[4]);
void PrintAdapterDesc(ADAPTER_DESC* adapterDesc);

int main()
{
	DisplayAdapter* adapter;
	ADAPTER_DESC adapterDesc;

	GetHighestPerformanceAdapter("11.0", &adapter);
	GetAdapterDesc(adapter, &adapterDesc);
	PrintAdapterDesc(&adapterDesc);

	CreateDevice(adapter, "11.0");

	Texture* renderTarget;
	float clearColor[4];
	NormalizeRGBA(243, 26, 149, 255, clearColor);
	CreateRenderTarget(WIDTH, HEIGHT, RENDER_TARGET_FORMAT, clearColor, 1, 0, &renderTarget);


	Buffer* readbackBuffer;
	unsigned long long bufferSize;
	GetPrivateData(renderTarget, 0, &bufferSize, NULL);
	CreateBuffer(bufferSize, BUFFER_TYPE_READBACK, &readbackBuffer, 0);


	BlendState* blendState;
	InitBlendState(&blendState);


	RasterizerState* rasterState;
	RASTERIZER_DESC rasterDesc;
	memset(&rasterDesc, 0, sizeof(RASTERIZER_DESC));
	rasterDesc.FillMode = FILL_MODE_SOLID;
	rasterDesc.CullMode = CULL_MODE_NONE;
	rasterDesc.SampleCount = 1;
	InitRasterizerState(&rasterState);
	SetRasterizerState(rasterDesc, rasterState);


	DepthStencilState* depthStencilState = nullptr;
	InitDepthStencilState(&depthStencilState);


	InputLayout* inputLayout = nullptr;
	INPUT_ELEMENT_DESC vertexDesc[1];
	memset(vertexDesc, 0, sizeof(INPUT_ELEMENT_DESC));
	vertexDesc[0].Format = RESOURCE_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].SemanticName = "VERTEX_POSITION";

	INPUT_ELEMENT_DESC instanceDesc[2];
	memset(instanceDesc, 0, 2 * sizeof(INPUT_ELEMENT_DESC));
	instanceDesc[0].Format = RESOURCE_FORMAT_R32_FLOAT;
	instanceDesc[0].SemanticName = "INSTANCE_OFFSET";
	instanceDesc[0].InstanceDataStepRate = 1;

	instanceDesc[1].Format = RESOURCE_FORMAT_R32G32B32A32_FLOAT;
	instanceDesc[1].SemanticName = "INSTANCE_COLOR";
	instanceDesc[1].InstanceDataStepRate = 1;

	CreateInputLayout(_countof(vertexDesc), vertexDesc, _countof(instanceDesc), instanceDesc, &inputLayout);


	RootSignature* rootSignature;
	CreateRootSignature(0, NULL, 0, 0, &rootSignature);


	ShaderBinary *vertexShader, *pixelShader;
	vertexShader = nullptr;
	LoadShaderFromFile("VertexShader.cso", &vertexShader);
	LoadShaderFromFile("PixelShader.cso", &pixelShader);


	GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc;
	memset(&pipelineStateDesc, 0, sizeof(GRAPHICS_PIPELINE_STATE_DESC));
	pipelineStateDesc.rootSignature = rootSignature;
	pipelineStateDesc.inputLayout = inputLayout;
	pipelineStateDesc.vertexShader = vertexShader;
	pipelineStateDesc.pixelShader = pixelShader;
	pipelineStateDesc.primitiveTopologyType = PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateDesc.rasterState = rasterState;
	pipelineStateDesc.blendState = blendState;
	pipelineStateDesc.depthStencilState = depthStencilState;
	pipelineStateDesc.renderTargetFormat = RENDER_TARGET_FORMAT;

	PipelineState* pipelineState;
	CreateGraphicsPipelineState(pipelineStateDesc, &pipelineState);


	// Lista de Vértices (triângulo)
	float vertices[] =
	{
		-0.7f, -0.7f, 0.5f, // vértice inferior esquerdo
		 0.0f,  0.7f, 0.5f, // vértice superior 
		 0.7f, -0.7f, 0.5f  // vértice inferior direito
	};
	
	// Lista de Índices
	unsigned int indices[] =
	{
		0, 1, 2
	};

	float instanceData[] =
	{
		 0.2f, 0.0f, 1.0f, 0.0f, 1.0f,
		-0.2f, 0.0f, 0.0f, 1.0f, 1.0f
	};

	VIEWPORT viewport[1];
	viewport[0].Left = 0;
	viewport[0].Top = 0;
	viewport[0].Right = WIDTH;
	viewport[0].Bottom = HEIGHT;

	SCISSOR_RECT scissorRect[1];
	memcpy(scissorRect, viewport, sizeof(SCISSOR_RECT));

	BeginScene(pipelineState);

	SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	SetVertexData(vertices, sizeof(vertices), 12);
	SetInstanceData(instanceData, sizeof(instanceData), 20, 1);
	SetIndexData(indices, sizeof(indices));

	SetScissorRects(_countof(scissorRect), scissorRect);
	SetViewports(_countof(viewport), viewport);

	SetRenderTarget(renderTarget);
	Clear(0, NULL, CLEAR_TARGET, clearColor, 1.0f, 0);

	DrawIndexedInstanced(3, 2, 0, 0, 0);

	EndScene();

	RenderFrame();


	unsigned int texelSize = TexelSize(RENDER_TARGET_FORMAT);
	unsigned int renderTargetDataSize = texelSize * WIDTH * HEIGHT;
	unsigned char* renderTargetData = (unsigned char*)malloc(texelSize * WIDTH * HEIGHT);
	//unsigned char* renderTargetData = new unsigned char[texelSize * WIDTH * HEIGHT];

	GetPrivateData(renderTarget, 0, NULL, readbackBuffer);
	ReadbackData(readbackBuffer, texelSize, WIDTH, HEIGHT, 1, renderTargetData);

	SaveAsPNG("demo.png", renderTargetData, renderTargetDataSize, WIDTH, HEIGHT);

	free(renderTargetData);
	//delete[] renderTargetData;
	DeleteRenderTarget(renderTarget);
	DeleteBuffer(readbackBuffer);
	ReleaseAdapter(adapter);

	return 0;
}

void NormalizeRGBA(unsigned char R, unsigned char G, unsigned char B, unsigned char A, float color[4])
{
	color[0] = (float)R / 255;
	color[1] = (float)G / 255;
	color[2] = (float)B / 255;
	color[3] = (float)A / 255;
}

void PrintAdapterDesc(ADAPTER_DESC* adapterDesc)
{
	std::cout << "Adaptador de video selecionado:" << std::endl;
	std::cout << "- Descricao: " << adapterDesc->Description << std::endl;
	std::cout << "- Memoria dedicada (VRAM): " << (float)adapterDesc->DedicatedVideoMemory / (1024 * 1024 * 1024) << " GB" << std::endl;
	std::cout << "- Memoria compartilhada (RAM): " << (float)adapterDesc->SharedSystemMemory / (1024 * 1024 * 1024) << " GB" << std::endl;
}