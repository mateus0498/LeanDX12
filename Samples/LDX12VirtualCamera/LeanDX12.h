/*
* LeanDX12 versão 1.0.0
* Desenvolvedor: Mateus Ferreira da Silva
* Descrição: Wrapper library para DX12. Biblioteca de Vínculo Estático (.lib)
*
*   Organização do arquivo de cabeçalho:
*	1.	Enumerações
*		•	Valores de Retorno
*		•	Demais enumerações
*	2.	Estruturas
*		•	Estruturas opacas
*		•	Estruturas transparentes
*	3.	Declaração das funções
*		•	Adaptador de Vídeo
*		•	Recursos
*			o	Criação e Exclusão
*			o	Leitura e Escrita (RAM e VRAM)
*			o	Informações sobre recursos
*			o	Tabela de Descritores
*		•	Pipeline Gráfico
*		•	Renderização
*		•	Funções auxiliares
*/

#ifndef _LEANDX12_
#define _LEANDX12_

// ----------------------------------------------------------- 1. Enumerações ------------------------------------------------------------- //

// ------------------------------------------------------ 1.1. Valores de Retorno  -------------------------------------------------------- //

typedef enum LeanDX12Result
{
	LEANDX12_OK = 0x00000000,
	LEANDX12_ERROR_DXGI_NOT_FOUND,
	LEANDX12_ERROR_HARDWARE_ADAPTER_NOT_FOUND,
	LEANDX12_ERROR_DEBUG_INTERFACE_NOT_FOUND,
	LEANDX12_ERROR_FEATURE_LEVEL_NOT_SUPPORTED,
	LEANDX12_ERROR_INVALID_CALL = 0x00000010,
	LEANDX12_ERROR_OUT_OF_MEMORY,
	LEANDX12_ERROR_NO_RENDER_TARGET_SELECTED,
	LEANDX12_ERROR_INSUFFICIENT_BUFFER_SIZE,
	LEANDX12_ERROR_INSUFFICIENT_TABLE_SIZE,
	LEANDX12_ERROR_DESCRIPTOR_TABLE_OFFSET_CONFLICT,
	LEANDX12_ERROR_NOT_SAME_SIZE,
	LEANDX12_ERROR_HEAP_TYPE_NOT_ALLOWED,
	LEANDX12_ERROR_RENDER_TARGET_NOT_ALLOWED,
	LEANDX12_ERROR_MIPLEVEL_NOT_FOUND,
	LEANDX12_INVALID_STEP_RATE,
	LEANDX12_ERROR_INVALID_SHADER_REGISTER,
	LEANDX12_ERROR_NUMBER_OF_CONSTANTS_EXCEEDED_REGISTER_LIMIT,
	LEANDX12_ERROR_OFFSET_OUT_OF_RANGE,
	LEANDX12_ERROR_FIRST_PARAMETER_MULTISAMPLED,
	LEANDX12_ERROR_SECOND_PARAMETER_NOT_MULTISAMPLED,
	LEANDX12_ERROR_RESOURCE_FORMATS_NOT_SAME,
	LEANDX12_ERROR_TEXTURE_DIMENSIONS_NOT_SAME,
	LEANDX12_ERROR_OPEN_FILE_FAILED,
	LEANDX12_ERROR_SAVE_FILE_FAILED,
	LEANDX12_INFO_REQUIRED_ARRAY_LENGTH = 0x00000100,
	LEANDX12_INFO_REQUIRED_BUFFER_SIZE
} LeanDX12Result;

// ------------------------------------------------------ 1.2. Demais enumerações  -------------------------------------------------------- //

typedef enum RESOURCE_FORMAT
{
	RESOURCE_FORMAT_UNKNOWN,
	RESOURCE_FORMAT_R8G8B8A8_UNORM,
	RESOURCE_FORMAT_R8G8B8A8_UNORM_SRGB,
	RESOURCE_FORMAT_R10G10B10A2_UNORM,
	RESOURCE_FORMAT_R10G10B10_XR_BIAS_A2_UNORM,
	RESOURCE_FORMAT_R16G16B16A16_FLOAT,
	RESOURCE_FORMAT_R8_UNORM = 100,
	RESOURCE_FORMAT_R8_SNORM,
	RESOURCE_FORMAT_R8_UINT,
	RESOURCE_FORMAT_R8_SINT,
	RESOURCE_FORMAT_R16_FLOAT,
	RESOURCE_FORMAT_R16_UNORM,
	RESOURCE_FORMAT_R16_SNORM,
	RESOURCE_FORMAT_R16_UINT,
	RESOURCE_FORMAT_R16_SINT,
	RESOURCE_FORMAT_R32_FLOAT,
	RESOURCE_FORMAT_R32_UINT,
	RESOURCE_FORMAT_R32_SINT,
	RESOURCE_FORMAT_R8G8_UNORM,
	RESOURCE_FORMAT_R8G8_SNORM,
	RESOURCE_FORMAT_R8G8_UINT,
	RESOURCE_FORMAT_R8G8_SINT,
	RESOURCE_FORMAT_R16G16_FLOAT,
	RESOURCE_FORMAT_R16G16_UNORM,
	RESOURCE_FORMAT_R16G16_SNORM,
	RESOURCE_FORMAT_R16G16_UINT,
	RESOURCE_FORMAT_R16G16_SINT,
	RESOURCE_FORMAT_R32G32_FLOAT,
	RESOURCE_FORMAT_R32G32_UINT,
	RESOURCE_FORMAT_R32G32_SINT,
	RESOURCE_FORMAT_R11G11B10_FLOAT,
	RESOURCE_FORMAT_R32G32B32_FLOAT,
	RESOURCE_FORMAT_R32G32B32_UINT,
	RESOURCE_FORMAT_R32G32B32_SINT,
	RESOURCE_FORMAT_R8G8B8A8_SNORM,
	RESOURCE_FORMAT_R8G8B8A8_UINT,
	RESOURCE_FORMAT_R8G8B8A8_SINT,
	RESOURCE_FORMAT_R10G10B10A2_UINT,
	RESOURCE_FORMAT_R16G16B16A16_UNORM,
	RESOURCE_FORMAT_R16G16B16A16_SNORM,
	RESOURCE_FORMAT_R16G16B16A16_UINT,
	RESOURCE_FORMAT_R16G16B16A16_SINT,
	RESOURCE_FORMAT_R32G32B32A32_FLOAT,
	RESOURCE_FORMAT_R32G32B32A32_UINT,
	RESOURCE_FORMAT_R32G32B32A32_SINT,
	RESOURCE_FORMAT_D32_FLOAT = 200,
	RESOURCE_FORMAT_D24_UNORM_S8_UINT,
	RESOURCE_FORMAT_D32_FLOAT_S8X24_UINT,
	RESOURCE_FORMAT_FORCE_UINT = 0xFFFFFFFF
} RESOURCE_FORMAT;

typedef enum BUFFER_TYPE
{
	BUFFER_TYPE_DEFAULT,
	BUFFER_TYPE_UPLOAD,
	BUFFER_TYPE_READBACK
} BUFFER_TYPE;

typedef enum CLEAR
{
	CLEAR_STENCIL = 1,
	CLEAR_TARGET = 2,
	CLEAR_DEPTH = 4
} CLEAR;

typedef enum PRIMITIVE_TOPOLOGY {
	PRIMITIVE_TOPOLOGY_UNDEFINED,
	PRIMITIVE_TOPOLOGY_POINTLIST,
	PRIMITIVE_TOPOLOGY_LINELIST,
	PRIMITIVE_TOPOLOGY_LINESTRIP,
	PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
	PRIMITIVE_TOPOLOGY_LINELIST_ADJ = 10,
	PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ,
	PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ,
	PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ,
	PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST = 33,
	PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST,
	PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST
} PRIMITIVE_TOPOLOGY;

typedef enum BLEND_FACTOR
{
	BLEND_FACTOR_ZERO = 1,
	BLEND_FACTOR_ONE,
	BLEND_FACTOR_SRC_COLOR,
	BLEND_FACTOR_INV_SRC_COLOR,
	BLEND_FACTOR_SRC_ALPHA,
	BLEND_FACTOR_INV_SRC_ALPHA,
	BLEND_FACTOR_DEST_ALPHA,
	BLEND_FACTOR_INV_DEST_ALPHA,
	BLEND_FACTOR_DEST_COLOR,
	BLEND_FACTOR_INV_DEST_COLOR,
	BLEND_FACTOR_SRC_ALPHA_SAT,
	BLEND_FACTOR_CUSTOM = 14,
	BLEND_FACTOR_INV_CUSTOM,
	BLEND_FACTOR_SRC1_COLOR,
	BLEND_FACTOR_INV_SRC1_COLOR,
	BLEND_FACTOR_SRC1_ALPHA,
	BLEND_FACTOR_INV_SRC1_ALPHA
} BLEND_FACTOR;

typedef enum BLEND_OPERATION
{
	BLEND_OPERATION_ADD = 1,
	BLEND_OPERATION_SUBTRACT,
	BLEND_OPERATION_REV_SUBTRACT,
	BLEND_OPERATION_OP_MIN,
	BLEND_OPERATION_OP_MAX
} BLEND_OPERATION;

typedef enum LOGIC_OPERATION
{
	LOGIC_OPERATION_CLEAR,
	LOGIC_OPERATION_SET,
	LOGIC_OPERATION_COPY,
	LOGIC_OPERATION_COPY_INVERTED,
	LOGIC_OPERATION_NOOP,
	LOGIC_OPERATION_INVERT,
	LOGIC_OPERATION_AND,
	LOGIC_OPERATION_NAND,
	LOGIC_OPERATION_OR,
	LOGIC_OPERATION_NOR,
	LOGIC_OPERATION_XOR,
	LOGIC_OPERATION_EQUIV,
	LOGIC_OPERATION_AND_REVERSE,
	LOGIC_OPERATION_AND_INVERTED,
	LOGIC_OPERATION_OR_REVERSE,
	LOGIC_OPERATION_OR_INVERTED
} LOGIC_OPERATION;

typedef enum FILL_MODE
{
	FILL_MODE_WIREFRAME = 2,
	FILL_MODE_SOLID
} FILL_MODE;

typedef enum CULL_MODE
{
	CULL_MODE_NONE = 1,
	CULL_MODE_FRONT,
	CULL_MODE_BACK
} CULL_MODE;

typedef enum COMPARISON_FUNC {
	COMPARISON_FUNC_NEVER = 1,
	COMPARISON_FUNC_LESS,
	COMPARISON_FUNC_EQUAL,
	COMPARISON_FUNC_LESS_EQUAL,
	COMPARISON_FUNC_GREATER,
	COMPARISON_FUNC_NOT_EQUAL,
	COMPARISON_FUNC_GREATER_EQUAL,
	COMPARISON_FUNC_ALWAYS
} COMPARISON_FUNC;

typedef enum STENCIL_OPERATION {
	STENCIL_OPERATION_KEEP = 1,
	STENCIL_OPERATION_ZERO,
	STENCIL_OPERATION_REPLACE,
	STENCIL_OPERATION_INCR_SAT,
	STENCIL_OPERATION_DECR_SAT,
	STENCIL_OPERATION_INVERT,
	STENCIL_OPERATION_INCR,
	STENCIL_OPERATION_DECR
} STENCIL_OPERATION;

typedef enum PRIMITIVE_TOPOLOGY_TYPE
{
	PRIMITIVE_TOPOLOGY_TYPE_POINT = 1,
	PRIMITIVE_TOPOLOGY_TYPE_LINE,
	PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
	PRIMITIVE_TOPOLOGY_TYPE_PATCH
} PRIMITIVE_TOPOLOGY_TYPE;

// ------------------------------------------------------------ 2. Estruturas ------------------------------------------------------------- //

// -------------------------------------------------------- 2.1. Estruturas opacas -------------------------------------------------------- //

typedef struct DisplayAdapter DisplayAdapter;
typedef struct Buffer Buffer;
typedef struct Texture Texture;
typedef struct RootSignature RootSignature;
typedef struct PipelineState PipelineState;
typedef struct BlendState BlendState;
typedef struct RasterizerState RasterizerState;
typedef struct DepthStencilState DepthStencilState;
typedef struct InputLayout InputLayout;
typedef struct ShaderBinary ShaderBinary;

// ---------------------------------------------------- 2.2. Estruturas transparentes ----------------------------------------------------- //

typedef unsigned char BOOLEAN;

typedef struct ADAPTER_DESC
{
	char Description[128];
	unsigned int VendorId;
	unsigned int DeviceId;
	unsigned int SubSysId;
	unsigned int Revision;
	unsigned long long DedicatedVideoMemory;
	unsigned long long DedicatedSystemMemory;
	unsigned long long SharedSystemMemory;
	unsigned long AdapterLuid;
} ADAPTER_DESC;

typedef struct MIP_DESC
{
	unsigned int Width;
	unsigned int Height;
	unsigned int Depth;
} MIP_DESC;

typedef struct VIEWPORT
{
	unsigned int Left;
	unsigned int Top;
	unsigned int Right;
	unsigned int Bottom;
} VIEWPORT;

typedef VIEWPORT SCISSOR_RECT;
typedef VIEWPORT CLEAR_RECT;

typedef struct BLEND_DESC
{
	BOOLEAN AlphaToCoverage;
	BOOLEAN BlendEnable;
	BLEND_FACTOR SrcBlend;
	BLEND_FACTOR DestBlend;
	BLEND_OPERATION BlendOp;
	BLEND_FACTOR SrcBlendAlpha;
	BLEND_FACTOR DestBlendAlpha;
	BLEND_OPERATION BlendOpAlpha;
	BOOLEAN LogicOpEnable;
	LOGIC_OPERATION LogicOp;
} BLEND_DESC;

typedef struct RASTERIZER_DESC
{
	FILL_MODE FillMode;
	CULL_MODE CullMode;
	BOOLEAN FrontCounterClockwise;
	int DepthBias;
	float DepthBiasClamp;
	float SlopeScaledDepthBias;
	BOOLEAN DepthClipEnable;
	BOOLEAN MultisampleEnable;
	BOOLEAN AntialiasedLineEnable;
	BOOLEAN ConservativeRaster;
	unsigned int SampleCount;
	unsigned int SampleQuality;
} RASTERIZER_DESC;

typedef struct DEPTH_STENCIL_DESC
{
	BOOLEAN DepthEnable;
	COMPARISON_FUNC DepthFunc;
	BOOLEAN StencilEnable;
	unsigned char StencilReadMask;
	unsigned char StencilWriteMask;
	COMPARISON_FUNC FrontFaceStencilFunc;
	STENCIL_OPERATION FrontFaceStencilPassOp;
	STENCIL_OPERATION FrontFaceStencilDepthFailOp;
	STENCIL_OPERATION FrontFaceStencilFailOp;
	COMPARISON_FUNC BackFaceStencilFunc;
	STENCIL_OPERATION BackFaceStencilPassOp;
	STENCIL_OPERATION BackFaceStencilDepthFailOp;
	STENCIL_OPERATION BackFaceStencilFailOp;
} DEPTH_STENCIL_DESC;

typedef struct INPUT_ELEMENT_DESC
{
	const char* SemanticName;
	unsigned int SemanticIndex;
	RESOURCE_FORMAT Format;
	unsigned int InstanceDataStepRate;
} INPUT_ELEMENT_DESC;

typedef struct GRAPHICS_PIPELINE_STATE_DESC
{
	RootSignature* rootSignature;
	InputLayout* inputLayout;
	ShaderBinary* vertexShader;
	ShaderBinary* hullShader;
	ShaderBinary* domainShader;
	ShaderBinary* geometryShader;
	ShaderBinary* pixelShader;
	PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType;
	RasterizerState* rasterState;
	BlendState* blendState;
	DepthStencilState* depthStencilState;
	RESOURCE_FORMAT renderTargetFormat;
	RESOURCE_FORMAT depthStencilFormat;
} GRAPHICS_PIPELINE_STATE_DESC;

// ------------------------------------------------------ 3. Declaração das funções ------------------------------------------------------- //

// ------------------------------------------------------ 3.1. Camada de Depuração -------------------------------------------------------- //
// Removido do arquivo de cabeçalho.

// ------------------------------------------------------ 3.2. Adaptador de  vídeo -------------------------------------------------------- //

LeanDX12Result GetAdapters(const char* featureLevel, unsigned int* numHardwareAdapters, DisplayAdapter** pHardwareAdapter, unsigned int* numSoftwareAdapters, DisplayAdapter** pSoftwareAdapter);
LeanDX12Result GetHighestPerformanceAdapter(const char* featureLevel, DisplayAdapter** adapter);
LeanDX12Result GetAdapterDesc(DisplayAdapter* adapter, ADAPTER_DESC* adapterDesc);
LeanDX12Result CreateDevice(DisplayAdapter* adapter, const char* featureLevel);
void ReleaseAdapter(DisplayAdapter* adapter);
void ReleaseDevice();

// ------------------------------------------------------------ 3.3. Recursos ------------------------------------------------------------ //
// ------------------------------------------------- 3.3.1 Criação e Exclusão de Recursos ------------------------------------------------ //

LeanDX12Result CreateBuffer(unsigned long long sizeInBytes, BUFFER_TYPE bufferType, Buffer** buffer, unsigned int offsetFromDescriptorTableStart = 0, RESOURCE_FORMAT format = RESOURCE_FORMAT_FORCE_UINT, unsigned int numElements = 0, unsigned int structureSize = 0);
LeanDX12Result DeleteBuffer(Buffer* buffer);
LeanDX12Result CreateTexture(unsigned int width, unsigned int height, unsigned int depth, unsigned short mipLevels, RESOURCE_FORMAT format, Texture** texture, unsigned int offsetFromDescriptorTableStart = 0);
LeanDX12Result DeleteTexture(Texture* texture);
LeanDX12Result CreateRenderTarget(unsigned int width, unsigned int height, RESOURCE_FORMAT format, const float colorRGBA[4], unsigned int sampleCount, unsigned int sampleQuality, Texture** renderTarget, unsigned int offsetFromDescriptorTableStart = 0);
LeanDX12Result DeleteRenderTarget(Texture* renderTarget);

// ------------------------------------------------- 3.3.2 Leitura e Escrita (RAM e VRAM) ------------------------------------------------ //

LeanDX12Result GetPrivateDataAsync(Buffer* defaultBuffer, Buffer* readbackBuffer);
LeanDX12Result GetPrivateDataAsync(Texture* texture, unsigned short mipLevel, unsigned long long* sizeInBytes, Buffer* readbackBuffer);
LeanDX12Result GetPrivateData(Buffer* defaultBuffer, Buffer* readbackBuffer);
LeanDX12Result GetPrivateData(Texture* texture, unsigned short mipLevel, unsigned long long* sizeInBytes, Buffer* readbackBuffer);
LeanDX12Result SetPrivateDataAsync(Buffer* defaultBuffer, Buffer* uploadBuffer);
LeanDX12Result SetPrivateDataAsync(Texture* texture, unsigned short mipLevel, unsigned long long* sizeInBytes, Buffer* uploadBuffer);
LeanDX12Result SetPrivateData(Buffer* defaultBuffer, Buffer* uploadBuffer);
LeanDX12Result SetPrivateData(Texture* texture, unsigned short mipLevel, unsigned long long* sizeInBytes, Buffer* uploadBuffer);
LeanDX12Result ReadbackData(Buffer* readbackBuffer, unsigned int texelSize, unsigned int textureWidth, unsigned int textureHeight, unsigned int textureDepth, void* pData);
LeanDX12Result UploadData(Buffer* uploadBuffer, unsigned long long* requiredBufferSize, unsigned int texelSize, unsigned int textureWidth, unsigned int textureHeight, unsigned int textureDepth, void* pData);
LeanDX12Result SetActiveMipLevel(Texture* texture, unsigned short mipLevel);

// --------------------------------------------------- 3.3.3 Informações sobre recursos -------------------------------------------------- //

LeanDX12Result GetBufferDesc(Buffer* buffer, BUFFER_TYPE* bufferType, unsigned long long* bufferSize);
LeanDX12Result GetTextureDesc(Texture* texture, RESOURCE_FORMAT* textureFormat, unsigned int* sampleCount, unsigned int* sampleQuality, unsigned short* mipLevels, MIP_DESC* mipDesc);

// ----------------------------------------------------- 3.3.4 Tabela de Descritores ------------------------------------------------------ //
// Descrição: Funções que facilitam o gerenciamento da Tabela de Descritores de Constant Buffer e Shader Resource Views (CBVs e SRVs).
// Observação: Os descritores de Unordered Access Views (UAVs) serão acrescidos à tabela em versões futuras da biblioteca LeanDX12, a fim
// de conferir suporte ao Compute Pipeline (Pipeline de Computação).
// Para mais informações sobre tabela de descritores, acesse https://docs.microsoft.com/en-us/windows/win32/direct3d12/descriptor-tables.
// Para mais informações sobre registradores de shaders, acesse:
//	•	https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-variable-register;
//	•	https://docs.microsoft.com/en-us/windows/win32/direct3d12/resource-binding-in-hlsl.

LeanDX12Result MoveDescriptor(Buffer* resource, unsigned int newOffsetFromDescriptorTableStart, BOOLEAN* swap = NULL);
LeanDX12Result MoveDescriptor(Texture* resource, unsigned int newOffsetFromDescriptorTableStart, BOOLEAN* swap = NULL);
LeanDX12Result GetDescriptorOffsetFromTableStart(Buffer* buffer, unsigned int* descriptorOffset);
LeanDX12Result GetDescriptorOffsetFromTableStart(Texture* texture, unsigned int* descriptorOffset);
LeanDX12Result MapDescriptorTableOffsetToBaseRegister(unsigned int descriptorTableOffset);

// -------------------------------------------------------- 3.4. Pipeline Gráfico --------------------------------------------------------- //

LeanDX12Result InitBlendState(BlendState** blendState);
LeanDX12Result InitRasterizerState(RasterizerState** rasterizerState);
LeanDX12Result InitDepthStencilState(DepthStencilState** depthStencilState);

LeanDX12Result SetBlendState(BLEND_DESC blendDesc, BlendState* blendState);
LeanDX12Result SetRasterizerState(RASTERIZER_DESC rasterizerDesc, RasterizerState* rasterizerState);
LeanDX12Result SetDepthStencilState(DEPTH_STENCIL_DESC depthStencilDesc, DepthStencilState* depthStencilState);
LeanDX12Result CreateInputLayout(unsigned int numVertexDataElements, INPUT_ELEMENT_DESC* vertexDataElements, unsigned int numInstanceDataElements, INPUT_ELEMENT_DESC* instanceDataElements, InputLayout** inputLayout);
LeanDX12Result CreateRootSignature(unsigned int num32bitConstants, unsigned int* num32bitValues, unsigned int numConstantBuffers, unsigned int numShaderResources, RootSignature** rootSignature);
LeanDX12Result LoadShaderFromFile(const char* filename, ShaderBinary** shaderBinary);
LeanDX12Result CreateGraphicsPipelineState(GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc, PipelineState** pipelineState);

// --------------------------------------------------------- 3.5. Renderização ------------------------------------------------------------ //

LeanDX12Result BeginScene(PipelineState* pipelineState);
void EndScene();
void SetRenderTarget(Texture* renderTarget);
LeanDX12Result Clear(unsigned int count, const CLEAR_RECT* pRects, unsigned int flags, const float colorRGBA[4], float z, unsigned int stencil);
LeanDX12Result SetViewports(unsigned int numViewports, VIEWPORT* pViewports);
LeanDX12Result SetScissorRects(unsigned int numScissorRects, SCISSOR_RECT* pScissorRects);
void RenderFrameAsync();
void RenderFrame();
void WaitForGPU();
void SetPrimitiveTopology(PRIMITIVE_TOPOLOGY primitiveTopology);
LeanDX12Result SetVertexData(void* pVertexData, unsigned int vertexDataSize, unsigned int dataSizePerVertex);
LeanDX12Result SetInstanceData(void* pInstanceData, unsigned int instanceDataSize, unsigned int dataSizePerInstance, unsigned int stepRate);
LeanDX12Result SetIndexData(unsigned int* pIndexData, unsigned int indexDataSize);
void DrawInstanced(unsigned int vertexCountPerInstance, unsigned int instanceCount, unsigned int startVertexCount, unsigned int startInstanceLocation);
void DrawIndexedInstanced(unsigned int indexCountPerInstance, unsigned int instanceCount, unsigned int startIndexCount, unsigned int startVertexCount, unsigned int startInstanceLocation);
LeanDX12Result Set32bitConstants(unsigned int shaderRegister, unsigned int num32bitValues, void* pConstants, unsigned int destOffset);
LeanDX12Result ResolveTextureAsync(Texture* nonMultisampledTexture, Texture* multisampledTexture);
LeanDX12Result ResolveTexture(Texture* nonMultisampledTexture, Texture* multisampledTexture);

// -------------------------------------------------------- 3.6. Funções auxiliares --------------------------------------------------------- //

unsigned int TexelSize(RESOURCE_FORMAT resourceFormat);
LeanDX12Result SaveAsPNG(const char* filename, void* pImageData, unsigned long long imageDataSize, unsigned int width, unsigned int height);
LeanDX12Result LoadWavefrontOBJ(
	const char* filename,
	float* vertices, unsigned int* numVertices, unsigned int* numVertexCoordinates,
	float* textureCoordinates, unsigned int* numUVWTexture, unsigned int* numTextureCoordinates,
	float* vertexNormals, unsigned int* numVertexNormals, unsigned int* numNormalCoordinates,
	unsigned int* vertexIndices, unsigned int* textureCoordinateIndices, unsigned int* vertexNormalIndices, unsigned int* numFaces);

#endif  // _LEANDX12_