#include "stdafx.h"
#include "Encodefuncs.h"
#include <cmath>
#include "..\3DCode\ColorsMask.h"
#include "..\3DCode\reedSolomon.h"
#include "..\3DCode\RS_ecc.h"
#include "..\3DCode\PixelsOperation.h"

/** 
* @brief ���붨λԪ.
* @details �ú�������λ��̽��Ԫ.
* @param[out] nCodes[81][81] ��ά��
* @param[in] nSymbolSize ���Ŵ�С
* @return ��
* @retval void
*/
void EncodePositionElem(int nCodes[81][81], int nSymbolSize)
{
	// i��ʾ�У�j��ʾ��
	// ���½�λ��̽��Ԫ
	for (int j = 0; j < 7; ++j)
	{
		for (int i = 0; i < 7; ++i)
		{
			nCodes[j][i] = POSITION_TAG;
		}
	}
	// ���Ͻ�λ��̽��Ԫ
	for (int j = nSymbolSize - 5; j < nSymbolSize; ++j)
	{
		for (int i = 0; i < 5; ++i)
		{
			nCodes[j][i] = POSITION_TAG;
		}
	}
	// ���½�λ��̽��Ԫ
	for (int j = 0; j < 5; ++j)
	{
		for (int i = nSymbolSize - 5; i < nSymbolSize; ++i)
		{
			nCodes[j][i] = POSITION_TAG;
		}
	}
	// ���Ͻ�λ��̽��Ԫ
	for (int j = nSymbolSize - 5; j < nSymbolSize; ++j)
	{
		for (int i = nSymbolSize - 5; i < nSymbolSize; ++i)
		{
			nCodes[j][i] = POSITION_TAG;
		}
	}
}


/** 
* @brief ��������ͷ. 
* @details �ú�����������ͷ. 
* @param[out] nCodes[81][81] ��ά��
* @param[in] nSymbolSize ���Ŵ�С
* @param[in] nVersion �汾��С
* @param[in] nDataFormat ���ݸ�ʽ
* @param[in] nDataLength ���ݳ���
* @param[in] nEcLevel ����ȼ�
* @param[in] nMaskingNo ����ģʽ
* @return ��
* @retval void
*/
void EncodeDataHeader(int nCodes[81][81], int nSymbolSize, int nVersion, int nDataFormat, int nDataLength, int nEcLevel, int nMaskingNo)
{
	int nHeader[24];
	memset(nHeader, 0, 24 * sizeof(int));
	// 1-6λ������汾����0��63
	BIT_CHECK_SET_ELEM(nVersion, 0, nHeader, 0);
	BIT_CHECK_SET_ELEM(nVersion, 1, nHeader, 1);
	BIT_CHECK_SET_ELEM(nVersion, 2, nHeader, 2);
	BIT_CHECK_SET_ELEM(nVersion, 3, nHeader, 3);
	BIT_CHECK_SET_ELEM(nVersion, 4, nHeader, 4);
	BIT_CHECK_SET_ELEM(nVersion, 5, nHeader, 5);
	// 7-8λ�����ݸ�ʽ
	BIT_CHECK_SET_ELEM(nDataFormat, 0, nHeader, 6);
	BIT_CHECK_SET_ELEM(nDataFormat, 1, nHeader, 7);
	// 9-18λ�����ݳ���
	BIT_CHECK_SET_ELEM(nDataLength, 0, nHeader, 8);
	BIT_CHECK_SET_ELEM(nDataLength, 1, nHeader, 9);
	BIT_CHECK_SET_ELEM(nDataLength, 2, nHeader, 10);
	BIT_CHECK_SET_ELEM(nDataLength, 3, nHeader, 11);
	BIT_CHECK_SET_ELEM(nDataLength, 4, nHeader, 12);
	BIT_CHECK_SET_ELEM(nDataLength, 5, nHeader, 13);
	BIT_CHECK_SET_ELEM(nDataLength, 6, nHeader, 14);
	BIT_CHECK_SET_ELEM(nDataLength, 7, nHeader, 15);
	BIT_CHECK_SET_ELEM(nDataLength, 8, nHeader, 16);
	BIT_CHECK_SET_ELEM(nDataLength, 9, nHeader, 17);
	// 19-21λ������ȼ�
	BIT_CHECK_SET_ELEM(nEcLevel, 0, nHeader, 18);
	BIT_CHECK_SET_ELEM(nEcLevel, 1, nHeader, 19);
	BIT_CHECK_SET_ELEM(nEcLevel, 2, nHeader, 20);
	// 22-24λ������ģʽ
	BIT_CHECK_SET_ELEM(nMaskingNo, 0, nHeader, 21);
	BIT_CHECK_SET_ELEM(nMaskingNo, 1, nHeader, 22);
	BIT_CHECK_SET_ELEM(nMaskingNo, 2, nHeader, 23);

	// ����ʱ���
#ifdef _DEBUG
	TRACE("* Encoded dataheader:\n");
	for (int i = 0; i < 24; ++i)
	{
		TRACE("%d, ", nHeader[i]);
	}
	TRACE("\n");
#endif

	// 24bits�ϲ�Ϊ12������
	MergedIntBy2Bits(nHeader, 24);
	// ������ͷ���б���
	int temp[3], encoded_header[72];
	reedSolomon rs(2, 1);
	for (int j = 0; j < 12; ++j)
	{
		ZeroMemory(temp, 3 * sizeof(int));
		temp[0] = nHeader[j];
		rs.rs_encode(temp);
		for (int i = 0; i < 3; ++i)
		{
			encoded_header[6 * j + 2 * i    ] = 0x00000001 &  temp[i];
			encoded_header[6 * j + 2 * i + 1] = 0x00000001 & (temp[i]>>1);
		}
	}
	// ����ͷ������
	int HEADER_MASKINGS[72] = {
		0, 0, 1, 0, 0, 1, 0, 0, 1,
		0, 0, 1, 0, 0, 1, 0, 0, 1,
		0, 0, 1, 0, 0, 1, 0, 0, 1,
		0, 0, 1, 0, 0, 1, 0, 0, 1,
		0, 0, 1, 0, 0, 1, 0, 0, 1,
		0, 0, 1, 0, 0, 1, 0, 0, 1,
		0, 0, 1, 0, 0, 1, 0, 0, 1,
		0, 0, 1, 0, 0, 1, 0, 0, 1};
	XOR(encoded_header, HEADER_MASKINGS, 72);
	// ���½�
	int k = 0;
	for (int j = 0; j < 7; ++j)
	{
		ELEM_CHECK_SET(encoded_header, k++, nCodes, j, nSymbolSize - 7, COLOR_TAG);
		ELEM_CHECK_SET(encoded_header, k++, nCodes, j, nSymbolSize - 6, COLOR_TAG);
	}
	for (int i = nSymbolSize - 5; i < nSymbolSize; ++i)
	{
		ELEM_CHECK_SET(encoded_header, k++, nCodes, 5, i, COLOR_TAG);
		ELEM_CHECK_SET(encoded_header, k++, nCodes, 6, i, COLOR_TAG);
	}
	// ���Ͻ�
	for (int j = nSymbolSize - 7; j < nSymbolSize; ++j)
	{
		ELEM_CHECK_SET(encoded_header, k++, nCodes, j, nSymbolSize - 7, COLOR_TAG);
		ELEM_CHECK_SET(encoded_header, k++, nCodes, j, nSymbolSize - 6, COLOR_TAG);
	}
	for (int i = nSymbolSize - 5; i < nSymbolSize; ++i)
	{
		ELEM_CHECK_SET(encoded_header, k++, nCodes, nSymbolSize - 7, i, COLOR_TAG);
		ELEM_CHECK_SET(encoded_header, k++, nCodes, nSymbolSize - 6, i, COLOR_TAG);
	}
	// ���Ͻ�
	for (int j = nSymbolSize - 7; j < nSymbolSize; ++j)
	{
		ELEM_CHECK_SET(encoded_header, k++, nCodes, j, 6, COLOR_TAG);
		ELEM_CHECK_SET(encoded_header, k++, nCodes, j, 5, COLOR_TAG);
	}
	for (int i = 4; i >= 0; --i)
	{
		ELEM_CHECK_SET(encoded_header, k++, nCodes, nSymbolSize - 7, i, COLOR_TAG);
		ELEM_CHECK_SET(encoded_header, k++, nCodes, nSymbolSize - 6, i, COLOR_TAG);
	}
	ASSERT(k == 72);

	// ����ʱ���
#ifdef _DEBUG
	TRACE("* Encoded dataheader bits:\n");
	for (int i = 0; i < 72; ++i)
	{
		TRACE("%d, ", encoded_header[i]);
	}
	TRACE("\n");
#endif
}


/** 
* @brief ��������. 
* @details �ú������ַ������б���. 
* @param[out] nCodes[81][81] ��ά��
* @param[in] nSymbolSize ���Ŵ�С
* @param[in] *code_bits ���������
* @param[in] bitsLen ������
* @param[in] nMaskingNo ����ģʽ
* @return ��
* @retval void
*/
void EncodeData(int nCodes[81][81], int nSymbolSize, int *code_bits, int bitsLen, int nMaskingNo)
{
	CDataMask *Mask = NULL;
	CDataMask0 Mask0;
	CDataMask1 Mask1;
	CDataMask2 Mask2;
	CDataMask3 Mask3;
	CDataMask4 Mask4;
	CDataMask5 Mask5;
	CDataMask6 Mask6;
	CDataMask7 Mask7;
	switch (nMaskingNo)
	{
	case 0 : Mask = &Mask0; break;
	case 1 : Mask = &Mask1; break;
	case 2 : Mask = &Mask2; break;
	case 3 : Mask = &Mask3; break;
	case 4 : Mask = &Mask4; break;
	case 5 : Mask = &Mask5; break;
	case 6 : Mask = &Mask6; break;
	case 7 : Mask = &Mask7; break;
	default: Mask = &Mask0; break;
	}
	int k = 0;
	srand(unsigned(time(0)));
	// ��߲���
	for (int j = 7; j < nSymbolSize - 7; ++j)
	{
		for (int i = 0; i < 7; ++i)
		{
			if (k < bitsLen)
			{
				ELEM_CHECK_SET(code_bits, k++, nCodes, j, i, COLOR_TAG);
				if (Mask->IsMasked(i, j))
					nCodes[j][i] = nCodes[j][i] ^ 1;
			}
			else
			{
				nCodes[j][i] = rand() & 1;
			}
		}
	}
	// �м䲿��
	for (int j = 0; j < nSymbolSize; ++j)
	{
		for (int i = 7; i < nSymbolSize - 7; ++i)
		{
			if (k < bitsLen)
			{
				ELEM_CHECK_SET(code_bits, k++, nCodes, j, i, COLOR_TAG);
				if (Mask->IsMasked(i, j))
					nCodes[j][i] = nCodes[j][i] ^ 1;
			}
			else
			{
				nCodes[j][i] = rand() & 1;
			}
		}
	}
	// �ұ߲���
	for (int j = 7; j < nSymbolSize - 7; ++j)
	{
		for (int i = nSymbolSize - 7; i < nSymbolSize; ++i)
		{
			if (k < bitsLen)
			{
				ELEM_CHECK_SET(code_bits, k++, nCodes, j, i, COLOR_TAG);
				if (Mask->IsMasked(i, j))
					nCodes[j][i] = nCodes[j][i] ^ 1;
			}
			else
			{
				nCodes[j][i] = rand() & 1;
			}
		}
	}
}


/** 
* @brief �������ζ�ά��. 
* @details 
* @param[out] nCodes[81][81] ������
* @param[in] *pUtf8 ��������
* @param[in] nVersion ����汾
* @param[in] nDataFormat ���ݸ�ʽ
* @param[in] nDataLength ���ݳ���
* @param[in] nEcLevel ����ȼ�
* @param[in] nMaskingNo ����ģʽ
* @return �������ط��Ŵ�С�����ʧ�ܷ���-1.
* @retval int
*/
int EncodeInvisibleCode(int nCodes[81][81], char *pUtf8, int nVersion, int nDataFormat, int nDataLength, int nEcLevel, int nMaskingNo)
{
	int nSymbolSize = nVersion + 18; // ���Ŵ�С
	int bitsLen = RS4BitsLen(nDataLength, nEcLevel); // ��������
	int *code_bits = RS4Encode((BYTE*)pUtf8, nDataLength, bitsLen, nEcLevel);
	int max_len = nSymbolSize * nSymbolSize - 196; // �ɱ��������
	// �����������ܳ�����������
	int res = max_len - bitsLen;
	if (res < 0)
	{
		nSymbolSize = AutoVersion(bitsLen);
		max_len = nSymbolSize * nSymbolSize - 196;
		res = max_len - bitsLen;
	}
	// ͳһ����Ϊ����ɫ
	for (int i = 0; i < nSymbolSize; ++i)
	{
		for (int j = 0; j < nSymbolSize; ++j)
		{
			nCodes[i][j] = BACKGROUND_TAG;
		}
	}
	// *** ����λ��Ԫ *** //
	EncodePositionElem(nCodes, nSymbolSize);
	// *** ��������ͷ(24 bits) *** //
	EncodeDataHeader(nCodes, nSymbolSize, nVersion, nDataFormat, nDataLength, nEcLevel, nMaskingNo);
	// *** �������� *** //
	EncodeData(nCodes, nSymbolSize, code_bits, bitsLen, nMaskingNo);
	SAFE_DELETE(code_bits);
	return nSymbolSize;
}


/** 
* @brief ȷ�����Ű汾. 
* @details 
* @param[in] nBitsLength ��Ҫ����ı�����
* @return ��ά��汾
* @retval int
*/
int AutoVersion(int nBitsLength)
{
	// ��ʽ��S*S - 196 >= N
	return int (sqrt(196 + nBitsLength) + 1);
}


/** 
* @brief �ɶ�ά����󴴽�ͼ��. 
* @details 
* @param[out] *pImage Ŀ��ͼ��
* @param[in] nCodes[81][81] ��ά�����
* @param[in] nSymbolSize ���Ŵ�С
* @param[in] cPositionElem ��λԪ
* @param[in] cColorElem ��ɫԪ
* @param[in] cBackgroundElem ����Ԫ
* @param[in] nPixelSiz ���ش�С
* @return ��������ͼ��Ĵ�С. 
* @retval int
*/
int Convert2Image(CyImage *pImage, int nCodes[81][81], int nSymbolSize, COLORREF cPositionElem, COLORREF cColorElem, COLORREF cBackgroundElem, int nPixelSize)
{
	int nImageSize = nPixelSize * nSymbolSize + QR_MARGIN * 2;
	pImage->Create(nImageSize, nImageSize, 24, 0UL);
	BYTE* pHead = pImage->GetHeadAddress();
	int nRowlen = pImage->GetRowlen();
	int nChannel = pImage->GetChannel();
	memset(pHead, 255, nImageSize * nRowlen * sizeof(BYTE));
	for (int j = 0; j < nSymbolSize; ++j)
	{
		for (int i = 0; i < nSymbolSize; ++i)
		{
			switch (nCodes[j][i])
			{
			case POSITION_TAG:
				SetPixel(pHead, nPixelSize, j * nPixelSize, i * nPixelSize, nRowlen, nChannel, cPositionElem);
				break;
			case COLOR_TAG:
				SetPixel(pHead, nPixelSize, j * nPixelSize, i * nPixelSize, nRowlen, nChannel, cColorElem);
				break;
			case BACKGROUND_TAG:
				SetPixel(pHead, nPixelSize, j * nPixelSize, i * nPixelSize, nRowlen, nChannel, cBackgroundElem);
				break;
			default:
				break;
			}
		}
	}
	
	return nImageSize;
}


/** 
* @brief ����λ��̽��ͼ��. 
* @details ������ͼ���rect�����������һ������Բ. 
* @param[in] * pHead ͼ������
* @param[in] nWidth ͼ����
* @param[in] nHeight ͼ��߶�
* @param[in] nRowlen ÿ���ֽ���
* @param[in] nChannel ͼ��ͨ��
* @param[in] rect��	��������
* @param[in] R ����ɫR����
* @param[in] G ����ɫG����
* @param[in] B ����ɫB����
* @param[in] nPixelSize ���ش�С
* @return ��
* @retval void
*/
void PaintPositionPartten(BYTE* pHead, int nWidth, int nHeight, int nRowlen, int nChannel, CMyRect rect, int R, int G, int B, int nPixelSize)
{
	int rtWidth = rect.Width();
	if (rtWidth != rect.Height())
		return;
	float r = rtWidth / 2.0f;
	float Cx = (rect.left + rect.right) / 2.0f;
	float Cy = (rect.top + rect.bottom) / 2.0f;
	for (int j = rect.top; j < rect.bottom; ++j)
	{
		for (int i = rect.left; i < rect.right; ++i)
		{
			float x = i + 0.5f - Cx, y = j + 0.5f - Cy;
			float dis = sqrt(x * x + y * y);
			if (dis > r || (1/3.0f * r < dis && dis < 2/3.0f * r))
			{
				pHead[    i * nChannel + j * nRowlen] = B;
				pHead[1 + i * nChannel + j * nRowlen] = G;
				pHead[2 + i * nChannel + j * nRowlen] = R;
			}
		}
	}
}


/** 
* @brief �Ƿ�ɱ���. 
* @details �жϵ�ǰ�汾�Ƿ���Ա��뵱ǰ���ȵ�����. 
* @param[in] nSymbolSize ���Ŵ�С
* @param[in] nBitsLen ���������
* @return TRUE or FALSE
* @retval BOOL
*/
BOOL CodeAble(int nSymbolSize, int nBitsLen)
{
	// �ɱ��������
	int max_len = nSymbolSize * nSymbolSize - 196;
	// �����������ܳ�����������
	return max_len >= nBitsLen;
}