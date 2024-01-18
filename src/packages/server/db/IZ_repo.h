#ifndef IZ_REPO_H
#define IZ_REPO_H

#include <sqlite/sqlite3.h>
#include <ini-config.h>
#include <ini-config/source/types/string.h>
#include <string.h>
#include <SDL2/SDL_filesystem.h>
#include "../../common/IZ_common.h"
#include "../../stdinc/IZ_string.h"

typedef struct {
	char path[64];
} IZ_RepoConfig;

typedef struct {
	IZ_RepoConfig config;
	sqlite3* db;
} IZ_RepoState;

static IZ_RepoState IZ_REPO_DEFAULT_STATE = {
	.config = {
		.path = "server.sqlite",
	},
	.db = NULL,
};

IZ_ProcedureResult IZ_RepoSaveConfig(IZ_RepoState*, const char*);

IZ_ProcedureResult IZ_RepoInitialize(IZ_RepoState*, const char*, u8, const char*[]);

void IZ_RepoTeardown(IZ_RepoState*);

#endif
