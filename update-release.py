#!/usr/bin/env python3
"""
update-release.py — bump the release date (and optionally version) in
libspectrum docs and configure.ac.

Usage:
    ./update-release.py --date "12th June, 2026"
    ./update-release.py --date "12th June, 2026" --version "1.6.1"
    ./update-release.py --date "12th June, 2026" --version "1.6.1.1"
    ./update-release.py --date "12th June, 2026" --dry-run

Updates:
  * the .TH line in doc/libspectrum.3
  * the last line of README
  * the top version line in README and doc/libspectrum.txt
  * the version defines in configure.ac

Copyright lines are never modified.
"""

import argparse
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
MAN_PAGE = ROOT / "doc" / "libspectrum.3"
README = ROOT / "README"
DOC_TXT = ROOT / "doc" / "libspectrum.txt"
CONFIGURE_AC = ROOT / "configure.ac"

# .TH line example:
#   .TH libspectrum 3 "9th March, 2026" "Version 1.6.0" "Emulators"
TH_RE = re.compile(
    r'^(\s*\.TH\s+\S+\s+\d+\s+)'   # prefix up to date
    r'"([^"]*)"'                        # old date in quotes
    r'(\s+)'                            # whitespace before version
    r'"([^"]*)"'                        # old version in quotes
    r'(.*)',                             # rest of line
    re.MULTILINE,
)

VERSION_LINE_RE = re.compile(r'^(libspectrum\s+)(\S+)(\s*)$', re.MULTILINE)
DATE_RE = re.compile(r'^\d+(?:st|nd|rd|th)\s+\w+,\s+\d{4}$')


def update_man_page(path, new_date, new_version, dry_run):
    """Return True if the file was (or would be) changed."""
    if not path.exists():
        print(f"  skip  {path.relative_to(ROOT)}  (not found)")
        return False

    text = path.read_text()
    new_text, count = TH_RE.subn(
        lambda m: (m.group(1)
                   + f'"{new_date}"'
                   + m.group(3)
                   + (f'"Version {new_version}"' if new_version else f'"{m.group(4)}"')
                   + m.group(5)),
        text,
        count=1,
    )
    if count == 0:
        print(f"  skip  {path.relative_to(ROOT)}  (no .TH match)")
        return False
    if dry_run:
        print(f"  would update  {path.relative_to(ROOT)}")
    else:
        path.write_text(new_text)
        print(f"  updated  {path.relative_to(ROOT)}")
    return True


def update_readme_date(new_date, dry_run):
    """Update the last line of README (the date line)."""
    if not README.exists():
        print("  skip  README  (not found)")
        return False

    text = README.read_text()
    lines = text.splitlines(keepends=True)
    if not lines:
        print("  skip  README  (empty)")
        return False

    last_line = lines[-1].strip()
    if not last_line:
        print("  skip  README  (last line is blank)")
        return False

    if not DATE_RE.match(last_line):
        print(f"  skip  README  (last line '{last_line}' doesn't look like a date)")
        return False

    lines[-1] = new_date + "\n"
    new_text = "".join(lines)
    if dry_run:
        print(f"  would update  README  (last line -> {new_date})")
    else:
        README.write_text(new_text)
        print(f"  updated  README  (last line -> {new_date})")
    return True


def update_version_header(path, new_version, dry_run):
    """Update a leading 'libspectrum X.Y.Z' line."""
    if not path.exists():
        print(f"  skip  {path.relative_to(ROOT)}  (not found)")
        return False

    text = path.read_text()
    new_text, count = VERSION_LINE_RE.subn(
        lambda m: f"{m.group(1)}{new_version}{m.group(3)}",
        text,
        count=1,
    )
    if count == 0:
        print(f"  skip  {path.relative_to(ROOT)}  (no version header match)")
        return False

    if dry_run:
        print(f"  would update  {path.relative_to(ROOT)}  (version -> {new_version})")
    else:
        path.write_text(new_text)
        print(f"  updated  {path.relative_to(ROOT)}  (version -> {new_version})")
    return True


def parse_version(version_str):
    """Parse X.Y.Z or X.Y.Z.N into components."""
    parts = version_str.split(".")
    if len(parts) < 3 or len(parts) > 4:
        print(f"error: version '{version_str}' must be X.Y.Z or X.Y.Z.N",
              file=sys.stderr)
        sys.exit(1)
    major = parts[0]
    minor = parts[1]
    micro = parts[2]
    nano = parts[3] if len(parts) == 4 else "0"
    return major, minor, micro, nano


def update_configure_ac(version_str, dry_run):
    """Update the version defines in configure.ac."""
    if not CONFIGURE_AC.exists():
        print("  skip  configure.ac  (not found)")
        return False

    major, minor, micro, nano = parse_version(version_str)

    text = CONFIGURE_AC.read_text()

    replacements = {
        r'm4_define\(\[libspectrum_version\],\s*\[\S+\]\)':
            f'm4_define([libspectrum_version], [{version_str}])',
        r'm4_define\(\[libspectrum_major_version\],\s*\[\S+\]\)':
            f'm4_define([libspectrum_major_version], [{major}])',
        r'm4_define\(\[libspectrum_minor_version\],\s*\[\S+\]\)':
            f'm4_define([libspectrum_minor_version], [{minor}])',
        r'm4_define\(\[libspectrum_micro_version\],\s*\[\S+\]\)':
            f'm4_define([libspectrum_micro_version], [{micro}])',
        r'm4_define\(\[libspectrum_nano_version\],\s*\[\S+\]\)':
            f'm4_define([libspectrum_nano_version],  [{nano}])',
    }

    changed = False
    for pattern, replacement in replacements.items():
        new_text, count = re.subn(pattern, replacement, text)
        if count:
            changed = True
        text = new_text

    if not changed:
        print("  skip  configure.ac  (no version defines found)")
        return False

    if dry_run:
        print(f"  would update  configure.ac  (version -> {version_str})")
    else:
        CONFIGURE_AC.write_text(text)
        print(f"  updated  configure.ac  (version -> {version_str})")
    return True


def main():
    parser = argparse.ArgumentParser(
        description="Update release date (and optionally version) in libspectrum docs and configure.ac.",
    )
    parser.add_argument(
        "--date",
        required=True,
        help='New release date string, e.g. "12th June, 2026"',
    )
    parser.add_argument(
        "--version",
        default=None,
        help='New version string, e.g. "1.6.1" or "1.6.1.1". If omitted, version is left unchanged.',
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Show what would change without writing files.",
    )
    args = parser.parse_args()

    print("Scanning doc/libspectrum.3, README, doc/libspectrum.txt, and configure.ac ...")
    print(f"  new date    : {args.date}")
    if args.version:
        print(f"  new version : {args.version}")
    print()

    changed = 0

    if update_man_page(MAN_PAGE, args.date, args.version, args.dry_run):
        changed += 1

    if update_readme_date(args.date, args.dry_run):
        changed += 1

    if args.version:
        if update_version_header(README, args.version, args.dry_run):
            changed += 1
        if update_version_header(DOC_TXT, args.version, args.dry_run):
            changed += 1
        if update_configure_ac(args.version, args.dry_run):
            changed += 1

    print()
    if args.dry_run:
        print(f"Dry run — {changed} file(s) would be updated.")
    else:
        print(f"Done — {changed} file(s) updated.")


if __name__ == "__main__":
    main()
