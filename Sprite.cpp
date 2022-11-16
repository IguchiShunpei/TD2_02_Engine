#include "Sprite.h"
#include<d3dcompiler.h>

#pragma comment(lib,"d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

//頂点データ
XMFLOAT3 vertices[] = {
	{-0.5f,-0.5f,0.0f},//左下
	{-0.5f,+0.5f,0.0f},//左上
	{+0.5f,-0.5f,0.0f},//右下
};

void Sprite::Initialize(DirectXCommon*dxCommon_)
{
	//頂点データの全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * _countof(vertices));

	//頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{};        //ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;  //GPUへの転送用

	//リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB; //頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//頂点バッファの作成
	result = dxCommon_->GetDevice()->CreateCommittedResource
	(
		&heapProp,  //ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc,   //リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff_)
	);
	assert(SUCCEEDED(result));

	//GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	XMFLOAT3* vertMap = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));

	//前頂点に対して
	for (int i = 0; i < _countof(vertices); i++)
	{
		vertMap[i] = vertices[i];   //座標をコピー
	}

	//繋がりを解除
	vertBuff_->Unmap(0, nullptr);
	//GPU仮想アドレス
	vbView.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	//頂点バッファのサイズ
	vbView.SizeInBytes = sizeVB;
	//頂点一つ分のデータサイズ
	vbView.StrideInBytes = sizeof(XMFLOAT3);

	ComPtr < ID3DBlob> vsBlob;    //頂点シェーダオブジェクト
	ComPtr < ID3DBlob> psBlob;    //ピクセルシェーダオブジェクト
	ComPtr < ID3DBlob> errorBlob; //エラーオブジェクト

	//頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile
	(
		L"Resources/shaders/SpriteVS.hlsl",//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,  //インクルード可能にする
		"main", "vs_5_0",  //エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,  //デバッグ用指定
		0,
		&vsBlob, &errorBlob
	);

	//エラーなら
	if (FAILED(result))
	{
		//errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	//ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile
	(
		L"Resources/shaders/SpritePS.hlsl",//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,  //インクルード可能にする
		"main", "ps_5_0",  //エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,  //デバッグ用指定
		0,
		&psBlob, &errorBlob
	);

	//エラーなら
	if (FAILED(result))
	{
		//errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
		   "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		   D3D12_APPEND_ALIGNED_ELEMENT,
		   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},//(1行で書いたほうが見やすい)
	};

	//グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	//シェーダーの設定
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	//サンプルマスクの設定
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//ラスタライザの設定
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;   //カリングしない
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;  //ポリゴン内塗りつぶし
	pipelineDesc.RasterizerState.DepthClipEnable = true;            //深度クリッピングを有効に

	//ブレンドステート
	pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask
		= D3D12_COLOR_WRITE_ENABLE_ALL; //RBGA全てのチャンネルを描画

	//頂点レイアウトの設定
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	//図形の形状設定
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//その他の設定
	pipelineDesc.NumRenderTargets = 1;  //描画対象は1つ
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; //0~255指定のRGBA
	pipelineDesc.SampleDesc.Count = 1;  //1ピクセルにつき1回サンプリング

	//ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{ };
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	//ルートシグネチャのシリアライズ
	ComPtr<ID3DBlob> rootSigBlob_;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob_, &errorBlob);
	assert(SUCCEEDED(result));
	result = dxCommon_->GetDevice()->CreateRootSignature(0, rootSigBlob_->GetBufferPointer(), rootSigBlob_->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(result));
	//rootSigBlob_->Release();
	//パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = rootSignature_.Get();

	//パイプラインステートを生成
	result = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(result));
}

void Sprite::Update()
{

}

void Sprite::Draw(DirectXCommon* dxCommon_)
{
	//パイプラインステートとルートシグネチャの設定コマンド
	dxCommon_->GetCommandList()->SetPipelineState(pipelineState_.Get());
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	
	//プリミティブ形状の設定コマンド
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//頂点バッファビューの設定コマンド
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vbView);
	//描画コマンド
	dxCommon_->GetCommandList()->DrawInstanced(_countof(vertices), 1, 0, 0);  //全ての頂点を使って描画
}
