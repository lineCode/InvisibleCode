/************************************************************************
* @file Encodefuncs.h
* @brief ���ζ�ά��ı��뺯��
* @author Ԭ����
* @version 1.0
* @date 2016/9/18
*************************************************************************/


#include "..\3DCode\DataTypes.h"
#include "..\3DCode\CyImage.h"

/// ���a��iλΪ1��������b��j��Ԫ��Ϊ1
#define BIT_CHECK_SET_ELEM(a, i, b, j) if(BIT_CHECK(a, i)) b[j] = 1;

/// ���a��i��Ԫ��Ϊ1��������b��(r, c)��Ԫ��Ϊx
#define ELEM_CHECK_SET(a, i, b, r, c, x) if(a[i]) b[r][c] = x;

/// λ��Ԫ��ǩ
#define POSITION_TAG 2

/// ��ɫԪ��ǩ
#define COLOR_TAG 1

/// ����Ԫ��ǩ
#define BACKGROUND_TAG 0

// ���붨λԪ
void EncodePositionElem(int nCodes[81][81], int nSymbolSize);

// ��������ͷ
void EncodeDataHeader(int nCodes[81][81], int nSymbolSize, int nVersion, int nDataFormat, int nDataLength, int nEcLevel, int nMaskingNo);

// ��������
void EncodeData(int nCodes[81][81], int nSymbolSize, int *code_bits, int bitsLen, int nMaskingNo);

// �������ζ�ά��
int EncodeInvisibleCode(int nCodes[81][81], char *pUtf8, int nVersion, int nDataFormat, int nDataLength, int nEcLevel, int nMaskingNo);

// ȷ�����Ű汾
int AutoVersion(int nBitsLength);

// �ɶ�ά����󴴽�ͼ��
int Convert2Image(CyImage *pImage, int nCodes[81][81], int nSymbolSize, COLORREF cPositionElem = RGB(255, 0, 0), COLORREF cColorElem = RGB(0, 255, 0), COLORREF cBackgroundElem = RGB(255, 255, 255), int nPixelSize = 4);

// ����λ��̽��ͼ��
void PaintPositionPartten(BYTE* pHead, int nWidth, int nHeight, int nRowlen, int nChannel, CRect rect, int R, int G, int B, int nPixelSize = 4);

// �ж��ض���С�ķ��ſɱ���
BOOL CodeAble(int nSymbolSize, int nBitsLen);