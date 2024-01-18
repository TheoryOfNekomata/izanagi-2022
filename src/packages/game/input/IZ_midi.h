#ifndef IZ_MIDI_H
#define IZ_MIDI_H

#include <string.h>
#include <portmidi.h>

#include "midi-utils.h"
#include "ini-config.h"
#include "types/int.h"
#include "../../stdinc/IZ_string.h"
#include "../../stdinc/IZ_stdlib.h"
#include "IZ_action.h"

#define MIDI_EVENT_BUFFER_SIZE 1024

typedef struct {
	PmDeviceID device_id;
	u8 channel;
	u8 control_mapping[IZ_CONTROLS];
} IZ_MIDIInputConfig;

typedef struct {
	IZ_MIDIInputConfig config;
	const PmDeviceInfo* device_info;
	PmStream* stream;
	PmEvent event_buffer[MIDI_EVENT_BUFFER_SIZE];
	i32 midi_events_count;
} IZ_MIDIInputState;

static const IZ_MIDIInputState IZ_MIDI_INPUT_DEFAULT_STATE[IZ_PLAYERS] = {
	{
		.config = {
			.control_mapping = {
				70,
				72,
				71,
				69,
				65,
				64,
				48,
				49,
				50,
				51,
				52,
				53,
				54,
				55,
				56,
				57,
			},
			.device_id = 0,
			.channel = 0,
		},
		.event_buffer = {},
		.stream = NULL,
		.device_info = NULL,
	},
#if IZ_PLAYERS > 1
	{
		.config = {
			.control_mapping = {
				70,
				72,
				71,
				69,
				65,
				64,
				48,
				49,
				50,
				51,
				52,
				53,
				54,
				55,
				56,
				57,
			},
			.device_id = 1,
			.channel = 1,
		},
		.event_buffer = {},
		.stream = NULL,
		.device_info = NULL,
	},
#endif
};

IZ_ProcedureResult IZ_MIDIInputSaveConfig(IZ_MIDIInputState(*)[IZ_PLAYERS], const char*);

void IZ_MIDIInputHandleEvents(IZ_MIDIInputState(*)[IZ_PLAYERS], IZ_Action(*)[IZ_PLAYERS], PmEvent);

IZ_ProcedureResult IZ_MIDIInputInitialize(IZ_MIDIInputState(*)[IZ_PLAYERS], const char*, u8, const char*[]);

void IZ_MIDIInputTeardown(IZ_MIDIInputState(*)[IZ_PLAYERS]);

#endif
