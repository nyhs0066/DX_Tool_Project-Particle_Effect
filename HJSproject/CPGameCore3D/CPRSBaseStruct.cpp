#include "CPRSBaseStruct.h"

void CPRS_TRANSFORM::setMT(const CPM_Vector3& VT)
{
	vT = VT;
	matT.CreateTranslation(VT);
}

void CPRS_TRANSFORM::setMR(const CPM_Vector3& VR)
{
	vR = VR;
	matR.CreateFromYawPitchRoll(VR);
}

void CPRS_TRANSFORM::setMS(const CPM_Vector3& VS)
{
	vS = VS;
	matS.CreateScale(VS);
}

void CPRS_TRANSFORM::setMTRS(const CPM_Vector3& VT, const CPM_Vector3& VR, const CPM_Vector3& VS)
{
	setMT(VT);
	setMR(VR);
	setMS(VS);
}

CPRS_AXIS::CPRS_AXIS()
{

}