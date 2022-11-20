#pragma once
#include "DirectXCommon.h"
#include <wrl.h>
#include<DirectXMath.h>

class Sprite
{
public:
	void Initialize(DirectXCommon*dxCommon_);

	void Update();

	void Draw(DirectXCommon* dxCommon_);

private:
	//���_�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	//�萔�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffMaterial_;
	//�e�N�X�`���o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> texBuff_;
	//���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//���[�g�V�O�l�`��
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	//�p�C�v���C���X�e�[�g
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
	//�f�X�N���v�^�q�[�v
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;

	// ���ʊm�F
	HRESULT result;

	////�������s�N�Z����
	//const size_t textureWidth = 256;

	////�c�����s�N�Z����
	//const size_t textureHeight = 256;

	//SRV�̍ő��
	const size_t kMaxSRVCount = 2056;

	////�z��̗v�f��
	//const size_t imageDataCount = textureWidth * textureHeight;

};

