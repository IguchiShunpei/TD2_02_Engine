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
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	//パイプラインステート
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
	// 結果確認
	HRESULT result;
};

