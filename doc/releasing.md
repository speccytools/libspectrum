# Releasing libspectrum

## Overview

This document describes the manual steps used to prepare a libspectrum
release. The `update-release.py` helper updates release metadata in the
main documentation files and in `configure.ac`, but it does not perform
the full release process by itself.

## Release metadata helper

The repository includes `update-release.py` in the top-level directory.

### Usage

Preview changes:

```sh
./update-release.py --date "12th June, 2026" --version "1.6.2" --dry-run
```

Update the release date only:

```sh
./update-release.py --date "12th June, 2026"
```

Update both release date and version:

```sh
./update-release.py --date "12th June, 2026" --version "1.6.2"
```

### Files updated

When given `--date`, the script updates:

- the `.TH` line in `doc/libspectrum.3`
- the final date line in `README`

When given `--version`, it also updates:

- the top version line in `README`
- the top version line in `doc/libspectrum.txt`
- the version definitions in `configure.ac`

### Files not updated

The script does not update:

- `ChangeLog`
- `libspectrum_la_LDFLAGS` / libtool ABI version info in `Makefile.am`
- generated files
- release tarballs, tags, or announcements

## Optional agent skill for ChangeLog updates

The repository also includes an optional maintainer skill at
`.agents/skills/update-changelog/SKILL.md`. If your agent environment
supports repository skills, it can use that skill to draft a new
release entry for `ChangeLog` in the existing project format.

The skill is a convenience helper only. It is not required to build
libspectrum or to prepare a release. If it is unavailable, update
`ChangeLog` manually in the existing style.

## Typical release workflow

1. Update `ChangeLog`. If your agent environment supports repository
   skills, use `.agents/skills/update-changelog/SKILL.md` to draft the
   new top-of-file release entry, review it, and then apply it after
   confirmation. Otherwise, update `ChangeLog` manually in the existing
   format.
2. Run `update-release.py` with `--dry-run`.
3. Run it again without `--dry-run` once the changes look correct.
4. Review the resulting diff.
5. If `configure.ac` changed, regenerate the build system as needed.
6. Build the project.
7. Run the test suite.
8. Create the release tarball, tag, and publish using the normal
   maintainer process.

## Versioning notes

The project release version in `configure.ac` is separate from the
library ABI version tracked in `libspectrum_la_LDFLAGS` in `Makefile.am`.
Review both during a release.

For libtool versioning rules, see the comment above
`libspectrum_la_LDFLAGS` in `Makefile.am`.

## ABI compatibility check with Docker

If you prepare releases, you can run the ABI comparison inside Docker
using the helper files in `tools/`.

The helper does not build in your current working directory. Instead, it
clones the repository into a temporary directory inside the container,
compares exported source snapshots from that clone, and removes the
temporary directory before exit. Only `abi-out/` is written back to the
host checkout.

### Build the ABI checker image

From the top-level `libspectrum/` directory, run:

```sh
docker build -f tools/Dockerfile.abi -t libspectrum-abi-check .
```

### Run the ABI comparison

With no arguments, the helper compares the most recent tag against
`HEAD`:

```sh
docker run --rm \
  -v "$PWD:/work" \
  libspectrum-abi-check \
  sh /work/tools/check-abi-in-docker.sh
```

You can also override the refs explicitly. With one argument, it is
compared against `HEAD`:

```sh
docker run --rm \
  -v "$PWD:/work" \
  libspectrum-abi-check \
  sh /work/tools/check-abi-in-docker.sh 1.6.0
```

With two arguments, both old and new refs are explicit:

```sh
docker run --rm \
  -v "$PWD:/work" \
  libspectrum-abi-check \
  sh /work/tools/check-abi-in-docker.sh 1.6.0 HEAD
```

The helper script will:

1. clone the mounted repository into a temporary directory inside the
   container
2. select the old ref and new ref to compare
3. export each ref into a separate source tree without mutating your
   checkout
4. bootstrap Autotools in each exported tree with `autogen.sh`
5. configure both trees with a stable feature set
6. build and install each tree into a separate prefix
7. run `abi-dumper` on each installed shared library
8. run `abi-compliance-checker` and write an HTML report under
   `abi-out/`

The configure flags used are:

```sh
--with-fake-glib --without-libgcrypt --without-libaudiofile
```

Using a fixed feature set helps keep the exported ABI consistent across
runs and avoids accidental differences caused by optional dependencies.

The Docker image also installs Universal Ctags for `abi-dumper`, and the
helper configures builds with `CFLAGS='-Og -g'` to improve ABI analysis
and avoid `abi-dumper` warnings about release-style optimisation flags.

By default, this compares committed `HEAD`. Uncommitted local changes in
your working tree are not included.

### Outputs

The run creates these host-visible files under `abi-out/`:

- `libspectrum-<ref>.dump` for each side of the comparison
- `libspectrum-abi-report-<old>-to-<new>.html`

Open the HTML report in a browser and use it, together with the libtool
rules in `Makefile.am`, to decide whether `libspectrum_la_LDFLAGS`
should change for a backward-compatible ABI update or an ABI break.

## Verification

After updating release metadata, verify with:

```sh
./configure
make
make check
```

Also inspect the diff manually to confirm the release date and version
were updated in the expected files.

## Caveats

- The date should be passed in the format used by the existing docs, for
  example `12th June, 2026`.
- The script is intended as a maintainer convenience tool.
- Review all changes before creating a release.
