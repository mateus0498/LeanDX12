# LeanDX12
Wrapper library para DirectX 12. Para utilizá-la basta incluir o arquivo de cabeçalho LeanDX12.h (#include LeanDX12.h) e adicionar as seguintes dependências:
![image](https://user-images.githubusercontent.com/48290411/132145705-19978417-dcff-4f83-8207-143dbf8c37f8.png)
Em caso de dúvidas utilize como ponto de partida os códigos de exemplo fornecidos (Samples).

## LeanDX12 Samples
1. [Hello Render Target](Samples/LDX12HelloRenderTarget):
    
    <img src="Images/HelloRenderTarget.png" height="300">
    
1. [Hello Triangle](Samples/LDX12HelloTriangle):
    
    <img src="Images/HelloTriangle.png" height="300">
    
1. [Virtual Camera](Samples/LDX12VirtualCamera): Uso da função LoadWavefrontOBJ para carregar arquivo .obj contendo as informações dos vértices de um modelo de uma pistola *Desert Eagle*. Implementação de uma câmera virtual, transformando o espaço do mundo no espaço projetivo através de matrizes de translação, rotação, entre outras.
    
    <img src="Images/VirtualCamera.png" height="300">
    
Saída gerada no console de ambos os samples (a saída varia conforme a configuração de hardware):
```
Adaptador de video selecionado:
- Descricao: NVIDIA GeForce GTX 1050 Ti
- Memoria dedicada (VRAM): 3.92383 GB
- Memoria compartilhada (RAM): 7.94316 GB
```
