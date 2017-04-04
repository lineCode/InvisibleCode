/************************************************************************/
// @file	DecodeFuncs.h
// @brief	���ζ�ά��Ľ��뺯��
// @author	Ԭ����
// @date	2016/9/17
// 
/************************************************************************/
#include <vector>
using namespace std;

#include "..\3DCode\DataTypes.h"
#include "..\3DCode\PerspectTransform.h"

/// λ��Ԫ��ǩ
#define POSITION_TAG 2

/// ��ɫԪ��ǩ
#define COLOR_TAG 1

/// ����Ԫ��ǩ
#define BACKGROUND_TAG 0

/// ���p��A�ľ��������������ΪCOLOR_TAG(1)��������ΪBACKGROUND_TAG(0)
#define CLUSTER(p, A, B) ((_Distance(p, A) < _Distance(p, B)) ? COLOR_TAG : BACKGROUND_TAG)

// ���p��A�ľ��������������ΪCOLOR_TAG(1)��������ΪBACKGROUND_TAG(0)
BOOL CLUSTER_FUN(float3 &p, float3 &A, float3 &B) ;

// Ѱ��ģ���Ԫ�صı��
int FindMaxmumIndex(float2 a, float2 b, float2 c);

// ��ȡģ���С
int GetBarCodeSize(float2 centers[4]);

//////////////////////////////////////////////////////////////////////////

// ��������ͷ
BOOL DecodeDataHeader(BYTE *pHead, int nPixelSize, int nRowlen, int nChannel, 
					  COLORREF cColorElem, COLORREF cBackgroundElem, int nSymbolSize, 
					  int &nVersion, int &nDataFormat, int &nDataLength, int &nEcLevel, int &nMaskingNo);
// ��������
char* DecodeData(BYTE *pHead, int nPixelSize, int nRowlen, int nChannel, 
				 COLORREF cColorElem, COLORREF cBackgroundElem, int nSymbolSize, 
				 int nVersion, int nDataFormat, int nDataLength, int nEcLevel, int nMaskingNo);

// ������ά��ͼ��
char* DecodeImage(BYTE* pHead, int nWidth, int nHeight, int nRowlen, int nChannel, RoiRect roi, 
				  COLORREF cPositionElem, COLORREF cColorElem, COLORREF cBackgroundElem, 
				  float2 Centers[4], int &nVersion, int &nDataFormat, int &nDataLength, int &nEcLevel, int &nMaskingNo);

//////////////////////////////////////////////////////////////////////////

#if _MSC_VER <= 1800
// ��������
float round(float r);
#endif

// ��ʼ����������
void InitializeClusterCenters(float2 *Center, vector<float2> & position, BOOL bTryHard = TRUE);

// ������position���о���
vector<int> K_means(vector<float2> & position, float2 centers[4], int nWidth, int nHeight, int nMaxepoches = 24);

BOOL medianFilter(BYTE* pData, int nWidth, int nHeight, int nRowlen, int nSize);

// �Ծ������Ľ�������
void SortClusterCenters(float2 centers[4]);

// ����һ��͸�ӱ任
PerspectTransform CreateTransform(float2 pos[4], int dimension, float fmodulesize);