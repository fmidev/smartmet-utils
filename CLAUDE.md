# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this repo is

`smartmet-utils` provides shared build infrastructure and developer utility scripts for the entire SmartMet ecosystem. It ships two key artifacts consumed by every other SmartMet project:

- **`makefile-fragments/makefile.inc`** — the shared Makefile include that handles compiler detection, C++ standard selection, `REQUIRES`-based dependency resolution (via pkg-config and custom paths), sanitizer/LTO/analyzer flags, and install paths. Installed to `$(datadir)/smartmet/devel/makefile.inc`.
- **A set of Perl/shell developer tools** installed to `$(bindir)/`, with man pages in `docs/man/`.

## Build commands

```bash
make                # Nothing to compile: all tools are scripts
make test           # Runs tests in test/ (proj/gdal/geos/sqlite detection)
make install        # Installs all scripts + man pages + makefile fragments
make rpm            # Builds RPM package (smartmet-utils)
make clean          # Removes build artifacts
```

There are no compiled artifacts: all tools are interpreted (Perl or shell). The image comparison tools `smartpngdiff`, `smartimagediff` and `smartimagediff_psnr` now live in `smartmet-library-regression`.

## Testing

Tests live in `test/` and validate:
- **check-proj.mk, check-gdal.mk, check-geos.mk, check-sqlite.mk** — that `makefile.inc` correctly detects library paths for PROJ, GDAL, GEOS, and SQLite

Run a single test group:
```bash
make -C test test-check-proj
make -C test test-check-sqlite
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
| `smartcheckinstalled` | Bash | Runs `ldd -r` on every installed binary, engine and plugin linking a `libsmartmet-*` library and explains unresolved symbols (engine late binding, missing `-l`, signature mismatch, stale package) |
| `smartcxxcheck` | Perl | Detects compiler (g++/clang++), version, and appropriate C++ standard; output consumed by `makefile.inc` |
| `smartmkrelease` | Perl | Bumps version in the `.spec` file, commits, and tags |
| `smartmktag` | Perl | Creates a git tag matching the current spec version if one doesn't exist |
| `smartmkciconfig` | Perl | Generates `.circleci/config.yml` from templates; RHEL versions come from `~/.ci-rhel-versions` |
| `smartmkpackagenote` | Bash | Generates the ELF note object (package name, version, git commit) that `makefile.inc` links into every binary; read by the server admin request `what=packages` |
| `smartrpmsort` | Perl | Keeps only the newest RPM version in a directory, archives older ones |
| `smarttestdep` | Shell | Installs test dependencies from spec file `#TestRequires:` lines |

## Key design details for makefile.inc

- **`REQUIRES` variable**: Projects list their dependencies (e.g., `REQUIRES = fmt jsoncpp gdal configpp`). Each recognized name adds the right `-isystem`, `-L`, and `-l` flags. Unrecognized entries produce a warning; missing dev files produce an error on build targets.
- **Non-standard library paths**: GDAL, GEOS, PROJ, SQLite, SpatiaLite, and libconfig are searched in versioned install prefixes (e.g., `/usr/gdal312/`, `/usr/proj97/`) before falling back to system paths.
- **Compiler auto-detection**: `smartcxxcheck` probes preprocessor defines to identify g++ vs clang++ and select `c++17` or `c++20` (GCC 11+).
- **Sanitizers**: `TSAN=yes` or `ASAN=yes` on the make command line enables thread or address sanitizer.
- **Coverage**: `COVERAGE=yes` builds with `--coverage` (gcov) for line coverage measurements.
- **Package note**: when the directory has a `smartmet-*.spec`, `makefile.inc` runs `smartmkpackagenote` at parse time to create `obj/smartmet-package-note.o` and appends it to `LIBS` (not `LDFLAGS`, which several library Makefiles do not use for linking). `SMARTMET_GIT_COMMIT` is resolved once in the checkout and exported so `make rpm` passes it into rpmbuild, whose build directory has no `.git`. The object also declares x86 IBT/SHSTK so it never disables CET.
- **ABI checking**: `makefile.inc` itself provides `make abicheck` (per-consumer link check + vtable/data signature diff) and `make abicheck-deep` (adds libabigail `abicompat`), backed by the `smartabicheck` script. Separately, the older `makefile-abicheck.inc` provides `make abi-check rev1=<tag> [rev2=<tag>]` for comparing two git revisions of the same library via `abi-dumper` + `abi-compliance-checker`.

## CI

CircleCI with `fmidev/smartmet-cibase-{8,10}` Docker images. The `smartmkciconfig` script generates `.circleci/config.yml`. Workflow: build RPM, test (install + `ci-build test`), upload to S3.
