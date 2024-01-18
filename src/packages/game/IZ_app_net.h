#ifndef IZ_APP_NET_H
#define IZ_APP_NET_H

#include "IZ_subsystem.h"
#include "../stdinc/IZ_stdlib.h"

typedef enum {
	IZ_MESSAGE_KIND_ACTION_SYNC = 0,
	IZ_MESSAGE_KIND_STATE_SYNC = 1,
} IZ_AppMessageKind;

typedef struct {
	u8 index: 3;
	u8 state: 5;
	u16 value;
} IZ_AppPlayerActionSyncMessage;

typedef struct {
	u8 player_index: 3;
	f32 x;
	f32 y;
	f32 right;
	f32 up;
} IZ_AppPlayerState;

typedef struct {
	u8 message_kind; // player
	u64 client_elapsed_time; // for synchronization
} IZ_AppMessageHeader;

typedef struct {
	u8 player_actions_count;
	IZ_AppPlayerActionSyncMessage player[];
} IZ_AppPlayerActionSection;

typedef struct {
	IZ_AppMessageHeader header;
	IZ_AppPlayerActionSection action;
} IZ_AppActionSyncMessage;

typedef struct {
	IZ_AppMessageHeader header;
	IZ_AppPlayerState player_state[IZ_PLAYERS];
	IZ_AppPlayerActionSection player_actions;
} IZ_AppStateSyncMessage;

struct IZ_App;

void IZ_AppBindConnection(struct IZ_App*, struct lws*);

void IZ_AppHandleOutboundNetworking(struct IZ_App*);

IZ_ProcedureResult IZ_AppRunNetworkingThread(struct IZ_App*);

#endif
