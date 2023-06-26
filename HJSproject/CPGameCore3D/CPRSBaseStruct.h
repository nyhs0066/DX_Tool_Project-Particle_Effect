#pragma once
#include "CPGCStd.h"

struct CPRS_TRANSFORM
{
	CPM_Vector3 vT;		//Translation
	CPM_Vector3 vR;		//Rotation : X,Y,Z Radian
	CPM_Vector3 vS;		//Scale

	CPM_Matrix matT;		//Translation
	CPM_Matrix matR;		//Rotation
	CPM_Matrix matS;		//Scale

	void setMT(const CPM_Vector3& VT);
	void setMR(const CPM_Vector3& VR);
	void setMS(const CPM_Vector3& VS);
	void setMTRS(const CPM_Vector3& VT,
		const CPM_Vector3& VR,
		const CPM_Vector3& VS);
};

//��ġ�� ��ǥ�� ��ȯ ��� ��Ʈ ������� ����ü
struct CPRS_CBUF_COORDCONV_MATSET
{
	CPM_Matrix	matTWorld;
	CPM_Matrix  matTInvWorld;
	CPM_Matrix	matTView;
	CPM_Matrix	matTProj;
};

//�� ������ �ǵ����̸� ȥ���ؼ� ���� �� ��
struct CPRS_AXIS
{
	union
	{
		struct
		{
			CPM_Vector3 vX;
			CPM_Vector3 vY;
			CPM_Vector3 vZ;
		};

		struct
		{
			CPM_Vector3 vRight;
			CPM_Vector3 vUp;
			CPM_Vector3 vLook;
		};
	};

	CPRS_AXIS();
};

//�⺻�� �Ǵ� ���� ���� ����ü
struct CPRS_BASE_PHYSICS_STATE
{
	CPM_Vector3 vDirection;
	CPM_Vector3 vVelocity;
	CPM_Vector3 vAcceleration;
	CPM_Vector3 vForce;

	FLOAT		fAngularSpeed;
	FLOAT		fSpeed;
};

//Shader �׷� ����ü
struct CPRS_ShaderGroup
{
	std::wstring									wszName;

	ID3D11VertexShader* pVS;
	ID3D11HullShader* pHS;
	ID3D11DomainShader* pDS;
	ID3D11GeometryShader* pGS;
	ID3D11PixelShader* pPS;
	ID3D11ComputeShader* pCS;

	CPRS_ShaderGroup()
	{
		pVS = nullptr;
		pHS = nullptr;
		pDS = nullptr;
		pGS = nullptr;
		pPS = nullptr;
		pCS = nullptr;
	}

	void bindToPipeline(ID3D11DeviceContext* pDContext)
	{
		if (pDContext)
		{
			pDContext->VSSetShader(pVS, NULL, 0);
			pDContext->HSSetShader(pHS, NULL, 0);
			pDContext->DSSetShader(pDS, NULL, 0);
			pDContext->GSSetShader(pGS, NULL, 0);
			pDContext->PSSetShader(pPS, NULL, 0);
			pDContext->CSSetShader(pCS, NULL, 0);
		}
	}
};

//SO�������� �ɼ� ���� ����ü
struct CPRS_SO_OPTION
{
	D3D11_SO_DECLARATION_ENTRY* pSOEntry;
	UINT								iNumEntry;
	UINT* pStrides;
	UINT								iNumStride;
	UINT								iRasterizedStream;
	ID3D11ClassLinkage* pClassLinkage;
};
