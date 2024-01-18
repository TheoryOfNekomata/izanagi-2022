#include "IZ_asset.h"

void IZ_AssetResolveDir(const char* asset_name, char* out) {
	// TODO:
	//   1. Check all folders under assets/
	//   2. Check if the asset name exists under assets/internal/<pack>
	//   3. Return the directory when directory exists for assets/internal/<pack>/<asset_name>
	//   4. Return the directory when directory exists for assets/external/<pack>/<asset_name>
	//   5. Else return assets/internal/default/<asset_name>
	sprintf(out, "%s/%s", "assets/internal/default", asset_name);
}
