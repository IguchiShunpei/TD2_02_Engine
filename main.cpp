#define DIRECTINPUT_VERSION    0x0800 //DirectInput�̃o�[�W�����w��
#include"Input.h"
#include"WinApp.h"
#include"DirectXCommon.h"
#include "Sprite.h"
#include "Object3d.h"
#include"Model.h"

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

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

#pragma region ��ՃV�X�e���̏�����
	//WindowsAPI�̃|�C���^
	WinApp* winApp = nullptr;
	//WindowsAPI�̏�����
	winApp = new WinApp();
	winApp->Initialize();

	//DirectX�̃|�C���^
	DirectXCommon* dxCommon = nullptr;
	//DirectX�̏�����
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	//���͂̃|�C���^
	Input* input = nullptr;
	//���͂̏�����
	input = new Input();
	input->Initialize(winApp);

	//�X�v���C�g�̃|�C���^
	Sprite* sprite = new Sprite;
	//�X�v���C�g�̏�����
	sprite->Initialize(dxCommon, WinApp::window_width, WinApp::window_height);
	sprite->LoadTexture(1, L"Resources/skydome.jpg",dxCommon);

	//3D�I�u�W�F�N�g�ÓI������
	Object3d::StaticInitialize(dxCommon->GetDevice(), WinApp::window_width, WinApp::window_height);
	//OBJ���烂�f���f�[�^��ǂݍ���
	Model* model_1 = Model:: LoadFromOBJ("triangle_mat");
	Model* model_2 = Model::LoadFromOBJ("triangle_mat");
	//3D�I�u�W�F�N�g����
	Object3d* object3d_1 = Object3d::Create();
	Object3d* object3d_2 = Object3d::Create();
	Object3d* object3d_3 = Object3d::Create();
	//�I�u�W�F�N�g�Ƀ��f����R�t����
	object3d_1->SetModel(model_1);
	object3d_2->SetModel(model_2);
	object3d_3->SetModel(model_2);
	//�I�u�W�F�N�g�̈ʒu���w��
	object3d_2->SetPosition({ -5,0,-5 });
	object3d_3->SetPosition({ +5,0,+5 });
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

		//3d�I�u�W�F�N�g�X�V
		object3d_1->Update();
		object3d_2->Update();
		object3d_3->Update();

#pragma endregion ��ՃV�X�e���̍X�V
		
		//�`��O����
		dxCommon->PreDraw();

#pragma region �ŏ��̃V�[���̕`��
		//3D�I�u�W�F�N�g�`��O����
		Object3d::PreDraw(dxCommon->GetCommandList());

		object3d_1->Draw();
		object3d_2->Draw();
		object3d_3->Draw();

		//3D�I�u�W�F�N�g�`��O����
		Object3d::PostDraw();

		//�����Ƀ|���S���Ȃǂ̕`�揈��������
		sprite->Draw(dxCommon);

#pragma endregion �ŏ��̃V�[���̕`��

		// �`��㏈��
		dxCommon->PostDraw();
	}

#pragma region �ŏ��̃V�[���̏I��

	//�����Ƀ|���S���̏���������������

#pragma endregion �ŏ��̃V�[���̏I��

#pragma region ��ՃV�X�e���̏I��

	//3D���f�����
	delete model_1;
	delete model_2;
	//3D�I�u�W�F�N�g���
	delete object3d_1;
	delete object3d_2;
	delete object3d_3;

	//WindowsAPI�̏I������
	winApp->Finalize();

	//DirectX���
	delete dxCommon;

	//���͉��
	delete input;

	//�X�v���C�g�̉��
	delete sprite;

	//WindowsAPI���
	delete winApp;

#pragma endregion ��ՃV�X�e���̏I��

	return 0;
}
