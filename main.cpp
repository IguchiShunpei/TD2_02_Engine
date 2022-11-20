#define DIRECTINPUT_VERSION    0x0800 //DirectInputのバージョン指定
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

// ウィンドウプロシージャ
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//メッセージに応じてゲーム固有の処理を行う
	switch (msg)
	{
		//ウィンドウが破棄された
	case WM_DESTROY:
		//OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//定数バッファ用データ構造体(マテリアル)
struct ConstBufferDataMaterial {
	XMFLOAT4 color;//色(RGBA)
};

//定数バッファ用データ構造体(3D変換行列)
struct ConstBufferDataTransform {
	XMMATRIX mat; //3D変換行列
};

//3Dオブジェクト型
struct Object3d
{
	//定数バッファ
	ComPtr<ID3D12Resource> constBuffTransform;

	//定数バッファマップ(行列用)
	ConstBufferDataTransform* constMapTransform = nullptr;

	//アフィン変換情報
	XMFLOAT3 scale = { 1,1,1 };
	XMFLOAT3 rotation = { 0,0,0 };
	XMFLOAT3 position = { 0,0,0 };

	//ワールド変換行列
	XMMATRIX matWorld;

	//親オブジェクトへのポインタ
	Object3d* parent = nullptr;
};

//初期化関数
void InitializeObject3d(Object3d* object, ID3D12Device* device)
{
	HRESULT result;

	//ヒープ設定
	D3D12_HEAP_PROPERTIES HeapProp{};
	HeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;  //GPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = (sizeof(ConstBufferDataTransform) * 0xff) & ~0xff;  //256バイトアラインメント
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//定数バッファの生成
	result = device->CreateCommittedResource(
		&HeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&object->constBuffTransform));
	assert(SUCCEEDED(result));

	//定数バッファのマッピング
	result = object->constBuffTransform->Map(0, nullptr, (void**)&object->constMapTransform);
	assert(SUCCEEDED(result));
}

//更新関数
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

	//定数バッファへデータ転送
	object->constMapTransform->mat = object->matWorld * matView * matProjection;
}

//描画関数
void DrawObject3d(Object3d* object, ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW& vbView,
	D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices)
{
	//頂点バッファの設定
	commandList->IASetVertexBuffers(0, 1, &vbView);

	//インデックスバッファの設定
	commandList->IASetIndexBuffer(&ibView);

	//定数バッファビュー(CBV)の設定コマンド
	commandList->SetGraphicsRootConstantBufferView(2, object->constBuffTransform->GetGPUVirtualAddress());

	//描画コマンド
	commandList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

#pragma region 基盤システムの初期化
	//WindowsAPIのポインタ
	WinApp* winApp = nullptr;
	//WindowsAPIの初期化
	winApp = new WinApp();
	winApp->Initialize();

	//DirectXのポインタ
	DirectXCommon* dxCommon_ = nullptr;
	//DirectXの初期化
	dxCommon_ = new DirectXCommon();
	dxCommon_->Initialize(winApp);

	//入力のポインタ
	Input* input = nullptr;
	//入力の初期化
	input = new Input();
	input->Initialize(winApp);

	//スプライトのポインタ
	Sprite* sprite = new Sprite;
	//スプライトの初期化
	sprite->Initialize(dxCommon_, WinApp::window_width, WinApp::window_height);
	sprite->LoadTexture(1, L"Resources/texture.jpg",dxCommon_);

#pragma endregion 基盤システムの初期化

		//ゲームループ
	while (true)
	{
#pragma region 基盤システムの更新

		//Windowsのメッセージ処理
		if (winApp->ProcessMessage())
		{
			//ゲームループを抜ける
			break;
		}

		//入力の更新
		input->Update();
		sprite->Update();

#pragma endregion 基盤システムの更新
		
		//描画前処理
		dxCommon_->PreDraw();

#pragma region 最初のシーンの描画

		//ここにポリゴンなどの描画処理を書く
		sprite->Draw(dxCommon_);
#pragma endregion 最初のシーンの描画

		// 描画後処理
		dxCommon_->PostDraw();
	}

#pragma region 最初のシーンの終了

	//ここにポリゴンの初期化処理を書く

#pragma endregion 最初のシーンの終了

#pragma region 基盤システムの終了

	//DirectX解放
	delete dxCommon_;

	//入力解放
	delete input;

	//スプライトの解放
	delete sprite;

	//WindowsAPIの終了処理
	winApp->Finalize();

	//WindowsAPI解放
	delete winApp;

#pragma endregion 基盤システムの終了

	return 0;
}
