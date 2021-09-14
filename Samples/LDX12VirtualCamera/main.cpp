// Autor: Mateus Ferreira da Silva
// Ano  : 2021
// Descrição: LeanDX12 Virtual Camera Sample

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <DirectXMath.h>
#include "LeanDX12.h"

#define WIDTH 1920
#define HEIGHT 1080
#define RENDER_TARGET_FORMAT RESOURCE_FORMAT_R8G8B8A8_UNORM

using namespace DirectX;

XMMATRIX cameraPosition;
XMMATRIX cameraRotation;
XMMATRIX projection;

Buffer** WVPBuffers;
Buffer* uploadWVPBuffer;
unsigned int numWVPBuffers;

// Matrizes utilizadas pelo pipeline gráfico (checar VertexShader.hlsl) para transformar as coordenadas dos objetos do espaço do mundo
// para o espaço projetivo.
struct WorldViewProjection
{
	XMMATRIX scaling;
	XMMATRIX rotation;
	XMMATRIX translation;
	XMMATRIX cameraPosition;
	XMMATRIX cameraRotation;
	XMMATRIX projection;
};

// Função para definir a posição e a orientação da câmera no espaço do mundo.
// Transformação do espaço do mundo no espaço da câmera virtual utilizando projeção perspectiva.
void SetCamera(
	float position[3], float rotation[3],
	float fovAngleY, float aspectRatio, float nearClippingPlane, float farClippingPlane)
{
	cameraPosition = XMMatrixTranslation(-position[0], -position[1], -position[2]);
	cameraRotation = XMMatrixRotationRollPitchYaw(-rotation[0] * XM_PI / 180.0f, -rotation[1] * XM_PI / 180.0f, -rotation[2] * XM_PI / 180.0f);
	projection = XMMatrixPerspectiveFovLH(fovAngleY * XM_PI / 180.0f, aspectRatio, nearClippingPlane, farClippingPlane);
}

// Função para definir a posição e a orientação de um objeto no espaço do mundo.
// É possível definir também aumentar ou diminuir a escala do objeto em cada um dos eixos coordenados.
void SetObject(float position[3], float rotation[3], float scale[3], unsigned int wvpConstantBufferIndex)
{
	WorldViewProjection wvp;

	wvp.scaling = XMMatrixScaling(scale[0], scale[1], scale[2]);
	wvp.rotation = XMMatrixRotationRollPitchYaw(rotation[0] * XM_PI / 180.0f, rotation[1] * XM_PI / 180.0f, rotation[2] * XM_PI / 180.0f);
	wvp.translation = XMMatrixTranslation(position[0], position[1], position[2]);
	wvp.cameraPosition = cameraPosition;
	wvp.cameraRotation = cameraRotation;
	wvp.projection = projection;

	if (wvpConstantBufferIndex >= numWVPBuffers)
	{
		Buffer** tempWVPBuffers = new Buffer * [wvpConstantBufferIndex + 1];
		memcpy(tempWVPBuffers, WVPBuffers, numWVPBuffers * sizeof(Buffer*));
		delete[] WVPBuffers;

		for (unsigned int i = numWVPBuffers; i < wvpConstantBufferIndex + 1; i++)
			CreateBuffer(sizeof(WorldViewProjection), BUFFER_TYPE_DEFAULT, &tempWVPBuffers[i], i);

		numWVPBuffers = wvpConstantBufferIndex + 1;
		WVPBuffers = tempWVPBuffers;
	}

	UploadData(uploadWVPBuffer, NULL, 1, sizeof(WorldViewProjection), 1, 1, &wvp);
	SetPrivateData(WVPBuffers[wvpConstantBufferIndex], uploadWVPBuffer);
}

void NormalizeRGBA(unsigned char R, unsigned char G, unsigned char B, unsigned char A, float color[4]);
void PrintAdapterDesc(ADAPTER_DESC* adapterDesc);

int main()
{
	numWVPBuffers = 0;

	float* vertices;
	unsigned int numVertices, numVertexCoordinates;

	unsigned int* vertexIndices;
	unsigned int numFaces, numVerticesPerFace = 3;

	LoadWavefrontOBJ(
		"deagle.obj",
		NULL, &numVertices, &numVertexCoordinates,
		NULL, NULL, NULL,
		NULL, NULL, NULL,
		NULL, NULL, NULL, &numFaces);

	vertices = new float[numVertices * numVertexCoordinates];
	vertexIndices = new unsigned int[numFaces * numVerticesPerFace];

	LoadWavefrontOBJ(
		"deagle.obj",
		vertices, NULL, NULL,
		NULL, NULL, NULL,
		NULL, NULL, NULL,
		vertexIndices, NULL, NULL, NULL);

	DisplayAdapter* adapter;
	ADAPTER_DESC adapterDesc;

	GetHighestPerformanceAdapter("11.0", &adapter);
	GetAdapterDesc(adapter, &adapterDesc);
	PrintAdapterDesc(&adapterDesc);

	CreateDevice(adapter, "11.0");


	Texture* renderTarget;
	float clearColor[4];
	NormalizeRGBA(0, 0, 0, 255, clearColor);
	CreateRenderTarget(WIDTH, HEIGHT, RENDER_TARGET_FORMAT, clearColor, 1, 0, &renderTarget);


	Buffer* readbackBuffer;
	unsigned long long bufferSize;
	GetPrivateData(renderTarget, 0, &bufferSize, NULL);
	CreateBuffer(bufferSize, BUFFER_TYPE_READBACK, &readbackBuffer, 0);


	CreateBuffer(sizeof(WorldViewProjection), BUFFER_TYPE_UPLOAD, &uploadWVPBuffer);


	BlendState* blendState;
	InitBlendState(&blendState);


	RasterizerState* rasterState;
	RASTERIZER_DESC rasterDesc;
	memset(&rasterDesc, 0, sizeof(RASTERIZER_DESC));
	rasterDesc.FillMode = FILL_MODE_WIREFRAME;
	rasterDesc.CullMode = CULL_MODE_FRONT;
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.SampleCount = 1;
	rasterDesc.DepthClipEnable = false;
	InitRasterizerState(&rasterState);
	SetRasterizerState(rasterDesc, rasterState);


	DepthStencilState* depthStencilState = nullptr;
	InitDepthStencilState(&depthStencilState);


	InputLayout* inputLayout = nullptr;
	INPUT_ELEMENT_DESC vertexDesc[1];
	memset(vertexDesc, 0, sizeof(INPUT_ELEMENT_DESC));
	vertexDesc[0].Format = RESOURCE_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].SemanticName = "VERTEX_POSITION";

	CreateInputLayout(_countof(vertexDesc), vertexDesc, 0, NULL, &inputLayout);


	RootSignature* rootSignature;
	CreateRootSignature(0, NULL, 1, 0, &rootSignature);


	ShaderBinary* vertexShader, * pixelShader;
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
	LeanDX12Result result = CreateGraphicsPipelineState(pipelineStateDesc, &pipelineState);


	VIEWPORT viewport[1];
	viewport[0].Left = 0;
	viewport[0].Top = 0;
	viewport[0].Right = WIDTH;
	viewport[0].Bottom = HEIGHT;

	SCISSOR_RECT scissorRect[1];
	memcpy(scissorRect, viewport, sizeof(SCISSOR_RECT));


	float axis[] =
	{
		0.0f, 0.0f, 0.0f,
		1000.0f, 0.0f, 0.0f,
		0.0f, 1000.0f, 0.0f,
		0.0f, 0.0f, -1000.0f,
		0.0f, 0.0f, 1000.0f
	};

	unsigned int indices[] =
	{
		0, 1, 0, 2, 3, 4
	};


	SetCamera(new float[] {11.0f, 4.0f, -20.0f}, new float[] { 20.0f, -15.0f, 0.0f }, 70.0f, (float)WIDTH / HEIGHT, 1.0f, 2000.0f);

	SetObject(new float[] {-2.0f, 0.0f, -4.0f}, new float[] {180.0f, 90.0f, 0.0f}, new float[] {1.5f, 1.5f, 1.5f}, 1);
	SetObject(new float[] {0.0f, 0.0f, 0.0f}, new float[] {0.0f, 0.0f, 0.0f}, new float[] {1.0f, 1.0f, 1.0f}, 2);



	BeginScene(pipelineState);

	SetScissorRects(_countof(scissorRect), scissorRect);
	SetViewports(_countof(viewport), viewport);

	SetRenderTarget(renderTarget);
	Clear(0, NULL, CLEAR_TARGET, clearColor, 1.0f, 0);


	MapDescriptorTableOffsetToBaseRegister(1);
	SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	SetVertexData(vertices, numVertices * numVertexCoordinates * sizeof(float), numVertexCoordinates * sizeof(float));
	SetIndexData(vertexIndices, numFaces * numVerticesPerFace * sizeof(int));

	DrawIndexedInstanced(numFaces * numVerticesPerFace, 1, 0, 0, 0);
	//DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0);


	EndScene();

	RenderFrame();


	BeginScene(pipelineState);

	SetScissorRects(_countof(scissorRect), scissorRect);
	SetViewports(_countof(viewport), viewport);

	SetRenderTarget(renderTarget);

	MapDescriptorTableOffsetToBaseRegister(2);
	SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_LINELIST);
	SetVertexData(axis, sizeof(axis), 3 * sizeof(float));
	SetIndexData(indices, sizeof(indices));
	DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0);

	EndScene();

	RenderFrame();


	unsigned int texelSize = TexelSize(RENDER_TARGET_FORMAT);
	unsigned int renderTargetDataSize = texelSize * WIDTH * HEIGHT;
	unsigned char* renderTargetData = (unsigned char*)malloc(renderTargetDataSize);
	//unsigned char* renderTargetData = new unsigned char[texelSize * WIDTH * HEIGHT];
	GetPrivateData(renderTarget, 0, NULL, readbackBuffer);
	ReadbackData(readbackBuffer, texelSize, WIDTH, HEIGHT, 1, renderTargetData);


	SaveAsPNG("demo.png", renderTargetData, renderTargetDataSize, WIDTH, HEIGHT);


	free(renderTargetData);
	//delete[] renderTargetData;
	DeleteRenderTarget(renderTarget);
	DeleteBuffer(readbackBuffer);
	DeleteBuffer(uploadWVPBuffer);

	for (unsigned int i = 0; i < numWVPBuffers; i++)
		DeleteBuffer(WVPBuffers[i]);

	ReleaseDevice();
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