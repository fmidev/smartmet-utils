# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this repo is

`smartmet-utils` provides shared build infrastructure and developer utility scripts for the entire SmartMet ecosystem. It ships two key artifacts consumed by every other SmartMet project:

- **`makefile-fragments/makefile.inc`** — the shared Makefile include that handles compiler detection, C++ standard selection, `REQUIRES`-based dependency resolution (via pkg-config and custom paths), sanitizer/LTO/analyzer flags, and install paths. Installed to `$(datadir)/smartmet/devel/makefile.inc`.
- **A set of Perl/shell/C++ developer tools** installed to `$(bindir)/`.

## Build commands

```bash
make                # Compiles only the C++ tool (smartimgdiff_psnr) via Magick++
make test           # Runs tests in test/ (png comparison, proj/gdal/geos/sqlite detection)
make install        # Installs all scripts + compiled tools + makefile fragments
make rpm            # Builds RPM package (smartmet-utils)
make clean          # Removes build artifacts
```

The only compiled artifact is `smartimgdiff_psnr` (C++17, links against Magick++). All other tools are interpreted (Perl or shell).

## Testing

Tests live in `test/` and validate:
- **pngdiff** — image comparison correctness (`test/pngdiff/`)
- **check-proj.mk, check-gdal.mk, check-geos.mk, check-sqlite.mk** — that `makefile.inc` correctly detects library paths for PROJ, GDAL, GEOS, and SQLite

Run a single test group:
```bash
make -C test test-pngcomp
make -C test test-check-proj
```

Tests reference `../smartcxxcheck` via `__SMARTCXXCHECK__` so they work from the source tree without installation.

## Developer tools overview

| Script | Language | Purpose |
|---|---|---|
| `smartabicheck` | Bash | ABI sanity check for a freshly-built library against installed consumers (used by `make abicheck` / `make abicheck-deep` in `makefile.inc`) |
| `smartbuild` | Perl | Orchestrates building all SmartMet packages in dependency order from git repos; manages workspace of bare clones |
| `smartbuildcfg` | Perl | `--prefix` returns the install prefix (derived from script location); also supports `--cflags`/`--libs` |
| `smartbuildrev` | Perl | Checks out a git revision into a temp dir and builds its RPM |
| `smartbuildtmprpm` | Perl | Like smartbuildrev but creates a temporary version commit first (for staging builds) |
| `smartcxxcheck` | Perl | Detects compiler (g++/clang++), version, and appropriate C++ standard; output consumed by `makefile.inc` |
| `smartmkrelease` | Perl | Bumps version in the `.spec` file, commits, and tags |
| `smartmktag` | Perl | Creates a git tag matching the current spec version if one doesn't exist |
| `smartmkciconfig` | Perl | Generates `.circleci/config.yml` from templates; RHEL versions come from `~/.ci-rhel-versions` |
| `smartpngdiff` | Shell | Compares two PNG images via PSNR (uses `smartimgdiff_psnr` for the actual measurement) |
| `smartimgdiff_psnr` | C++ | Computes PSNR between two images using ImageMagick; optionally writes a difference image |
| `smartrpmsort` | Perl | Keeps only the newest RPM version in a directory, archives older ones |
| `smarttestdep` | Shell | Installs test dependencies from spec file `#TestRequires:` lines |

## Key design details for makefile.inc

- **`REQUIRES` variable**: Projects list their dependencies (e.g., `REQUIRES = fmt jsoncpp gdal configpp`). Each recognized name adds the right `-isystem`, `-L`, and `-l` flags. Unrecognized entries produce a warning; missing dev files produce an error on build targets.
- **Non-standard library paths**: GDAL, GEOS, PROJ, SQLite, SpatiaLite, and libconfig are searched in versioned install prefixes (e.g., `/usr/gdal312/`, `/usr/proj97/`) before falling back to system paths.
- **Compiler auto-detection**: `smartcxxcheck` probes preprocessor defines to identify g++ vs clang++ and select `c++17` or `c++20` (GCC 11+).
- **Sanitizers**: `TSAN=yes` or `ASAN=yes` on the make command line enables thread or address sanitizer.
- **ABI checking**: `makefile.inc` itself provides `make abicheck` (per-consumer link check + vtable/data signature diff) and `make abicheck-deep` (adds libabigail `abicompat`), backed by the `smartabicheck` script. Separately, the older `makefile-abicheck.inc` provides `make abi-check rev1=<tag> [rev2=<tag>]` for comparing two git revisions of the same library via `abi-dumper` + `abi-compliance-checker`.

## CI

CircleCI with `fmidev/smartmet-cibase-{8,10}` Docker images. The `smartmkciconfig` script generates `.circleci/config.yml`. Workflow: build RPM, test (install + `ci-build test`), upload to S3.
