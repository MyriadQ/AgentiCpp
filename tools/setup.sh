#!/usr/bin/env bash
# Day 1: toolchain. Idempotent — safe to re-run.
# Verified against Debian 13 (trixie) on WSL2. See PLAN.md §1.
set -euo pipefail

PKGS=(
  build-essential git curl jq ca-certificates
  clang-19 libclang-rt-19-dev g++-14
  lldb-19 gdb valgrind
  cmake ninja-build pkg-config
  libcurl4-openssl-dev
  libseccomp-dev
  linux-perf ltrace strace xxd
  asciinema            # demo recording (PLAN §2)
)

echo "==> apt update"
sudo apt-get update -qq
echo "==> installing: ${PKGS[*]}"
sudo apt-get install -y --no-install-recommends "${PKGS[@]}"

echo
echo "==> versions"
for t in clang++-19 g++-14 cmake ninja gdb lldb-19 valgrind; do
  printf '  %-14s ' "$t"
  command -v "$t" >/dev/null && "$t" --version 2>/dev/null | head -1 || echo MISSING
done
for p in libcurl libseccomp; do
  printf '  %-14s ' "$p"
  pkg-config --modversion "$p" 2>/dev/null || echo MISSING
done

echo
echo "==> capability checks (PLAN §7.3 / §12)"
printf '  userns (unshare -Ur) : '
unshare -Ur true 2>/dev/null && echo OK || echo BLOCKED
printf '  seccomp              : '
grep -q '^Seccomp:' /proc/self/status && echo "$(grep '^Seccomp:' /proc/self/status)" || echo UNKNOWN
printf '  inotify watches      : '
cat /proc/sys/fs/inotify/max_user_watches
printf '  fs of \$HOME          : '
df -T "$HOME" | tail -1 | awk '{print $2}'

echo
echo "==> next (PLAN.md day 1 gate)"
cat <<'NEXT'
  cmake --preset debug
  cmake --build --preset debug
  ctest --preset debug
  ./build/debug/bin/agentd --version

  If \$HOME is not ext4, move the project. If userns is BLOCKED, PLAN.md D22 is cut.
NEXT
