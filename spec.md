# SPEC.md — Mega Drive Hello World

**Version:** 0.1
**Goal:** bootstrap a working Sega Mega Drive/Genesis development environment on macOS.

## Objective

Create a minimal SGDK project that:

1. Builds successfully with `make`.
2. Produces a Mega Drive ROM.
3. Boots in a Mega Drive emulator.
4. Displays `HELLO WORLD!`.
5. Is ready for future game-development specs.

## Environment

Target:

* macOS
* `arm64` and `x86_64`
* C + SGDK
* VS Code
* Git
* Mega Drive emulator

Detect the host architecture and existing tools before making changes.

## Dependencies

Required:

* Git
* Make
* SGDK
* SGDK-compatible 68000 toolchain
* Mega Drive/Genesis emulator
* VS Code integration

Rules:

* Reuse working installations.
* Prefer official/reliable sources.
* Prefer native Apple Silicon tools when available.
* Do not install unnecessary dependencies.
* Do not modify unrelated system configuration.
* Back up shell configuration before modifying it.
* Never overwrite existing projects.
* Never push to a remote Git repository.
* Never commit secrets.
* Ask before destructive or privileged operations.

## SGDK

Install/configure SGDK if missing.

Ensure:

```bash
echo "$GDK"
```

points to a valid SGDK installation and that its required tools are available.

Verify the toolchain by performing an actual Mega Drive build.

Do not continue with a broken toolchain.

## Project

Create:

```text
mega-drive-hello-world/
├── src/main.c
├── res/
├── inc/
├── out/
├── .vscode/
│   └── tasks.json
├── .gitignore
├── Makefile
└── README.md
```

Additional SGDK-generated files are allowed.

## Program

`src/main.c` must:

* use SGDK;
* target Mega Drive/Genesis;
* display `HELLO WORLD!`;
* remain running;
* use the appropriate SGDK frame/VBlank mechanism;
* contain no macOS-specific runtime dependency.

Keep the implementation minimal.

## Build

The project must support:

```bash
make
make clean
```

`make` must generate a Mega Drive ROM, preferably under `out/`.

The Makefile must use the configured SGDK environment rather than hardcoded user-specific paths.

## Emulator

Use an existing compatible emulator if available.

Otherwise install a reputable Mega Drive emulator; prefer BlastEm when suitable.

Verify that the generated ROM boots.

If visual verification is available, verify:

```text
HELLO WORLD!
```

Never claim visual verification if it was not possible.

## VS Code

Configure project-local VS Code tasks for:

```bash
make
make clean
```

Add an emulator task only if reliable.

Do not modify global VS Code configuration unnecessarily.

## Git

Initialize Git and create an appropriate `.gitignore`.

Create the initial commit:

```text
Initial Mega Drive Hello World project
```

Do not configure or push a remote.

## README

Document:

* project purpose;
* platform;
* SGDK/toolchain requirements;
* build command;
* clean command;
* ROM location;
* emulator usage;
* detected tool versions.

## Execution Strategy

Work autonomously:

```text
inspect → install/configure → create → build → verify → fix → rebuild
```

For recoverable errors, diagnose and fix them before stopping.

Do not report success without verification.

## Acceptance Criteria

The task is **DONE** only when all mandatory checks pass:

* [ ] SGDK available and functional.
* [ ] Toolchain successfully builds a Mega Drive ROM.
* [ ] Project structure exists.
* [ ] `src/main.c` exists.
* [ ] `make clean && make` succeeds.
* [ ] ROM is generated.
* [ ] Emulator accepts and boots the ROM.
* [ ] `HELLO WORLD!` is visually verified when possible.
* [ ] VS Code build task exists.
* [ ] Git repository initialized.
* [ ] Initial commit exists.
* [ ] README exists.
* [ ] No secrets or unrelated files were modified.

## Definition of Done

This command must succeed:

```bash
make clean && make
```

and produce a bootable Mega Drive ROM.

Final response:

```text
Status: READY | BLOCKED
Architecture:
macOS:
SGDK:
Toolchain:
Emulator:
Project:
ROM:
Build:
Emulator:
Hello World:
Git:
```

If `BLOCKED`, report the blocker and the remaining action required.
