# izanagi

A run-and-gun shooter inspired by Metal Slug and Counter-Strike.

## Setup

The requirements for this project are:

* Chocolatey (Windows only)
* Clang
* Meson
* Ninja
* CMake

Ensure that these tools are accessible under `PATH`.

(For Windows, it is recommended to obtain the tools via Chocolatey as much as possible.)

Run the following Meson commands in order:

```shell
meson setup build
meson subprojects download
```

Build the libwebsockets subproject to get a config for your environment.

```shell
cd subprojects/libwebsockets-xx-xx # replace with actual libwebsockets directory
mkdir build
cd build
cmake .. -DLWS_WITH_SSL=OFF -DLWS_WITHOUT_BUILTIN_SHA1=OFF
make
```

```shell
meson compile -C build
```

Copy the `assets/gamecontrollerdb.txt` file to `build/assets/gamecontrollerdb.txt`.

Afterwards, ensure the game runs:

```shell
./build/game
```
