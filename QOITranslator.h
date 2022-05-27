/*
 * Copyright 2022 Jacob Secunda, secundaja@gmail.com
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef _QOI_TRANSLATOR_H
#define _QOI_TRANSLATOR_H


#include "BaseTranslator.h"


#define QOI_TRANSLATOR_VERSION B_TRANSLATION_MAKE_VERSION(0, 1, 0)
#define QOI_IMAGE_FORMAT 'QOI'

#define QOI_IN_QUALITY 0.9
#define QOI_IN_CAPABILITY 0.9

#define QOI_OUT_QUALITY 0.9
#define QOI_OUT_CAPABILITY 0.5

#define BITS_IN_QUALITY 0.8
#define BITS_IN_CAPABILITY 0.6

#define BITS_OUT_QUALITY 0.5
#define BITS_OUT_CAPABILITY 0.4


class QOITranslator : public BaseTranslator {
public:
                            QOITranslator();

	virtual status_t 		DerivedIdentify(BPositionIO* stream,
								const translation_format* format,
								BMessage* settings, translator_info* info,
								uint32 outType);

	virtual status_t 		DerivedTranslate(BPositionIO* stream,
								const translator_info* info,
								BMessage* settings, uint32 outType,
								BPositionIO* target, int32 baseType);

	virtual BView*			NewConfigView(TranslatorSettings* settings);

protected:
    virtual                 ~QOITranslator();
		// This is protected because the object is deleted by the
		// Release() function instead of being deleted directly by
		// the user.

private:
			status_t		_TranslateFromQOI(BPositionIO* stream,
								BMessage* settings, uint32 outType,
								BPositionIO* target);

			status_t		_TranslateFromBits(BPositionIO* stream,
								BMessage* settings, uint32 outType,
								BPositionIO* target);
};


#endif //_QOI_TRANSLATOR_H
