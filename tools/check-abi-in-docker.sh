#!/bin/sh
set -eu

ROOT=/work
OUT="$ROOT/abi-out"
BUILD="$(mktemp -d /tmp/libspectrum-abi.XXXXXX)"
REPO="$BUILD/repo"

cleanup() {
  rm -rf "$BUILD"
}
trap cleanup EXIT HUP INT TERM

mkdir -p "$OUT"

case "$#" in
  0)
    OLD_REF=
    NEW_REF=HEAD
    ;;
  1)
    OLD_REF="$1"
    NEW_REF=HEAD
    ;;
  2)
    OLD_REF="$1"
    NEW_REF="$2"
    ;;
  *)
    echo "usage: $0 [OLD_REF [NEW_REF]]" >&2
    exit 1
    ;;
esac

git clone --no-local "$ROOT" "$REPO"

if [ -z "${OLD_REF}" ]; then
  OLD_REF="$(git -C "$REPO" describe --tags --abbrev=0)"
fi

git -C "$REPO" rev-parse --verify "$OLD_REF" >/dev/null
git -C "$REPO" rev-parse --verify "$NEW_REF" >/dev/null

ref_label() {
  git -C "$REPO" describe --tags --always "$1" | tr '/ ' '__'
}

build_one() {
  REF="$1"
  LABEL="$2"
  SRC="$BUILD/src-$LABEL"
  PREFIX="$BUILD/install-$LABEL"

  mkdir -p "$SRC"
  git -C "$REPO" archive "$REF" | tar -x -C "$SRC"

  cd "$SRC"

  sh ./autogen.sh

  CFLAGS='-Og -g' ./configure \
    --prefix="$PREFIX" \
    --with-fake-glib \
    --without-libgcrypt \
    --without-libaudiofile

  make -j"$(nproc)"
  make install

  abi-dumper \
    "$PREFIX/lib/libspectrum.so" \
    -o "$OUT/libspectrum-$LABEL.dump" \
    -lver "$LABEL" \
    -public-headers "$PREFIX/include"
}

OLD_LABEL="$(ref_label "$OLD_REF")"
NEW_LABEL="$(ref_label "$NEW_REF")"

build_one "$OLD_REF" "$OLD_LABEL"
build_one "$NEW_REF" "$NEW_LABEL"

abi-compliance-checker \
  -l libspectrum \
  -old "$OUT/libspectrum-$OLD_LABEL.dump" \
  -new "$OUT/libspectrum-$NEW_LABEL.dump" \
  -report-path "$OUT/libspectrum-abi-report-$OLD_LABEL-to-$NEW_LABEL.html"
