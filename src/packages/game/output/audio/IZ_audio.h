#ifndef IZ_AUDIO_H
#define IZ_AUDIO_H

#include "../../../common/IZ_common.h"

typedef enum {
	IZ_AUDIO_INITIALIZE_RESULT_OK = 0
} IZ_AudioInitializeResult;

typedef enum {
	IZ_AUDIO_SAVE_CONFIG_RESULT_OK = 0,
} IZ_AudioSaveConfigResult;

typedef struct {

} IZ_AudioState;

IZ_AudioInitializeResult IZ_AudioInitialize(IZ_AudioState*, void*, const char*, u8, const char*[]);

IZ_AudioSaveConfigResult IZ_AudioSaveConfig(IZ_AudioState*, const char*);

void IZ_AudioTeardown(IZ_AudioState*);

#endif
