#include "Sprite.h"
#include<d3dcompiler.h>

#pragma comment(lib,"d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

//���_�f�[�^
XMFLOAT3 vertices[] = {
	{-0.5f,-0.5f,0.0f},//����
	{-0.5f,+0.5f,0.0f},//����
	{+0.5f,-0.5f,0.0f},//�E��
};

void Sprite::Initialize(DirectXCommon*dxCommon_)
{
	//���_�f�[�^�̑S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	UINT sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * _countof(vertices));

	//���_�o�b�t�@�̐ݒ�
	D3D12_HEAP_PROPERTIES heapProp{};        //�q�[�v�ݒ�
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;  //GPU�ւ̓]���p

	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB; //���_�f�[�^�S�̂̃T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//���_�o�b�t�@�̍쐬
	result = dxCommon_->GetDevice()->CreateCommittedResource
	(
		&heapProp,  //�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc,   //���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff_)
	);
	assert(SUCCEEDED(result));

	//GPU��̃o�b�t�@�ɑΉ��������z������(���C����������)���擾
	XMFLOAT3* vertMap = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));

	//�O���_�ɑ΂���
	for (int i = 0; i < _countof(vertices); i++)
	{
		vertMap[i] = vertices[i];   //���W���R�s�[
	}

	//�q���������
	vertBuff_->Unmap(0, nullptr);
	//GPU���z�A�h���X
	vbView.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	//���_�o�b�t�@�̃T�C�Y
	vbView.SizeInBytes = sizeVB;
	//���_����̃f�[�^�T�C�Y
	vbView.StrideInBytes = sizeof(XMFLOAT3);

	ComPtr < ID3DBlob> vsBlob;    //���_�V�F�[�_�I�u�W�F�N�g
	ComPtr < ID3DBlob> psBlob;    //�s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr < ID3DBlob> errorBlob; //�G���[�I�u�W�F�N�g

	//���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile
	(
		L"Resources/shaders/SpriteVS.hlsl",//�V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,  //�C���N���[�h�\�ɂ���
		"main", "vs_5_0",  //�G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,  //�f�o�b�O�p�w��
		0,
		&vsBlob, &errorBlob
	);

	//�G���[�Ȃ�
	if (FAILED(result))
	{
		//errorBlob����G���[���e��string�^�ɃR�s�[
		std::string error;
		error.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		//�G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	//�s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile
	(
		L"Resources/shaders/SpritePS.hlsl",//�V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,  //�C���N���[�h�\�ɂ���
		"main", "ps_5_0",  //�G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,  //�f�o�b�O�p�w��
		0,
		&psBlob, &errorBlob
	);

	//�G���[�Ȃ�
	if (FAILED(result))
	{
		//errorBlob����G���[���e��string�^�ɃR�s�[
		std::string error;
		error.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		//�G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	//���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
		   "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		   D3D12_APPEND_ALIGNED_ELEMENT,
		   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},//(1�s�ŏ������ق������₷��)
	};

	//�O���t�B�b�N�X�p�C�v���C���ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	//�V�F�[�_�[�̐ݒ�
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	//�T���v���}�X�N�̐ݒ�
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//���X�^���C�U�̐ݒ�
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;   //�J�����O���Ȃ�
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;  //�|���S�����h��Ԃ�
	pipelineDesc.RasterizerState.DepthClipEnable = true;            //�[�x�N���b�s���O��L����

	//�u�����h�X�e�[�g
	pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask
		= D3D12_COLOR_WRITE_ENABLE_ALL; //RBGA�S�Ẵ`�����l����`��

	//���_���C�A�E�g�̐ݒ�
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	//�}�`�̌`��ݒ�
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//���̑��̐ݒ�
	pipelineDesc.NumRenderTargets = 1;  //�`��Ώۂ�1��
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; //0~255�w���RGBA
	pipelineDesc.SampleDesc.Count = 1;  //1�s�N�Z���ɂ�1��T���v�����O

	//���[�g�V�O�l�`���̐ݒ�
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{ };
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	//���[�g�V�O�l�`���̃V���A���C�Y
	ComPtr<ID3DBlob> rootSigBlob_;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob_, &errorBlob);
	assert(SUCCEEDED(result));
	result = dxCommon_->GetDevice()->CreateRootSignature(0, rootSigBlob_->GetBufferPointer(), rootSigBlob_->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(result));
	//rootSigBlob_->Release();
	//�p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	pipelineDesc.pRootSignature = rootSignature_.Get();

	//�p�C�v���C���X�e�[�g�𐶐�
	result = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(result));
}

void Sprite::Update()
{

}

void Sprite::Draw(DirectXCommon* dxCommon_)
{
	//�p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
	dxCommon_->GetCommandList()->SetPipelineState(pipelineState_.Get());
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	
	//�v���~�e�B�u�`��̐ݒ�R�}���h
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//���_�o�b�t�@�r���[�̐ݒ�R�}���h
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vbView);
	//�`��R�}���h
	dxCommon_->GetCommandList()->DrawInstanced(_countof(vertices), 1, 0, 0);  //�S�Ă̒��_���g���ĕ`��
}
