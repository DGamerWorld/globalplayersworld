#include "../Platform.h"
#include "../Image.h"

//Decode ATITC encode block to 4x4 RGB32 pixels
static void atitc_decode_block(uint8 **blockData,
                              uint32 *decodeBlockData,
                              uint32 stride,
                              bool oneBitAlphaFlag,
                              uint64 alpha,
							  ATITCDECODEFLAG::ATITCDecodeFlag decodeFlag)
{
	uint32 colorValue0 = 0 , colorValue1 = 0, initAlpha = (!oneBitAlphaFlag * 255u) << 24;
	uint32 rb0 = 0, rb1 = 0, rb2 = 0, rb3 = 0, g0 = 0, g1 = 0, g2 = 0, g3 = 0;
	bool msb = 0;
	
	uint32 colors[4], pixelsIndex = 0;
	
	/* load the two color values*/
	memcpy((void *)&colorValue0, *blockData, 2);
	(*blockData) += 2;
	
	memcpy((void *)&colorValue1, *blockData, 2);
	(*blockData) += 2;
	
	//extract the msb flag
	msb = (colorValue0 & 0x8000) != 0;
	
	/* the channel is r5g6b5 , 16 bits */
	rb0  = (colorValue0 << 3 | colorValue0 << 9) & 0xf800f8;
	rb1  = (colorValue1 << 3 | colorValue1 << 8) & 0xf800f8;
	g0   = (colorValue0 << 6) & 0x00fc00;
	g1   = (colorValue1 << 5) & 0x00fc00;
	g0  += (g0 >> 6) & 0x000300;
	g1  += (g1 >> 6) & 0x000300;
	
	/* interpolate the other two color values */
	if (!msb)
	{
		colors[0] = rb0 + g0 + initAlpha;
		colors[3] = rb1 + g1 + initAlpha;
		
		rb2 = (((2*rb0 + rb1) * 21) >> 6) & 0xff00ff;
		rb3 = (((2*rb1 + rb0) * 21) >> 6) & 0xff00ff;
		g2  = (((2*g0 + g1 ) * 21) >> 6) & 0x00ff00;
		g3  = (((2*g1 + g0 ) * 21) >> 6) & 0x00ff00;
		
		colors[2] = rb3 + g3 + initAlpha;
		colors[1] = rb2 + g2 + initAlpha;
	}
	else
	{
		colors[2] = rb0 + g0 + initAlpha;
		colors[3] = rb1 + g1 + initAlpha;
		
		rb2 = (rb0 - (rb1 >> 2)) & 0xff00ff;
		g2  = (g0 - (g1 >> 2)) & 0x00ff00;
		colors[0] = 0 ;
		
		 colors[1] = rb2 + g2 + initAlpha;
	}
   
	/*read the pixelsIndex , 2bits per pixel, 4 bytes */
	memcpy((void*)&pixelsIndex, *blockData, 4);
	(*blockData) += 4;
	
	if (ATITCDECODEFLAG::ATC_INTERPOLATED_ALPHA == decodeFlag)
	{
		// atitc_interpolated_alpha use interpolate alpha
		// 8-Alpha block: derive the other six alphas.
		// Bit code 000 = alpha0, 001 = alpha1, other are interpolated.
		
		uint32 alphaArray[8];
		
		alphaArray[0] = (uint32)((alpha     ) & 0xff);
		alphaArray[1] = (uint32)((alpha >> 8) & 0xff);
		
		if (alphaArray[0] >= alphaArray[1])
		{
			alphaArray[2] = (alphaArray[0]*6 + alphaArray[1]*1) / 7;
			alphaArray[3] = (alphaArray[0]*5 + alphaArray[1]*2) / 7;
			alphaArray[4] = (alphaArray[0]*4 + alphaArray[1]*3) / 7;
			alphaArray[5] = (alphaArray[0]*3 + alphaArray[1]*4) / 7;
			alphaArray[6] = (alphaArray[0]*2 + alphaArray[1]*5) / 7;
			alphaArray[7] = (alphaArray[0]*1 + alphaArray[1]*6) / 7;
		}
		else if (alphaArray[0] < alphaArray[1])
		{
			alphaArray[2] = (alphaArray[0]*4 + alphaArray[1]*1) / 5;
			alphaArray[3] = (alphaArray[0]*3 + alphaArray[1]*2) / 5;
			alphaArray[4] = (alphaArray[0]*2 + alphaArray[1]*3) / 5;
			alphaArray[5] = (alphaArray[0]*1 + alphaArray[1]*4) / 5;
			alphaArray[6] = 0;
			alphaArray[7] = 255;
		}
		
		// read the flowing 48bit indices (16*3)
		alpha >>= 16;
		
		for (int y = 0; y < 4; ++y)
		{
			for (int x = 0; x < 4; ++x)
			{
				decodeBlockData[x] = (alphaArray[alpha & 5] << 24) + colors[pixelsIndex & 3];
				pixelsIndex >>= 2;
				alpha >>= 3;
			}
			decodeBlockData += stride;
		}
	} //if (atc_interpolated_alpha == comFlag)
	else
	{
		/* atc_rgb atc_explicit_alpha use explicit alpha */
		
		for (int y = 0; y < 4; ++y)
		{
			for (int x = 0; x < 4; ++x)
		   {
				initAlpha   = ((int)(alpha) & 0x0f) << 28;
				initAlpha   += initAlpha >> 4;
				decodeBlockData[x] = initAlpha + colors[pixelsIndex & 3];
				pixelsIndex >>= 2;
				alpha	   >>= 4;
			}
			decodeBlockData += stride;
		}
	}
}

//Decode ATITC encode data to RGB32
void atitc_decode(uint8 *encodeData,			 //in_data
				 uint8 *decodeData,			  //out_data
				 const int pixelsWidth,
				 const int pixelsHeight,
				 ATITCDECODEFLAG::ATITCDecodeFlag decodeFlag)
{
	uint32 *decodeBlockData = (uint32 *)decodeData;
	
	for (int block_y = 0; block_y < pixelsHeight / 4; ++block_y, decodeBlockData += 3 * pixelsWidth)   //stride = 3*width
	{
		for (int block_x = 0; block_x < pixelsWidth / 4; ++block_x, decodeBlockData += 4)			//skip 4 pixels
		{
			uint64 blockAlpha = 0;
			
			switch (decodeFlag)
			{
			case ATITCDECODEFLAG::ATC_RGB:
				{
					atitc_decode_block(&encodeData, decodeBlockData, pixelsWidth, 0, 0L, ATITCDECODEFLAG::ATC_RGB);
				}
				break;
			case ATITCDECODEFLAG::ATC_EXPLICIT_ALPHA:
				{
					memcpy((void *)&blockAlpha, encodeData, 8);
					encodeData += 8;
					atitc_decode_block(&encodeData, decodeBlockData, pixelsWidth, 1, blockAlpha, ATITCDECODEFLAG::ATC_EXPLICIT_ALPHA);
				}
				break;
			case ATITCDECODEFLAG::ATC_INTERPOLATED_ALPHA:
				{
					memcpy((void *)&blockAlpha, encodeData, 8);
					encodeData += 8;
					atitc_decode_block(&encodeData, decodeBlockData, pixelsWidth, 1, blockAlpha, ATITCDECODEFLAG::ATC_INTERPOLATED_ALPHA);
				}
				break;
			default:
				break;
			}//switch
		}//for block_x
	}//for block_y
}


