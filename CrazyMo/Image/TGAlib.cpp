#include "../Platform.h"

uint8* tgaLoadBuffer(uint8& type, uint8& pixelDepth, uint16& imgWide, uint16& imgHigh, const uint8 buffer[], uint32 bufSize)
{
	// get the file header info
	int Step = sizeof(uint8) * 2;
	if ((Step + sizeof(uint8)) > bufSize) return 0;

	memcpy(&type, buffer + Step, sizeof(uint8));
	Step += sizeof(uint8) * 2;
	Step += sizeof(uint16) * 4;
	if ((Step + sizeof(uint16) * 2 + sizeof(uint8)) > bufSize) return 0;
	memcpy(&imgWide, buffer + Step, sizeof(uint16));
	memcpy(&imgHigh, buffer + Step + sizeof(uint16), sizeof(uint16));
	memcpy(&pixelDepth, buffer + Step + sizeof(uint16) * 2, sizeof(uint8));
	Step += sizeof(uint8);
	Step += sizeof(uint16) * 2;
	if ((Step + sizeof(uint8)) > bufSize) return 0;
	uint8 cGarbage;
	memcpy(&cGarbage, buffer + Step, sizeof(uint8));

	int flipped = 1;
	if (cGarbage & 0x20)
	{
		flipped = 0;
	}
	// check if the image is color indexed
	if (type == 1) return 0;
	// check for other types (compressed images)
	if ((type != 2) && (type != 3) && (type != 10)) return 0;
	// mode equals the number of image components
	int mode = pixelDepth / 8;
	// total is the number of unsigned chars to read
	int total = imgHigh * imgWide * mode;
	// allocate memory for image pixels
	uint8* imageData = SAFE_NEW_ARRAY(uint8, total);
	// check to make sure we have the memory required
	if (imageData == 0) return 0;

	// finally load the image pixels
	if (type == 10)
	{
		uint32 mode,total,i, index = 0;
		uint8 aux[4], runlength = 0;
		uint32 skip = 0, flag = 0;
		size_t Step = (sizeof(uint8) + sizeof(uint16)) * 6;

		// mode equal the number of components for each pixel
		mode = pixelDepth / 8;
		// total is the number of unsigned chars we'll have to read
		total = imgHigh * imgWide;

		for( i = 0; i < total; i++ )
		{
			// if we have a run length pending, run it
			if ( runlength != 0 )
			{
				// we do, update the run length count
				runlength--;
				skip = (flag != 0);
			}else {
				// otherwise, read in the run length token
				if ((Step + sizeof(uint8)) > bufSize) break;
				memcpy(&runlength, buffer + Step, sizeof(uint8));
				Step += sizeof(uint8);

				// see if it's a RLE encoded sequence
				flag = runlength & 0x80;
				if ( flag )
				{
					runlength -= 128;
				}
				skip = 0;
			}

			// do we need to skip reading this pixel?
			if ( !skip )
			{
				// no, read in the pixel data
				if ((Step + sizeof(uint8) * mode) > bufSize) break;

				memcpy(aux, buffer + Step, sizeof(uint8) * mode);
				Step += sizeof(uint8) * mode;

				// mode=3 or 4 implies that the image is RGB(A). However TGA
				// stores it as BGR(A) so we'll have to swap R and B.
				if ( mode >= 3 )
				{
					uint8 tmp;

					tmp = aux[0];
					aux[0] = aux[2];
					aux[2] = tmp;
				}
			}

			// add the pixel to our image
			memcpy(&imageData[index], aux, mode);
			index += mode;
		}
	}else {
		int mode,total,i;
		uint8 aux;
		size_t Step = (sizeof(uint8) + sizeof(uint16)) * 6;

		// mode equal the number of components for each pixel
		mode = pixelDepth / 8;
		// total is the number of unsigned chars we'll have to read
		total = imgHigh * imgWide * mode;

		size_t dataSize = sizeof(uint8) * total;
		if ((Step + dataSize) > bufSize) return 0;
		memcpy(imageData, buffer + Step, dataSize);

		// mode=3 or 4 implies that the image is RGB(A). However TGA
		// stores it as BGR(A) so we'll have to swap R and B.
		if (mode >= 3)
		{
			for (i=0; i < total; i+= mode)
			{
				aux = imageData[i];
				imageData[i] = imageData[i+2];
				imageData[i+2] = aux;
			}
		}
	}

	if (flipped)
	{
		// mode equal the number of components for each pixel
		int mode = pixelDepth / 8;
		int rowbytes = imgWide*mode;
		uint8 *row = SAFE_NEW_ARRAY(uint8, rowbytes);
		if (row == 0) return 0;

		for (int y=0; y<(imgHigh/2); ++y)
		{
			memcpy(row, &imageData[y*rowbytes],rowbytes);
			memcpy(&imageData[y*rowbytes], &imageData[(imgHigh-(y+1))*rowbytes], rowbytes);
			memcpy(&imageData[(imgHigh-(y+1))*rowbytes], row, rowbytes);
		}
		SAFE_DELETE_ARRAY(row);
	}
	return imageData;
}
