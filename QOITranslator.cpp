/*
 * Copyright 2022 Jacob Secunda, secundaja@gmail.com
 * All rights reserved. Distributed under the terms of the MIT License.
 */


#include "QOITranslator.h"

#include "QOIDefinitions.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "QOITranslator"


const char* kTranslatorName = B_TRANSLATE("QOI Image Translator");
const char* kTranslatorInfo = B_TRANSLATE("Translator by CodeforEvolution (Jacob Secunda).");
const int32 kTranslatorVersion = QOI_TRANSLATOR_VERSION;

const char* kQOIMimeType = "image/x-qoi";
const char* kQOIName = "Quite OK Image Format";


// The input formats that this translator knows how to read
static const translation_format sInputFormats[] = {
	{
		QOI_IMAGE_FORMAT,
		B_TRANSLATOR_BITMAP,
		QOI_IN_QUALITY,
		QOI_IN_CAPABILITY,
		kQOIMimeType,
		kQOIName
	},
	{
		B_TRANSLATOR_BITMAP,
		B_TRANSLATOR_BITMAP,
		BITS_IN_QUALITY,
		BITS_IN_CAPABILITY,
		"image/x-be-bitmap",
		"Be Bitmap Format (QOITranslator)"
	},
};


// The output formats that this translator knows how to write
static const translation_format sOutputFormats[] = {
	{
		QOI_IMAGE_FORMAT,
		B_TRANSLATOR_BITMAP,
		QOI_OUT_QUALITY,
		QOI_OUT_CAPABILITY,
		kQOIMimeType,
		kQOIName
	},
	{
		B_TRANSLATOR_BITMAP,
		B_TRANSLATOR_BITMAP,
		BITS_OUT_QUALITY,
		BITS_OUT_CAPABILITY,
		"image/x-be-bitmap",
		"Be Bitmap Format (QOITranslator)"
	},
};


// Default settings for the translator
static const char* sTranslatorSettingsFile = "QOITranslator_Settings";

static const TranSetting sDefaultSettings[] = {
	{B_TRANSLATOR_EXT_HEADER_ONLY, TRAN_SETTING_BOOL, false},
	{B_TRANSLATOR_EXT_DATA_ONLY, TRAN_SETTING_BOOL, false},
	{B_TRANSLATOR_EXT_BITMAP_COLOR_SPACE, TRAN_SETTING_INT32, QOI_RGBA},
};


const uint32 kNumInputFormats = B_COUNT_OF(sInputFormats);
const uint32 kNumOutputFormats = B_COUNT_OF(sOutputFormats);
const uint32 kNumDefaultSettings = B_COUNT_OF(sDefaultSettings);


QOITranslator::QOITranslator()
    :
    BaseTranslator(sTranslatorName, sTranslatorInfo, sTranslatorVersion,
        sInputFormats, kNumInputFormats, sOutputFormats, kNumOutputFormats,
        sTranslatorSettingsFile, sDefaultSettings,
        kNumDefaultSettings, B_TRANSLATOR_BITMAP, QOI_IMAGE_FORMAT)
{
}


status_t
DerivedIdentify(BPositionIO* inSource, const translation_format* inFormat,
	BMessage* ioSettings, translator_info* outInfo, uint32 outType)
{
	if (!outType)
		outType = B_TRANSLATOR_BITMAP;
	if (outType != B_TRANSLATOR_BITMAP && outType != QOI_IMAGE_FORMAT)
		return B_NO_TRANSLATOR;

	uint32 maybeMagic = 0;
	if (inSource->Read(&maybeMagic, QOI_MAGIC_LENGTH) != QOI_MAGIC_LENGTH)
		return B_IO_ERROR;

	if (maybeMagic != kQOIMagic)
		return B_NO_TRANSLATOR;

	outInfo->type = QOI_IMAGE_FORMAT;
	outInfo->group = B_TRANSLATOR_BITMAP;
	outInfo->quality = QOI_IN_QUALITY;
	outInfo->capability = QOI_IN_CAPABILITY;
	strcpy(outInfo->name, kQOIName);
	strcpy(outInfo->MIME, kQOIMimeType);

	return B_OK;
}


status_t
DerivedTranslate(BPositionIO* inSource, const translator_info* inInfo,
	BMessage* ioSettings, uint32 outType, BPositionIO* outDestination,
	int32 baseType)
{
	if (baseType == 0)
		// inSource is not in the B_TRANSLATOR_BITMAP format
		return _TranslateFromQOI(inSource, ioSettings, outType, outDestination);
	else if (baseType == 1)
		// inSource is in the B_TRANSLATOR_BITMAP format
		return _TranslateFromBitsToQOI(inSource, ioSettings, outDestination);
	else
		return B_NO_TRANSLATOR;
}


status_t
_TranslateFromQOI(BPositionIO* inSource, BMessage* ioSettings, uint32 outType,
	BPositionIO* outDestination)
{
	// TODO: Make use of ioSettings to specify output colorspace and channels.
	// For now, given they are informative only, we can ignore these for now.

	if (outType == QOI_IMAGE_FORMAT) {
		// QOI to QOI? Let's copy everything over.
		return translate_direct_copy(inSource, outDestination);
	} else if (outType != B_TRANSLATOR_BITMAP)
		return B_NO_TRANSLATOR;

	qoi_header header;
	if (inSource->Read(&header, sizeof(qoi_header)) != sizeof(qoi_header))
		return B_IO_ERROR;

	// QOI Header Sanity Check

	if (header.magic != kQOIMagic)
		return B_ILLEGAL_DATA;

	if (header.channels != QOI_RGB && header.channels != QOI_RGBA)
		return B_ILLEGAL_DATA;

	if (headers.colorspace != QOI_SRGB_WITH_LINEAR_ALPHA
		&& headers.colorspace != QOI_ALL_CHANNELS_LINEAR)
		return B_ILLEGAL_DATA;

	uint32 width = B_BENDIAN_TO_HOST_INT32(header.width);
	uint32 height = B_BENDIAN_TO_HOST_INT32(header.height);

	// Write B_TRANSLATOR_BITMAP header

	TranslatorBitmap bitmapHeader;
	bitmapHeader.magic = B_TRANSLATOR_BITMAP;
	bitsHeader.bounds.left = 0;
	bitsHeader.bounds.top = 0;
	bitsHeader.bounds.right = width - 1;
	bitsHeader.bounds.bottom = height - 1;
	bitsHeader.rowBytes = 4 * width;
	bitsHeader.colors = B_RGBA32;
	bitsHeader.dataSize = bitsHeader.rowBytes * height;

	if (swap_data(B_UINT32_TYPE, &bitsHeader,
		sizeof(TranslatorBitmap), B_SWAP_HOST_TO_BENDIAN) != B_OK)
		return B_ERROR;

	outDestination->Write(&bitmapHeader, sizeof(TranslatorBitmap));

	uint32 prevPixelArray[PREVIOUS_PIXELS_LENGTH] = {0};
	rgb_color curPixel;
	uint8 pixelIndex = 0;

	bool endSequence = false;

	do {
		uint8 readBuffer = 0;
		if (inSource->Read(&readBuffer, sizeof(uint8)) != sizeof(uint8))
			return B_IO_ERROR;

		if (inSource->Seek(-sizeof(uint8), SEEK_CUR) < B_NO_ERROR)
			return B_IO_ERROR;

		switch (readBuffer) {
			// Check for 8-byte tags first
			case QOI_OP_RGB_TAG:
			{
				qoi_op_rgb curChunk;

				if (inSource->Read(&curChunk, sizeof(qoi_op_rgb))
					!= sizeof(qoi_op_rgb))
					return B_IO_ERROR;

				curPixel = make_color(curChunk.red, curChunk.green,
					curChunk.blue, curPixel.alpha);

				pixelIndex = pixel_to_index_hash(curPixel);
				prevPixelArray[pixelIndex] = static_cast<uint32>(curPixel);

				outDestination->Write(&curPixel, sizeof(rgb_color));

				break;
			}

			case QOI_OP_RGBA_TAG:
			{
				qoi_op_rgba curChunk;

				if (inSource->Read(&curChunk, sizeof(qoi_op_rgba))
					!= sizeof(qoi_op_rgba))
					return B_IO_ERROR;

				curPixel = make_color(curChunk.red, curChunk.green,
					curChunk.blue, curChunk.alpha);

				pixelIndex = pixel_to_index_hash(curPixel);
				prevPixelArray[pixelIndex] = static_cast<uint32>(curPixel);

				outDestination->Write(&curPixel, sizeof(uint32));

				break;
			}

			default:
			{
				// Check for 2-byte tags next
				switch (readBuffer & QOI_2_BYTE_TAG_MASK) {
					case QOI_OP_INDEX_TAG:
					{
						qoi_op_index curChunk;

						if (inSource->Read(&curChunk, sizeof(qoi_op_index))
							!= sizeof(qoi_op_index))
							return B_IO_ERROR;

						uint32 pixelAtIndex = prevPixelArray[curChunk.index];

						outDestination->Write(&pixelAtIndex, sizeof(uint32));

						break;
					}

					case QOI_OP_DIFF_TAG:
					{
						qoi_op_diff curChunk;

						if (inSource->Read(&curChunk, sizeof(qoi_op_diff))
							!= sizeof(qoi_op_diff))
							return B_IO_ERROR;

						uint8 red = (curPixel.red + curChunk.red_diff
							+ DIFF_BIAS) % UINT8_MAX;
						uint8 green = (curPixel.green + curChunk.green_diff
							+ DIFF_BIAS) % UINT8_MAX;
						uint8 blue = (curPixel.blue + curChunk.blue_diff
							+ DIFF_BIAS) % UINT8_MAX;

						curPixel = make_color(red, green, blue, curPixel.alpha);

						pixelIndex = pixel_to_index_hash(curPixel);
						prevPixelArray[pixelIndex] = static_cast<uint32>(
							curPixel);

						outDestination->Write(&curPixel, sizeof(uint32));

						break;
					}


					case QOI_OP_LUMA_TAG:
					{
						qoi_op_luma curChunk;

						if (inSource->Read(&curChunk, sizeof(qoi_op_luma))
							!= sizeof(qoi_op_luma))
							return B_IO_ERROR;



						curPixel = make_color(red, green, blue, curPixel.alpha);

						break;
					}


					case QOI_OP_RUN_TAG:
					{
						qoi_op_run curChunk;

						if (inSource->Read(&curChunk, sizeof(qoi_op_run))
							!= sizeof(qoi_op_run))
							return B_IO_ERROR;

						for (uint8 count = 0; count < curChunk.run + RUN_BIAS;
							count++)
							outDestination->Write(&curPixel, sizeof(uint32));

						break;
					}

					default:
						return B_ILLEGAL_DATA;
				}

				break;
			}
		}
	} while (!endSequence);
}


status_t
_TranslateFromBits(BPositionIO* inSource, BMessage* ioSettings,
	BPositionIO* outDestination)
{


}
