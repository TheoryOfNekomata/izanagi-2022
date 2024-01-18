# izanagi

A run-and-gun shooter inspired by Metal Slug and Counter-Strike.

## Setup

For Windows, it is recommended to use [Chocolatey](https://community.chocolatey.org) for retrieving the required tools. Use [Homebrew](https://brew.sh) for macOS instead.

For *NIX, use whatever dependency manager you prefer.

The following are required for building on all platforms:

* CMake
* Ninja
* Clang

### Windows

1. Install the [Windows SDK](https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/).
2. Ensure your environment variables are set up.

   > **Important:** Set your `CC` environment variable to the full path of `clang-cl.exe`

   > **Important:** Set your `CMAKE_MT` environment variable to the full path of `mt.exe`, which can be found on your
   > Windows SDK installation folder (usually `C:/Program Files (x86)/Windows Kits/10/bin/10.0.19041.0/x86/mt.exe`)

3. Clone this repo recursively.
4. Build the dependencies under `subprojects`. Follow the instructions on building for Visual C.

   > **Important:** For `libwebsockets`, follow the [instructions on building LWS](https://libwebsockets.org/lws-api-doc-master/html/md_README_8build.html).

5. Generate the CMake cache.

   ```batch
   cmake^
     -DCMAKE_SIZEOF_VOID_P=8^
     "-DCMAKE_MT=%CMAKE_MT%"^
     "-DCMAKE_BUILD_TYPE=%BUILD_TYPE%"^
     -DCMAKE_MAKE_PROGRAM=ninja^
     -DCMAKE_C_COMPILER=clang-cl^
     -DCMAKE_C_FLAGS=-m64^
     -G Ninja^
     -S .^
     -B "./build/%BUILD_TYPE%"
   ```
   
   > **Important:** Create the directories under `/build` per each `BUILD_TYPE` you are generating.

6. Build the specific executables you want to build, which are defined in `CMakeLists.txt`.

   ```batch
   cmake^
     --build "./build/%BUILD_TYPE%"^
     -t "%EXECUTABLE%"
   ```

### macOS/*NIX

1. Ensure additional tools are present in the system:

   * `autoconf`
   * `make`
   * `libtool` (*NIX only)

2. Clone this repo recursively.
3. Build the dependencies under `subprojects`. Follow the instructions on building for *NIX (macOS should have similar
   instructions for *NIX).
4. Generate the CMake cache.

   ```shell
   cmake \
     -DCMAKE_SIZEOF_VOID_P=8 \
     "-DCMAKE_BUILD_TYPE=$BUILD_TYPE" \
     -DCMAKE_MAKE_PROGRAM=ninja \
     -DCMAKE_C_COMPILER=cc \
     -DCMAKE_C_FLAGS=-m64 \
     -G Ninja \
     -S . \
     -B "./build/$BUILD_TYPE"
   ```

   > **Important:** Create the directories under `/build` per each `BUILD_TYPE` you are generating.
	 
5. Build the specific executables you want to build, which are defined in `CMakeLists.txt`.

   ```shell
   cmake \
     --build "./build/$BUILD_TYPE" \
     -t "$EXECUTABLE"
   ```
