// Autor: Mateus Ferreira da Silva
// Ano  : 2021
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
	NormalizeRGBA(20, 102, 179, 255, clearColor);
	CreateRenderTarget(WIDTH, HEIGHT, RENDER_TARGET_FORMAT, clearColor, 1, 0, &renderTarget);


	Buffer* readbackBuffer;
	unsigned long long bufferSize;
	GetPrivateData(renderTarget, 0, &bufferSize, NULL);
	CreateBuffer(bufferSize, BUFFER_TYPE_READBACK, &readbackBuffer, 0);


	BeginScene(NULL);

	SetRenderTarget(renderTarget);

	Clear(0, NULL, CLEAR_TARGET, clearColor, 1.0f, 0);

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