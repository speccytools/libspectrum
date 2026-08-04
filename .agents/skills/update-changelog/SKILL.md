---
name: update-changelog
description: Drafts a new top-of-file libspectrum release entry in ChangeLog using unrecorded commits and the project’s existing format. Use when preparing a libspectrum release or updating release notes. Produces a draft first, then waits for confirmation before editing ChangeLog.
---

# Update ChangeLog

This is a project-specific skill for preparing a libspectrum release ChangeLog entry.

Use this skill when the user wants to update `ChangeLog` for a release.

## Rules

- Follow the existing style in `ChangeLog` exactly.
- Preserve all existing entries unchanged.
- Draft first.
- Do not edit `ChangeLog` until the user confirms.
- If the release version is not yet known, ask the user for it before drafting the new entry.
- Use the most recent unrecorded commit date for the new heading.
- Use Philip Kendall’s maintainer identity in the heading:
  `YYYY-MM-DD  Philip Kendall  <philip-fuse@shadowmagic.org.uk>`

## Workflow

1. Read `ChangeLog` in the current directory.
2. Find the most recent commit that modified `ChangeLog`:
   ```bash
   git log ChangeLog
   ```
3. Identify unrecorded commits since that commit:
   ```bash
   git log --format="%H %ai %s" <last-change-hash>..HEAD
   git log <last-change-hash>..HEAD
   ```
4. Filter out noise commits, including:
   - merges
   - revert accidental commit
   - add ignore
   - update for version
   - first updates for
   - bring up to date
   - tidy up commit text
   - similar meta-commits that do not describe user-visible changes
5. Summarise the remaining commits into a new release entry.
6. Ask the user for the release version if it was not already provided.
7. Draft a new entry to be inserted at the top of `ChangeLog` after the first line of the file.
8. Show the draft to the user and ask for confirmation.
9. Only after confirmation, edit `ChangeLog`.

## Required output structure

Create a new dated section at the top of `ChangeLog` with:

- a date heading using the most recent unrecorded commit date
- a release line:
  `* libspectrum X.Y.Z released.`
- grouped entries below it

## Grouping rules

- Use top-level bullets for:
  - notable user-visible changes
  - major new features
  - important bug fixes
  - API changes
  - compatibility changes
  - significant build or CI changes
- Put smaller fixes, cleanups, tests, warnings, documentation tweaks, and minor build changes under:

  ```text
          * Various minor bug fixes/improvements:
            * ...
  ```

## Formatting rules

- Use 8 spaces before top-level `*` bullets.
- Use 10 spaces before nested `*` bullets.
- Wrap long lines to match the existing file.
- Continuation lines should align under the bullet text.
- End bullet items with periods.
- Attribute each change at the end in parentheses, for example:
  `(Sergio Baldoví).`
- For multiple authors, use existing forms already present in `ChangeLog`, such as:
  - `(Philip Kendall and Sergio Baldoví).`
  - `(Miroslav Ďurčík, Sergio Baldoví).`
- If a reporter or tester is mentioned, use:
  `(Author, thanks Reporter).`
- If there is a bug reference, include it before the author attribution, for example:
  `Fix RZX recording in competition mode (fixes bug #485) (Sergio Baldoví, thanks crem).`
- Keep wording concise and user-facing.
- Prefer verbs like:
  - Add
  - Fix
  - Enable
  - Support
  - Remove
  - Include
  - Use
  - Document
  - Replace
  - Avoid
  - Ensure
- Do not list every commit mechanically.
- Summarise related commits into a single readable ChangeLog item where appropriate.

## Drafting instructions

When presenting the draft:

- show only the proposed new ChangeLog entry
- do not edit the file yet
- ask the user to confirm or request changes

When the user confirms:

- insert the new entry at the top of `ChangeLog` after the first line
- do not alter older entries
