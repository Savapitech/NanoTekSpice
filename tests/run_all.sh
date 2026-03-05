#!/bin/bash
# Comprehensive test script for NanoTekSpice
# Tests all components from doc/nts_single/ and doc/nts_advanced/

BINARY="./test"
PASS=0
FAIL=0
TOTAL=0

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

run_test() {
    local name="$1"
    local nts_file="$2"
    local input="$3"
    local expected="$4"

    TOTAL=$((TOTAL + 1))
    local output
    output=$(printf "$input" | timeout 5 $BINARY "$nts_file" 2>&1)
    local exit_code=$?

    if [ $exit_code -eq 124 ]; then
        echo -e "  ${RED}[TIMEOUT]${NC} $name"
        FAIL=$((FAIL + 1))
        return
    fi

    local filtered
    filtered=$(echo "$output" | grep -E "^\s*(out|s |out_)" | sed 's/^[[:space:]]*//')

    local match=true
    while IFS= read -r line; do
        [ -z "$line" ] && continue
        if ! echo "$filtered" | grep -qF "$line"; then
            match=false
            break
        fi
    done <<< "$expected"

    if $match; then
        echo -e "  ${GREEN}[PASS]${NC} $name"
        PASS=$((PASS + 1))
    else
        echo -e "  ${RED}[FAIL]${NC} $name"
        echo -e "    ${YELLOW}Expected:${NC}"
        echo "$expected" | sed 's/^/      /'
        echo -e "    ${YELLOW}Got:${NC}"
        echo "$filtered" | sed 's/^/      /'
        FAIL=$((FAIL + 1))
    fi
}

run_test_exact() {
    local name="$1"
    local nts_file="$2"
    local input="$3"
    local expected="$4"
    local grep_pattern="$5"

    TOTAL=$((TOTAL + 1))
    local output
    output=$(printf "$input" | timeout 5 $BINARY "$nts_file" 2>&1)
    local exit_code=$?

    if [ $exit_code -eq 124 ]; then
        echo -e "  ${RED}[TIMEOUT]${NC} $name"
        FAIL=$((FAIL + 1))
        return
    fi

    local filtered
    filtered=$(echo "$output" | grep -E "$grep_pattern" | grep -v "output(s):" | grep -v "input(s):" | sed 's/^[[:space:]]*//' | sed '/^$/d')

    if [ "$filtered" = "$expected" ]; then
        echo -e "  ${GREEN}[PASS]${NC} $name"
        PASS=$((PASS + 1))
    else
        echo -e "  ${RED}[FAIL]${NC} $name"
        echo -e "    ${YELLOW}Expected:${NC}"
        echo "$expected" | sed 's/^/      /'
        echo -e "    ${YELLOW}Got:${NC}"
        echo "$filtered" | sed 's/^/      /'
        FAIL=$((FAIL + 1))
    fi
}

# Expect error exit code 84
run_test_error() {
    local name="$1"
    local nts_file="$2"

    TOTAL=$((TOTAL + 1))
    local output
    output=$(printf 'exit\n' | timeout 5 $BINARY "$nts_file" 2>&1)
    local exit_code=$?

    if [ $exit_code -eq 84 ]; then
        echo -e "  ${GREEN}[PASS]${NC} $name"
        PASS=$((PASS + 1))
    else
        echo -e "  ${RED}[FAIL]${NC} $name (expected exit 84, got $exit_code)"
        FAIL=$((FAIL + 1))
    fi
}

echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}   NanoTekSpice - Test Suite${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""

# ============================================================
# BASIC COMPONENTS
# ============================================================
echo -e "${CYAN}--- Basic Components ---${NC}"

# true
run_test_exact "true -> output = 1" \
    "doc/nts_single/true.nts" \
    "simulate\ndisplay\nexit\n" \
    "out: 1" \
    "out:"

# false
run_test_exact "false -> output = 0" \
    "doc/nts_single/false.nts" \
    "simulate\ndisplay\nexit\n" \
    "out: 0" \
    "out:"

# input_output: undefined before simulate
run_test_exact "input_output: U before set" \
    "doc/nts_single/input_output.nts" \
    "simulate\ndisplay\nexit\n" \
    "out: U" \
    "out:"

# input_output: set value
run_test_exact "input_output: set to 1" \
    "doc/nts_single/input_output.nts" \
    "in=1\nsimulate\ndisplay\nexit\n" \
    "out: 1" \
    "out:"

run_test_exact "input_output: set to 0" \
    "doc/nts_single/input_output.nts" \
    "in=0\nsimulate\ndisplay\nexit\n" \
    "out: 0" \
    "out:"

# clock: initial U, then set, then toggles
run_test_exact "clock: initial display = U" \
    "doc/nts_single/clock.nts" \
    "display\nexit\n" \
    "out: U" \
    "out:"

run_test_exact "clock: set 0, simulate -> 0" \
    "doc/nts_single/clock.nts" \
    "cl=0\nsimulate\ndisplay\nexit\n" \
    "out: 0" \
    "out:"

run_test_exact "clock: set 0, sim, sim -> 1 (toggle)" \
    "doc/nts_single/clock.nts" \
    "cl=0\nsimulate\nsimulate\ndisplay\nexit\n" \
    "out: 1" \
    "out:"

run_test_exact "clock: set 0, sim x3 -> 0 (toggle back)" \
    "doc/nts_single/clock.nts" \
    "cl=0\nsimulate\nsimulate\nsimulate\ndisplay\nexit\n" \
    "out: 0" \
    "out:"

echo ""

# ============================================================
# AND GATE (elementary)
# ============================================================
echo -e "${CYAN}--- AND Gate (elementary) ---${NC}"

# Truth table: AND
# 0 0 -> 0 | 0 1 -> 0 | 1 0 -> 0 | 1 1 -> 1 | U x -> U
run_test_exact "and: 0 & 0 = 0" "doc/nts_single/and.nts" \
    "in_1=0\nin_2=0\nsimulate\ndisplay\nexit\n" "out: 0" "out:"

run_test_exact "and: 0 & 1 = 0" "doc/nts_single/and.nts" \
    "in_1=0\nin_2=1\nsimulate\ndisplay\nexit\n" "out: 0" "out:"

run_test_exact "and: 1 & 0 = 0" "doc/nts_single/and.nts" \
    "in_1=1\nin_2=0\nsimulate\ndisplay\nexit\n" "out: 0" "out:"

run_test_exact "and: 1 & 1 = 1" "doc/nts_single/and.nts" \
    "in_1=1\nin_2=1\nsimulate\ndisplay\nexit\n" "out: 1" "out:"

run_test_exact "and: U & U = U" "doc/nts_single/and.nts" \
    "simulate\ndisplay\nexit\n" "out: U" "out:"

run_test_exact "and: 0 & U = 0" "doc/nts_single/and.nts" \
    "in_1=0\nsimulate\ndisplay\nexit\n" "out: 0" "out:"

run_test_exact "and: U & 0 = 0" "doc/nts_single/and.nts" \
    "in_2=0\nsimulate\ndisplay\nexit\n" "out: 0" "out:"

echo ""

# ============================================================
# OR GATE (elementary)
# ============================================================
echo -e "${CYAN}--- OR Gate (elementary) ---${NC}"

# Truth table: OR
# 0 0 -> 0 | 0 1 -> 1 | 1 0 -> 1 | 1 1 -> 1 | U x -> U (if x not 1)
run_test_exact "or: 0 | 0 = 0" "doc/nts_single/or.nts" \
    "in_1=0\nin_2=0\nsimulate\ndisplay\nexit\n" "out: 0" "out:"

run_test_exact "or: 0 | 1 = 1" "doc/nts_single/or.nts" \
    "in_1=0\nin_2=1\nsimulate\ndisplay\nexit\n" "out: 1" "out:"

run_test_exact "or: 1 | 0 = 1" "doc/nts_single/or.nts" \
    "in_1=1\nin_2=0\nsimulate\ndisplay\nexit\n" "out: 1" "out:"

run_test_exact "or: 1 | 1 = 1" "doc/nts_single/or.nts" \
    "in_1=1\nin_2=1\nsimulate\ndisplay\nexit\n" "out: 1" "out:"

run_test_exact "or: U | U = U" "doc/nts_single/or.nts" \
    "simulate\ndisplay\nexit\n" "out: U" "out:"

run_test_exact "or: 1 | U = 1" "doc/nts_single/or.nts" \
    "in_1=1\nsimulate\ndisplay\nexit\n" "out: 1" "out:"

run_test_exact "or: U | 1 = 1" "doc/nts_single/or.nts" \
    "in_2=1\nsimulate\ndisplay\nexit\n" "out: 1" "out:"

echo ""

# ============================================================
# NOT GATE (elementary)
# ============================================================
echo -e "${CYAN}--- NOT Gate (elementary) ---${NC}"

run_test_exact "not: !0 = 1" "doc/nts_single/not.nts" \
    "in=0\nsimulate\ndisplay\nexit\n" "out: 1" "out:"

run_test_exact "not: !1 = 0" "doc/nts_single/not.nts" \
    "in=1\nsimulate\ndisplay\nexit\n" "out: 0" "out:"

run_test_exact "not: !U = U" "doc/nts_single/not.nts" \
    "simulate\ndisplay\nexit\n" "out: U" "out:"

echo ""

# ============================================================
# XOR GATE (elementary)
# ============================================================
echo -e "${CYAN}--- XOR Gate (elementary) ---${NC}"

run_test_exact "xor: 0 ^ 0 = 0" "doc/nts_single/xor.nts" \
    "in_1=0\nin_2=0\nsimulate\ndisplay\nexit\n" "out: 0" "out:"

run_test_exact "xor: 0 ^ 1 = 1" "doc/nts_single/xor.nts" \
    "in_1=0\nin_2=1\nsimulate\ndisplay\nexit\n" "out: 1" "out:"

run_test_exact "xor: 1 ^ 0 = 1" "doc/nts_single/xor.nts" \
    "in_1=1\nin_2=0\nsimulate\ndisplay\nexit\n" "out: 1" "out:"

run_test_exact "xor: 1 ^ 1 = 0" "doc/nts_single/xor.nts" \
    "in_1=1\nin_2=1\nsimulate\ndisplay\nexit\n" "out: 0" "out:"

echo ""

# ============================================================
# 4081 - AND GATES (4 gates in chip)
# ============================================================
echo -e "${CYAN}--- 4081 (AND gates) ---${NC}"

# All combos for 4 gates: test all 0s and all 1s + mixed
run_test "4081: all 0s -> all 0" "doc/nts_single/4081_and.nts" \
    "in_01=0\nin_02=0\nin_05=0\nin_06=0\nin_08=0\nin_09=0\nin_12=0\nin_13=0\nsimulate\ndisplay\nexit\n" \
    "out_03: 0
out_04: 0
out_10: 0
out_11: 0"

run_test "4081: all 1s -> all 1" "doc/nts_single/4081_and.nts" \
    "in_01=1\nin_02=1\nin_05=1\nin_06=1\nin_08=1\nin_09=1\nin_12=1\nin_13=1\nsimulate\ndisplay\nexit\n" \
    "out_03: 1
out_04: 1
out_10: 1
out_11: 1"

run_test "4081: mixed 0,1" "doc/nts_single/4081_and.nts" \
    "in_01=0\nin_02=1\nin_05=1\nin_06=0\nin_08=1\nin_09=1\nin_12=0\nin_13=0\nsimulate\ndisplay\nexit\n" \
    "out_03: 0
out_04: 0
out_10: 1
out_11: 0"

echo ""

# ============================================================
# 4001 - NOR GATES
# ============================================================
echo -e "${CYAN}--- 4001 (NOR gates) ---${NC}"

# NOR: 0,0->1 | 0,1->0 | 1,0->0 | 1,1->0
run_test "4001: all 0s -> all 1 (NOR)" "doc/nts_single/4001_nor.nts" \
    "in_01=0\nin_02=0\nin_05=0\nin_06=0\nin_08=0\nin_09=0\nin_12=0\nin_13=0\nsimulate\ndisplay\nexit\n" \
    "out_03: 1
out_04: 1
out_10: 1
out_11: 1"

run_test "4001: all 1s -> all 0 (NOR)" "doc/nts_single/4001_nor.nts" \
    "in_01=1\nin_02=1\nin_05=1\nin_06=1\nin_08=1\nin_09=1\nin_12=1\nin_13=1\nsimulate\ndisplay\nexit\n" \
    "out_03: 0
out_04: 0
out_10: 0
out_11: 0"

run_test "4001: mixed (0,1)(1,0)(1,1)(0,0)" "doc/nts_single/4001_nor.nts" \
    "in_01=0\nin_02=1\nin_05=1\nin_06=0\nin_08=1\nin_09=1\nin_12=0\nin_13=0\nsimulate\ndisplay\nexit\n" \
    "out_03: 0
out_04: 0
out_10: 0
out_11: 1"

echo ""

# ============================================================
# 4011 - NAND GATES
# ============================================================
echo -e "${CYAN}--- 4011 (NAND gates) ---${NC}"

# NAND: 0,0->1 | 0,1->1 | 1,0->1 | 1,1->0
run_test "4011: all 0s -> all 1 (NAND)" "doc/nts_single/4011_nand.nts" \
    "in_01=0\nin_02=0\nin_05=0\nin_06=0\nin_08=0\nin_09=0\nin_12=0\nin_13=0\nsimulate\ndisplay\nexit\n" \
    "out_03: 1
out_04: 1
out_10: 1
out_11: 1"

run_test "4011: all 1s -> all 0 (NAND)" "doc/nts_single/4011_nand.nts" \
    "in_01=1\nin_02=1\nin_05=1\nin_06=1\nin_08=1\nin_09=1\nin_12=1\nin_13=1\nsimulate\ndisplay\nexit\n" \
    "out_03: 0
out_04: 0
out_10: 0
out_11: 0"

run_test "4011: mixed -> NAND results" "doc/nts_single/4011_nand.nts" \
    "in_01=1\nin_02=0\nin_05=0\nin_06=1\nin_08=1\nin_09=1\nin_12=0\nin_13=1\nsimulate\ndisplay\nexit\n" \
    "out_03: 1
out_04: 1
out_10: 0
out_11: 1"

echo ""

# ============================================================
# 4030 - XOR GATES
# ============================================================
echo -e "${CYAN}--- 4030 (XOR gates) ---${NC}"

# XOR truth table: 0,0->0 | 0,1->1 | 1,0->1 | 1,1->0
run_test "4030: all 0s -> all 0 (XOR)" "doc/nts_single/4030_xor.nts" \
    "in_01=0\nin_02=0\nin_05=0\nin_06=0\nin_08=0\nin_09=0\nin_12=0\nin_13=0\nsimulate\ndisplay\nexit\n" \
    "out_03: 0
out_04: 0
out_10: 0
out_11: 0"

run_test "4030: all 1s -> all 0 (XOR)" "doc/nts_single/4030_xor.nts" \
    "in_01=1\nin_02=1\nin_05=1\nin_06=1\nin_08=1\nin_09=1\nin_12=1\nin_13=1\nsimulate\ndisplay\nexit\n" \
    "out_03: 0
out_04: 0
out_10: 0
out_11: 0"

run_test "4030: mixed (0,1)(1,0)(1,1)(0,0)" "doc/nts_single/4030_xor.nts" \
    "in_01=0\nin_02=1\nin_05=1\nin_06=0\nin_08=1\nin_09=1\nin_12=0\nin_13=0\nsimulate\ndisplay\nexit\n" \
    "out_03: 1
out_04: 1
out_10: 0
out_11: 0"

echo ""

# ============================================================
# 4071 - OR GATES
# ============================================================
echo -e "${CYAN}--- 4071 (OR gates) ---${NC}"

run_test "4071: all 0s -> all 0 (OR)" "doc/nts_single/4071_or.nts" \
    "in_01=0\nin_02=0\nin_05=0\nin_06=0\nin_08=0\nin_09=0\nin_12=0\nin_13=0\nsimulate\ndisplay\nexit\n" \
    "out_03: 0
out_04: 0
out_10: 0
out_11: 0"

run_test "4071: all 1s -> all 1 (OR)" "doc/nts_single/4071_or.nts" \
    "in_01=1\nin_02=1\nin_05=1\nin_06=1\nin_08=1\nin_09=1\nin_12=1\nin_13=1\nsimulate\ndisplay\nexit\n" \
    "out_03: 1
out_04: 1
out_10: 1
out_11: 1"

run_test "4071: mixed (0,1)(1,0)(0,0)(1,1)" "doc/nts_single/4071_or.nts" \
    "in_01=0\nin_02=1\nin_05=1\nin_06=0\nin_08=0\nin_09=0\nin_12=1\nin_13=1\nsimulate\ndisplay\nexit\n" \
    "out_03: 1
out_04: 1
out_10: 0
out_11: 1"

echo ""

# ============================================================
# 4069 - NOT GATES (6 inverters)
# ============================================================
echo -e "${CYAN}--- 4069 (NOT gates) ---${NC}"

run_test "4069: all 0s -> all 1" "doc/nts_single/4069_not.nts" \
    "in_01=0\nin_03=0\nin_05=0\nin_09=0\nin_11=0\nin_13=0\nsimulate\ndisplay\nexit\n" \
    "out_02: 1
out_04: 1
out_06: 1
out_08: 1
out_10: 1
out_12: 1"

run_test "4069: all 1s -> all 0" "doc/nts_single/4069_not.nts" \
    "in_01=1\nin_03=1\nin_05=1\nin_09=1\nin_11=1\nin_13=1\nsimulate\ndisplay\nexit\n" \
    "out_02: 0
out_04: 0
out_06: 0
out_08: 0
out_10: 0
out_12: 0"

run_test "4069: mixed" "doc/nts_single/4069_not.nts" \
    "in_01=1\nin_03=0\nin_05=1\nin_09=0\nin_11=1\nin_13=0\nsimulate\ndisplay\nexit\n" \
    "out_02: 0
out_04: 1
out_06: 0
out_08: 1
out_10: 0
out_12: 1"

echo ""

# ============================================================
# 4008 - 4-BIT ADDER
# ============================================================
echo -e "${CYAN}--- 4008 (4-bit Adder) ---${NC}"

# 0000 + 0000 + 0 = 00000
run_test "4008: 0+0+0 = 0" "doc/nts_single/4008_adder.nts" \
    "in_a1=0\nin_a2=0\nin_a3=0\nin_a4=0\nin_b1=0\nin_b2=0\nin_b3=0\nin_b4=0\nin_c=0\nsimulate\ndisplay\nexit\n" \
    "out_0: 0
out_1: 0
out_2: 0
out_3: 0
out_c: 0"

# 0001 + 0001 + 0 = 00010 -> out_0=0, out_1=1, out_2=0, out_3=0, carry=0
run_test "4008: 1+1 = 2" "doc/nts_single/4008_adder.nts" \
    "in_a1=1\nin_a2=0\nin_a3=0\nin_a4=0\nin_b1=1\nin_b2=0\nin_b3=0\nin_b4=0\nin_c=0\nsimulate\ndisplay\nexit\n" \
    "out_0: 0
out_1: 1
out_2: 0
out_3: 0
out_c: 0"

# 1111 + 1111 + 0 = 11110 -> out_0=0, out_1=1, out_2=1, out_3=1, carry=1
run_test "4008: 15+15 = 30" "doc/nts_single/4008_adder.nts" \
    "in_a1=1\nin_a2=1\nin_a3=1\nin_a4=1\nin_b1=1\nin_b2=1\nin_b3=1\nin_b4=1\nin_c=0\nsimulate\ndisplay\nexit\n" \
    "out_0: 0
out_1: 1
out_2: 1
out_3: 1
out_c: 1"

# 1111 + 1111 + 1 = 11111 -> out_0=1, out_1=1, out_2=1, out_3=1, carry=1
run_test "4008: 15+15+1 = 31" "doc/nts_single/4008_adder.nts" \
    "in_a1=1\nin_a2=1\nin_a3=1\nin_a4=1\nin_b1=1\nin_b2=1\nin_b3=1\nin_b4=1\nin_c=1\nsimulate\ndisplay\nexit\n" \
    "out_0: 1
out_1: 1
out_2: 1
out_3: 1
out_c: 1"

# 0101 (5) + 0011 (3) = 1000 (8) -> out_0=0, out_1=0, out_2=0, out_3=1, carry=0
run_test "4008: 5+3 = 8" "doc/nts_single/4008_adder.nts" \
    "in_a1=1\nin_a2=0\nin_a3=1\nin_a4=0\nin_b1=1\nin_b2=1\nin_b3=0\nin_b4=0\nin_c=0\nsimulate\ndisplay\nexit\n" \
    "out_0: 0
out_1: 0
out_2: 0
out_3: 1
out_c: 0"

# 1001 (9) + 0111 (7) + 1 = 10001 (17) -> out_0=1, out_1=0, out_2=0, out_3=0, carry=1
run_test "4008: 9+7+1 = 17" "doc/nts_single/4008_adder.nts" \
    "in_a1=1\nin_a2=0\nin_a3=0\nin_a4=1\nin_b1=1\nin_b2=1\nin_b3=1\nin_b4=0\nin_c=1\nsimulate\ndisplay\nexit\n" \
    "out_0: 1
out_1: 0
out_2: 0
out_3: 0
out_c: 1"

echo ""

# ============================================================
# 4013 - DUAL D-TYPE FLIP-FLOP
# ============================================================
echo -e "${CYAN}--- 4013 (Dual D-Type Flip-Flop) ---${NC}"

# Set: when set=1, reset=0 -> Q=1, Qb=0
run_test "4013: set=1 -> Q=1, Qb=0" "doc/nts_single/4013_flipflop.nts" \
    "in_1_data=0\nin_1_set=1\nin_1_reset=0\nin_2_data=0\nin_2_set=1\nin_2_reset=0\nsimulate\ndisplay\nexit\n" \
    "out_1_q: 1
out_1_qb: 0
out_2_q: 1
out_2_qb: 0"

# Reset: when set=0, reset=1 -> Q=0, Qb=1
run_test "4013: reset=1 -> Q=0, Qb=1" "doc/nts_single/4013_flipflop.nts" \
    "in_1_data=0\nin_1_set=0\nin_1_reset=1\nin_2_data=0\nin_2_set=0\nin_2_reset=1\nsimulate\ndisplay\nexit\n" \
    "out_1_q: 0
out_1_qb: 1
out_2_q: 0
out_2_qb: 1"

# Clock rising edge with data=1: Q=1, Qb=0
run_test "4013: clock rise + data=1 -> Q=1" "doc/nts_single/4013_flipflop.nts" \
    "cl_1_clock=0\ncl_2_clock=0\nin_1_data=1\nin_1_set=0\nin_1_reset=0\nin_2_data=1\nin_2_set=0\nin_2_reset=0\nsimulate\nsimulate\ndisplay\nexit\n" \
    "out_1_q: 1
out_1_qb: 0
out_2_q: 1
out_2_qb: 0"

# Clock rising edge with data=0: Q=0, Qb=1
run_test "4013: clock rise + data=0 -> Q=0" "doc/nts_single/4013_flipflop.nts" \
    "cl_1_clock=0\ncl_2_clock=0\nin_1_data=0\nin_1_set=0\nin_1_reset=0\nin_2_data=0\nin_2_set=0\nin_2_reset=0\nsimulate\nsimulate\ndisplay\nexit\n" \
    "out_1_q: 0
out_1_qb: 1
out_2_q: 0
out_2_qb: 1"

echo ""

# ============================================================
# 4017 - JOHNSON DECADE COUNTER
# ============================================================
echo -e "${CYAN}--- 4017 (Johnson Decade Counter) ---${NC}"

# Initial state: out_0=1, others=0
# in_0=clock, in_1=clock inhibit (active high), in_r=reset
# Clock on pin 14 (CP0), inhibit on pin 13 (CP1), reset on pin 15
# CP0 positive-edge triggered, CP1 negative-edge triggered
# For simple count: set in_1=0 (not inhibited), in_r=0 (not reset)
# Then toggle clock to count

run_test_exact "4017: initial state (after reset)" "doc/nts_single/4017_johnson.nts" \
    "in_0=0\nin_1=0\nin_r=1\nsimulate\nin_r=0\nsimulate\ndisplay\nexit\n" \
    "out_0: 1
out_1: 0
out_2: 0
out_3: 0
out_4: 0
out_5: 0
out_6: 0
out_7: 0
out_8: 0
out_9: 0
out_s: 1" \
    "out_"

# Count 1 clock pulse -> out_1=1
run_test_exact "4017: 1 clock pulse -> out_1" "doc/nts_single/4017_johnson.nts" \
    "in_0=0\nin_1=0\nin_r=1\nsimulate\nin_r=0\nsimulate\nin_0=1\nsimulate\nin_0=0\nsimulate\ndisplay\nexit\n" \
    "out_0: 0
out_1: 1
out_2: 0
out_3: 0
out_4: 0
out_5: 0
out_6: 0
out_7: 0
out_8: 0
out_9: 0
out_s: 1" \
    "out_"

echo ""

# ============================================================
# 4040 - 12-BIT BINARY COUNTER
# ============================================================
echo -e "${CYAN}--- 4040 (12-bit Binary Counter) ---${NC}"

# Reset then count
# Clock on pin 10 (falling edge), reset on pin 11
# After reset: all 0
run_test_exact "4040: after reset -> all 0" "doc/nts_single/4040_counter.nts" \
    "cl_clock=0\nin_reset=1\nsimulate\nin_reset=0\nsimulate\ndisplay\nexit\n" \
    "out_00: 0
out_01: 0
out_02: 0
out_03: 0
out_04: 0
out_05: 0
out_06: 0
out_07: 0
out_08: 0
out_09: 0
out_10: 0
out_11: 0" \
    "out_"

# 1 clock tick: counter=1 -> out_00=1, rest=0
# Clock falling edge: need clock go 1->0
run_test_exact "4040: 1 tick -> count=1" "doc/nts_single/4040_counter.nts" \
    "cl_clock=1\nin_reset=1\nsimulate\nin_reset=0\nsimulate\nsimulate\ndisplay\nexit\n" \
    "out_00: 1
out_01: 0
out_02: 0
out_03: 0
out_04: 0
out_05: 0
out_06: 0
out_07: 0
out_08: 0
out_09: 0
out_10: 0
out_11: 0" \
    "out_"

# 2 ticks -> count=2 -> out_00=0, out_01=1
run_test_exact "4040: 2 ticks -> count=2" "doc/nts_single/4040_counter.nts" \
    "cl_clock=1\nin_reset=1\nsimulate\nin_reset=0\nsimulate\nsimulate\nsimulate\ndisplay\nexit\n" \
    "out_00: 0
out_01: 1
out_02: 0
out_03: 0
out_04: 0
out_05: 0
out_06: 0
out_07: 0
out_08: 0
out_09: 0
out_10: 0
out_11: 0" \
    "out_"

# 3 ticks -> count=3 -> out_00=1, out_01=1
# Clock toggles: 1->0 (count), 0->1(no), 1->0(count), 0->1(no), 1->0(count)
run_test_exact "4040: 3 ticks -> count=3" "doc/nts_single/4040_counter.nts" \
    "cl_clock=1\nin_reset=1\nsimulate\nin_reset=0\nsimulate\nsimulate\nsimulate\nsimulate\nsimulate\nsimulate\ndisplay\nexit\n" \
    "out_00: 1
out_01: 1
out_02: 0
out_03: 0
out_04: 0
out_05: 0
out_06: 0
out_07: 0
out_08: 0
out_09: 0
out_10: 0
out_11: 0" \
    "out_"

echo ""

# ============================================================
# 4094 - 8-BIT SHIFT REGISTER
# ============================================================
echo -e "${CYAN}--- 4094 (8-bit Shift Register) ---${NC}"

# strobe=true, enable=true already wired to true in the .nts
# Need to set clock=0 first so rising edge is detected
run_test_exact "4094: shift in 1" "doc/nts_single/4094_shift.nts" \
    "clock=0\ndata=1\nsimulate\nsimulate\ndisplay\nexit\n" \
    "out_0: 1
out_1: 0
out_2: 0
out_3: 0
out_4: 0
out_5: 0
out_6: 0
out_7: 0
out_qe: 0
out_qs: 0" \
    "out_"

# Shift in 1 then 0
run_test_exact "4094: shift 1 then 0" "doc/nts_single/4094_shift.nts" \
    "clock=0\ndata=1\nsimulate\nsimulate\ndata=0\nsimulate\nsimulate\ndisplay\nexit\n" \
    "out_0: 0
out_1: 1
out_2: 0
out_3: 0
out_4: 0
out_5: 0
out_6: 0
out_7: 0
out_qe: 0
out_qs: 0" \
    "out_"

echo ""

# ============================================================
# 4512 - 8-CHANNEL DATA SELECTOR
# ============================================================
echo -e "${CYAN}--- 4512 (8-Channel Data Selector) ---${NC}"

# Select channel 0 (a=0,b=0,c=0), in_0=1, enable=0, inhibit=0
run_test_exact "4512: select ch0=1" "doc/nts_single/4512_selector.nts" \
    "in_a=0\nin_b=0\nin_c=0\nin_0=1\nin_1=0\nin_2=0\nin_3=0\nin_4=0\nin_5=0\nin_6=0\nin_7=0\ninhibit=0\nenable=0\nsimulate\ndisplay\nexit\n" \
    "out_data: 1" \
    "out_data:"

# Select channel 3 (a=1,b=1,c=0), in_3=1
run_test_exact "4512: select ch3=1" "doc/nts_single/4512_selector.nts" \
    "in_a=1\nin_b=1\nin_c=0\nin_0=0\nin_1=0\nin_2=0\nin_3=1\nin_4=0\nin_5=0\nin_6=0\nin_7=0\ninhibit=0\nenable=0\nsimulate\ndisplay\nexit\n" \
    "out_data: 1" \
    "out_data:"

# Select channel 0 but in_0=0
run_test_exact "4512: select ch0=0" "doc/nts_single/4512_selector.nts" \
    "in_a=0\nin_b=0\nin_c=0\nin_0=0\nin_1=1\nin_2=1\nin_3=1\nin_4=1\nin_5=1\nin_6=1\nin_7=1\ninhibit=0\nenable=0\nsimulate\ndisplay\nexit\n" \
    "out_data: 0" \
    "out_data:"

# Inhibit active -> output 0
run_test_exact "4512: inhibit=1 -> 0" "doc/nts_single/4512_selector.nts" \
    "in_a=0\nin_b=0\nin_c=0\nin_0=1\nin_1=0\nin_2=0\nin_3=0\nin_4=0\nin_5=0\nin_6=0\nin_7=0\ninhibit=1\nenable=0\nsimulate\ndisplay\nexit\n" \
    "out_data: 0" \
    "out_data:"

# Select channel 7 (a=1,b=1,c=1), in_7=1
run_test_exact "4512: select ch7=1" "doc/nts_single/4512_selector.nts" \
    "in_a=1\nin_b=1\nin_c=1\nin_0=0\nin_1=0\nin_2=0\nin_3=0\nin_4=0\nin_5=0\nin_6=0\nin_7=1\ninhibit=0\nenable=0\nsimulate\ndisplay\nexit\n" \
    "out_data: 1" \
    "out_data:"

echo ""

# ============================================================
# 4514 - 4-BIT DECODER
# ============================================================
echo -e "${CYAN}--- 4514 (4-bit Decoder) ---${NC}"

# Latch address 0000, inhibit=0 -> out_00=1, rest=0
# Latch on falling edge of strobe: strobe 1->0
run_test_exact "4514: addr 0000 -> out_00=1" "doc/nts_single/4514_decoder.nts" \
    "in_0=0\nin_1=0\nin_2=0\nin_3=0\nstrobe=1\ninhibit=0\nsimulate\nstrobe=0\nsimulate\ndisplay\nexit\n" \
    "out_00: 1
out_01: 0
out_02: 0
out_03: 0
out_04: 0
out_05: 0
out_06: 0
out_07: 0
out_08: 0
out_09: 0
out_10: 0
out_11: 0
out_12: 0
out_13: 0
out_14: 0
out_15: 0" \
    "out_"

# Latch address 0001 -> out_01=1
run_test_exact "4514: addr 0001 -> out_01=1" "doc/nts_single/4514_decoder.nts" \
    "in_0=1\nin_1=0\nin_2=0\nin_3=0\nstrobe=1\ninhibit=0\nsimulate\nstrobe=0\nsimulate\ndisplay\nexit\n" \
    "out_00: 0
out_01: 1
out_02: 0
out_03: 0
out_04: 0
out_05: 0
out_06: 0
out_07: 0
out_08: 0
out_09: 0
out_10: 0
out_11: 0
out_12: 0
out_13: 0
out_14: 0
out_15: 0" \
    "out_"

# Inhibit=1 -> all 0
run_test_exact "4514: inhibit=1 -> all 0" "doc/nts_single/4514_decoder.nts" \
    "in_0=0\nin_1=0\nin_2=0\nin_3=0\nstrobe=1\ninhibit=1\nsimulate\nstrobe=0\nsimulate\ndisplay\nexit\n" \
    "out_00: 0
out_01: 0
out_02: 0
out_03: 0
out_04: 0
out_05: 0
out_06: 0
out_07: 0
out_08: 0
out_09: 0
out_10: 0
out_11: 0
out_12: 0
out_13: 0
out_14: 0
out_15: 0" \
    "out_"

echo ""

# ============================================================
# 4801 - 1024 BYTES RAM
# ============================================================
echo -e "${CYAN}--- 4801 (RAM) ---${NC}"

# 4801 RAM test - write then read
# Note: bidirectional pins on data bus - data pins are shared in/out  
# During write mode, compute returns Undefined
TOTAL=$((TOTAL + 1))
ram_out=$(printf 'enable=0\nwrite=0\nread=1\naddr_0=0\naddr_1=0\naddr_2=0\naddr_3=0\naddr_4=0\naddr_5=0\naddr_6=0\naddr_7=0\naddr_8=0\naddr_9=0\nin_0=1\nin_1=0\nin_2=1\nin_3=0\nin_4=1\nin_5=0\nin_6=1\nin_7=0\nsimulate\ndisplay\nexit\n' | timeout 5 $BINARY doc/nts_single/4801_ram.nts 2>&1)
if echo "$ram_out" | grep -q "out_0: U"; then
    echo -e "  ${GREEN}[PASS]${NC} 4801: write mode returns U"
    PASS=$((PASS + 1))
else
    echo -e "  ${RED}[FAIL]${NC} 4801: write mode"
    echo "$ram_out" | grep "out_" | sed 's/^/      /'
    FAIL=$((FAIL + 1))
fi

echo ""

# ============================================================
# 2716 - 2048 BYTES ROM
# ============================================================
echo -e "${CYAN}--- 2716 (ROM) ---${NC}"

# ROM needs rom.bin to exist
TOTAL=$((TOTAL + 1))
if [ -f "rom.bin" ]; then
    output=$(printf 'enable=0\nread=0\naddr_00=0\naddr_01=0\naddr_02=0\naddr_03=0\naddr_04=0\naddr_05=0\naddr_06=0\naddr_07=0\naddr_08=0\naddr_09=0\naddr_10=0\nsimulate\ndisplay\nexit\n' | timeout 5 $BINARY doc/nts_single/2716_rom.nts 2>&1)
    if echo "$output" | grep -q "out_0:"; then
        echo -e "  ${GREEN}[PASS]${NC} 2716: loads and responds"
        PASS=$((PASS + 1))
    else
        echo -e "  ${RED}[FAIL]${NC} 2716: no output"
        FAIL=$((FAIL + 1))
    fi
else
    echo -e "  ${YELLOW}[SKIP]${NC} 2716: no rom.bin file"
    TOTAL=$((TOTAL - 1))
fi

echo ""

# ============================================================
# ADVANCED CIRCUITS
# ============================================================
echo -e "${CYAN}--- Advanced: and-or-not ---${NC}"

# Circuit: out = AND(NOT(NOT(OR(NOT(NOT(a)), NOT(b)))), c)
# Simplify: NOT(NOT(a)) = a, so OR(a, NOT(b)), NOT(OR(...)) = NOR(a, NOT(b))
# Actually: not1=!a, not2=!!a=a, or=a|!b, not4=!(a|!b), and=!(a|!b) & c
# For a=1,b=1,c=1: or=1|0=1, !or=0, and=0&1=0
run_test_exact "and-or-not: a=1,b=1,c=1 -> 0" "doc/nts_advanced/and-or-not.nts" \
    "in_a=1\nin_b=1\nin_c=1\nsimulate\ndisplay\nexit\n" "out: 0" "out:"

# a=0,b=0,c=1: not1=1, not2=0, not3=1, or=0|1=1, not4=0, and=0&1=0
run_test_exact "and-or-not: a=0,b=0,c=1 -> 0" "doc/nts_advanced/and-or-not.nts" \
    "in_a=0\nin_b=0\nin_c=1\nsimulate\ndisplay\nexit\n" "out: 0" "out:"

# a=0,b=1,c=1: not1=1, not2=0, not3=0, or=0|0=0, not4=1, and=1&1=1
run_test_exact "and-or-not: a=0,b=1,c=1 -> 1" "doc/nts_advanced/and-or-not.nts" \
    "in_a=0\nin_b=1\nin_c=1\nsimulate\ndisplay\nexit\n" "out: 1" "out:"

# a=0,b=1,c=0: same path but and=1&0=0
run_test_exact "and-or-not: a=0,b=1,c=0 -> 0" "doc/nts_advanced/and-or-not.nts" \
    "in_a=0\nin_b=1\nin_c=0\nsimulate\ndisplay\nexit\n" "out: 0" "out:"

echo ""

echo -e "${CYAN}--- Advanced: and (cascaded 4081) ---${NC}"

# out = ((a & b) & (c & d)) & e
run_test_exact "cascaded-and: all 1 -> 1" "doc/nts_advanced/and.nts" \
    "in_a=1\nin_b=1\nin_c=1\nin_d=1\nin_e=1\nsimulate\ndisplay\nexit\n" "out: 1" "out:"

run_test_exact "cascaded-and: one 0 -> 0" "doc/nts_advanced/and.nts" \
    "in_a=1\nin_b=1\nin_c=1\nin_d=1\nin_e=0\nsimulate\ndisplay\nexit\n" "out: 0" "out:"

run_test_exact "cascaded-and: all 0 -> 0" "doc/nts_advanced/and.nts" \
    "in_a=0\nin_b=0\nin_c=0\nin_d=0\nin_e=0\nsimulate\ndisplay\nexit\n" "out: 0" "out:"

echo ""

# ============================================================
# SUBJECT EXAMPLES
# ============================================================
echo -e "${CYAN}--- Subject Examples ---${NC}"

# Example 1: OR gate - a=1, b=0 -> s=1
run_test_exact "subject ex1: OR a=1,b=0 -> s=1" "doc/nts_single/4071_or.nts" \
    "in_01=1\nin_02=0\nin_05=0\nin_06=0\nin_08=0\nin_09=0\nin_12=0\nin_13=0\nsimulate\ndisplay\nexit\n" \
    "out_03: 1" \
    "out_03:"

# Example 3: clock behavior (from subject)
run_test_exact "subject ex3: clock toggle" "doc/nts_single/clock.nts" \
    "cl=0\nsimulate\ndisplay\nsimulate\ndisplay\nsimulate\nsimulate\nsimulate\ndisplay\nexit\n" \
    "out: 0
out: 1
out: 0" \
    "out:"

echo ""

# ============================================================
# ADVANCED: ALTERED COUNTER (feedback 4040)
# ============================================================
echo -e "${CYAN}--- Advanced: altered-counter ---${NC}"

# After reset + 1 falling edge: count=1, out_1=OR(1,0)=1, out_2=OR(0,0)=0
run_test_exact "altered-counter: count=1" "doc/nts_advanced/altered-counter.nts" \
    "in_reset=0\ncl_clock=1\nsimulate\nsimulate\ndisplay\nexit\n" \
    "out_1: 1
out_2: 0" \
    "out_"

# After 4 falling edges: count=4, out_1=OR(0,0)=0, out_2=OR(0,1)=1
run_test_exact "altered-counter: count=4" "doc/nts_advanced/altered-counter.nts" \
    "in_reset=0\ncl_clock=1\nsimulate\nsimulate\nsimulate\nsimulate\nsimulate\nsimulate\nsimulate\nsimulate\ndisplay\nexit\n" \
    "out_1: 0
out_2: 1" \
    "out_"

# After 16 falling edges: count=16 auto-resets to 0
run_test_exact "altered-counter: auto-reset at 16" "doc/nts_advanced/altered-counter.nts" \
    "$(python3 -c "cmds=['in_reset=0','cl_clock=1']+['simulate']*32+['display','exit'];print('\\\\n'.join(cmds))")" \
    "out_1: 0
out_2: 0" \
    "out_"

# After 17 falling edges: back to count=1
run_test_exact "altered-counter: wraps to 1" "doc/nts_advanced/altered-counter.nts" \
    "$(python3 -c "cmds=['in_reset=0','cl_clock=1']+['simulate']*34+['display','exit'];print('\\\\n'.join(cmds))")" \
    "out_1: 1
out_2: 0" \
    "out_"

echo ""

# ============================================================
# 4017 - more tests
# ============================================================
echo -e "${CYAN}--- 4017 (more tests) ---${NC}"

# Count 5 pulses -> out_5=1, out_s=0 (carry goes low at 5)
run_test_exact "4017: 5 pulses -> out_5" "doc/nts_single/4017_johnson.nts" \
    "in_0=0\nin_1=0\nin_r=1\nsimulate\nin_r=0\nsimulate\nin_0=1\nsimulate\nin_0=0\nsimulate\nin_0=1\nsimulate\nin_0=0\nsimulate\nin_0=1\nsimulate\nin_0=0\nsimulate\nin_0=1\nsimulate\nin_0=0\nsimulate\nin_0=1\nsimulate\nin_0=0\nsimulate\ndisplay\nexit\n" \
    "out_0: 0
out_1: 0
out_2: 0
out_3: 0
out_4: 0
out_5: 1
out_6: 0
out_7: 0
out_8: 0
out_9: 0
out_s: 0" \
    "out_"

# Count 10 pulses -> wraps to out_0
run_test_exact "4017: 10 pulses wraps to 0" "doc/nts_single/4017_johnson.nts" \
    "in_0=0\nin_1=0\nin_r=1\nsimulate\nin_r=0\nsimulate\n$(for i in $(seq 1 10); do echo "in_0=1\nsimulate\nin_0=0\nsimulate\n"; done)display\nexit\n" \
    "out_0: 1
out_1: 0
out_2: 0
out_3: 0
out_4: 0
out_5: 0
out_6: 0
out_7: 0
out_8: 0
out_9: 0
out_s: 1" \
    "out_"

echo ""

# ============================================================
# 4040 - more tests
# ============================================================
echo -e "${CYAN}--- 4040 (more tests) ---${NC}"

# Count to 5 (binary 101): out_00=1, out_02=1
run_test_exact "4040: count=5" "doc/nts_single/4040_counter.nts" \
    "cl_clock=1\nin_reset=1\nsimulate\nin_reset=0\nsimulate\nsimulate\nsimulate\nsimulate\nsimulate\nsimulate\nsimulate\nsimulate\nsimulate\ndisplay\nexit\n" \
    "out_00: 1
out_01: 0
out_02: 1
out_03: 0
out_04: 0
out_05: 0
out_06: 0
out_07: 0
out_08: 0
out_09: 0
out_10: 0
out_11: 0" \
    "out_"

echo ""

# ============================================================
# 4094 - more tests
# ============================================================
echo -e "${CYAN}--- 4094 (more tests) ---${NC}"

# Shift in 11010000 -> read back
run_test_exact "4094: shift 8 bits" "doc/nts_single/4094_shift.nts" \
    "clock=0\ndata=0\nsimulate\nsimulate\ndata=0\nsimulate\nsimulate\ndata=0\nsimulate\nsimulate\ndata=1\nsimulate\nsimulate\ndata=0\nsimulate\nsimulate\ndata=1\nsimulate\nsimulate\ndata=1\nsimulate\nsimulate\ndata=0\nsimulate\nsimulate\ndisplay\nexit\n" \
    "out_0: 0
out_1: 1
out_2: 1
out_3: 0
out_4: 1
out_5: 0
out_6: 0
out_7: 0
out_qe: 0
out_qs: 0" \
    "out_"

echo ""

# ============================================================
# 4514 - more tests
# ============================================================
echo -e "${CYAN}--- 4514 (more tests) ---${NC}"

# Latch addr 1111 (15) -> out_15=1
run_test_exact "4514: addr 1111 -> out_15=1" "doc/nts_single/4514_decoder.nts" \
    "in_0=1\nin_1=1\nin_2=1\nin_3=1\nstrobe=1\ninhibit=0\nsimulate\nstrobe=0\nsimulate\ndisplay\nexit\n" \
    "out_00: 0
out_01: 0
out_02: 0
out_03: 0
out_04: 0
out_05: 0
out_06: 0
out_07: 0
out_08: 0
out_09: 0
out_10: 0
out_11: 0
out_12: 0
out_13: 0
out_14: 0
out_15: 1" \
    "out_"

# Change addr after latch -> output stays latched
run_test_exact "4514: latched value stays" "doc/nts_single/4514_decoder.nts" \
    "in_0=1\nin_1=0\nin_2=0\nin_3=0\nstrobe=1\ninhibit=0\nsimulate\nstrobe=0\nsimulate\nin_0=0\nsimulate\ndisplay\nexit\n" \
    "out_00: 0
out_01: 1
out_02: 0
out_03: 0
out_04: 0
out_05: 0
out_06: 0
out_07: 0
out_08: 0
out_09: 0
out_10: 0
out_11: 0
out_12: 0
out_13: 0
out_14: 0
out_15: 0" \
    "out_"

echo ""

# ============================================================
# 4801 - write then read
# ============================================================
echo -e "${CYAN}--- 4801 (write then read) ---${NC}"

# Write 0xA5 (10100101) to addr 0, then read it back
TOTAL=$((TOTAL + 1))
ram_out=$(printf 'enable=0\nwrite=0\nread=1\naddr_0=0\naddr_1=0\naddr_2=0\naddr_3=0\naddr_4=0\naddr_5=0\naddr_6=0\naddr_7=0\naddr_8=0\naddr_9=0\nin_0=1\nin_1=0\nin_2=1\nin_3=0\nin_4=0\nin_5=1\nin_6=0\nin_7=1\nsimulate\nwrite=1\nread=0\nsimulate\ndisplay\nexit\n' | timeout 5 $BINARY doc/nts_single/4801_ram.nts 2>&1)
ram_filtered=$(echo "$ram_out" | grep -E "^\s*out_" | sed 's/^[[:space:]]*//')
ram_expected="out_0: 1
out_1: 0
out_2: 1
out_3: 0
out_4: 0
out_5: 1
out_6: 0
out_7: 1"
if [ "$ram_filtered" = "$ram_expected" ]; then
    echo -e "  ${GREEN}[PASS]${NC} 4801: write 0xA5 then read"
    PASS=$((PASS + 1))
else
    echo -e "  ${RED}[FAIL]${NC} 4801: write 0xA5 then read"
    echo -e "    ${YELLOW}Expected:${NC}"
    echo "$ram_expected" | sed 's/^/      /'
    echo -e "    ${YELLOW}Got:${NC}"
    echo "$ram_filtered" | sed 's/^/      /'
    FAIL=$((FAIL + 1))
fi

echo ""

# ============================================================
# ERROR HANDLING
# ============================================================
echo -e "${CYAN}--- Error Handling ---${NC}"

# No args
TOTAL=$((TOTAL + 1))
output=$(timeout 5 $BINARY 2>&1)
ec=$?
if [ $ec -eq 84 ]; then
    echo -e "  ${GREEN}[PASS]${NC} no arguments -> exit 84"
    PASS=$((PASS + 1))
else
    echo -e "  ${RED}[FAIL]${NC} no arguments -> expected 84, got $ec"
    FAIL=$((FAIL + 1))
fi

# Non-existent file
TOTAL=$((TOTAL + 1))
output=$(timeout 5 $BINARY "nonexistent.nts" 2>&1)
ec=$?
if [ $ec -eq 84 ]; then
    echo -e "  ${GREEN}[PASS]${NC} nonexistent file -> exit 84"
    PASS=$((PASS + 1))
else
    echo -e "  ${RED}[FAIL]${NC} nonexistent file -> expected 84, got $ec"
    FAIL=$((FAIL + 1))
fi

echo ""

# ============================================================
# SUMMARY
# ============================================================
echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}   Results: $PASS/$TOTAL passed${NC}"
if [ $FAIL -eq 0 ]; then
    echo -e "   ${GREEN}ALL TESTS PASSED!${NC}"
else
    echo -e "   ${RED}$FAIL test(s) failed${NC}"
fi
echo -e "${CYAN}========================================${NC}"

exit $FAIL
