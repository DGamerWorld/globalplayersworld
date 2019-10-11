#ifndef _CRAZYMO_IMAGE_H_
#define _CRAZYMO_IMAGE_H_

//+TGA
uint8* tgaLoadBuffer(uint8& type, uint8& pixelDepth, uint16& imgWide, uint16& imgHigh, const uint8 buffer[], uint32 bufSize);
//-TGA

//+S3TC Decode S3TC encode data to RGB32
void s3tc_decode1(uint8 decodeData[], const uint8 encodeData[], uint16 imgWide, uint16 imgHigh);
void s3tc_decode3(uint8 decodeData[], const uint8 encodeData[], uint16 imgWide, uint16 imgHigh);
void s3tc_decode5(uint8 decodeData[], const uint8 encodeData[], uint16 imgWide, uint16 imgHigh);
//-S3TC

//+ETC
#ifndef GL_ETC1_RGB8_OES
#define GL_ETC1_RGB8_OES					0x8D64
#endif
#define ETC_PKM_HEADER_SIZE					16
uint32 etc1_decode_image(uint8 decodeData[], const uint8 encodeData[], uint16 imgWide, uint16 imgHigh, uint8 pixelSize, uint32 stride);
bool etc1_pkm_is_valid(uint16& Wide, uint16& High, const uint8 encodeData[]);
//-ETC

//+ATITC Decode ATITC encode data to RGB32
class ATITCDECODEFLAG
{
public:
enum ATITCDecodeFlag
{
	ATC_RGB = 1,
	ATC_EXPLICIT_ALPHA = 3,
	ATC_INTERPOLATED_ALPHA = 5,
};
};
void atitc_decode(uint8 *encode_data, uint8 *decode_data, const int pixelsWidth, const int pixelsHeight, ATITCDECODEFLAG::ATITCDecodeFlag decodeFlag);
//-ATITC

#endif	// _CRAZYMO_IMAGE_H_
