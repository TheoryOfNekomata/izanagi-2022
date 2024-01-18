#include "IZ_audio.h"

IZ_AudioInitializeResult IZ_AudioInitialize(IZ_AudioState* state, void* user_data, const char* config_path, u8 argc, const char* argv[]) {
	return IZ_AUDIO_INITIALIZE_RESULT_OK;
}

IZ_AudioSaveConfigResult IZ_AudioSaveConfig(IZ_AudioState* state, const char* config_path) {
	return IZ_AUDIO_SAVE_CONFIG_RESULT_OK;
}

void IZ_AudioTeardown(IZ_AudioState* state) {

}
