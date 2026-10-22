#!/usr/bin/env bash
# Builds and runs the native unit test suite for the hardware-independent
# firmware logic, then reports line-coverage metrics via gcov.
#
# Usage: scripts/run_tests.sh
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

SRC_DIR="firmware/src"
INC_DIR="firmware/include"
TEST_DIR="test"
BUILD_DIR="build/test"

rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

# main.cpp is intentionally excluded: it's the Arduino/ESP32 hardware entry
# point and depends on Arduino.h, Wire.h, Adafruit_PWMServoDriver.h and
# PS4Controller.h, none of which exist off-target.
SOURCES=(
  "$SRC_DIR/controller_input.cpp"
  "$SRC_DIR/motion_control.cpp"
  "$SRC_DIR/servo_control.cpp"
  "$SRC_DIR/expression_engine.cpp"
  "$TEST_DIR/test_main.cpp"
)

echo "Compiling native test binary..."
g++ -std=c++17 -Wall -Wextra -O0 --coverage \
    -I "$INC_DIR" -I "$TEST_DIR" \
    "${SOURCES[@]}" \
    -o "$BUILD_DIR/run_tests"

echo
echo "Running tests:"
echo "--------------"
set +e
"./$BUILD_DIR/run_tests"
TEST_EXIT_CODE=$?
set -e

echo
echo "Line coverage (hardware-independent logic only):"
echo "--------------------------------------------------"
for s in "${SOURCES[@]}"; do
  [[ "$s" == *.cpp && "$s" != *test_main* ]] || continue
  base="$(basename "${s%.cpp}")"
  gcov -o "$BUILD_DIR/run_tests-${base}.gcda" "$s" 2>/dev/null \
    | grep -E "File '(firmware|test)|Lines executed" \
    | grep -A1 "^File '\(firmware\|test\)"
done

# gcov drops .gcov files in the current directory; keep the tree clean.
rm -f ./*.gcov

exit $TEST_EXIT_CODE
