/*
 * Copyright 2022 Jacob Secunda, secundaja@gmail.com
 * All rights reserved. Distributed under the terms of the MIT License.
 */


#include "QOITranslator.h"

#include "QOIDefinitions.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "QOITranslator"


static const char* sTranslatorName = B_TRANSLATE("QOI Image Translator");
static const char* sTranslatorInfo = B_TRANSLATE("");
static const int32 sTranslatorVersion = QOI_TRANSLATOR_VERSION;


// The input formats that this translator knows how to read
static const translation_format sInputFormats[] = {
	{
		QOI_IMAGE_FORMAT,
		B_TRANSLATOR_BITMAP,
		QOI_IN_QUALITY,
		QOI_IN_CAPABILITY,
		"image/x-qoi",
		"Quite OK Image Format"
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
		"image/x-qoi",
		"The Quite OK Image Format"
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


// Default settings for the Translator
static const char* sTranslatorSettingsFile = "QOITranslator_Settings";

static const TranSetting sDefaultSettings[] = {
	{ B_TRANSLATOR_EXT_BITMAP_COLOR_SPACE, TRAN_SETTING_INT32, QOI_RGBA },
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
DerivedIdentify(BPositionIO* stream, const translation_format* format, BMessage* settings,
	translator_info* info, uint32 outType)
{

}


status_t
DerivedTranslate(BPositionIO* stream, const translator_info* info, BMessage* settings,
	 uint32 outType, BPositionIO* target, int32 baseType)
{

}