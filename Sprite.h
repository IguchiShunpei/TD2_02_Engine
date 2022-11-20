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
	//頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	//定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffMaterial_;
	//テクスチャバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> texBuff_;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	//パイプラインステート
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
	//デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;

	// 結果確認
	HRESULT result;

	////横方向ピクセル数
	//const size_t textureWidth = 256;

	////縦方向ピクセル数
	//const size_t textureHeight = 256;

	//SRVの最大個数
	const size_t kMaxSRVCount = 2056;

	////配列の要素数
	//const size_t imageDataCount = textureWidth * textureHeight;

};

