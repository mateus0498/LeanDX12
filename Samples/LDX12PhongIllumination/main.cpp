// Autor: Mateus Ferreira da Silva
// Ano  : 2021
// Descrição: LeanDX12 Phong Illumination Model Sample

#include <iostream>
#include <DirectXMath.h>
#include "LeanDX12.h"

#define WIDTH 1920
#define HEIGHT 1080
#define RENDER_TARGET_FORMAT RESOURCE_FORMAT_R8G8B8A8_UNORM
#define NUM_LIGHTS 2

#define NUM_32BIT_CONSTANTS 1
#define NUM_CONSTANT_BUFFERS 1
#define NUM_SHADER_RESOURCES 1

using namespace DirectX;

Buffer* uploadWVPBuffer;

void* vertexData;
unsigned int vertexDataSize;
unsigned int* indexData;
unsigned int indexDataSize;

XMMATRIX cameraPosition;
XMMATRIX cameraRoll;
XMMATRIX cameraYaw;
XMMATRIX cameraPitch;
XMMATRIX projection;

// Matrizes utilizadas pelo pipeline gráfico (checar VertexShader.hlsl) para transformar as coordenadas dos objetos do espaço do mundo
// para o espaço projetivo.
typedef struct WorldViewProjection
{
	XMMATRIX scaling;
	XMMATRIX rotation;
	XMMATRIX translation;
	XMMATRIX cameraPosition;
	XMMATRIX cameraRoll;
	XMMATRIX cameraYaw;
	XMMATRIX cameraPitch;
	XMMATRIX projection;
} WorldViewProjection;

typedef struct POSITION
{
	float x;
	float y;
	float z;
} POSITION;

typedef struct ROTATION
{
	float pitch;
	float yaw;
	float roll;
} ROTATION;

// Estrutura para descrever um modelo/objeto.
typedef struct Model
{
	float* vertices;
	unsigned int numVertices;
	unsigned int numCoordinatesPerVertex;
	unsigned int* vertexIndices;
	unsigned int numVertexIndices;
	PRIMITIVE_TOPOLOGY primitiveTopology;
	Buffer** worldViewProjection;
	unsigned int* instanceIndices;
	unsigned int numInstanceIndices;
} Model;

// Estrutura para descrever as propriedades de uma fonte luminosa. Usada no Pixel Shader (conferir PixelShader.hlsl)
typedef struct LightSource
{
	unsigned int type;
	float diffuse[3];
	float specular[3];
	float ambient[3];
	float position[3];
	float pitch;
	float yaw;
	float range;
	float falloff;
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
	float theta;
	float phi;
} LightSource;


// Variáveis globais:
// •	Lista de ponteiros de todos os modelos existentes na aplicação.
Model** models;
unsigned int numModels;

// •	Lista de ponteiros dos modelos a serem renderizados.
Model** modelsToRender;
unsigned int numModelsToRender;


// Declaração de funções
void NormalizeRGBA(unsigned char R, unsigned char G, unsigned char B, unsigned char A, float color[4]);
void PrintAdapterDesc(ADAPTER_DESC* adapterDesc);
void LoadObjFile(const char* filename, float* vertexData, unsigned int* numVertices, unsigned int* numFloatsPerVertex, unsigned int* indices, unsigned int* numIndices);
void SetCamera(
	POSITION position, ROTATION rotation,
	float aspectRatio, float nearClippingPlane, float farClippingPlane, bool isPerspectiveProjection = false, float fovAngleY = 0.0f);
void CreateModel(
	float* vertices, unsigned int numVertices, unsigned int numCoordinatesPerVertex,
	unsigned int* indices, unsigned int numVertexIndices,
	PRIMITIVE_TOPOLOGY primitiveTopology,
	unsigned int instanceCount,
	Model* model);
void DestroyModel(Model* model);
void ClearListOfModelsToRender();
void SetModel(POSITION position, ROTATION rotation, float scaleFactors[3], unsigned int instanceIndex, Model* model);
void DrawModels();


int main()
{
	numModels = 0;

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
	CreateBuffer(bufferSize, BUFFER_TYPE_READBACK, &readbackBuffer);

	CreateBuffer(sizeof(WorldViewProjection), BUFFER_TYPE_UPLOAD, &uploadWVPBuffer);

	Buffer* lightBuffer;
	Buffer* uploadLightBuffer;

	CreateBuffer(NUM_LIGHTS * sizeof(LightSource), BUFFER_TYPE_DEFAULT, &lightBuffer, 5, RESOURCE_FORMAT_UNKNOWN, NUM_LIGHTS, sizeof(LightSource));
	CreateBuffer(NUM_LIGHTS * sizeof(LightSource), BUFFER_TYPE_UPLOAD, &uploadLightBuffer);
	
	LightSource light[NUM_LIGHTS];
	memset(light, 0, _countof(light) * sizeof(LightSource));
	light[0].type = 1;
	memcpy(light[0].diffuse, new float[] { 1.0f, 1.0f, 1.0f }, sizeof(light[0].diffuse));
	memcpy(light[0].specular, light[0].diffuse, 3 * sizeof(float));
	memcpy(light[0].position, new float[] { 0.0f, 8.5f, 7.0f }, sizeof(light[0].position));
	light[0].range = 100.0f;
	light[0].constantAttenuation = 1.0f;
	light[0].linearAttenuation = 0.0f;
	light[0].quadraticAttenuation = 0.0f;

	light[1].type = 1;
	memcpy(light[1].diffuse, new float[] { 1.0f, 1.0f, 1.0f }, sizeof(light[1].diffuse));
	memcpy(light[1].specular, light[1].diffuse, 3 * sizeof(float));
	memcpy(light[1].position, new float[] { 3.0f, 0.0f, -5.0f }, sizeof(light[1].position));
	light[1].range = 100.0f;
	light[1].constantAttenuation = 0.0f;
	light[1].linearAttenuation = 0.25f;
	light[1].quadraticAttenuation = 0.0f;
	UploadData(uploadLightBuffer, NULL, 1, _countof(light) * sizeof(LightSource), 1, 1, light);
	SetPrivateData(lightBuffer, uploadLightBuffer);

	BlendState* blendState;
	InitBlendState(&blendState);

	RasterizerState* rasterState[2];
	RASTERIZER_DESC rasterDesc;
	memset(&rasterDesc, 0, sizeof(RASTERIZER_DESC));
	rasterDesc.FillMode = FILL_MODE_SOLID;
	rasterDesc.CullMode = CULL_MODE_FRONT;
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.SampleCount = 1;
	rasterDesc.DepthClipEnable = true;
	InitRasterizerState(&rasterState[0]);
	InitRasterizerState(&rasterState[1]);
	SetRasterizerState(rasterDesc, rasterState[0]);
	rasterDesc.FillMode = FILL_MODE_WIREFRAME;
	SetRasterizerState(rasterDesc, rasterState[1]);


	DepthStencilState* depthStencilState = nullptr;
	InitDepthStencilState(&depthStencilState);


	InputLayout* inputLayout = nullptr;
	INPUT_ELEMENT_DESC vertexDesc[2];
	memset(vertexDesc, 0, _countof(vertexDesc) * sizeof(INPUT_ELEMENT_DESC));
	vertexDesc[0].Format = RESOURCE_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].SemanticName = "POSITION";

	vertexDesc[1].Format = RESOURCE_FORMAT_R32G32B32_FLOAT;
	vertexDesc[1].SemanticName = "NORMAL";

	CreateInputLayout(_countof(vertexDesc), vertexDesc, 0, NULL, &inputLayout);


	RootSignature* rootSignature;
	unsigned int num32bitValues[] = { 1 };
	CreateRootSignature(NUM_32BIT_CONSTANTS, num32bitValues, NUM_CONSTANT_BUFFERS, NUM_SHADER_RESOURCES, &rootSignature);


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
	pipelineStateDesc.rasterState = rasterState[0];
	pipelineStateDesc.blendState = blendState;
	pipelineStateDesc.depthStencilState = depthStencilState;
	pipelineStateDesc.renderTargetFormat = RENDER_TARGET_FORMAT;

	PipelineState* pipelineState[2];
	CreateGraphicsPipelineState(pipelineStateDesc, &pipelineState[0]);
	pipelineStateDesc.rasterState = rasterState[1];
	CreateGraphicsPipelineState(pipelineStateDesc, &pipelineState[1]);

	VIEWPORT viewport[1];
	viewport[0].Left = 0;
	viewport[0].Top = 0;
	viewport[0].Right = WIDTH;
	viewport[0].Bottom = HEIGHT;

	SCISSOR_RECT scissorRect[1];
	memcpy(scissorRect, viewport, sizeof(SCISSOR_RECT));
	
	float axisData[] =
	{
		0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		100.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 100.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 100.0f, 0.0f, 0.0f, 1.0f,
	};

	unsigned int axisIndices[] =
	{
		0, 1, 2, 3, 4, 5
	};

	Model lightBulb;
	Model deagle;
	Model axis;

	float* vertexData = new float[1000000];
	unsigned int* faceIndices = new unsigned int[1000000];
	unsigned int numVertices, numFloatsPerVertex, numIndices;

	LoadObjFile("lightbulb.obj", vertexData, &numVertices, &numFloatsPerVertex, faceIndices, &numIndices);
	CreateModel(vertexData, numVertices, numFloatsPerVertex, faceIndices, numIndices, PRIMITIVE_TOPOLOGY_TRIANGLELIST, NUM_LIGHTS, &lightBulb);

	LoadObjFile("deagle.obj", vertexData, &numVertices, &numFloatsPerVertex, faceIndices, &numIndices);
	CreateModel(vertexData, numVertices, numFloatsPerVertex, faceIndices, numIndices, PRIMITIVE_TOPOLOGY_TRIANGLELIST, 1, &deagle);

	CreateModel(axisData, 6, 6, axisIndices, _countof(axisIndices), PRIMITIVE_TOPOLOGY_LINELIST, 3, &axis);

	delete[] vertexData;
	delete[] faceIndices;


	//POSITION cameraPos = { 0.0f, 0.0f, -20.0f };
	POSITION cameraPos = { -8.0f, 10.0f, -10.0f };
	ROTATION cameraRot = { 40.0f, 40.0f, 0.0f };
	SetCamera(cameraPos, cameraRot, (float)WIDTH / HEIGHT, 1.0f, 100.0f, true, 80.0f);



	ClearListOfModelsToRender();

	POSITION deaglePos = { -9.0f, 0.0f, 0.0f };
	ROTATION deagleRot = { 0.0f, -90.0f, 180.0f };
	SetModel(deaglePos, deagleRot, NULL, 0, &deagle);
	
	BeginScene(pipelineState[0]);
	Set32bitConstants(0, 1, new unsigned int[] { 2 }, 0);

	SetScissorRects(_countof(scissorRect), scissorRect);
	SetViewports(_countof(viewport), viewport);

	SetRenderTarget(renderTarget);
	Clear(0, NULL, CLEAR_TARGET, clearColor, 1.0f, 0);

	DrawModels();

	EndScene();

	RenderFrame();
	

	
	ClearListOfModelsToRender();

	POSITION lightBulbPos;
	memcpy(&lightBulbPos, light[0].position, sizeof(POSITION));
	ROTATION lightBulbRot = { 0.0f, 0.0f, 0.0f };
	float lightBulbScale[] = { 2.5f, 2.5f, 2.5f };
	SetModel(lightBulbPos, lightBulbRot, lightBulbScale, 0, &lightBulb);

	memcpy(&lightBulbPos, light[1].position, sizeof(POSITION));
	SetModel(lightBulbPos, lightBulbRot, lightBulbScale, 1, &lightBulb);
	//POSITION spherePos = { 0.0f, -12.8f, 0.0f };

	BeginScene(pipelineState[1]);
	Set32bitConstants(0, 1, new unsigned int[] {0}, 0);

	SetScissorRects(_countof(scissorRect), scissorRect);
	SetViewports(_countof(viewport), viewport);

	SetRenderTarget(renderTarget);

	DrawModels();

	EndScene();

	RenderFrame();



	ClearListOfModelsToRender();

	POSITION axisPos;
	ROTATION axisRot = { 0.0f, 0.0f, 0.0f };

	memcpy(&axisPos, light[0].position, sizeof(POSITION));
	SetModel(axisPos, axisRot, NULL, 0, &axis);

	memcpy(&axisPos, light[1].position, sizeof(POSITION));
	SetModel(axisPos, axisRot, NULL, 1, &axis);

	memcpy(&axisPos, new float[] { 0.0f, 0.0f, 5.0f }, sizeof(POSITION));
	SetModel(axisPos, axisRot, NULL, 2, &axis);
	
	BeginScene(pipelineState[0]);
	Set32bitConstants(0, 1, new unsigned int[] {15}, 0);

	SetScissorRects(_countof(scissorRect), scissorRect);
	SetViewports(_countof(viewport), viewport);

	SetRenderTarget(renderTarget);

	DrawModels();

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
	DeleteBuffer(lightBuffer);
	DeleteBuffer(uploadLightBuffer);

	DestroyModel(&lightBulb);
	DestroyModel(&deagle);
	DestroyModel(&axis);

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

void LoadObjFile(const char* filename, float* vertexData, unsigned int* numVertices, unsigned int* numFloatsPerVertex, unsigned int* indices, unsigned int* numIndices)
{
	float* vertices = new float[1000000];
	float* normals = new float[1000000];
	unsigned int* vertexIndices = new unsigned int[2000000];
	unsigned int* normalIndices = new unsigned int[2000000];

	unsigned int numVertexCoordinates, numNormalCoordinates, numFaces;

	LoadWavefrontOBJ(
		filename,
		vertices, NULL, &numVertexCoordinates,
		NULL, NULL, NULL,
		normals, NULL, &numNormalCoordinates,
		vertexIndices, NULL, normalIndices, &numFaces);

	unsigned int* newIndices = NULL, * tempNewIndices;
	unsigned int newIndicesLength = 0;
	unsigned int* domain = NULL, * tempDomain;
	unsigned int domainLength = 0;

	bool exists;

	for (unsigned int i = 0; i < 3 * numFaces; i++)
	{
		exists = false;

		for (unsigned int j = 0; j < 2 * domainLength; j = j + 2)
		{
			if (domain[j] == vertexIndices[i] && domain[j + 1] == normalIndices[i])
			{
				exists = true;
				break;
			}
		}

		if (exists)
			continue;

		newIndicesLength++;
		tempNewIndices = new unsigned int[newIndicesLength];
		memcpy(tempNewIndices, newIndices, (newIndicesLength - 1) * sizeof(int));
		delete[] newIndices;
		newIndices = tempNewIndices;
		newIndices[newIndicesLength - 1] = newIndicesLength - 1;

		domainLength++;
		tempDomain = new unsigned int[2 * domainLength];
		memcpy(tempDomain, domain, 2 * (domainLength - 1) * sizeof(int));
		delete[] domain;
		domain = tempDomain;

		domain[2 * domainLength - 2] = vertexIndices[i];
		domain[2 * domainLength - 1] = normalIndices[i];
	}

	if (numIndices != NULL)
		*numIndices = 3 * numFaces;

	if (indices != NULL)
	{
		for (unsigned int i = 0; i < 3 * numFaces; i++)
			for (unsigned int j = 0; j < 2 * domainLength; j = j + 2)
				if (domain[j] == vertexIndices[i] && domain[j + 1] == normalIndices[i])
					indices[i] = newIndices[j / 2];
	}

	if (numVertices != NULL)
		*numVertices = domainLength;

	if (numFloatsPerVertex != NULL)
		*numFloatsPerVertex = numVertexCoordinates + numNormalCoordinates;

	if (vertexData != NULL)
	{
		for (unsigned int i = 0; i < 2 * domainLength; i = i + 2)
		{
			memcpy(&vertexData[(i / 2) * (numVertexCoordinates + numNormalCoordinates)], &vertices[domain[i] * numVertexCoordinates], numVertexCoordinates * sizeof(float));
			memcpy(&vertexData[numVertexCoordinates + (i / 2) * (numVertexCoordinates + numNormalCoordinates)], &normals[domain[i + 1] * numNormalCoordinates], numNormalCoordinates * sizeof(float));
		}
	}

	delete[] vertices;
	delete[] normals;
	delete[] vertexIndices;
	delete[] normalIndices;
}

void CreateModel(
	float* vertices, unsigned int numVertices, unsigned int numCoordinatesPerVertex,
	unsigned int* indices, unsigned int numVertexIndices,
	PRIMITIVE_TOPOLOGY primitiveTopology,
	unsigned int instanceCount,
	Model* model)
{
	model->vertices = new float[numVertices * numCoordinatesPerVertex];
	model->numVertices = numVertices;
	model->numCoordinatesPerVertex = numCoordinatesPerVertex;
	model->vertexIndices = new unsigned int[numVertexIndices];
	model->numVertexIndices = numVertexIndices;
	model->primitiveTopology = primitiveTopology;
	model->worldViewProjection = new Buffer * [instanceCount];
	model->instanceIndices = new unsigned int[instanceCount];
	model->numInstanceIndices = instanceCount;

	if (numModels > 0)
		for (unsigned int i = 0; i < instanceCount; i++)
			model->instanceIndices[i] = models[numModels - 1]->instanceIndices[models[numModels - 1]->numInstanceIndices - 1] + i + 1;
	else
		for (unsigned int i = 0; i < instanceCount; i++)
			model->instanceIndices[i] = i;

	for (unsigned int i = 0; i < instanceCount; i++)
		CreateBuffer(sizeof(WorldViewProjection), BUFFER_TYPE_DEFAULT, &model->worldViewProjection[i],
			(NUM_CONSTANT_BUFFERS + NUM_SHADER_RESOURCES) * model->instanceIndices[i]);

	numModels++;
	Model** tempModels = new Model * [numModels];
	memcpy(tempModels, models, (numModels - 1) * sizeof(Model*));
	delete[] models;

	tempModels[numModels - 1] = model;
	models = tempModels;

	memcpy(model->vertices, vertices, numVertices * numCoordinatesPerVertex * sizeof(float));
	memcpy(model->vertexIndices, indices, numVertexIndices * sizeof(int));
}

// Acertar tabela de descritores
void DestroyModel(Model* model)
{
	unsigned int modelIndex;
	unsigned int offsetFromDescriptorTable;

	for (unsigned int i = 0; i < numModels; i++)
		if (model == models[i])
			modelIndex = i;

	for (unsigned int i = 0; i < model->numInstanceIndices; i++)
		DeleteBuffer(model->worldViewProjection[i]);

	for (unsigned int i = modelIndex + 1; i < numModels; i++)
	{
		for (unsigned int j = 0; j < models[i]->numInstanceIndices; j++)
		{
			models[i]->instanceIndices[j] = models[i]->instanceIndices[j] - model->numInstanceIndices;
			offsetFromDescriptorTable = (NUM_CONSTANT_BUFFERS + NUM_SHADER_RESOURCES) * models[i]->instanceIndices[j];
			MoveDescriptor(models[i]->worldViewProjection[j], offsetFromDescriptorTable);
		}
	}

	numModels--;
	Model** tempModels = new Model * [numModels];
	memcpy(tempModels, models, modelIndex * sizeof(Model*));
	memcpy(&tempModels[modelIndex], &models[modelIndex + 1], (numModels - modelIndex) * sizeof(Model*));
	delete[] models;
	models = tempModels;

	delete[] model->instanceIndices;
	delete[] model->vertices;
	delete[] model->vertexIndices;
}

void SetCamera(
	POSITION position, ROTATION rotation,
	float aspectRatio, float nearClippingPlane, float farClippingPlane, bool isPerspectiveProjection, float fovAngleY)
{
	cameraPosition = XMMatrixTranslation(-position.x, -position.y, -position.z);
	cameraRoll = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, -rotation.roll * XM_PI / 180.0f);
	cameraYaw = XMMatrixRotationRollPitchYaw(0.0f, -rotation.yaw * XM_PI / 180.0f, 0.0f);
	cameraPitch = XMMatrixRotationRollPitchYaw(-rotation.pitch * XM_PI / 180.0f, 0.0f, 0.0f);
	if (isPerspectiveProjection)
		projection = XMMatrixPerspectiveFovLH(fovAngleY * XM_PI / 180.0f, aspectRatio, nearClippingPlane, farClippingPlane);
	else
		projection = XMMatrixOrthographicLH(aspectRatio, 1, nearClippingPlane, farClippingPlane);
}

void ClearListOfModelsToRender()
{
	vertexDataSize = 0;
	indexDataSize = 0;
	numModelsToRender = 0;
}

// VertexData por enquanto só contem os vértices. Deverá incluir outras informações como normal, coordenada de textura, etc.
// Da mesma forma, VertexDataSize deverá ser ajustado.
void SetModel(POSITION position, ROTATION rotation, float scaleFactors[3], unsigned int instanceIndex, Model* model)
{
	WorldViewProjection wvp;

	if (scaleFactors == NULL)
		wvp.scaling = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	else
		wvp.scaling = XMMatrixScaling(scaleFactors[0], scaleFactors[1], scaleFactors[2]);

	wvp.rotation = XMMatrixRotationRollPitchYaw(rotation.pitch * XM_PI / 180.0f, rotation.yaw * XM_PI / 180.0f, rotation.roll * XM_PI / 180.0f);
	wvp.translation = XMMatrixTranslation(position.x, position.y, position.z);
	wvp.cameraPosition = cameraPosition;
	wvp.cameraYaw = cameraYaw;
	wvp.cameraPitch = cameraPitch;
	wvp.cameraRoll = cameraRoll;
	wvp.projection = projection;

	UploadData(uploadWVPBuffer, NULL, 1, sizeof(WorldViewProjection), 1, 1, &wvp);
	SetPrivateData(model->worldViewProjection[instanceIndex], uploadWVPBuffer);

	unsigned int tempVertexDataSize = vertexDataSize + model->numVertices * model->numCoordinatesPerVertex * sizeof(float);
	unsigned char* tempVertexData = new unsigned char[tempVertexDataSize];
	memcpy(tempVertexData, vertexData, vertexDataSize);
	memcpy(&tempVertexData[vertexDataSize], model->vertices, tempVertexDataSize - vertexDataSize);
	delete[] vertexData;
	vertexData = (void*)tempVertexData;
	vertexDataSize = tempVertexDataSize;

	unsigned int tempIndiceDataSize = indexDataSize + model->numVertexIndices * sizeof(int);
	unsigned char* tempIndiceData = new unsigned char[tempIndiceDataSize];
	memcpy(tempIndiceData, indexData, indexDataSize);
	memcpy(&tempIndiceData[indexDataSize], model->vertexIndices, tempIndiceDataSize - indexDataSize);
	delete[] indexData;
	indexData = (unsigned int*)tempIndiceData;
	indexDataSize = tempIndiceDataSize;

	numModelsToRender++;
	Model** tempModelsToRender = new Model * [numModelsToRender];
	memcpy(tempModelsToRender, modelsToRender, (numModelsToRender - 1) * sizeof(Model*));
	delete[] modelsToRender;

	tempModelsToRender[numModelsToRender - 1] = model;

	modelsToRender = tempModelsToRender;
}

// dataSizePerVertex deverá ser ajustado quando outros dados forem acrescidos aos vértices (normal, coordenadas de textura, etc)
void DrawModels()
{
	if (numModelsToRender < 1)
		return;

	unsigned int startIndexCount, startVertexCount;

	unsigned int dataSizePerVertex = modelsToRender[0]->numCoordinatesPerVertex * sizeof(float);
	SetVertexData(vertexData, vertexDataSize, dataSizePerVertex);
	SetIndexData(indexData, indexDataSize);

	for (unsigned int i = 0; i < numModelsToRender; i++)
	{
		startIndexCount = 0;
		startVertexCount = 0;
		for (unsigned int j = 0; j < i; j++)
		{
			startIndexCount += modelsToRender[j]->numVertexIndices;
			startVertexCount += modelsToRender[j]->numVertices;
		}

		SetPrimitiveTopology(modelsToRender[i]->primitiveTopology);
		for (unsigned int j = 0; j < modelsToRender[i]->numInstanceIndices; j++)
		{
			MapDescriptorTableOffsetToBaseRegister((NUM_CONSTANT_BUFFERS + NUM_SHADER_RESOURCES) * modelsToRender[i]->instanceIndices[j]);
			DrawIndexedInstanced(modelsToRender[i]->numVertexIndices, 1, startIndexCount, startVertexCount, 0);
		}
	}
}