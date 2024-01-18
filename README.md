# izanagi

A run-and-gun shooter inspired by Metal Slug and Counter-Strike.

## Setup

The requirements for this project are:

* Clang
* Meson
* Ninja

Run the following Meson commands in order:

```shell
meson setup build
```

```shell
meson compile -C build
```

Copy the `assets/gamecontrollerdb.txt` file to `build/assets/gamecontrollerdb.txt`.

Afterwards, ensure the game runs:

```shell
./build/game
```
