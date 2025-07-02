#!/bin/bash

# QMK Userspace Tap Dance Test Runner
# This script demonstrates how to run the tap dance tests for the soycabanillas keymap

set -e

echo "🧪 Running QMK Userspace Tap Dance Tests"
echo "========================================"

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to run a specific test
run_test() {
    local test_name="$1"
    echo -e "${YELLOW}Running: $test_name${NC}"

    if make test:$test_name; then
        echo -e "${GREEN}✅ $test_name PASSED${NC}"
        return 0
    else
        echo -e "${RED}❌ $test_name FAILED${NC}"
        return 1
    fi
}

# Check if QMK is properly configured
if ! qmk config -ro user.qmk_home >/dev/null 2>&1; then
    echo -e "${RED}❌ QMK not properly configured. Please run 'qmk setup' first.${NC}"
    exit 1
fi

if ! qmk config -ro user.overlay_dir >/dev/null 2>&1; then
    echo -e "${RED}❌ QMK userspace not configured. Please run 'qmk config user.overlay_dir=\"\$(realpath .)\"'${NC}"
    exit 1
fi

# Array of test names
tests=(
    "tap_dance:basic_tap_functionality"
    "tap_dance:basic_hold_functionality"
    "tap_dance:multiple_tap_functionality"
    "tap_dance:tap_hold_interaction"
    "tap_dance:layer_switching"
    "tap_dance:timeout_behavior"
    "tap_dance:interruption_handling"
)

# Track results
passed=0
failed=0

echo ""
echo "Running individual tests..."
echo ""

# Run each test
for test in "${tests[@]}"; do
    if run_test "$test"; then
        ((passed++))
    else
        ((failed++))
    fi
    echo ""
done

# Summary
echo "========================================"
echo "Test Summary:"
echo -e "  ${GREEN}Passed: $passed${NC}"
echo -e "  ${RED}Failed: $failed${NC}"
echo -e "  Total:  $((passed + failed))"

if [ $failed -eq 0 ]; then
    echo ""
    echo -e "${GREEN}🎉 All tests passed!${NC}"
    exit 0
else
    echo ""
    echo -e "${RED}💥 Some tests failed. Check the output above for details.${NC}"
    exit 1
fi
