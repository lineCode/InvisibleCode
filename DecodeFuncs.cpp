#include "stdafx.h"
#include "DecodeFuncs.h"
#include "..\3DCode\ColorsMask.h"
#include "..\3DCode\reedSolomon.h"
#include "..\3DCode\templatefuncs.h"
#include "..\3DCode\Transform.h"
#include "..\3DCode\K_Means.h"
#include "..\3DCode\PixelsOperation.h"
#include "..\3DCode\RS_ecc.h"

/** ����ͼ�񵽸óߴ���н��� */
#define IMAGE_ZOOM_TO 320

/** 
* @brief ���p��A�ľ������������TRUE������FALSE. 
* @param[in] p ��ȷ�ϵ�
* @param[in] A A��
* @param[in] B B��
* @return TRUE/FALSE
* @retval BOOL
*/
BOOL CLUSTER_FUN(float3 &p, float3 &A, float3 &B) 
{
	BOOL result = (_Distance(p, A) < _Distance(p, B)) ? COLOR_TAG : BACKGROUND_TAG;
	return result;
}


/** 
* @brief Ѱ��ģ���Ԫ�صı��. 
* @param[in] a ��һ��Ԫ��
* @param[in] b �ڶ���Ԫ��
* @param[in] c ������Ԫ��
* @return �ĸ�Ԫ�ص�ģ���ͷ�������. 
* @retval int
*/
int FindMaxmumIndex(float2 a, float2 b, float2 c)
{
	if (fabs(a) > fabs(b))
	{
		return fabs(a) > fabs(c) ? 1 : 3;
	}
	else
	{
		return fabs(b) > fabs(c) ? 2 : 3;
	}
}


/** 
* @brief ��ȡ��ά��ߴ�. 
* @details �����½ǵ����ϡ����½����ĵľ���Ϊd1;
* �����Ͻǵ����ϡ����½����ĵľ���Ϊd2; ��������:
* [(x+4)^2 + 1] * s^2 = d1^2
* (x+5) * s = d2
* �õ���(1-r)x^2 + (8-10r)x + (17-25r) = 0, r = d1^2 / d2^2
* ��Ϊ��ax^2 + bx + c = 0
* ���У�sΪģ���С
* @param[in] centers �������ĵ�
* @return ����ģ���С. 
*/
int GetBarCodeSize(float2 centers[4])
{
	float d1 = (Distance(centers[0], centers[1]) + Distance(centers[0], centers[3])) / 2.0f;
	float d2 = (Distance(centers[2], centers[1]) + Distance(centers[2], centers[3])) / 2.0f;
	float r = d1 * d1 / d2 / d2;
	float a = 1 - r, b = 8 - 10 * r, c = 17 - 25 * r;
	float delta = b * b - 4 * a * c;
	if (delta < 0)
		return 0;
	float x1 = (-b + sqrt(delta)) / (2 * a);
	float x2 = (-b - sqrt(delta)) / (2 * a);
	float temp = max(x1, x2);
	int nModuleSize = (int)round(10.0f + temp);
	if (nModuleSize < 18 || nModuleSize > 81)
	{
		TRACE(" * Warning : Barcode size exception\n");
	}

	return nModuleSize;
}


/** 
* @brief ��������ͷ. 
* @details �ú�����������ͷ. 
* @param[in] *pHead ͼ��ͷָ��
* @param[in] nPixelSize ���ش�С
* @param[in] nRowlen ͼ��ÿ��Ԫ�ظ���
* @param[in] nChannel ͼ��Ԫ��ͨ����
* @param[in] cColorElem ��ɫֵ
* @param[in] cBackgroundElem ����ֵ
* @param[in] nSymbolSize ��ά��ߴ�
* @param[out] &nVersion �汾��С
* @param[out] &nDataFormat ���ݸ�ʽ
* @param[out] &nDataLength ���ݳ���
* @param[out] &nEcLevel ����ȼ�
* @param[out] &nMaskingNo ����ģʽ
* @return ����
* @retval BOOL
*/
BOOL DecodeDataHeader(BYTE *pHead, int nPixelSize, int nRowlen, int nChannel, 
					  COLORREF cColorElem, COLORREF cBackgroundElem, int nSymbolSize, 
					  int &nVersion, int &nDataFormat, int &nDataLength, int &nEcLevel, int &nMaskingNo)
{
	// ��ɫ�ͱ���ɫתΪfloat3
	float3 fColorElem = RgbColorRef2Float3(cColorElem);
	float3 fBackgroundElem = RgbColorRef2Float3(cBackgroundElem);
	int decoded_header[72] = {0};

	// ���½�
	int k = 0;
	for (int j = 0; j < 7; ++j)
	{
		decoded_header[k++] = CLUSTER_FUN(GetPixel(pHead, nPixelSize, j * nPixelSize, (nSymbolSize - 7) * nPixelSize, nRowlen, nChannel), fColorElem, fBackgroundElem);
		decoded_header[k++] = CLUSTER_FUN(GetPixel(pHead, nPixelSize, j * nPixelSize, (nSymbolSize - 6) * nPixelSize, nRowlen, nChannel), fColorElem, fBackgroundElem);
	}
	for (int i = nSymbolSize - 5; i < nSymbolSize; ++i)
	{
		decoded_header[k++] = CLUSTER_FUN(GetPixel(pHead, nPixelSize, 5 * nPixelSize, i * nPixelSize, nRowlen, nChannel), fColorElem, fBackgroundElem);
		decoded_header[k++] = CLUSTER_FUN(GetPixel(pHead, nPixelSize, 6 * nPixelSize, i * nPixelSize, nRowlen, nChannel), fColorElem, fBackgroundElem);
	}
	// ���Ͻ�
	for (int j = nSymbolSize - 7; j < nSymbolSize; ++j)
	{
		decoded_header[k++] = CLUSTER_FUN(GetPixel(pHead, nPixelSize, j * nPixelSize, (nSymbolSize - 7) * nPixelSize, nRowlen, nChannel), fColorElem, fBackgroundElem);
		decoded_header[k++] = CLUSTER_FUN(GetPixel(pHead, nPixelSize, j * nPixelSize, (nSymbolSize - 6) * nPixelSize, nRowlen, nChannel), fColorElem, fBackgroundElem);
	}
	for (int i = nSymbolSize - 5; i < nSymbolSize; ++i)
	{
		decoded_header[k++] = CLUSTER_FUN(GetPixel(pHead, nPixelSize, (nSymbolSize - 7) * nPixelSize, i * nPixelSize, nRowlen, nChannel), fColorElem, fBackgroundElem);
		decoded_header[k++] = CLUSTER_FUN(GetPixel(pHead, nPixelSize, (nSymbolSize - 6) * nPixelSize, i * nPixelSize, nRowlen, nChannel), fColorElem, fBackgroundElem);
	}
	// ���Ͻ�
	for (int j = nSymbolSize - 7; j < nSymbolSize; ++j)
	{
		decoded_header[k++] = CLUSTER_FUN(GetPixel(pHead, nPixelSize, j * nPixelSize, 6 * nPixelSize, nRowlen, nChannel), fColorElem, fBackgroundElem);
		decoded_header[k++] = CLUSTER_FUN(GetPixel(pHead, nPixelSize, j * nPixelSize, 5 * nPixelSize, nRowlen, nChannel), fColorElem, fBackgroundElem);
	}
	for (int i = 4; i >= 0; --i)
	{
		decoded_header[k++] = CLUSTER_FUN(GetPixel(pHead, nPixelSize, (nSymbolSize - 7) * nPixelSize, i * nPixelSize, nRowlen, nChannel), fColorElem, fBackgroundElem);
		decoded_header[k++] = CLUSTER_FUN(GetPixel(pHead, nPixelSize, (nSymbolSize - 6) * nPixelSize, i * nPixelSize, nRowlen, nChannel), fColorElem, fBackgroundElem);
	}
	ASSERT(72 == k);

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
	XOR(decoded_header, HEADER_MASKINGS, 72);

	// ����ʱ���
#ifdef _DEBUG
	TRACE("* Decoded dataheader bits:\n");
	for (int i = 0; i < 72; ++i)
	{
		TRACE("%d, ", decoded_header[i]);
	}
	TRACE("\n");
#endif

	// 72b��2bit�ϲ���36������(3��һ����н���)
	MergedIntBy2Bits(decoded_header, 72);
	// ������ͷ���н���
	int nHeader[24] = {0};
	int temp[3];
	reedSolomon rs(2, 1);
	for (int j = 0; j < 12; ++j)
	{
		memcpy(temp, decoded_header + 3 * j, 3 * sizeof(int));
		rs.rs_decode(temp);
		nHeader[2 * j    ] = 0x00000001 &  temp[2];
		nHeader[2 * j + 1] = 0x00000001 & (temp[2]>>1);
	}

#ifdef _DEBUG
	TRACE("* Decoded dataheader:\n");
	for (int i = 0; i < 24; ++i)
	{
		TRACE("%d, ", nHeader[i]);
	}
	TRACE("\n");
#endif

	// 1-6λ������汾����0��63
	nVersion = nHeader[0] + 2 * nHeader[1] + 4 * nHeader[2] + 8 * nHeader[3] + 16 * nHeader[4] + 32 * nHeader[5];
	// 7-8λ�����ݸ�ʽ����0��3
	nDataFormat = nHeader[6] + 2 * nHeader[7];
	// 9-18λ�����ݳ��ȣ���0��1023
	nDataLength = nHeader[8] + 2 * nHeader[9] + 4 * nHeader[10] + 8 * nHeader[11] + 16 * nHeader[12] + 32 * nHeader[13] 
					+ 64 * nHeader[14] + 128 * nHeader[15] + 256 * nHeader[16] + 512 * nHeader[17];
	// 19-21λ������ȼ�����1��7
	nEcLevel = nHeader[18] + 2 * nHeader[19] + 4 * nHeader[20];
	// 22-24λ������ģʽ����0��7
	nMaskingNo = nHeader[21] + 2 * nHeader[22] + 4 * nHeader[23];
	// �ж��Ƿ�ɹ�
	if (0 == nDataLength * nEcLevel)
		return FALSE;
	int nMaxDataLength = nSymbolSize * nSymbolSize - 4 * 49;
	if(nDataLength > nMaxDataLength)
		return FALSE;

	return TRUE;
}


/** 
* @brief ��������. 
* @details �ú�����ͼ����н���. 
* @param[in] *pHead ͼ��ͷָ��
* @param[in] nPixelSize ���ش�С
* @param[in] nRowlen ͼ��ÿ��Ԫ�ظ���
* @param[in] nChannel ͼ��Ԫ��ͨ����
* @param[in] cColorElem ��ɫֵ
* @param[in] cBackgroundElem ����ֵ
* @param[in] nSymbolSize ��ά��ߴ�
* @param[out] &nVersion �汾��С
* @param[out] &nDataFormat ���ݸ�ʽ
* @param[out] &nDataLength ���ݳ���
* @param[out] &nEcLevel ����ȼ�
* @param[out] &nMaskingNo ����ģʽ
* @return ����
* @retval BOOL
*/
char* DecodeData(BYTE *pHead, int nPixelSize, int nRowlen, int nChannel, 
				 COLORREF cColorElem, COLORREF cBackgroundElem, int nSymbolSize, 
				 int nVersion, int nDataFormat, int nDataLength, int nEcLevel, int nMaskingNo)
{
	// ��ɫ�ͱ���ɫתΪfloat3
	float3 fColorElem = RgbColorRef2Float3(cColorElem);
	float3 fBackgroundElem = RgbColorRef2Float3(cBackgroundElem);
	// 4bitsΪһ�����ţ���Ϣ������
	int nMsgSymbols = 2 * nDataLength;
	// ��Ϣռ��,һ���ܱ���ķ�����
	int nDataRate = 15 - 2 * nEcLevel;
	// 15������Ϊһ�飬����
	int nBlocks = (int)ceil(1.f * nMsgSymbols / nDataRate);
	// ������Ҫ�ķ�������
	int nRealMsgSymbols = nBlocks * 15;
	// ��ű�������λ����(������ X ÿ���ŵ�λ��)
	int bitsLen = 4 * nRealMsgSymbols;
	int *code_bits = new int[bitsLen];
	memset(code_bits, 0, bitsLen * sizeof(int));

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
	// ��߲���
	for (int j = 7; j < nSymbolSize - 7; ++j)
	{
		for (int i = 0; i < 7; ++i)
		{
			if (k < bitsLen)
			{
				code_bits[k] = CLUSTER_FUN(GetPixel(pHead, nPixelSize, j * nPixelSize, i * nPixelSize, nRowlen, nChannel), fColorElem, fBackgroundElem);
				if (Mask->IsMasked(i, j))
					code_bits[k] ^= 0x00000001;
				k++;
			}
			else
			{
				goto next;
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
				code_bits[k] = CLUSTER_FUN(GetPixel(pHead, nPixelSize, j * nPixelSize, i * nPixelSize, nRowlen, nChannel), fColorElem, fBackgroundElem);
				if (Mask->IsMasked(i, j))
					code_bits[k] ^= 0x00000001;
				k++;
			}
			else
			{
				goto next;
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
				code_bits[k] = CLUSTER_FUN(GetPixel(pHead, nPixelSize, j * nPixelSize, i * nPixelSize, nRowlen, nChannel), fColorElem, fBackgroundElem);
				if (Mask->IsMasked(i, j))
					code_bits[k] ^= 0x00000001;
				k++;
			}
			else
			{
				goto next;
			}
		}
	}
	// ���ݳ��������ˣ����ܽ����Ѿ�ʧ��
	bitsLen = nSymbolSize * nSymbolSize - 196;
	nRealMsgSymbols = bitsLen / 4;
	nBlocks = (int)ceil(nRealMsgSymbols / 15.0);
	nMsgSymbols = nBlocks * nDataRate;
	nDataLength = nMsgSymbols / 2;

next:
	char *ch = new char[nDataLength];
	int *result = RS4Decode(code_bits, nDataLength, bitsLen, nEcLevel);
	for (int i = 0; i < nDataLength; ++i)
	{
		ch[i] = Bits2Byte(result + 8 * i);
	}
	SAFE_DELETE(result);
	SAFE_DELETE(code_bits);

	return ch;
}


/**
* @brief ������ά��ͼ��. 
* @param[in] * pHead ͼ������
* @param[in] nWidth ͼ����
* @param[in] nHeight ͼ��߶�
* @param[in] nRowlen ͼ��ÿ���ֽ���
* @param[in] nChannel ͼ��ͨ������
* @param[in] roi ����Ȥ����
* @param[in] cPositionElem λ��Ԫ
* @param[in] cColorElem ��ɫԪ
* @param[in] cBackgroundElem ����Ԫ
* @param[in] Centers λ��̽��Ԫ����
* @param[in] &nVersion �汾
* @param[in] &nDataFormat ���ݸ�ʽ
* @param[in] &nDataLength ���ݳ���
* @param[in] &nEcLevel ����ȼ�
* @param[in] &nMaskingNo ����汾
*/
char* DecodeImage(BYTE* pHead, int nWidth, int nHeight, int nRowlen, int nChannel, RoiRect roi, 
				 COLORREF cPositionElem, COLORREF cColorElem, COLORREF cBackgroundElem, 
				 float2 Centers[4], int &nVersion, int &nDataFormat, int &nDataLength, int &nEcLevel, int &nMaskingNo)
{
	// ��ȡ����Ȥ����
	BYTE *temp = ImageROI(pHead, nWidth, nHeight, nRowlen, roi);

	// ��ͼ����һ�ߴ���ָ��ֵʱ����
	LimitImage(&temp, nWidth, nHeight, nRowlen, nChannel, IMAGE_ZOOM_TO);

#ifdef _DEBUG
	ImageWrite(".\\ImageROI.txt", temp, nWidth, nHeight, nRowlen);
#endif

	// RGBתGray
	BYTE* gray = Rgb2Gray(temp, nWidth, nHeight, nRowlen);
	int nRowlenNew = WIDTHBYTES(nWidth * 8);

#ifdef _DEBUG
	ImageWrite(".\\Rgb2Gray.txt", gray, nWidth, nHeight, nRowlenNew);
#endif

	// ��ֵ�˲�
	medianFilter(gray, nWidth, nHeight, nRowlenNew, 3);

#ifdef _DEBUG
	ImageWrite(".\\medianFilter.txt", gray, nWidth, nHeight, nRowlenNew);
#endif

	// K-means �����ȡ����
	// a11		a12
	// a21		a22
	K_means(gray, nRowlenNew, RoiRect(0, 0, nWidth/2, nHeight/2), 3, RgbColorRef2Gray(cPositionElem), 10);// a11
	K_means(gray, nRowlenNew, RoiRect(nWidth/2, nHeight/2, nWidth, nHeight), 3, RgbColorRef2Gray(cPositionElem), 10);// a22
	K_means(gray, nRowlenNew, RoiRect(nWidth/2, 0, nWidth, nHeight/2), 3, RgbColorRef2Gray(cPositionElem), 10);// a12
	K_means(gray, nRowlenNew, RoiRect(0, nHeight/2, nWidth/2, nHeight), 3, RgbColorRef2Gray(cPositionElem), 10);// a21

#ifdef _DEBUG
	ImageWrite(".\\K_means.txt", gray, nWidth, nHeight, nRowlenNew);
#endif

	// ��ֵ�˲�
	medianFilter(gray, nWidth, nHeight, nRowlenNew, 7);

#ifdef _DEBUG
	ImageWrite(".\\medianFilter2.txt", gray, nWidth, nHeight, nRowlenNew);
#endif

	// ׼��K��ֵ����
	vector<float2> position;
	for (int j = 0 ; j < nHeight; ++j)
	{
		for (int i = 0; i < nWidth; ++i)
		{
			if (0 == gray[i + j * nRowlenNew])
			{
				float x = i + 0.5f;
				float y = j + 0.5f;
				position.push_back(float2(x, y));
			}
		}
	}
	vector<int> retval = K_means(position, Centers, nWidth, nHeight, 10);
	int nCount = retval.size();
	// ��ԭʼͼ����зָ�
	for (int j = 0; j < nCount; ++j)
	{
		int x = position[j].x;
		int y = position[j].y;
		gray[x + y * nRowlenNew] = (retval[j] + 1) * 64;
	}

#ifdef _DEBUG
	ImageWrite(".\\K_means2.txt", gray, nWidth, nHeight, nRowlenNew);
#endif

	// ͸��У����QR��ߴ�
	int nSymbolSize = GetBarCodeSize(Centers);

	// ͸�ӱ任��ͼ����
	int nDstSize = 0;
	// ÿ��ģ��Ĵ�С
	int nMoudleSize = 16;
	PerspectTransform transform = CreateTransform(Centers, nSymbolSize, (float)nMoudleSize);
	// ��������ΪBYTE*����������ģ��
	BYTE *afterPPT = perspectiveTransform(temp, nWidth, nHeight, nRowlen, nChannel, 
		transform, Centers, nSymbolSize, nMoudleSize, nDstSize);
	int nRowlenTemp = WIDTHBYTES(nDstSize * 8 * nChannel);
#ifdef _DEBUG
	ImageWrite(".\\PerspectTransform.txt", afterPPT, nDstSize, nDstSize, nRowlenTemp);
#endif

	char *result = NULL;
	if (nSymbolSize)
	{
		// ��ת�ǲ����ٵģ��������ʧ��
		ImageTranspose(&afterPPT, nDstSize, nDstSize, nRowlenTemp);
#ifdef _DEBUG
		ImageWrite(".\\ImageTranspose.txt", afterPPT, nDstSize, nDstSize, nRowlenTemp);
#endif
		// ��������ͷ
		if (DecodeDataHeader(afterPPT, nMoudleSize, nRowlenTemp, nChannel, 
			cColorElem, cBackgroundElem, nSymbolSize, 
			nVersion, nDataFormat, nDataLength, nEcLevel, nMaskingNo))
		{
			result = DecodeData(afterPPT, nMoudleSize, nRowlenTemp, nChannel, 
				cColorElem, cBackgroundElem, nSymbolSize, 
				nVersion, nDataFormat, nDataLength, nEcLevel, nMaskingNo);
		}

		SAFE_DELETE(afterPPT);
	}

	SAFE_DELETE(gray);

	SAFE_DELETE(temp);

	return result;
}


/** 
* @brief ��ֵ�˲�. 
* @details ��ֵ�˲��ȽϺ�ʱ.
* @param[in] * pData ͼ������
* @param[in] nWidth ͼ����
* @param[in] nHeight ͼ��߶�
* @param[in] nRowlen ͼ��ÿ���ֽ���
* @param[in] nSize �˲��˳ߴ�
* @warning ���޲�ɫͼ��
* �ڽ�������Ϊ��ʱ.
*/
BOOL medianFilter(BYTE* pData, int nWidth, int nHeight, int nRowlen, int nSize)
{
	// �˲��˳ߴ����Ϊ����
	ASSERT(nSize & 0x00000001);
	BYTE *pCopy = new BYTE[nHeight * nRowlen];
	memcpy(pCopy, pData, nHeight * nRowlen * sizeof(BYTE));
	int nChannel = nRowlen / nWidth;
	ASSERT(nChannel == 1);
	int K = nSize >> 1;// �˲��˰뾶
	BYTE *temp = new BYTE[nSize * nSize];// �����������
	// ������ؽ����˲�(�߽粻���������㲻��Ҫ��Խ���ж�)
	for (int r = K; r < nHeight - K; ++r)
	{
		for (int c = K; c < nWidth - K; ++c)
		{
			int rgbCount = 0;// ��ǰ����(r, c)���������ظ���
			for (int r0 = r - K; r0 <= r + K; ++r0)
			{
				for (int c0 = c - K; c0 <= c + K; ++c0)
				{
					// (r0, c0)����
					BYTE* Pixel = pData + r0*nRowlen + c0*nChannel;
					temp[rgbCount] = *Pixel++;
#if QUICK_SORT
					if (rgbCount > 0)
					{
						// ��С�����������
						for (int j = rgbCount; j > 0 && temp[rgbCount] < temp[rgbCount - 1]; --j)
						{
							BYTE t;
							t = temp[j];
							temp[j] = temp[j - 1];
							temp[j - 1] = t;
						}
					}
#endif
					++rgbCount;
				}
			}
			ASSERT(rgbCount == nSize * nSize);
#if !(QUICK_SORT)
			for (int i = 0; i < (rgbCount>>1); ++i)
			{
				for (int j = i + 1; j < rgbCount; ++j)
				{
					if (temp[j] < temp[i])
					{
						BYTE t;
						t = temp[j];
						temp[j] = temp[j - 1];
						temp[j - 1] = t;
					}
				}
			}
#endif
			rgbCount >>= 1;// ��2,ȡ��ֵ
			BYTE* Pixel = pCopy + r*nRowlen + c*nChannel;
			*Pixel++ = temp[rgbCount];
		}
	}
	SAFE_DELETE(temp);
	memcpy(pData, pCopy, nHeight * nRowlen * sizeof(BYTE));
	SAFE_DELETE(pCopy);
	return TRUE;
}


#if _MSC_VER <= 1800
/** 
* @brief ��������. 
*/
inline float round(float r)
{
	return (r > 0.0f) ? floor(r + 0.5f) : ceil(r - 0.5f);
}
#endif


/** 
* @brief ��ʼ����������(2016/9/19). 
* @param[in] *Center ��������
* @param[in] K �������
* @param[in] position
* @param[in] bTryHard ǿ����ʼ��
* @note ��@ref bTryHardΪTRUEʱ���������������һ��������ģ�ȡ��������������. 
*/
void InitializeClusterCenters(float2 *Center, vector<float2> & position, BOOL bTryHard)
{
	int nCount = position.size();
	// ����4������
	unsigned int seed = (unsigned int)time(NULL);
	for (int i = 0; i < 4; i++)
	{
		srand(seed);
		int id = rand() % nCount;
		Center[i] = position[id];
		TRACE("C[%d] = (%.6f, %.6f)\t", i, Center[i].x, Center[i].y);
		seed = rand();
	}
	TRACE("\n");
	// �ж��Ƿ����
	if (!bTryHard)
		return;
	// �������������ֵ
	float diff = Difference(Center, 4);
	float2 *newCenter = new float2[4];
	// ѭ��24�Σ����ɾ������ģ������Ƿ��и��õ�
	for (int j = 0; j < 24; ++j)
	{
		for (int i = 0; i < 4; i++)
		{
			srand(seed);
			int id = rand() % nCount;
			newCenter[i] = position[id];
			seed = rand();
		}
		float new_diff = Difference(newCenter, 4);
		// ����ҵ����������ľ�������
		if (new_diff > diff)
		{
			diff = new_diff;
			memcpy(Center, newCenter, 4 * sizeof(float2));
			for (int i = 0; i < 4; i++)
			{
				TRACE("C[%d] = (%.6f, %.6f)\t", i, Center[i].x, Center[i].y);
			}
			TRACE("\n");
		}
	}
	delete [] newCenter;
}


/// ������position���о���
vector<int> K_means(vector<float2> & position, float2 centers[4], int nWidth, int nHeight, int nMaxepoches)
{
	// ����������������
	float2 *oldCenter = new float2[4];
	float *sum = new float[4];
	int nCount = position.size();
	int *Cluster = new int[nCount];
	// ����4������
	TRACE("���ڳ�ʼ����������...\n");
	InitializeClusterCenters(centers, position, TRUE);
	/* ��ε���ֱ�������������������nMaxepoches�� */
	for (int it = 0; it < nMaxepoches; ++it)
	{
		/* ���ÿ���������Ӧ��������һ������ */
		for (int j = 0; j < nCount; ++j)
		{
			/* ����ʼ�����ڵ�0������ */    
			int c = 0;
			float min_distance = _Distance(position[j], centers[c]);
			for (int s = 1; s < 4; s++)
			{
				float new_distance = _Distance(position[j], centers[s]);
				if (new_distance < min_distance)
				{
					min_distance = new_distance;
					c = s;
				}
			}
			// ��������Ϊc��
			Cluster[j] = c;
		}
		/* ���¾������� */
		memcpy(oldCenter, centers, 4 * sizeof(float2));
		ZeroMemory(sum, 4 * sizeof(float));
		ZeroMemory(centers, 4 * sizeof(float2));
		for (int j = 0; j < nCount; ++j)
		{
			centers[Cluster[j]] += position[j];
			sum[Cluster[j]]++;
		}
		float diff = 0;
		for (int i = 0; i < 4; ++i)
		{
			if (sum[i])
			{
				centers[i] /= sum[i];
			}
			diff += fabs(oldCenter[i] - centers[i]);
		}
		// ע������������Լ��ӵ�������
		TRACE(" * �������� = %d��\n", it + 1);
		for (int i = 0; i < 4; ++i)
		{
			TRACE("C[%d] = (%.6f, %.6f)\t%.1f��\t", i, centers[i].x, centers[i].y, sum[i]);
		}
		TRACE("\n");
		// ��ǰ�������������ı仯��Сʱ��0.01�����أ��˳�
		if (diff < 0.0004f)
			break;
	}
	// ʹ��������������������ڵ�һ
	for (int i = 1; i < 4; ++i)
	{
		if (sum[i] > sum[0])
		{
			float temp(sum[0]);
			sum[0] = sum[i];
			sum[i] = temp;
			float2 temp2(centers[0]);
			centers[0] = centers[i];
			centers[i] = temp2;
		}
	}
	SortClusterCenters(centers);
	vector<int> retval(Cluster, Cluster + nCount);
	delete [] oldCenter;
	delete [] sum;
	delete [] Cluster;
	return retval;
}


/** 
* @brief �Ծ����������. 
* @details ������c0-���¡�c1-���¡�c2���ϡ�c3����.
*/
void SortClusterCenters(float2 centers[4])
{
	// Ѱ�����Ͻǵĵ�
	float2 vec[3];
	vec[0] = centers[1] - centers[0];
	vec[1] = centers[2] - centers[0];
	vec[2] = centers[3] - centers[0];
	int maxmumIndex = FindMaxmumIndex(vec[0], vec[1], vec[2]);
	// ����λ��
	if (maxmumIndex != 2)
	{
		float2 temp(centers[2]);
		centers[2] = centers[maxmumIndex];
		centers[maxmumIndex] = temp;
	}
	// �������Ͻǡ����½�������
	if (acos(vec[2].y / fabs(vec[2])) - acos(vec[0].y / fabs(vec[0])) < 0)
	{
		float2 temp(centers[1]);
		centers[1] = centers[3];
		centers[3] = temp;
	}
}


/** 
* @brief �ο���ZXing - Detector
* @param[in] pos λ��̽��Ԫ
* @param[in] dimension QR��ߴ�
* @param[in] fmodulesize ģ���
*/
PerspectTransform CreateTransform(float2 pos[4], int dimension, float fmodulesize)
{
	float dimMinusTwo = (dimension - 2.5f) * fmodulesize;

	return PerspectTransform::quadrilateralToQuadrilateral(
		3.5f * fmodulesize, 3.5f * fmodulesize, 
		dimMinusTwo, 2.5f * fmodulesize, 
		dimMinusTwo, dimMinusTwo, 
		2.5f * fmodulesize, dimMinusTwo, 
		pos[0].x, pos[0].y, 
		pos[1].x, pos[1].y, 
		pos[2].x, pos[2].y, 
		pos[3].x, pos[3].y);
}