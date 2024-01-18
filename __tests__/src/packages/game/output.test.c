#include <bdd-for-c.h>
#include "../../../../__mocks__/subprojects/minIni/minIni.mock.h"
#include "../../../../__mocks__/subprojects/SDL/SDL_stdinc.mock.h"
#include "../../../../__mocks__/subprojects/SDL/SDL_render.mock.h"
#include "../../../../src/packages/game/output/video/IZ_video.h"

const char* IZ_ConfigGetCommandlineOption(u8 argc, const char* argv[], const char* val) {
	size_t n = strlen(val);
	int c = argc;

	while (--c > 0) {

		if (!strncmp(argv[c], val, n)) {
			if (!*(argv[c] + n) && c < argc - 1) {
				/* coverity treats unchecked argv as "tainted" */
				if (!argv[c + 1] || strlen(argv[c + 1]) > 1024)
					return NULL;
				return argv[c + 1];
			}

			if (argv[c][n] == '=')
				return &argv[c][n + 1];
			return argv[c] + n;
		}
	}

	return NULL;
}

spec("output") {
	describe("video") {
		describe("SaveConfig") {
			static IZ_VideoState state;

			after_each() {
				mock_reset(ini_putl);
			}

			before_each() {
				state.config.width = 1337;
				state.config.height = 420;
				state.config.max_fps = 69;
			}

			it("calls save method") {
				mock_set_expected_calls(ini_putl, 3);

				IZ_VideoSaveConfig(&state, IZ_CONFIG_GAME_PATH);

				check(
					mock_get_expected_calls(ini_putl) == mock_get_actual_calls(ini_putl),
					"Call count mismatch for ini_putl() (expected %u, received %u).",
					mock_get_expected_calls(ini_putl),
					mock_get_actual_calls(ini_putl)
				);
			}
		}
	}
}
