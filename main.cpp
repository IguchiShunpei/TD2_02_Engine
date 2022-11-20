#define DIRECTINPUT_VERSION    0x0800 //DirectInput�̃o�[�W�����w��
#include"Input.h"
#include"WinApp.h"
#include"DirectXCommon.h"
#include "Sprite.h"

#include<windows.h>
#include<cassert>
#include<vector>
#include<string>
#include<d3dcompiler.h>
#include<DirectXMath.h>
#include<dinput.h>
#include<DirectXTex.h>

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dxguid.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

// �E�B���h�E�v���V�[�W��
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//���b�Z�[�W�ɉ����ăQ�[���ŗL�̏������s��
	switch (msg)
	{
		//�E�B���h�E���j�����ꂽ
	case WM_DESTROY:
		//OS�ɑ΂��āA�A�v���̏I����`����
		PostQuitMessage(0);
		return 0;
	}

	//�W���̃��b�Z�[�W�������s��
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//�萔�o�b�t�@�p�f�[�^�\����(�}�e���A��)
struct ConstBufferDataMaterial {
	XMFLOAT4 color;//�F(RGBA)
};

//�萔�o�b�t�@�p�f�[�^�\����(3D�ϊ��s��)
struct ConstBufferDataTransform {
	XMMATRIX mat; //3D�ϊ��s��
};

//3D�I�u�W�F�N�g�^
struct Object3d
{
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuffTransform;

	//�萔�o�b�t�@�}�b�v(�s��p)
	ConstBufferDataTransform* constMapTransform = nullptr;

	//�A�t�B���ϊ����
	XMFLOAT3 scale = { 1,1,1 };
	XMFLOAT3 rotation = { 0,0,0 };
	XMFLOAT3 position = { 0,0,0 };

	//���[���h�ϊ��s��
	XMMATRIX matWorld;

	//�e�I�u�W�F�N�g�ւ̃|�C���^
	Object3d* parent = nullptr;
};

//�������֐�
void InitializeObject3d(Object3d* object, ID3D12Device* device)
{
	HRESULT result;

	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES HeapProp{};
	HeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;  //GPU�ւ̓]���p
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = (sizeof(ConstBufferDataTransform) * 0xff) & ~0xff;  //256�o�C�g�A���C�������g
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//�萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&HeapProp,//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc,//���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&object->constBuffTransform));
	assert(SUCCEEDED(result));

	//�萔�o�b�t�@�̃}�b�s���O
	result = object->constBuffTransform->Map(0, nullptr, (void**)&object->constMapTransform);
	assert(SUCCEEDED(result));
}

//�X�V�֐�
void UpdateObject3d(Object3d* object, XMMATRIX& matView, XMMATRIX& matProjection)
{
	XMMATRIX matScale, matRot, matTrans;

	matScale = XMMatrixScaling(object->scale.x, object->scale.y, object->scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(object->rotation.z);
	matRot *= XMMatrixRotationX(object->rotation.x);
	matRot *= XMMatrixRotationY(object->rotation.y);
	matTrans = XMMatrixTranslation(object->position.x, object->position.y, object->position.z);

	object->matWorld = XMMatrixIdentity();
	object->matWorld *= matScale;
	object->matWorld *= matRot;
	object->matWorld *= matTrans;

	if (object->parent != nullptr)
	{
		object->matWorld *= object->parent->matWorld;
	}

	//�萔�o�b�t�@�փf�[�^�]��
	object->constMapTransform->mat = object->matWorld * matView * matProjection;
}

//�`��֐�
void DrawObject3d(Object3d* object, ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW& vbView,
	D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices)
{
	//���_�o�b�t�@�̐ݒ�
	commandList->IASetVertexBuffers(0, 1, &vbView);

	//�C���f�b�N�X�o�b�t�@�̐ݒ�
	commandList->IASetIndexBuffer(&ibView);

	//�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	commandList->SetGraphicsRootConstantBufferView(2, object->constBuffTransform->GetGPUVirtualAddress());

	//�`��R�}���h
	commandList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

#pragma region ��ՃV�X�e���̏�����
	//WindowsAPI�̃|�C���^
	WinApp* winApp = nullptr;
	//WindowsAPI�̏�����
	winApp = new WinApp();
	winApp->Initialize();

	//DirectX�̃|�C���^
	DirectXCommon* dxCommon_ = nullptr;
	//DirectX�̏�����
	dxCommon_ = new DirectXCommon();
	dxCommon_->Initialize(winApp);

	//���͂̃|�C���^
	Input* input = nullptr;
	//���͂̏�����
	input = new Input();
	input->Initialize(winApp);

	//�X�v���C�g�̃|�C���^
	Sprite* sprite = new Sprite;
	//�X�v���C�g�̏�����
	sprite->Initialize(dxCommon_, WinApp::window_width, WinApp::window_height);
	sprite->LoadTexture(1, L"Resources/texture.jpg",dxCommon_);

#pragma endregion ��ՃV�X�e���̏�����

		//�Q�[�����[�v
	while (true)
	{
#pragma region ��ՃV�X�e���̍X�V

		//Windows�̃��b�Z�[�W����
		if (winApp->ProcessMessage())
		{
			//�Q�[�����[�v�𔲂���
			break;
		}

		//���͂̍X�V
		input->Update();
		sprite->Update();

#pragma endregion ��ՃV�X�e���̍X�V
		
		//�`��O����
		dxCommon_->PreDraw();

#pragma region �ŏ��̃V�[���̕`��

		//�����Ƀ|���S���Ȃǂ̕`�揈��������
		sprite->Draw(dxCommon_);
#pragma endregion �ŏ��̃V�[���̕`��

		// �`��㏈��
		dxCommon_->PostDraw();
	}

#pragma region �ŏ��̃V�[���̏I��

	//�����Ƀ|���S���̏���������������

#pragma endregion �ŏ��̃V�[���̏I��

#pragma region ��ՃV�X�e���̏I��

	//DirectX���
	delete dxCommon_;

	//���͉��
	delete input;

	//�X�v���C�g�̉��
	delete sprite;

	//WindowsAPI�̏I������
	winApp->Finalize();

	//WindowsAPI���
	delete winApp;

#pragma endregion ��ՃV�X�e���̏I��

	return 0;
}
