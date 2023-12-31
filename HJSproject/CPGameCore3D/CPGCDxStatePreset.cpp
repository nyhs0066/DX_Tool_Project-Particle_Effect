#include "CPGCDxStateManager.h"

HRESULT CPGCDXStateManager::createDefaultStates()
{
	HRESULT hr = S_OK;

	//Sampler State 기본 설정값
	/*Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;		필터값 - 텍스처에서 가져오는 색상의 보간 형식
	AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;			UV주소 모드 - 0.0f~1.0f를 벗어나는 UV좌표에 대한 처리방식
	AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	MipLODBias = 0.0f;								LOD 레벨 보정값
	MaxAnisotropy = 1;								비등방성 밉맵단계 상한
	ComparisonFunc = D3D11_COMPARISON_NEVER;		샘플링 전후 비교 함수
	BorderColor[0] = 1.0f;							//주소 모드 Border지정시 범위를 벗어난 TexCoord좌표에 부여할 색상
	BorderColor[1] = 1.0f;
	BorderColor[2] = 1.0f;
	BorderColor[3] = 1.0f;
	MinLOD = -FLT_MAX;								//최소 밉맵 LOD레벨
	MaxLOD = +FLT_MAX;*/							//최대 밉맵 LOD레벨 : MinLOD보다 커야한다.

	//기본 샘플러 스테이트 - 선형 보간 텍스처 필터링 + 클램프 모드
	D3D11_SAMPLER_DESC SS_LINEAR_CLAMP;
	ZeroMemory(&SS_LINEAR_CLAMP, sizeof(D3D11_SAMPLER_DESC));
	SS_LINEAR_CLAMP.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SS_LINEAR_CLAMP.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	SS_LINEAR_CLAMP.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	SS_LINEAR_CLAMP.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	hr = createSState(CPGC_VNAME2WSTR(SS_LINEAR_CLAMP), SS_LINEAR_CLAMP);

	//선형 보간 텍스처 필터링 - 래핑 모드
	D3D11_SAMPLER_DESC SS_LINEAR_WRAP;
	ZeroMemory(&SS_LINEAR_WRAP, sizeof(D3D11_SAMPLER_DESC));
	SS_LINEAR_WRAP.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SS_LINEAR_WRAP.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SS_LINEAR_WRAP.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SS_LINEAR_WRAP.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	hr = createSState(CPGC_VNAME2WSTR(SS_LINEAR_WRAP), SS_LINEAR_WRAP);

	//포인트 텍스처 필터링 - 래핑 모드
	D3D11_SAMPLER_DESC SS_POINT_WRAP;
	ZeroMemory(&SS_POINT_WRAP, sizeof(D3D11_SAMPLER_DESC));
	SS_POINT_WRAP.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	SS_POINT_WRAP.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SS_POINT_WRAP.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SS_POINT_WRAP.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	hr = createSState(CPGC_VNAME2WSTR(SS_POINT_WRAP), SS_POINT_WRAP);

	//그림자 맵 PCF를 위한 비교 샘플러 - 포인트 필터링 (LESS => SRC < COMP)
	D3D11_SAMPLER_DESC SS_SHADOWMAP_COMP_BORDER;
	ZeroMemory(&SS_SHADOWMAP_COMP_BORDER, sizeof(D3D11_SAMPLER_DESC));
	SS_SHADOWMAP_COMP_BORDER.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	SS_SHADOWMAP_COMP_BORDER.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	SS_SHADOWMAP_COMP_BORDER.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	SS_SHADOWMAP_COMP_BORDER.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	for (auto& it : SS_SHADOWMAP_COMP_BORDER.BorderColor) { it = 0.0f; }
	SS_SHADOWMAP_COMP_BORDER.ComparisonFunc = D3D11_COMPARISON_LESS;

	hr = createSState(CPGC_VNAME2WSTR(SS_SHADOWMAP_COMP_BORDER), SS_SHADOWMAP_COMP_BORDER);
	
	//Rasterizer State 기본 설정값
	/*FillMode = D3D11_FILL_SOLID;					//채우기 모드
	CullMode = D3D11_CULL_BACK;						//은면 제거 방식
	FrontCounterClockwise = FALSE;					//앞, 뒤 판정 정점 구성방식
	DepthBias = 0;									//깊이 보정값
	DepthBiasClamp = 0.0f;							//깊이 보정값의 상한/하한
	SlopeScaledDepthBias = 0.0f;					//깊이 기울기의 스케일 보정값
	DepthClipEnable = TRUE;							//깊이에 따른 절단 과정 수행여부
	ScissorEnable = FALSE;							//시저 사각형 활성화 여부
	MultisampleEnable = FALSE;						//멀티 샘플링 사용 여부
	AntialiasedLineEnable = FALSE;*/				//선에 대한 안티 앨리어싱 수행 여부

	//와이어 프레임
	D3D11_RASTERIZER_DESC RS_WIREFRAME;
	ZeroMemory(&RS_WIREFRAME, sizeof(D3D11_RASTERIZER_DESC));
	RS_WIREFRAME.FillMode = D3D11_FILL_WIREFRAME;
	RS_WIREFRAME.CullMode = D3D11_CULL_BACK;
	RS_WIREFRAME.DepthClipEnable = TRUE;

	hr = createRState(CPGC_VNAME2WSTR(RS_WIREFRAME), RS_WIREFRAME);

	//솔리드
	D3D11_RASTERIZER_DESC RS_SOLID;
	ZeroMemory(&RS_SOLID, sizeof(D3D11_RASTERIZER_DESC));
	RS_SOLID.FillMode = D3D11_FILL_SOLID;
	RS_SOLID.CullMode = D3D11_CULL_BACK;
	RS_SOLID.DepthClipEnable = TRUE;

	hr = createRState(CPGC_VNAME2WSTR(RS_SOLID), RS_SOLID);

	//솔리드 : 컬링 작업 제외
	D3D11_RASTERIZER_DESC RS_SOLID_NOCULL;
	ZeroMemory(&RS_SOLID_NOCULL, sizeof(D3D11_RASTERIZER_DESC));
	RS_SOLID_NOCULL.FillMode = D3D11_FILL_SOLID;
	RS_SOLID_NOCULL.CullMode = D3D11_CULL_NONE;
	RS_SOLID_NOCULL.DepthClipEnable = TRUE;

	hr = createRState(CPGC_VNAME2WSTR(RS_SOLID_NOCULL), RS_SOLID_NOCULL);

	//와이어 프레임 : 컬링 작업 제외
	D3D11_RASTERIZER_DESC RS_WIREFRAME_NOCULL;
	ZeroMemory(&RS_WIREFRAME_NOCULL, sizeof(D3D11_RASTERIZER_DESC));
	RS_WIREFRAME_NOCULL.FillMode = D3D11_FILL_WIREFRAME;
	RS_WIREFRAME_NOCULL.CullMode = D3D11_CULL_NONE;
	RS_WIREFRAME_NOCULL.DepthClipEnable = TRUE;

	hr = createRState(CPGC_VNAME2WSTR(RS_WIREFRAME_NOCULL), RS_WIREFRAME_NOCULL);

	//솔리드 : 앞면 컬링
	D3D11_RASTERIZER_DESC RS_SOLID_FRONTCULL;
	ZeroMemory(&RS_SOLID_FRONTCULL, sizeof(D3D11_RASTERIZER_DESC));
	RS_SOLID_FRONTCULL.FillMode = D3D11_FILL_SOLID;
	RS_SOLID_FRONTCULL.CullMode = D3D11_CULL_FRONT;
	RS_SOLID_FRONTCULL.DepthClipEnable = TRUE;

	hr = createRState(CPGC_VNAME2WSTR(RS_SOLID_FRONTCULL), RS_SOLID_FRONTCULL);

	//와이어 프레임 : 앞면 컬링
	D3D11_RASTERIZER_DESC RS_WIREFRAME_FRONTCULL;
	ZeroMemory(&RS_WIREFRAME_FRONTCULL, sizeof(D3D11_RASTERIZER_DESC));
	RS_WIREFRAME_FRONTCULL.FillMode = D3D11_FILL_WIREFRAME;
	RS_WIREFRAME_FRONTCULL.CullMode = D3D11_CULL_FRONT;
	RS_WIREFRAME_FRONTCULL.DepthClipEnable = TRUE;

	hr = createRState(CPGC_VNAME2WSTR(RS_WIREFRAME_FRONTCULL), RS_WIREFRAME_FRONTCULL);

	//Blend State 기본 설정값
	/*AlphaToCoverageEnable = FALSE;											//Alpha To Coverage라는 멀티 샘플링 방법 사용 여부
	IndependentBlendEnable = FALSE;												//독립적인 렌더타겟 블렌딩 사용 여부
	RenderTarget[0].BlendEnable = FALSE;										//블렌딩 옵션 활성화 여부
	RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;									//추가할 대상 색상의 블렌딩 계수
	RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;								//기존 대상 색상의 블렌딩 계수
	RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;								//색상의 블렌딩 연산 방법
	RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;							//추가할 대상 알파값 블렌딩 계수
	RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;							//기존 대상 알파값 블렌딩 계수
	RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;							//알파값의 블렌딩 연산 방법
	RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;*/		//렌더 타겟으로 출력할 색상 채널 마스크

	/////////////////////////////////////////////////////////////
	//	블렌드 스테이트
	//		- FinalColor = SRC_COLOR * SRC_FACTOR <OPERATOR> DEST_COLOR * DEST_FACTOR
	//			- SRC_COLOR : 소스 색상
	//			- SRC_FACTOR : 소스 색상 인수
	//			- DEST_COLOR : 배경 색상
	//	 		- DEST_FACTOR : 배경 색상 인수
	// 	 		- OPERATOR : 혼합 연산자
	// 	 		- ALPHA는 배경색상과 어느정도로 혼합을 수행할 지를 결정하는 인수가 된다.
	//			- SrcBlendAlpha = D3D11_BLEND_ONE 이라는 것은 소스 색상에 전달된 알파 인수 그대로
	//				배경과의 혼합 색상 인수로 사용한다는 의미
	/////////////////////////////////////////////////////////////

	//기본 블렌드 스테이트 : 블렌딩 없음
	D3D11_BLEND_DESC BS_DEFAULT;
	ZeroMemory(&BS_DEFAULT, sizeof(D3D11_BLEND_DESC));
	BS_DEFAULT.AlphaToCoverageEnable = FALSE;
	BS_DEFAULT.IndependentBlendEnable = FALSE;
	BS_DEFAULT.RenderTarget[0].BlendEnable = FALSE;

	//BlendColor = SrcColor;
	//BlendAlpha = SrcAlpha;
	//FinalColor = SrcColor * SrcAlpha + BGColor * InvSrcAlpha;

	BS_DEFAULT.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	BS_DEFAULT.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	BS_DEFAULT.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BS_DEFAULT.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BS_DEFAULT.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BS_DEFAULT.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BS_DEFAULT.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = createBState(CPGC_VNAME2WSTR(BS_DEFAULT), BS_DEFAULT);

	//멀티 샘플링 알파 테스팅 - 알파테스트에 의한 안티 앨리어싱
	D3D11_BLEND_DESC BS_MSALPHATEST;
	ZeroMemory(&BS_MSALPHATEST, sizeof(D3D11_BLEND_DESC));
	BS_MSALPHATEST.AlphaToCoverageEnable = TRUE;
	BS_MSALPHATEST.IndependentBlendEnable = FALSE;
	BS_MSALPHATEST.RenderTarget[0].BlendEnable = TRUE;

	BS_MSALPHATEST.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BS_MSALPHATEST.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BS_MSALPHATEST.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	BS_MSALPHATEST.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BS_MSALPHATEST.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BS_MSALPHATEST.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	BS_MSALPHATEST.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = createBState(CPGC_VNAME2WSTR(BS_MSALPHATEST), BS_MSALPHATEST);

	//알파 블렌딩
	D3D11_BLEND_DESC BS_ALPHABLEND;
	ZeroMemory(&BS_ALPHABLEND, sizeof(D3D11_BLEND_DESC));
	BS_ALPHABLEND.AlphaToCoverageEnable = FALSE;
	BS_ALPHABLEND.IndependentBlendEnable = FALSE;
	BS_ALPHABLEND.RenderTarget[0].BlendEnable = TRUE;

	//BlendColor = DestColor * (1.0f - SrcAlpha) + SrcColor * SrcAlpha
	//BlendAlpha = SrcAlpha
	//FinalColor = BlendColor * SrcAlpha + BGColor * InvSrcAlpha;

	BS_ALPHABLEND.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BS_ALPHABLEND.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BS_ALPHABLEND.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	BS_ALPHABLEND.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BS_ALPHABLEND.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BS_ALPHABLEND.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BS_ALPHABLEND.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = createBState(CPGC_VNAME2WSTR(BS_ALPHABLEND), BS_ALPHABLEND);

	//듀얼 소스 블렌딩
	D3D11_BLEND_DESC BS_DUALSOURCEBLEND;
	ZeroMemory(&BS_DUALSOURCEBLEND, sizeof(D3D11_BLEND_DESC));

	BS_DUALSOURCEBLEND.AlphaToCoverageEnable = FALSE;
	BS_DUALSOURCEBLEND.IndependentBlendEnable = FALSE;
	BS_DUALSOURCEBLEND.RenderTarget[0].BlendEnable = TRUE;

	//Src0Color = texColor
	//ALPHA = MAX(Src0Color.R, Src0Color.G, Src0Color.B)
	//Src1Color = (Red : 1.0f - ALPHA, Green : 1.0f - ALPHA, Blue : 1.0f - ALPHA)

	//FinalColor = DestColor * Src1Color + SrcColor * 1.0f
	BS_DUALSOURCEBLEND.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BS_DUALSOURCEBLEND.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	BS_DUALSOURCEBLEND.RenderTarget[0].DestBlend = D3D11_BLEND_SRC1_COLOR;

	BS_DUALSOURCEBLEND.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BS_DUALSOURCEBLEND.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BS_DUALSOURCEBLEND.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	
	BS_DUALSOURCEBLEND.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = createBState(CPGC_VNAME2WSTR(BS_DUALSOURCEBLEND), BS_DUALSOURCEBLEND);

	//가산 블렌딩
	D3D11_BLEND_DESC BS_ADDITIVEBLEND;
	ZeroMemory(&BS_ADDITIVEBLEND, sizeof(D3D11_BLEND_DESC));

	BS_ADDITIVEBLEND.AlphaToCoverageEnable = FALSE;
	BS_ADDITIVEBLEND.IndependentBlendEnable = FALSE;
	BS_ADDITIVEBLEND.RenderTarget[0].BlendEnable = TRUE;

	//BlendColor = SrcColor + DestColor
	//BlendAlpha = SrcAlpha
	//FinalColor = BlendColor * SrcAlpha + BGColor * InvSrcAlpha;

	//FinalColor = DestColor + SrcColor

	BS_ADDITIVEBLEND.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	BS_ADDITIVEBLEND.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	BS_ADDITIVEBLEND.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	BS_ADDITIVEBLEND.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BS_ADDITIVEBLEND.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BS_ADDITIVEBLEND.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	BS_ADDITIVEBLEND.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = createBState(CPGC_VNAME2WSTR(BS_ADDITIVEBLEND), BS_ADDITIVEBLEND);

	////듀얼 소스 가산 블렌딩
	//D3D11_BLEND_DESC BS_DUALSOURCEADDITIVEBLEND;
	//ZeroMemory(&BS_DUALSOURCEADDITIVEBLEND, sizeof(D3D11_BLEND_DESC));

	//BS_DUALSOURCEADDITIVEBLEND.AlphaToCoverageEnable = FALSE;
	//BS_DUALSOURCEADDITIVEBLEND.IndependentBlendEnable = FALSE;
	//BS_DUALSOURCEADDITIVEBLEND.RenderTarget[0].BlendEnable = TRUE;

	////Src0Color = texColor
	////ALPHA = MAX(Src0Color.R, Src0Color.G, Src0Color.B)
	////Src1Color = (Red : 1.0f - ALPHA, Green : 1.0f - ALPHA, Blue : 1.0f - ALPHA)

	////FinalColor = DestColor * Src1Color + SrcColor * 1.0f
	//BS_DUALSOURCEADDITIVEBLEND.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//BS_DUALSOURCEADDITIVEBLEND.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	//BS_DUALSOURCEADDITIVEBLEND.RenderTarget[0].DestBlend = D3D11_BLEND_SRC1_COLOR;

	//BS_DUALSOURCEADDITIVEBLEND.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	//BS_DUALSOURCEADDITIVEBLEND.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	//BS_DUALSOURCEADDITIVEBLEND.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

	//BS_DUALSOURCEADDITIVEBLEND.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	//hr = createBState(CPGC_VNAME2WSTR(BS_DUALSOURCEADDITIVEBLEND), BS_DUALSOURCEADDITIVEBLEND);

	//소스 블렌드 제외 : 배경만 렌더
	D3D11_BLEND_DESC BS_NOSOURCEBLEND;

	ZeroMemory(&BS_NOSOURCEBLEND, sizeof(D3D11_BLEND_DESC));

	BS_NOSOURCEBLEND.AlphaToCoverageEnable = FALSE;
	BS_NOSOURCEBLEND.IndependentBlendEnable = FALSE;
	BS_NOSOURCEBLEND.RenderTarget[0].BlendEnable = TRUE;

	//Src0Color = texColor
	//ALPHA = MAX(Src0Color.R, Src0Color.G, Src0Color.B)
	//Src1Color = (Red : 1.0f - ALPHA, Green : 1.0f - ALPHA, Blue : 1.0f - ALPHA)

	//FinalColor = DestColor * Src1Color + SrcColor * 1.0f
	BS_NOSOURCEBLEND.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BS_NOSOURCEBLEND.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
	BS_NOSOURCEBLEND.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

	BS_NOSOURCEBLEND.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BS_NOSOURCEBLEND.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	BS_NOSOURCEBLEND.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;

	BS_NOSOURCEBLEND.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = createBState(CPGC_VNAME2WSTR(BS_NOSOURCEBLEND), BS_NOSOURCEBLEND);
	

	//Depth-Stencil 기본 설정값
	//DSS_D_COMPLESS.DepthEnable = TRUE;
	//DSS_D_COMPLESS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//DSS_D_COMPLESS.DepthFunc = D3D11_COMPARISON_LESS;
	//DSS_D_COMPLESS.StencilEnable = FALSE;
	//DSS_D_COMPLESS.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	//DSS_D_COMPLESS.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	//DSS_D_COMPLESS.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//DSS_D_COMPLESS.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	//DSS_D_COMPLESS.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//DSS_D_COMPLESS.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//DSS_D_COMPLESS.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//DSS_D_COMPLESS.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	//DSS_D_COMPLESS.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//DSS_D_COMPLESS.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//기본 깊이 스텐실 스테이트
	D3D11_DEPTH_STENCIL_DESC DSS_D_COMPLESS;
	ZeroMemory(&DSS_D_COMPLESS, sizeof(D3D11_DEPTH_STENCIL_DESC));

	DSS_D_COMPLESS.DepthEnable = TRUE;
	DSS_D_COMPLESS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DSS_D_COMPLESS.DepthFunc = D3D11_COMPARISON_LESS;

	hr = createDSState(CPGC_VNAME2WSTR(DSS_D_COMPLESS), DSS_D_COMPLESS);

	//후위 순위 먼저 렌더
	D3D11_DEPTH_STENCIL_DESC DSS_D_COMPGREATER;
	ZeroMemory(&DSS_D_COMPGREATER, sizeof(D3D11_DEPTH_STENCIL_DESC));

	DSS_D_COMPGREATER.DepthEnable = TRUE;
	DSS_D_COMPGREATER.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DSS_D_COMPGREATER.DepthFunc = D3D11_COMPARISON_GREATER;

	hr = createDSState(CPGC_VNAME2WSTR(DSS_D_COMPGREATER), DSS_D_COMPGREATER);

	//비교후 기입 안함
	D3D11_DEPTH_STENCIL_DESC DSS_D_COMPLESS_NOWRITE;
	ZeroMemory(&DSS_D_COMPLESS_NOWRITE, sizeof(D3D11_DEPTH_STENCIL_DESC));

	DSS_D_COMPLESS_NOWRITE.DepthEnable = TRUE;
	DSS_D_COMPLESS_NOWRITE.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	DSS_D_COMPLESS_NOWRITE.DepthFunc = D3D11_COMPARISON_LESS;

	hr = createDSState(CPGC_VNAME2WSTR(DSS_D_COMPLESS_NOWRITE), DSS_D_COMPLESS_NOWRITE);

	//깊이 스텐실 모두 사용안함
	D3D11_DEPTH_STENCIL_DESC DSS_DISABLE;
	ZeroMemory(&DSS_DISABLE, sizeof(D3D11_DEPTH_STENCIL_DESC));

	DSS_DISABLE.DepthEnable = FALSE;

	hr = createDSState(CPGC_VNAME2WSTR(DSS_DISABLE), DSS_DISABLE);

	//깊이 테스트 : 더 작은 값 / 스텐실 테스트 : 앞면 - 항상 통과 - 통과 시 스텐실 버퍼값 증가
	D3D11_DEPTH_STENCIL_DESC DSS_DS_ALWAYS_FRONT_INC;
	ZeroMemory(&DSS_DS_ALWAYS_FRONT_INC, sizeof(D3D11_DEPTH_STENCIL_DESC));

	DSS_DS_ALWAYS_FRONT_INC.DepthEnable = TRUE;
	DSS_DS_ALWAYS_FRONT_INC.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DSS_DS_ALWAYS_FRONT_INC.DepthFunc = D3D11_COMPARISON_LESS;

	DSS_DS_ALWAYS_FRONT_INC.StencilEnable = TRUE;
	DSS_DS_ALWAYS_FRONT_INC.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	DSS_DS_ALWAYS_FRONT_INC.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	DSS_DS_ALWAYS_FRONT_INC.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	DSS_DS_ALWAYS_FRONT_INC.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	DSS_DS_ALWAYS_FRONT_INC.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DSS_DS_ALWAYS_FRONT_INC.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	DSS_DS_ALWAYS_FRONT_INC.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
	DSS_DS_ALWAYS_FRONT_INC.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DSS_DS_ALWAYS_FRONT_INC.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DSS_DS_ALWAYS_FRONT_INC.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	//깊이 테스트 : 더 작은 값 / 스텐실 테스트 : 앞면 - 항상 통과 - 통과 시 스텐실 버퍼값 교체
	D3D11_DEPTH_STENCIL_DESC DSS_DS_ALWAYS_FRONT_REPLACE;
	ZeroMemory(&DSS_DS_ALWAYS_FRONT_REPLACE, sizeof(D3D11_DEPTH_STENCIL_DESC));

	DSS_DS_ALWAYS_FRONT_REPLACE.DepthEnable = TRUE;
	DSS_DS_ALWAYS_FRONT_REPLACE.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DSS_DS_ALWAYS_FRONT_REPLACE.DepthFunc = D3D11_COMPARISON_LESS;

	DSS_DS_ALWAYS_FRONT_REPLACE.StencilEnable = TRUE;
	DSS_DS_ALWAYS_FRONT_REPLACE.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	DSS_DS_ALWAYS_FRONT_REPLACE.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	DSS_DS_ALWAYS_FRONT_REPLACE.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	DSS_DS_ALWAYS_FRONT_REPLACE.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	DSS_DS_ALWAYS_FRONT_REPLACE.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DSS_DS_ALWAYS_FRONT_REPLACE.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	DSS_DS_ALWAYS_FRONT_REPLACE.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
	DSS_DS_ALWAYS_FRONT_REPLACE.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DSS_DS_ALWAYS_FRONT_REPLACE.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DSS_DS_ALWAYS_FRONT_REPLACE.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	hr = createDSState(CPGC_VNAME2WSTR(DSS_DS_ALWAYS_FRONT_REPLACE), DSS_DS_ALWAYS_FRONT_REPLACE);

	//깊이 테스트 : 더 작은값 / 스텐실 테스트 : 앞면 - 더 큰값 - 통과 시 스텐실 버퍼값 증가
	D3D11_DEPTH_STENCIL_DESC DSS_DS_GREATER_FRONT_INC;
	ZeroMemory(&DSS_DS_GREATER_FRONT_INC, sizeof(D3D11_DEPTH_STENCIL_DESC));

	DSS_DS_GREATER_FRONT_INC.DepthEnable = TRUE;
	DSS_DS_GREATER_FRONT_INC.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DSS_DS_GREATER_FRONT_INC.DepthFunc = D3D11_COMPARISON_LESS;

	DSS_DS_GREATER_FRONT_INC.StencilEnable = TRUE;
	DSS_DS_GREATER_FRONT_INC.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	DSS_DS_GREATER_FRONT_INC.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	DSS_DS_GREATER_FRONT_INC.FrontFace.StencilFunc = D3D11_COMPARISON_GREATER;
	DSS_DS_GREATER_FRONT_INC.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	DSS_DS_GREATER_FRONT_INC.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DSS_DS_GREATER_FRONT_INC.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	DSS_DS_GREATER_FRONT_INC.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
	DSS_DS_GREATER_FRONT_INC.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DSS_DS_GREATER_FRONT_INC.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DSS_DS_GREATER_FRONT_INC.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	hr = createDSState(CPGC_VNAME2WSTR(DSS_DS_GREATER_FRONT_INC), DSS_DS_GREATER_FRONT_INC);

	//깊이 테스트 : 더 작은값 / 스텐실 테스트 : 앞면 - 더 큰값 - 통과 시 스텐실 버퍼값 교체
	D3D11_DEPTH_STENCIL_DESC DSS_DS_GREATER_FRONT_REPLACE;
	ZeroMemory(&DSS_DS_GREATER_FRONT_REPLACE, sizeof(D3D11_DEPTH_STENCIL_DESC));

	DSS_DS_GREATER_FRONT_REPLACE.DepthEnable = TRUE;
	DSS_DS_GREATER_FRONT_REPLACE.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DSS_DS_GREATER_FRONT_REPLACE.DepthFunc = D3D11_COMPARISON_LESS;

	DSS_DS_GREATER_FRONT_REPLACE.StencilEnable = TRUE;
	DSS_DS_GREATER_FRONT_REPLACE.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	DSS_DS_GREATER_FRONT_REPLACE.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	DSS_DS_GREATER_FRONT_REPLACE.FrontFace.StencilFunc = D3D11_COMPARISON_GREATER;
	DSS_DS_GREATER_FRONT_REPLACE.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	DSS_DS_GREATER_FRONT_REPLACE.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DSS_DS_GREATER_FRONT_REPLACE.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	DSS_DS_GREATER_FRONT_REPLACE.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
	DSS_DS_GREATER_FRONT_REPLACE.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DSS_DS_GREATER_FRONT_REPLACE.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DSS_DS_GREATER_FRONT_REPLACE.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	hr = createDSState(CPGC_VNAME2WSTR(DSS_DS_GREATER_FRONT_REPLACE), DSS_DS_GREATER_FRONT_REPLACE);

	//깊이 테스트 : 더 작은값 / 스텐실 테스트 : 앞면 - 같지 않은 값
	D3D11_DEPTH_STENCIL_DESC DSS_DS_NOTEQUAL_FRONT_KEEP;
	ZeroMemory(&DSS_DS_NOTEQUAL_FRONT_KEEP, sizeof(D3D11_DEPTH_STENCIL_DESC));

	DSS_DS_NOTEQUAL_FRONT_KEEP.DepthEnable = TRUE;
	DSS_DS_NOTEQUAL_FRONT_KEEP.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DSS_DS_NOTEQUAL_FRONT_KEEP.DepthFunc = D3D11_COMPARISON_LESS;

	DSS_DS_NOTEQUAL_FRONT_KEEP.StencilEnable = TRUE;
	DSS_DS_NOTEQUAL_FRONT_KEEP.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	DSS_DS_NOTEQUAL_FRONT_KEEP.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	DSS_DS_NOTEQUAL_FRONT_KEEP.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
	DSS_DS_NOTEQUAL_FRONT_KEEP.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DSS_DS_NOTEQUAL_FRONT_KEEP.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DSS_DS_NOTEQUAL_FRONT_KEEP.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	DSS_DS_NOTEQUAL_FRONT_KEEP.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
	DSS_DS_NOTEQUAL_FRONT_KEEP.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DSS_DS_NOTEQUAL_FRONT_KEEP.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DSS_DS_NOTEQUAL_FRONT_KEEP.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	hr = createDSState(CPGC_VNAME2WSTR(DSS_DS_NOTEQUAL_FRONT_KEEP), DSS_DS_NOTEQUAL_FRONT_KEEP);

	//깊이 테스트 : 없음 / 스텐실 테스트 : 앞면 - 같은 값
	D3D11_DEPTH_STENCIL_DESC DSS_S_EQUAL_FRONT_KEEP;
	ZeroMemory(&DSS_S_EQUAL_FRONT_KEEP, sizeof(D3D11_DEPTH_STENCIL_DESC));

	DSS_S_EQUAL_FRONT_KEEP.DepthEnable = FALSE;
	DSS_S_EQUAL_FRONT_KEEP.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DSS_S_EQUAL_FRONT_KEEP.DepthFunc = D3D11_COMPARISON_LESS;

	DSS_S_EQUAL_FRONT_KEEP.StencilEnable = TRUE;
	DSS_S_EQUAL_FRONT_KEEP.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	DSS_S_EQUAL_FRONT_KEEP.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	DSS_S_EQUAL_FRONT_KEEP.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	DSS_S_EQUAL_FRONT_KEEP.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DSS_S_EQUAL_FRONT_KEEP.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DSS_S_EQUAL_FRONT_KEEP.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	DSS_S_EQUAL_FRONT_KEEP.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
	DSS_S_EQUAL_FRONT_KEEP.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DSS_S_EQUAL_FRONT_KEEP.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DSS_S_EQUAL_FRONT_KEEP.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	hr = createDSState(CPGC_VNAME2WSTR(DSS_S_EQUAL_FRONT_KEEP), DSS_S_EQUAL_FRONT_KEEP);

	return hr;
}