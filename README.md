# Mega Drive Hello World

Minimal Sega Mega Drive / Genesis "Hello World" built with [SGDK](https://github.com/Stephane-D/SGDK).
Bootstrap project intended as a base for future Mega Drive game-development specs.

## Platform

- Target: Sega Mega Drive / Genesis (Motorola 68000)
- Host: macOS (x86_64)

## Requirements

- [SGDK](https://github.com/Stephane-D/SGDK) — cloned to `~/dev/sgdk`, referenced via the `GDK` environment variable
- `m68k-elf-gcc` / `m68k-elf-binutils` (installed via Homebrew) — the 68000 cross-compiler toolchain SGDK builds against
- `texinfo` (Homebrew) — build-only dependency of `m68k-elf-gcc`; macOS's built-in `makeinfo` (4.8) can't process modern GCC's docs
- Java (for SGDK's `rescomp`/`sizebnd` resource tools)
- Make
- [BlastEm](https://www.retrodev.com/blastem/) — Mega Drive emulator, built from source at `~/dev/blastem-src`, wrapped by `/usr/local/bin/blastem`

`GDK` is exported in `~/.zshrc`:

```bash
export GDK="$HOME/dev/sgdk"
```

### Note on `lib/libmd.a`

The SGDK repo ships a prebuilt `lib/libmd.a` (fat-LTO objects from GCC 13). This
toolchain is GCC 16, and `lto1` refuses to mix LTO bytecode across major GCC
versions. Rather than recompile the whole library locally (which produced a
build whose Z80 sound-driver boot code crashed BlastEm's Z80 core), the fix
was to strip the now-redundant `.gnu.lto_*` sections from each object in the
official archive with `m68k-elf-objcopy --wildcard --remove-section='.gnu.lto_*'`
and repackage it — the fat objects still carry a full native-code section, so
the official, known-good machine code links fine once the incompatible IR is
gone. `makefile.gen`'s release build no longer passes `-flto` for the same
reason (see the comments in `~/dev/sgdk/makefile.gen`).

## Build

```bash
make        # builds out/rom.bin
make clean  # removes out/
```

## ROM output

The build produces `out/rom.bin` (checksummed/padded) as well as `out/release/rom.bin`.

## Running in an emulator

```bash
blastem out/rom.bin
```

Or open the ROM in any Mega Drive/Genesis emulator (Gens, Kega Fusion, Regen, etc.).

Verified: `out/rom.bin` boots in BlastEm and visually displays `HELLO WORLD!`.

## VS Code

Project-local tasks are defined in `.vscode/tasks.json`:

- **SGDK: build** — runs `make` (default build task)
- **SGDK: clean** — runs `make clean`
- **SGDK: run in BlastEm** — builds then launches the ROM in BlastEm

## Detected tool versions

- macOS 12.7.4, x86_64 (MacBookPro11,4)
- `m68k-elf-gcc` (GCC) 16.2.0
- GNU Binutils (`m68k-elf-as` etc.) 2.47
- SGDK: git clone of [Stephane-D/SGDK](https://github.com/Stephane-D/SGDK), commit `2eac605` (2026-08-17)
- BlastEm: git clone of [libretro/blastem](https://github.com/libretro/blastem), commit `2fbf253` (2026-09-02)
- Java 1.8.0_503
- Git 2.37.1
- GNU Make 3.81
