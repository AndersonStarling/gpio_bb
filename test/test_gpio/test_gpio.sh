#!/bin/sh
# GPIO Test Suite - Modular Test Framework

# Configuration
GPIO=608

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Test counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# ============================================
# Helper Functions
# ============================================

pass() {
    PASSED_TESTS=$((PASSED_TESTS + 1))
    echo -e "${GREEN} PASS${NC} - $1"
}

fail() {
    FAILED_TESTS=$((FAILED_TESTS + 1))
    echo -e "${RED} FAIL${NC} - $1"
}

print_test_header() {
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    echo ""
    echo -e "${BLUE}[TEST $TOTAL_TESTS]${NC} $1"
    echo ""
}

cleanup() {
    echo ""
    echo "Cleaning up..."
    echo $GPIO > /sys/class/gpio/unexport 2>/dev/null
}

# ============================================
# Setup Functions
# ============================================

setup_gpio() {
    echo "Setting up GPIO ${GPIO}..."
    if echo ${GPIO} > /sys/class/gpio/export 2>/dev/null; then
        sleep 0.5
        echo "GPIO exported"
        return 0
    else
        echo "Failed to export GPIO"
        return 1
    fi
}

# ============================================
# Test Cases
# ============================================

test_direction_output() {
    print_test_header "GPIO IN-OUT on single gpio pin"
    
    # Set direction to OUTPUT
    echo "Setting direction to 'out'..."
    echo "out" > /sys/class/gpio/gpio${GPIO}/direction
    
    # Read and verify
    DIR=$(cat /sys/class/gpio/gpio${GPIO}/direction)

    echo "     Expected: out"
    echo "     Actual:      $DIR"
    echo ""
    
    # Compare
    if [ "$DIR" = "out" ]; then
        pass
    else
        fail
    fi
}

test_direction_input() {
    print_test_header "Set direction to INPUT"
    
    echo "Setting direction to 'in'..."
    echo "in" > /sys/class/gpio/gpio${GPIO}/direction
    
    DIR=$(cat /sys/class/gpio/gpio${GPIO}/direction)

    echo "     Expected: in"
    echo "     Actual:      $DIR"
    echo ""
    
    if [ "$DIR" = "in" ]; then
        pass
    else
        fail
    fi
}

# test_output_high() {
#     print_test_header "Set output value to HIGH"
    
#     # Ensure output mode
#     echo "out" > /sys/class/gpio/gpio${GPIO}/direction
    
#     echo "  → Setting value to 1..."
#     echo 1 > /sys/class/gpio/gpio${GPIO}/value
    
#     VAL=$(cat /sys/class/gpio/gpio${GPIO}/value)
    
#     echo "  → Reading value..."
#     echo "     Expected: 1"
#     echo "     Got:      $VAL"
#     echo ""
    
#     if [ "$VAL" = "1" ]; then
#         pass "Value is HIGH"
#     else
#         fail "Value mismatch (expected: 1, got: $VAL)"
#     fi
# }

# test_output_low() {
#     print_test_header "Set output value to LOW"
    
#     # Ensure output mode
#     echo "out" > /sys/class/gpio/gpio${GPIO}/direction
    
#     echo "  → Setting value to 0..."
#     echo 0 > /sys/class/gpio/gpio${GPIO}/value
    
#     VAL=$(cat /sys/class/gpio/gpio${GPIO}/value)
    
#     echo "  → Reading value..."
#     echo "     Expected: 0"
#     echo "     Got:      $VAL"
#     echo ""
    
#     if [ "$VAL" = "0" ]; then
#         pass "Value is LOW"
#     else
#         fail "Value mismatch (expected: 0, got: $VAL)"
#     fi
# }

# test_toggle() {
#     print_test_header "GPIO toggle test (5 cycles)"
    
#     # Ensure output mode
#     echo "out" > /sys/class/gpio/gpio${GPIO}/direction
    
#     echo "  → Toggling GPIO 5 times..."
    
#     TOGGLE_PASS=true
#     for i in {1..5}; do
#         # Set HIGH
#         echo 1 > /sys/class/gpio/gpio${GPIO}/value
#         sleep 0.1
#         VAL=$(cat /sys/class/gpio/gpio${GPIO}/value)
#         if [ "$VAL" != "1" ]; then
#             TOGGLE_PASS=false
#             echo " Cycle $i: HIGH failed (got: $VAL)"
#             break
#         fi
#         echo " Cycle $i: HIGH"
        
#         # Set LOW
#         echo 0 > /sys/class/gpio/gpio${GPIO}/value
#         sleep 0.1
#         VAL=$(cat /sys/class/gpio/gpio${GPIO}/value)
#         if [ "$VAL" != "0" ]; then
#             TOGGLE_PASS=false
#             echo " Cycle $i: LOW failed (got: $VAL)"
#             break
#         fi
#         echo " Cycle $i: LOW"
#     done
    
#     echo ""
    
#     if [ "$TOGGLE_PASS" = true ]; then
#         pass "Toggle test completed successfully"
#     else
#         fail "Toggle test failed at cycle $i"
#     fi
# }

# ============================================
# Summary
# ============================================

print_summary() {
    echo ""
    echo "=========================================="
    echo "Test Summary"
    echo "=========================================="
    echo -e "Total Tests: ${BLUE}$TOTAL_TESTS${NC}"
    echo -e "Passed:      ${GREEN}$PASSED_TESTS${NC}"
    echo -e "Failed:      ${RED}$FAILED_TESTS${NC}"
    echo ""
    
    if [ $FAILED_TESTS -eq 0 ]; then
        echo -e "${GREEN}=== ALL TESTS PASSED ===${NC}"
        return 0
    else
        echo -e "${RED}=== SOME TESTS FAILED ===${NC}"
        return 1
    fi
}

# ============================================
# Main Function
# ============================================

main() {
    echo "=========================================="
    echo "GPIO Test Suite"
    echo "=========================================="
    echo "GPIO Pin: $GPIO"
    echo "Date: $(date)"
    echo ""
    
    # Setup
    if ! setup_gpio; then
        echo "Setup failed! Exiting..."
        exit 1
    fi
    
    # Run test cases
    test_direction_output
    test_direction_input
    # test_output_high
    # test_output_low
    # test_toggle

    # Print summary
    print_summary
    
    return $?
}

# ============================================
# Execute
# ============================================

trap cleanup EXIT
main
exit $?