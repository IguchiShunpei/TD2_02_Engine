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
	//���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//���[�g�V�O�l�`��
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	//�p�C�v���C���X�e�[�g
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
	// ���ʊm�F
	HRESULT result;
};

