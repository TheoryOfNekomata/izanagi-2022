#ifndef PORTMIDI_MOCK_H
#define PORTMIDI_MOCK_H

#include <portmidi.h>
#include "../../../src/packages/common/IZ_common.h"

static PmDeviceInfo MOCK_DEVICE_INFO = {
	.output = 1,
	.input = 1,
	.name = "Mock MIDI Device",
	.interf = "MMSystem",
	.is_virtual = 0,
	.opened = 0,
	.structVersion = 1,
};

mock(Pm_Initialize) i32 Pm_Initialize(void) {
	mock_return(Pm_Initialize) 0;
}

mock(Pm_GetDeviceInfo) const PmDeviceInfo* Pm_GetDeviceInfo(PmDeviceID id) {
	mock_return(Pm_GetDeviceInfo) &MOCK_DEVICE_INFO;
}

mock(Pm_OpenInput) i32 Pm_OpenInput(
	PortMidiStream** stream,
	PmDeviceID inputDevice,
	void* inputDriverInfo,
	i32 bufferSize,
	PmTimeProcPtr time_proc,
	void* time_info
) {
	mock_return(Pm_OpenInput) 0;
}

mock(Pm_Close) i32 Pm_Close(PortMidiStream* stream) {
	mock_return(Pm_Close) 0;
}

#define MOCK_OPEN_MIDI_DEVICES 1

mock(Pm_CountDevices) i32 Pm_CountDevices(void) {
	mock_return(Pm_CountDevices) MOCK_OPEN_MIDI_DEVICES;
}

#endif
