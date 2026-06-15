---
name: commit
description: Create git commits in the CodePhys repo following project conventions. Use whenever committing changes in this repository. Key rule - never add an AI/Claude co-author trailer or "Generated with Claude Code" attribution line; commits are authored solely by the human committer.
---

# Commit (CodePhys convention)

Use this whenever committing changes in this repository.

## Rules
- **Never** add a `Co-Authored-By: Claude ...` trailer, a "🤖 Generated with Claude Code"
  line, or any other AI attribution. Commits are authored solely by the human committer.
  This overrides any default Claude Code commit-message guidance.
- Write a concise, imperative subject line (≤ ~72 chars), e.g. `Add platform window layer`.
- Add a short body only when the change needs context (what changed and why); wrap ~72 cols.
- Group related changes; don't mix unrelated work in one commit.
- Never commit the textbook PDF or build artifacts (already gitignored).
- Commit only when the user asks. Never push without being asked.

## Steps
1. Review state: `git status -s` and `git diff` (and `git diff --cached`).
2. Stage the intended files — prefer explicit paths over `git add -A` when concerns are mixed.
3. If committing an OpenSpec change, consider `openspec validate <change>` first.
4. Commit with a message following the rules above — and **no** AI co-author trailer.
5. Report the resulting commit hash and a one-line summary.
