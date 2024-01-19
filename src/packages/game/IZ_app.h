#ifndef IZ_APP_H
#define IZ_APP_H

#include "SDL.h"
#include "libwebsockets.h"
#include "asset/IZ_asset.h"
#include "memory/IZ_pool.h"
#include "IZ_app_config.h"
#include "IZ_app_input.h"
#include "IZ_app_net.h"
#include "IZ_app_video.h"
#include "../log/IZ_log.h"
#include "../log/IZ_intercept.h"
#include "../stdinc/IZ_string.h"
#include "../timer/IZ_timer.h"

typedef enum {
	IZ_APP_RESULT_INITIALIZATION_ERROR = -1,
	IZ_APP_RESULT_OK,
} IZ_AppResult;

typedef struct IZ_App {
	IZ_InputState input_state;
	IZ_VideoState video_state;
	IZ_Pool pool;

	u64 ticks;
	IZ_NetClientState net_state;
} IZ_App;

IZ_AppResult IZ_AppRun(struct IZ_App*, u8, const char*[]);

#endif
