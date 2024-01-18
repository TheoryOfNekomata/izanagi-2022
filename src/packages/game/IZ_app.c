#include "IZ_app.h"

u64 IZ_AppGetTicks(struct IZ_App* app) {
	return app->ticks;
}

void IZ_AppBindConnection(struct IZ_App* app, struct lws* wsi) {
	app->net_state.binding.connection = wsi;
}

IZ_NetClientState* IZ_AppGetNetState(struct IZ_App* app) {
	return &app->net_state;
}

IZ_InputState* IZ_AppGetInputState(struct IZ_App* app) {
	return &app->input_state;
}

typedef enum {
	IZ_APP_INITIALIZE_RESULT_NETWORKING_ERROR = -6,
	IZ_APP_INITIALIZE_RESULT_POOL_ERROR,
	IZ_APP_INITIALIZE_RESULT_INPUT_ERROR,
	IZ_APP_INITIALIZE_RESULT_AUDIO_ERROR,
	IZ_APP_INITIALIZE_RESULT_VIDEO_ERROR,
	IZ_APP_INITIALIZE_RESULT_SDL_ERROR,
	IZ_APP_INITIALIZE_RESULT_OK,
} IZ_AppInitializeResult;

IZ_AppInitializeResult IZ_AppInitialize(struct IZ_App* app, u8 argc, const char* argv[]) {
	IZ_LogInitialize("game", false);
	IZ_LogInterceptWSMessages(LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE);

	IZ_LogInfo(IZ_LOG_CATEGORY_GLOBAL, "global", "Starting %s...", IZ_APP_NAME);
	IZ_memset(app, 0, sizeof(struct IZ_App));

	const char* cmdline_buffer;
	char config_path[128];
	// TODO abstract command line args parsing
	if ((cmdline_buffer = INI_ConfigGetCommandlineOption(argc, argv, "-c"))) {
		IZ_memcpy(config_path, 128, cmdline_buffer, 128);
	} else {
		IZ_ConfigGetDefaultPath(config_path, 128);
	}

	u32 flags = (
		SDL_INIT_VIDEO
		| SDL_INIT_GAMECONTROLLER
		| SDL_INIT_EVENTS
	);

	if (SDL_Init(flags) < 0) {
		IZ_LogError("global", "Framework initialization failed! Reason: %s", SDL_GetError());
		return IZ_APP_INITIALIZE_RESULT_SDL_ERROR;
	}

	if (IZ_VideoInitialize(&app->video_state, app, config_path, argc, argv)) {
		return IZ_APP_INITIALIZE_RESULT_VIDEO_ERROR;
	}

	if (IZ_InputInitialize(&app->input_state, config_path, argc, argv)) {
		return IZ_APP_INITIALIZE_RESULT_INPUT_ERROR;
	}

	if (IZ_NetClientInitialize(&app->net_state, app, IZ_AppRunNetworkingThread, config_path, argc, argv)) {
		return IZ_APP_INITIALIZE_RESULT_NETWORKING_ERROR;
	}

	IZ_PoolInitialize(&app->pool, POOL_MAX_SIZE);

	app->ticks = 0;
	return IZ_APP_INITIALIZE_RESULT_OK;
}

void IZ_AppTeardown(struct IZ_App* app) {
	IZ_PoolTeardown(&app->pool);
	IZ_NetClientTeardown(&app->net_state);
	IZ_InputTeardown(&app->input_state);
	IZ_VideoTeardown(&app->video_state);
	IZ_LogInfo(IZ_LOG_CATEGORY_GLOBAL, "global", "Stopping %s...", IZ_APP_NAME);
	IZ_LogTeardown();
	SDL_Quit();
}

void IZ_AppPrintHelpOptions(void) {
	printf("\n");
	printf("Options:\n");
	printf("\n");
	printf(
		"\n"
		"Options:\n"
		"\n"
		"  -c <path>     Specifies the path to the config file.       (default: \"./" IZ_CONFIG_GAME_PATH "\")\n"
		"  -f <value>    Specifies the frames per second.                              (default: 30)\n"
		"  -h            Displays this help screen.\n"
		"  -i <value>    Specifies the interval of sending packets                    (default: 200)\n"
		"                in milliseconds.\n"
	);
}

void IZ_AppPrintHelpUsage(void) {
	printf("\n");
	printf("Usage:");
	printf("  %s [options]\n", "game.exe");
}

void IZ_AppPrintHelpHeader(void) {
	printf("\n");
	printf("%s - %s\n", IZ_APP_NAME, IZ_APP_DESCRIPTION);
}

void IZ_AppPrintHelp(void) {
	IZ_AppPrintHelpHeader();
	IZ_AppPrintHelpUsage();
	IZ_AppPrintHelpOptions();
}

IZ_AppResult IZ_AppRun(struct IZ_App* app, u8 argc, const char* argv[]) {
	IZ_TimerStart();
	if (INI_ConfigGetCommandlineOption(argc, argv, "-h")) {
		IZ_AppPrintHelp();
		return IZ_APP_RESULT_OK;
	}

	if (IZ_AppInitialize(app, argc, argv) < 0) {

		return IZ_APP_RESULT_INITIALIZATION_ERROR;
	}

	// what to do:
	// - allocate memory for player object + agent (i.e. the controller) using the pool
	// - render the logic here

	char asset_dir[255];
	IZ_AssetResolveDir("weapon-operator", asset_dir);
	u16 sprite_slot_index = IZ_VideoGetNextFreeSpriteSlot(&app->video_state, IZ_VIDEO_SPRITE_PRIORITY_MEDIUM);
	IZ_VideoLoadSprite(&app->video_state, (IZ_VideoLoadSpriteParams) {
		.dir = asset_dir,
		.filename = "sprite.svg",
		.priority = IZ_VIDEO_SPRITE_PRIORITY_MEDIUM,
	}, &app->video_state.active_sprites[sprite_slot_index]);
	app->video_state.active_sprites[sprite_slot_index].sprite.scale_factor = 0.25f;
	app->video_state.active_sprites[sprite_slot_index].sprite.position = (IZ_Vector2D) { 100.f, 100.f };

	IZ_AssetResolveDir("weapon-specialist", asset_dir);
	sprite_slot_index = IZ_VideoGetNextFreeSpriteSlot(&app->video_state, IZ_VIDEO_SPRITE_PRIORITY_MEDIUM);
	IZ_VideoLoadSprite(&app->video_state, (IZ_VideoLoadSpriteParams) {
		.dir = asset_dir,
		.filename = "sprite.svg",
		.priority = IZ_VIDEO_SPRITE_PRIORITY_MEDIUM,
	}, &app->video_state.active_sprites[sprite_slot_index]);
	app->video_state.active_sprites[sprite_slot_index].sprite.scale_factor = 0.25f;
	app->video_state.active_sprites[sprite_slot_index].sprite.position = (IZ_Vector2D) { 50.f, 50.f };
	app->video_state.active_sprites[sprite_slot_index].sprite.flip_x = true;

	while (true) {
		app->ticks = SDL_GetTicks64();

		// TODO do audio processing
		// TODO do inbound networking

		if (IZ_AppHandleInputEvents(app)) {
			break;
		}

		// use shared objects in server to prevent inconsistencies among clients

		IZ_AppHandleOutboundNetworking(app);
		IZ_VideoUpdate(&app->video_state);
	}

	IZ_AppTeardown(app);
	return IZ_APP_RESULT_OK;
}
