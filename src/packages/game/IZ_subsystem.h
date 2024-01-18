#ifndef IZ_SUBSYSTEM_H
#define IZ_SUBSYSTEM_H

#include "../common/IZ_common.h"
#include "../net/IZ_net_client.h"
#include "input/IZ_input.h"

struct IZ_App;

u64 IZ_AppGetTicks(struct IZ_App*);

IZ_NetClientState* IZ_AppGetNetState(struct IZ_App*);

IZ_InputState* IZ_AppGetInputState(struct IZ_App*);

#endif
