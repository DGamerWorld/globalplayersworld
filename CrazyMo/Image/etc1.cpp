#include "../Platform.h"

#define ETC1_ENCODED_BLOCK_SIZE 8
#define ETC1_DECODED_BLOCK_SIZE 48

const int32 kModifierTable[] =
{
/* 0 */ 2,   8,  -2,   -8,
/* 1 */ 5,  17,  -5,  -17,
/* 2 */ 9,  29,  -9,  -29,
/* 3 */13,  42, -13,  -42,
/* 4 */18,  60, -18,  -60,
/* 5 */24,  80, -24,  -80,
/* 6 */33, 106, -33, -106,
/* 7 */47, 183, -47, -183
};
const int32 kLookup[8] = {0, 1, 2, 3, -4, -3, -2, -1};

inline uint8 clamp(int32 x)
{
	return (uint32)(x >= 0? (x < 0xff? x: 0xff): 0);
}
inline int32 convert4To8(int32 b)
{
	int32 c = b & 0xf;
	return (c << 4) | c;
}
inline int32 convert5To8(int32 b)
{
	int c = b & 0x1f;
	return (c << 3) | (c >> 2);
}
inline int32 convertDiff(int32 base, int32 diff)
{
	return convert5To8((0x1f & base) + kLookup[0x7 & diff]);
}
uint32 etc1_decode_image(uint8 decodeData[], const uint8 encodeData[], uint16 imgWide, uint16 imgHigh, uint8 pixelSize, uint32 stride)
{
	uint8 block[ETC1_DECODED_BLOCK_SIZE];
	uint32 encodedWide = (imgWide + 3) & ~3, encodedHigh = (imgHigh + 3) & ~3;
	uint32 yEnd, xEnd, high, loww;
	int32 r1, r2, g1, g2, b1, b2, baseX, baseY;
	for (uint32 h=0; h<encodedHigh; h+=4)
	{
		yEnd = imgHigh - h;
		if (yEnd > 4) yEnd = 4;

		for (uint32 w=0; w<encodedWide; w+=4)
		{
			xEnd = imgWide - w;
			if (xEnd > 4) xEnd = 4;

			high = (encodeData[0] << 24) | (encodeData[1] << 16) | (encodeData[2] << 8) | encodeData[3];
			loww = (encodeData[4] << 24) | (encodeData[5] << 16) | (encodeData[6] << 8) | encodeData[7];
			if (high & 2)
			{
				// differential
				int rBase = high >> 27;
				int gBase = high >> 19;
				int bBase = high >> 11;
				r1 = convert5To8(rBase);
				r2 = convertDiff(rBase, high >> 24);
				g1 = convert5To8(gBase);
				g2 = convertDiff(gBase, high >> 16);
				b1 = convert5To8(bBase);
				b2 = convertDiff(bBase, high >> 8);
			}else {
				// not differential
				r1 = convert4To8(high >> 28);
				r2 = convert4To8(high >> 24);
				g1 = convert4To8(high >> 20);
				g2 = convert4To8(high >> 16);
				b1 = convert4To8(high >> 12);
				b2 = convert4To8(high >> 8);
			}
			int tableIndexA = 7 & (high >> 5);
			int tableIndexB = 7 & (high >> 2);
			const int32* tableA = kModifierTable + tableIndexA * 4;
			const int32* tableB = kModifierTable + tableIndexB * 4;
			bool flipped = (high & 1) != 0;

			int x, y, i;
			baseX = 0; baseY = 0;
			for (i=0; i<8; ++i)
			{
				if (flipped)
				{
					x = baseX + (i >> 1);
					y = baseY + (i & 1);
				}else {
					x = baseX + (i >> 2);
					y = baseY + (i & 3);
				}
				int k = y + (x * 4);
				int offset = ((loww >> k) & 1) | ((loww >> (k + 15)) & 2);
				int delta = tableA[offset];
				uint8* q = block + 3 * (x + 4 * y);
				*q++ = clamp(r1 + delta);
				*q++ = clamp(g1 + delta);
				*q++ = clamp(b1 + delta);
			}
			if (flipped)	{baseX = 0; baseY = 2;}
			else			{baseX = 2; baseY = 0;}
			for (i=0; i<8; ++i)
			{
				if (flipped)
				{
					x = baseX + (i >> 1);
					y = baseY + (i & 1);
				}else {
					x = baseX + (i >> 2);
					y = baseY + (i & 3);
				}
				int k = y + (x * 4);
				int offset = ((loww >> k) & 1) | ((loww >> (k + 15)) & 2);
				int delta = tableB[offset];
				uint8* q = block + 3 * (x + 4 * y);
				*q++ = clamp(r2 + delta);
				*q++ = clamp(g2 + delta);
				*q++ = clamp(b2 + delta);
			}
			encodeData += ETC1_ENCODED_BLOCK_SIZE;

			for (uint32 cy=0; cy<yEnd; ++cy)
			{
				const uint8* q = block + (cy * 4) * 3;
				uint8* p = decodeData + pixelSize*w + stride*(h + cy);
				if (pixelSize == 3)
				{
					memcpy(p, q, xEnd * 3);
				}else {
					for (uint32 cx=0; cx<xEnd; ++cx)
					{
						uint8 r = *q++;
						uint8 g = *q++;
						uint8 b = *q++;
						uint32 pixel = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
						*p++ = (uint8)pixel;
						*p++ = (uint8)(pixel >> 8);
					}
				}
			}
		}
	}
	return 0;
}

// Check if a PKM header is correctly formatted.
#define ETC1_PKM_FORMAT_OFFSET					6
#define ETC1_PKM_ENCODED_WIDE_OFFSET			8
#define ETC1_PKM_ENCODED_HIGH_OFFSET			10
#define ETC1_PKM_WIDE_OFFSET					12
#define ETC1_PKM_HIGH_OFFSET					14
#define ETC1_RGB_NO_MIPMAPS						0
inline uint32 readBEUint16(const uint8 pIn[])
{
	return (pIn[0] << 8) | pIn[1];
}
bool etc1_pkm_is_valid(uint16& Wide, uint16& High, const uint8 pIn[])
{
	if ((pIn[0] != 'P') || (pIn[1] != 'K') || (pIn[2] != 'M') || (pIn[3] != ' ') || (pIn[4] != '1') || (pIn[5] != '0')) return false;

	uint32 format = readBEUint16(pIn + ETC1_PKM_FORMAT_OFFSET);
	uint32 encodedWide = readBEUint16(pIn + ETC1_PKM_ENCODED_WIDE_OFFSET);
	uint32 encodedHigh = readBEUint16(pIn + ETC1_PKM_ENCODED_HIGH_OFFSET);
	Wide = readBEUint16(pIn + ETC1_PKM_WIDE_OFFSET);
	High = readBEUint16(pIn + ETC1_PKM_HIGH_OFFSET);
	return	(format == ETC1_RGB_NO_MIPMAPS) &&
			(encodedWide >= Wide) && (encodedWide - Wide < 4) &&
			(encodedHigh >= High) && (encodedHigh - High < 4);
}
