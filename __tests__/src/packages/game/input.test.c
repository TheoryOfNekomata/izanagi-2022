#if defined IZ_MACOS
#include <SDL.h>
#endif
#include <bdd-for-c.h>
#include <subprojects/SDL/SDL_keyboard.mock.h>
#include <subprojects/SDL/SDL_gamecontroller.mock.h>
#include <subprojects/SDL/SDL_stdinc.mock.h>
#include <subprojects/minIni/minIni.mock.h>
#include <subprojects/portmidi/portmidi.mock.h>
#include <stdinc/IZ_string.mock.h>
#include <stdinc/IZ_stdlib.mock.h>
#include <log/IZ_log.mock.h>
#include <subprojects/ini-config/source/ini-config.mock.h>
#include <game/input/IZ_keyboard.h>
#include <game/input/IZ_gamecontroller.h>
#include <game/input/IZ_midi.h>

spec("input") {
	describe("gamecontroller") {
		describe("Initialize") {
			static IZ_GameControllerState state[IZ_PLAYERS];

			after_each() {
				mock_reset(IZ_memcpy);
			}

			after_each() {
				mock_reset(SDL_NumJoysticks);
			}

			after_each() {
				mock_reset(SDL_GameControllerOpen);
			}

			after_each() {
				mock_reset(INI_ConfigSave);
			}

			after_each() {
				mock_reset(INI_ConfigInitialize);
			}

			it("sets initial state") {
				IZ_GameControllerInitialize(&state, IZ_CONFIG_GAME_PATH, 0, NULL);

				check(mock_is_called(IZ_memcpy), "Initial state not loaded.");
				check(mock_is_called(SDL_NumJoysticks), "Connected joysticks not checked.");
			}

			it("calls load method") {
				IZ_GameControllerInitialize(&state, IZ_CONFIG_GAME_PATH, 0, NULL);

				check(mock_is_called(INI_ConfigInitialize), "Config load function not called.");
			}

			it("calls save method") {
				IZ_GameControllerInitialize(&state, IZ_CONFIG_GAME_PATH, 0, NULL);

				check(mock_is_called(INI_ConfigSave), "Config save function not called.");
			}

			it("opens device handles") {
				mock_set_expected_calls(SDL_GameControllerOpen, MOCK_OPEN_JOYSTICKS);

				IZ_GameControllerInitialize(&state, IZ_CONFIG_GAME_PATH, 0, NULL);

				check(
					mock_get_expected_calls(SDL_GameControllerOpen) == mock_get_actual_calls(SDL_GameControllerOpen),
					"Call count mismatch for SDL_GameControllerOpen() (expected %u, received %u).",
					mock_get_expected_calls(SDL_GameControllerOpen),
					mock_get_actual_calls(SDL_GameControllerOpen)
				);
			}
		}

		describe("HandleEvents") {
			static SDL_Event e;
			static IZ_GameControllerState state[IZ_PLAYERS] = {};
			static IZ_Action action[IZ_PLAYERS] = {};

			u8 p;
			for (p = 0; p < IZ_PLAYERS; p += 1) {
				describe("on player %u", p) {
					describe("on axis motion events") {
						before_each() {
							state[p].config.axis_threshold = IZ_GAME_CONTROLLER_AXIS_THRESHOLD;
						}

						for (u8 i = 0; i < SDL_CONTROLLER_AXIS_MAX; i += 1) {
							static const char* axis_name;
							axis_name = SDL_GameControllerGetStringForAxis(i);

							it("handles %s positive motion activation", axis_name) {
								e.type = SDL_CONTROLLERAXISMOTION;
								e.caxis.axis = i;
								e.caxis.value = IZ_GAME_CONTROLLER_AXIS_THRESHOLD + 1;
								sprintf(state[p].config.control_mapping[0], "axis:+%s", axis_name);
								action[p] = 0;
								IZ_GameControllerHandleEvents(&state, &action, e);
								check(action[p] != 0, "Action not set.");
							}

							it("handles %s negative motion activation", axis_name) {
								e.type = SDL_CONTROLLERAXISMOTION;
								e.caxis.axis = i;
								e.caxis.value = -(IZ_GAME_CONTROLLER_AXIS_THRESHOLD + 1);
								sprintf(state[p].config.control_mapping[0], "axis:-%s", axis_name);
								action[p] = 0;
								IZ_GameControllerHandleEvents(&state, &action, e);
								check(action[p] != 0, "Action not set.");
							}

							it("handles %s positive motion deactivation", axis_name) {
								e.type = SDL_CONTROLLERAXISMOTION;
								e.caxis.axis = i;
								e.caxis.value = IZ_GAME_CONTROLLER_AXIS_THRESHOLD - 1;
								sprintf(state[p].config.control_mapping[0], "axis:+%s", axis_name);
								action[p] = 1;
								IZ_GameControllerHandleEvents(&state, &action, e);
								check(action[p] != 1, "Action not unset. %d", action[p]);
							}

							it("handles %s negative motion deactivation", axis_name) {
								e.type = SDL_CONTROLLERAXISMOTION;
								e.caxis.axis = i;
								e.caxis.value = -(IZ_GAME_CONTROLLER_AXIS_THRESHOLD - 1);
								sprintf(state[p].config.control_mapping[0], "axis:-%s", axis_name);
								action[p] = 1;
								IZ_GameControllerHandleEvents(&state, &action, e);
								check(action[p] != 1, "Action not unset. %d", action[p]);
							}
						}
					}

					describe("on button events") {
						for (u8 i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i += 1) {
							static const char* button_name;
							button_name = SDL_GameControllerGetStringForButton(i);
							it("handles %s action activation", button_name) {
								e.type = SDL_CONTROLLERBUTTONDOWN;
								e.cbutton.button = i;
								sprintf(state[p].config.control_mapping[0], "button:%s", button_name);
								action[p] = 0;
								IZ_GameControllerHandleEvents(&state, &action, e);
								check(action[p] != 0, "Action not set.");
							}

							it("handles %s action deactivation", button_name) {
								e.type = SDL_CONTROLLERBUTTONUP;
								e.cbutton.button = i;
								sprintf(state[p].config.control_mapping[0], "button:%s", button_name);
								action[p] = 1;
								IZ_GameControllerHandleEvents(&state, &action, e);
								check(action[p] != 1, "Action not unset.");
							}
						}
					}
				}
			}
		}

		describe("SaveConfig") {
			static IZ_GameControllerState state[IZ_PLAYERS];

			after_each() {
				mock_reset(INI_ConfigSave);
			}

			before_each() {
				for (u8 p = 0; p < IZ_PLAYERS; p += 1) {
					for (u8 i = 0; i < IZ_CONTROLS; i += 1) {
						IZ_memcpy(
							state[p].config.control_mapping[i],
							IZ_GAME_CONTROLLER_MAX_CONTROL_NAME_LENGTH,
							IZ_GAME_CONTROLLER_DEFAULT_STATE[p].config.control_mapping[i],
							IZ_GAME_CONTROLLER_MAX_CONTROL_NAME_LENGTH
						);
					}
				}
			}

			it("calls save method") {
				IZ_GameControllerSaveConfig(&state, IZ_CONFIG_GAME_PATH);

				check(mock_is_called(INI_ConfigSave), "Config save function not called.");
			}
		}

		describe("Teardown") {
			static void* device = (void*) 1;
			static IZ_GameControllerState state[IZ_PLAYERS] = {};

			before_each() {
				for (u8 p = 0; p < IZ_PLAYERS; p += 1) {
					state[p].device = device;
				}
			}

			after_each() {
				mock_reset(SDL_GameControllerClose);
			}

			it("closes opened devices") {
				mock_set_expected_calls(SDL_GameControllerClose, IZ_PLAYERS);

				IZ_GameControllerTeardown(&state);

				check(
					mock_get_expected_calls(SDL_GameControllerClose) == mock_get_actual_calls(SDL_GameControllerClose),
					"Call count mismatch for SDL_GameControllerClose() (expected %u, received %u).",
					mock_get_expected_calls(SDL_GameControllerClose),
					mock_get_actual_calls(SDL_GameControllerClose)
				);
			}
		}
	}

	describe("keyboard") {
		describe("Initialize") {
			static IZ_KeyboardState state[IZ_PLAYERS] = {};

			after_each() {
				mock_reset(IZ_memcpy);
			}

			after_each() {
				mock_reset(INI_ConfigInitialize);
			}

			after_each() {
				mock_reset(INI_ConfigSave);
			}

			before_each() {
				for (u8 p = 0; p < IZ_PLAYERS; p += 1) {
					for (u8 i = 0; i < IZ_CONTROLS; i += 1) {
						state[p].config.control_mapping[i] = IZ_KEYBOARD_DEFAULT_STATE[p].config.control_mapping[i];
					}
				}
			}

			it("sets initial state") {
				IZ_KeyboardInitialize(&state, IZ_CONFIG_GAME_PATH, 0, NULL);

				check(mock_is_called(IZ_memcpy), "Initial state not loaded.");
			}

			it("calls load method") {
				IZ_KeyboardInitialize(&state, IZ_CONFIG_GAME_PATH, 0, NULL);

				check(mock_is_called(INI_ConfigInitialize), "Config load function not called.");
			}

			it("calls save method") {
				IZ_KeyboardInitialize(&state, IZ_CONFIG_GAME_PATH, 0, NULL);

				check(mock_is_called(INI_ConfigSave), "Config save function not called.");
			}
		}

		describe("HandleEvents") {
			static SDL_Event e;
			static IZ_KeyboardState state[IZ_PLAYERS] = {};
			static IZ_Action action[IZ_PLAYERS] = {};

			for (u8 p = 0; p < IZ_PLAYERS; p += 1) {
				describe("on player %u", p) {
					for (u8 i = 0; i < IZ_CONTROLS; i += 1) {
						it("handles %s action activation", IZ_ACTION_NAMES[i]) {
							e.type = SDL_KEYDOWN;
							e.key.keysym.sym = IZ_KEYBOARD_DEFAULT_STATE[p].config.control_mapping[i];
							state[p].config.control_mapping[i] = IZ_KEYBOARD_DEFAULT_STATE[p].config.control_mapping[i];
							action[p] = 0;

							IZ_KeyboardHandleEvents(&state, &action, e);
							check(
								action[p] == (0x1 << i),
								"Action not set."
							);
						}

						it("handles %s action deactivation", IZ_ACTION_NAMES[i]) {
							e.type = SDL_KEYUP;
							e.key.keysym.sym = IZ_KEYBOARD_DEFAULT_STATE[p].config.control_mapping[i];
							state[p].config.control_mapping[i] = IZ_KEYBOARD_DEFAULT_STATE[p].config.control_mapping[i];
							action[p] = ~0;

							IZ_KeyboardHandleEvents(&state, &action, e);
							check(
								!(action[p] & (0x1 << i)),
								"Action not unset."
							);
						}
					}
				}
			}
		}

		describe("SaveConfig") {
			static IZ_KeyboardState state[IZ_PLAYERS] = {};

			after_each() {
				mock_reset(INI_ConfigSave);
			}

			before_each() {
				for (u8 p = 0; p < IZ_PLAYERS; p += 1) {
					for (u8 i = 0; i < IZ_CONTROLS; i += 1) {
						state[p].config.control_mapping[i] = IZ_KEYBOARD_DEFAULT_STATE[p].config.control_mapping[i];
					}
				}
			}

			it("calls save method") {
				IZ_KeyboardSaveConfig(&state, IZ_CONFIG_GAME_PATH);

				check(mock_is_called(INI_ConfigSave), "Config save function not called.");
			}
		}
	}

	describe("midi") {
		describe("Initialize") {
			static IZ_MIDIInputState state[IZ_PLAYERS];

			after_each() {
				mock_reset(IZ_memcpy);
			}

			after_each() {
				mock_reset(Pm_CountDevices);
			}

			after_each() {
				mock_reset(Pm_OpenInput);
			}

			after_each() {
				mock_reset(INI_ConfigSave);
			}

			after_each() {
				mock_reset(INI_ConfigInitialize);
			}

			after_each() {
				mock_reset(INI_ConfigSave);
			}

			after_each() {
				mock_reset(INI_ConfigInitialize);
			}

			it("sets initial state") {
				IZ_MIDIInputInitialize(&state, IZ_CONFIG_GAME_PATH, 0, NULL);

				check(mock_is_called(IZ_memcpy), "Initial state not loaded.");
				check(mock_is_called(Pm_CountDevices), "Connected MIDI devices not checked.");
			}

			it("calls load method") {
				IZ_MIDIInputInitialize(&state, IZ_CONFIG_GAME_PATH, 0, NULL);

				check(mock_is_called(INI_ConfigInitialize), "Config load function not called.");
			}

			it("calls save method") {
				IZ_MIDIInputInitialize(&state, IZ_CONFIG_GAME_PATH, 0, NULL);

				check(mock_is_called(INI_ConfigSave), "Config save function not called.");
			}

			it("opens device handles") {
				mock_set_expected_calls(Pm_OpenInput, MOCK_OPEN_JOYSTICKS);

				IZ_MIDIInputInitialize(&state, IZ_CONFIG_GAME_PATH, 0, NULL);

				check(
					mock_get_expected_calls(Pm_OpenInput) == mock_get_actual_calls(Pm_OpenInput),
					"Call count mismatch for Pm_OpenInput() (expected %u, received %u).",
					mock_get_expected_calls(Pm_OpenInput),
					mock_get_actual_calls(Pm_OpenInput)
				);
			}
		}

		describe("SaveConfig") {
			static IZ_MIDIInputState state[IZ_PLAYERS];

			after_each() {
				mock_reset(INI_ConfigSave);
			}

			after_each() {
				mock_reset(INI_ConfigSave);
			}

			it("calls save method") {
				IZ_MIDIInputSaveConfig(&state, IZ_CONFIG_GAME_PATH);

				check(mock_is_called(INI_ConfigSave), "Config save function not called.");
			}
		}

		describe("HandleEvents") {
			static PmEvent e;
			static IZ_MIDIInputState state[IZ_PLAYERS] = {};
			static IZ_Action action[IZ_PLAYERS] = {};

			for (u8 p = 0; p < IZ_PLAYERS; p += 1) {
				describe("on player %u", p) {
					for (u8 i = 0; i < IZ_CONTROLS; i += 1) {
						it("handles %s action activation", IZ_ACTION_NAMES[i]) {
							e.message = MIDI_MESSAGE_NOTEON | (IZ_MIDI_INPUT_DEFAULT_STATE[p].config.control_mapping[i] << 8);
							state[p].config.control_mapping[i] = IZ_MIDI_INPUT_DEFAULT_STATE[p].config.control_mapping[i];
							action[p] = 0;

							IZ_MIDIInputHandleEvents(&state, &action, e);
							check(
								action[p] == (0x1 << i),
								"Action not set."
							);
						}

						it("handles %s action deactivation", IZ_ACTION_NAMES[i]) {
							e.message = MIDI_MESSAGE_NOTEOFF | (IZ_MIDI_INPUT_DEFAULT_STATE[p].config.control_mapping[i] << 8);
							state[p].config.control_mapping[i] = IZ_MIDI_INPUT_DEFAULT_STATE[p].config.control_mapping[i];
							action[p] = ~0;

							IZ_MIDIInputHandleEvents(&state, &action, e);
							check(
								!(action[p] & (0x1 << i)),
								"Action not unset."
							);
						}
					}
				}
			}
		}

		describe("Teardown") {
			static PmStream* stream;
			static IZ_MIDIInputState state[IZ_PLAYERS] = {};

			before_each() {
				for (u8 p = 0; p < IZ_PLAYERS; p += 1) {
					state[p].stream = &stream;
				}
			}

			after_each() {
				mock_reset(Pm_Close);
			}

			it("closes opened devices") {
				mock_set_expected_calls(Pm_Close, IZ_PLAYERS);

				IZ_MIDIInputTeardown(&state);

				check(
					mock_get_expected_calls(Pm_Close) == mock_get_actual_calls(Pm_Close),
					"Call count mismatch for Pm_Close() (expected %u, received %u).",
					mock_get_expected_calls(Pm_Close),
					mock_get_actual_calls(Pm_Close)
				);
			}
		}
	}
}
