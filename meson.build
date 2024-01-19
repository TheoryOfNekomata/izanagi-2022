project(
	'izanagi',
	'c',
	version: '0.1',
	default_options: ['warning_level=3', 'default_library=static']
)

add_global_arguments('-DIZ_APP_NAME="Izanagi"', language: 'c')
add_global_arguments('-DIZ_APP_DESCRIPTION="Run and gun game"', language: 'c')
add_global_arguments('-DIZ_APP_SERVER_DESCRIPTION="Dedicated server"', language: 'c')
add_global_arguments('-DIZ_PLAYERS=1', language: 'c')
# add_global_arguments('-DIZ_LOG_DATE_FUNCTION=IZ_TimerElapsed', language: 'c')
add_global_arguments('-DIZ_LOG_DATE_FUNCTION=IZ_TimerNow', language: 'c')
add_global_arguments('-DIZ_CONFIG_GAME_PATH="config-game.ini"', language: 'c')
add_global_arguments('-DIZ_CONFIG_SERVER_PATH="config-server.ini"', language: 'c')

if target_machine.system() == 'windows'
	add_global_arguments('-DIZ_WINDOWS', language: 'c')
	add_global_arguments('-DWIN32', language: 'c')
	if host_machine.cpu() == 'x86_64'
		add_global_arguments('-DIZ_WIN64', language: 'c')
	elif host_machine.cpu() == 'x86'
		add_global_arguments('-DIZ_WIN32', language: 'c')
	endif
elif target_machine.system() == 'darwin'
	add_global_arguments('-DIZ_MACOS', language: 'c')
endif

sdl2_dep = dependency('sdl2')
sdl2_image_dep = dependency('sdl2_image')
freetype2_dep = dependency('freetype2')
sdl2_ttf_dep = dependency('sdl2_ttf')
ini_config_dep = dependency('ini_config')
midi_utils_dep = dependency('midi_utils')
lws_dep = dependency('libwebsockets', native: true)
portmidi_dep = dependency('portmidi', native: true)

game = executable(
	'game',
	[
		'src/packages/game/output/video/IZ_video.h',
		'src/packages/game/output/video/IZ_video.c',
		'src/packages/common/IZ_common.h',
		'src/packages/game/input/IZ_action.h',
		'src/packages/game/IZ_app.h',
		'src/packages/game/IZ_app.c',
		'src/packages/game/main.c',
		'src/packages/game/input/IZ_gamecontroller.c',
		'src/packages/game/input/IZ_gamecontroller.h',
		'src/packages/game/input/IZ_keyboard.c',
		'src/packages/game/input/IZ_keyboard.h',
		'src/packages/game/geometry/IZ_vector2d.c',
		'src/packages/game/geometry/IZ_vector2d.h',
		'src/packages/game/geometry/IZ_rect.c',
		'src/packages/game/geometry/IZ_rect.h',
		'src/packages/game/core/IZ_object.c',
		'src/packages/game/core/IZ_object.h',
		'src/packages/game/core/IZ_creature.c',
		'src/packages/game/core/IZ_creature.h',
		'src/packages/game/core/IZ_entity.c',
		'src/packages/game/core/IZ_entity.h',
		'src/packages/game/memory/IZ_pool.c',
		'src/packages/game/memory/IZ_pool.h',
		'src/packages/game/input/IZ_input.c',
		'src/packages/game/input/IZ_input.h',
		'src/packages/game/input/IZ_midi.c',
		'src/packages/game/input/IZ_midi.h',
		'src/packages/game/data/IZ_list.c',
		'src/packages/game/data/IZ_list.h',
		'src/packages/net/svc/IZ_wsclient.c',
		'src/packages/net/svc/IZ_wsclient.h',
		'src/packages/log/IZ_intercept.c',
		'src/packages/log/IZ_intercept.h',
		'src/packages/net/core/IZ_websocket.h',
		'src/packages/net/core/IZ_websocket.c',
		'src/packages/net/IZ_net_client.c',
		'src/packages/net/IZ_net_client.h',
		'src/packages/game/IZ_app_net.c',
		'src/packages/game/IZ_app_net.h',
		'src/packages/game/IZ_app_video.c',
		'src/packages/game/IZ_app_video.h',
		'src/packages/game/IZ_subsystem.h',
		'src/packages/game/IZ_app_input.c',
		'src/packages/game/IZ_app_input.h',
		'src/packages/game/IZ_app_config.c',
		'src/packages/game/IZ_app_config.h',
		'src/packages/game/asset/IZ_asset.c',
		'src/packages/game/asset/IZ_asset.h',
		'src/packages/stdinc/IZ_string.c',
		'src/packages/stdinc/IZ_string.h',
		'src/packages/io/IZ_io.c',
		'src/packages/io/IZ_io.h',
		'src/packages/log/IZ_log.c',
		'src/packages/log/IZ_log.h',
		'src/packages/timer/IZ_timer.c',
		'src/packages/timer/IZ_timer.h',
		'src/packages/compat/IZ_compat.h',
		'src/packages/stdinc/IZ_stdlib.c',
		'src/packages/stdinc/IZ_stdlib.h',
		'src/packages/config/IZ_config_guid.c',
		'src/packages/config/IZ_config_guid.h',
		'src/packages/game/output/audio/IZ_audio.c',
		'src/packages/game/output/audio/IZ_audio.h'
	],
	dependencies: [
  		ini_config_dep,
		freetype2_dep,
  		midi_utils_dep,
  		portmidi_dep,
  		sdl2_dep,
  		sdl2_ttf_dep,
  		sdl2_image_dep,
  		lws_dep
	]
)

test('basic', game)
