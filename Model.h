#pragma once
#include<DirectXMath.h>
#include<vector>
#include<sstream>
#include <d3d12.h>
#include <d3dx12.h>
#include<wrl.h>

class Model
{
public:
	// ���_�f�[�^�\����
	struct VertexPosNormalUv
	{
		DirectX::XMFLOAT3 pos; // xyz���W
		DirectX::XMFLOAT3 normal; // �@���x�N�g��
		DirectX::XMFLOAT2 uv;  // uv���W
	};

	//�}�e���A��
	struct Material
	{
		std::string name;// �}�e���A����
		DirectX::XMFLOAT3 ambient; // �A���r�G���g�e���x
		DirectX::XMFLOAT3 diffuse; // �f�B�t���[�Y�e���x
		DirectX::XMFLOAT3 specular; // �X�y�L�����[�e���x
		float alpha; // �A���t�@
		std::string textureFilename; // �e�N�X�`���t�@�C����
		// �R���X�g���N�^
		Material() {
			ambient = { 0.3f,0.3f,0.3f };
			diffuse = { 0.0f,0.0f,0.0f };
			specular = { 0.0f,0.0f,0.0f };
			alpha = 1.0f;
		}
	};

	// �萔�o�b�t�@�p�f�[�^�\����B1
	struct ConstBufferDataB1
	{
		DirectX::XMFLOAT3 ambient; // �A���r�G���g�W��
		float pad1; // �p�f�B���O
		DirectX::XMFLOAT3 diffuse; // �f�B�t���[�Y�W��
		float pad2; // �p�f�B���O
		DirectX::XMFLOAT3 specular; // �X�y�L�����[�W��
		float alpha; // �A���t�@
	};


public://�ÓI�����o�֐�
	//OBJ�t�@�C������3D���f����ǂݍ���
	static Model* LoadFromOBJ(const std::string& modelname);

	//�}�e���A���ǂݍ���
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);
	//�e�N�X�`���ǂݍ���
	bool LoadTexture(const std::string& directoryPath, const std::string& filename);

	void Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParamIndexMaterial);

	//Device��setter
	static void SetDevice(ID3D12Device* device) { Model::device = device; }

private:
	//�f�o�C�X(�؂�Ă���)
	static ID3D12Device* device;
	// �f�X�N���v�^�T�C�Y
	UINT descriptorHandleIncrementSize;
	// ���_�f�[�^�z��
	std::vector<VertexPosNormalUv> vertices;
	// ���_�C���f�b�N�X�z��
	std::vector<unsigned short> indices;
	// �}�e���A��
	Material material;
	// �f�X�N���v�^�q�[�v
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeap;
	// �e�N�X�`���o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> texbuff;
	// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
	// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView;
	// �C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView;
	// �}�e���A���p�萔�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffB1; 
	// ���_�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff;
	// �C���f�b�N�X�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff;
	//�f�[�^�ǂݍ���
	void LoadFromOBJInternal(const std::string& modelname);
	//�f�X�N���v�^�q�[�v
	void InitializeDescriptorHeap();
	//�e��o�b�t�@����
	void CreateBuffers();
};
