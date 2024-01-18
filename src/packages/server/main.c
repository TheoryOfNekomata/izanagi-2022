#include "IZ_app.h"

IZ_ProcedureResult main(i32 argc, char* argv[]) {
	IZ_App app;
	return IZ_AppRun(&app, argc, (const char**) argv);
}
