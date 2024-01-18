#ifndef IZ_ACTION_H
#define IZ_ACTION_H

#include "../../common/IZ_common.h"

#define IZ_CONTROLS ((u8) 16)

typedef u16 IZ_Action;

static const char* IZ_ACTION_NAMES[IZ_CONTROLS] = {
	"Up",
	"Right",
	"Down",
	"Left",
	"Affirm",
	"Negate",
	"Action0",
	"Action1",
	"Action2",
	"Action3",
	"Action4",
	"Action5",
	"Action6",
	"Action7",
	"Action8",
	"Action9",
};

typedef enum {
	IZ_ACTION_INDEX_UP,
	IZ_ACTION_INDEX_RIGHT,
	IZ_ACTION_INDEX_DOWN,
	IZ_ACTION_INDEX_LEFT,
	IZ_ACTION_INDEX_AFFIRM,
	IZ_ACTION_INDEX_NEGATE,
	IZ_ACTION_INDEX_ACTION0,
	IZ_ACTION_INDEX_ACTION1,
	IZ_ACTION_INDEX_ACTION2,
	IZ_ACTION_INDEX_ACTION3,
	IZ_ACTION_INDEX_ACTION4,
	IZ_ACTION_INDEX_ACTION5,
	IZ_ACTION_INDEX_ACTION6,
	IZ_ACTION_INDEX_ACTION7,
	IZ_ACTION_INDEX_ACTION8,
	IZ_ACTION_INDEX_ACTION9,
} IZ_ActionIndex;

#endif
