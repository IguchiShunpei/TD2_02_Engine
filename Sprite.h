#pragma once
#include "DirectXCommon.h"
#include <wrl.h>
#include<DirectXMath.h>

class Sprite
{
public:
	void Initialize(DirectXCommon*dxCommon_, int window_width, int window_height);

	void Update();

	void Draw(DirectXCommon* dxCommon_);

	//���Wsetter
	void SetPosition(const DirectX::XMFLOAT2& position) { position_ = position; }
	//���Wgetter
	const DirectX::XMFLOAT2& GetPosition() const { return position_; }
	//��]setter
	void SetRotation(float rotation) { rotation_ = rotation; }
	//��]getter
	float GetRotation() const { return rotation_; }
	//�Fsetter
	void SetColor(const DirectX::XMFLOAT4& color) { color_ = color; }
	//�Fgetter
	const DirectX::XMFLOAT4& GetColor() const { return color_; }
	//�\���T�C�Ysetter
	void SetSize(const DirectX::XMFLOAT2& size) { size_ = size; }
	//�\���T�C�Ygetter
	const DirectX::XMFLOAT2& GetSize() const { return size_; }
	//�A���J�[�|�C���gsetter
	void SetAnchorPoint(const DirectX::XMFLOAT2& anchorPoint) { anchorPoint_ = anchorPoint; }
	//�A���J�[�|�C���ggetter
	const DirectX::XMFLOAT2& GetAnchorPoint() const { return anchorPoint_; }
	//���E�t���b�vsetter
	bool SetFripX(bool isFlipX) { isFlipX_ = isFlipX; }
	//���E�t���b�vgetter
	bool GetFlipX() const { return isFlipX_; }
	//�㉺�t���b�vsetter
	bool SetFripY(bool isFlipY) { isFlipY_ = isFlipY; }
	//�㉺�t���b�vgetter
	bool GetFlipY() const { return isFlipY_; }
	//��\���t���Osetter
	bool SetInvisible(bool isInvisible) { isInvisible_ = isInvisible; }
	//��\���t���Ogetter
	bool GetInvisible() const { return isInvisible_; }
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
	//�萔�o�b�t�@��GPU���\�[�X�̃|�C���^
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffTransform_;

	// ���ʊm�F
	HRESULT result;

	////�������s�N�Z����
	//const size_t textureWidth = 256;

	////�c�����s�N�Z����
	//const size_t textureHeight = 256;

	//SRV�̍ő��
	const size_t kMaxSRVCount = 2056;

	//���W
	DirectX::XMFLOAT2 position_ = { 0.0f,0.0f };
	// Z�����̉�]�p
	float rotation_ = 0.0f;
	//�F(RGBA)
	DirectX::XMFLOAT4 color_ = { 1,1,1,1 };
	//�\���T�C�Y
	DirectX::XMFLOAT2 size_ = { 100.0f,100.0f };
	//���_�ԍ�
	enum VertexNumber
	{
		LB,  //����
		LT,  //����
		RB,  //�E��
		RT,  //�E��
	};
	//�A���J�[�|�C���g
	DirectX::XMFLOAT2 anchorPoint_ = { 0.0f,0.0f };
	//���E�t���b�v
	bool isFlipX_ = false;
	//�㉺�t���b�v
	bool isFlipY_ = false;
	//��\���t���O
	bool isInvisible_ = false;
	////�z��̗v�f��
	//const size_t imageDataCount = textureWidth * textureHeight;

};

