# smartmet-utils

Shared build infrastructure and developer utility scripts for the [SmartMet Server](https://github.com/fmidev/smartmet-server) ecosystem. Every other SmartMet project (libraries, engines, plugins, tools) depends on this package at build time.

## What this module provides

Two artifacts that the rest of the ecosystem consumes:

- **`makefile-fragments/makefile.inc`** — the shared Makefile include that handles compiler detection, C++ standard selection, dependency resolution via the `REQUIRES` variable, sanitizer / LTO / analyzer flags, and install paths. Installed to `$(datadir)/smartmet/devel/makefile.inc`. Every project's `Makefile` starts with:

  ```makefile
  include $(shell smartbuildcfg --prefix)/share/smartmet/devel/makefile.inc
  ```

- **A set of developer tools** (Perl, shell, and one small C++ binary) installed to `$(bindir)/`. See [Tool reference](#tool-reference) below.

## Building, installing, testing

```bash
make                # Compile the only C++ tool (smartimgdiff_psnr) via Magick++
make install        # Install scripts, compiled tools, and makefile fragments (PREFIX=/usr by default)
make rpm            # Build the smartmet-utils RPM
make test           # Run tests in test/
make clean          # Remove build artifacts
```

Tests live under `test/` and validate:

- **pngdiff** — image comparison correctness (`test/pngdiff/`)
- **check-proj.mk, check-gdal.mk, check-geos.mk, check-sqlite.mk** — that `makefile.inc` finds the right paths for PROJ, GDAL, GEOS, and SQLite

```bash
make -C test test-pngcomp
make -C test test-check-proj
```

Tests reference `../smartcxxcheck` via the `__SMARTCXXCHECK__` variable so they work in-tree without installing the package first.

## Tool reference

| Script | Language | Purpose |
|---|---|---|
| `smartabicheck` | Bash | ABI sanity check for a freshly-built library (see [ABI checking](#abi-checking)) |
| `smartbuild` | Perl | Orchestrates building all SmartMet packages in dependency order; manages a workspace of bare git clones |
| `smartbuildcfg` | Perl | Reports the install prefix (derived from script location); also supports `--cflags`, `--libs` |
| `smartbuildrev` | Perl | Checks out a git revision into a temp dir and builds its RPM |
| `smartbuildtmprpm` | Perl | Like `smartbuildrev` but creates a temporary version commit first (for staging builds) |
| `smartcxxcheck` | Perl | Detects compiler (g++ / clang++), version, and the appropriate C++ standard; output is consumed by `makefile.inc` |
| `smartmkciconfig` | Perl | Generates `.circleci/config.yml` from templates; RHEL versions come from `~/.ci-rhel-versions` |
| `smartmkrelease` | Perl | Bumps the version in the `.spec` file, commits, and tags |
| `smartmktag` | Perl | Creates a git tag matching the current spec version if one doesn't exist |
| `smartpngdiff` | Shell | Compares two PNG images via PSNR (uses `smartimgdiff_psnr` for the measurement) |
| `smartimgdiff_psnr` | C++ | Computes PSNR between two images using ImageMagick; optionally writes a difference image |
| `smartrpmsort` | Perl | Keeps only the newest RPM version in a directory; archives older ones |
| `smarttestdep` | Shell | Installs test dependencies from `#TestRequires:` lines in spec files |

## Using makefile.inc in a SmartMet project

### `REQUIRES` — declare external dependencies

```makefile
REQUIRES = fmt jsoncpp gdal configpp
```

Each recognized name adds the right `-isystem`, `-L`, and `-l` flags. Unrecognized entries produce a warning; missing development files produce a hard error on build targets. Recognized names include `cairo`, `configpp`, `ctpp2`, `fmt`, `gdal`, `geos`, `icu-i18n`, `jsoncpp`, `libpq`, `libpqxx`, `librsvg`, `libcurl`, `mariadb`, `proj`, `spatialite`, `sqlite3`, `webp`, `xerces-c`, plus `filesystem` for `libstdc++fs`.

### Non-standard library paths

GDAL, GEOS, PROJ, SQLite, SpatiaLite, and libconfig are searched in versioned install prefixes (e.g. `/usr/gdal312/`, `/usr/proj97/`, `/usr/libconfig17/`) before falling back to system paths. This matches FMI's PGDG-style packaging.

### Build modes

```bash
make                    # Release (default): -O2 -DNDEBUG
make debug              # Debug: -Og + extra warnings
make profile            # -pg + optimizations
```

### Sanitizers and analyzers

```bash
make TSAN=yes           # ThreadSanitizer
make ASAN=yes           # AddressSanitizer + UBSan
make ANALYZER=yes       # gcc -fanalyzer (gcc-10+)
make USE_LTO=yes        # link-time optimization
```

### Custom flags

`EXTRA_FLAGS` is appended to `FLAGS` so per-build overrides work without editing the project Makefile:

```bash
make EXTRA_FLAGS=-DDEBUG_FOO
```

## ABI checking

For libraries (projects that produce `libsmartmet-<name>.so`), `makefile.inc` exposes two targets:

```bash
make abicheck       # quick check: would installed consumers still link?
make abicheck-deep  # full type-aware check via libabigail's abicompat
```

`make abicheck` finds every installed binary that depends on `libsmartmet-<name>` (via `rpm --whatrequires` + `readelf`), runs `ldd -r` against each with the freshly-built library preempting the installed one, and reports any consumers that would no longer link. It also diffs vtable / typeinfo / exported-data symbol sizes between old and new `.so` and warns on changes that wouldn't surface as missing symbols (virtual function added/reordered, exported global resized).

`make abicheck-deep` additionally runs `abicompat` per consumer, which uses DWARF to catch private struct-member layout changes, inline-body changes, and enum renumbering that the cheap layer can't see. Requires `libabigail` to be installed.

A separate, older facility — `make abi-check rev1=<tag> [rev2=<tag>]`, defined in `makefile-fragments/makefile-abicheck.inc` — compares two git revisions of the *same* library using `abi-compliance-checker`. Useful for release-time diffs but does not check downstream consumers.

## CI

CircleCI with `fmidev/smartmet-cibase-{8,10}` Docker images. `smartmkciconfig` generates `.circleci/config.yml` from templates. Workflow: build RPM, test (`ci-build test` against the installed package), upload to S3.

## License

MIT — see [LICENSE](LICENSE).

## Contributing

Bug reports and pull requests welcome on [GitHub](../../issues).
