#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

# Test 1: Single command execution (echo)
@test "Echo command" {
  run ./dsh <<EOF
echo hello, world
EOF
  # Check that the output (which includes prompts) contains the echoed string.
  [[ "$output" == *"hello, world"* ]]
  [ "$status" -eq 0 ]
}

# Test 2: Built-in dragon command displays ASCII art
@test "Dragon command" {
  run ./dsh <<EOF
dragon
EOF
  # Look for a substring common in the dragon ASCII art
  [[ "$output" == *"@%%%%"* ]]
  [ "$status" -eq 0 ]
}

# Test 3: Change directory using the built-in cd command and then run pwd
@test "Change directory command" {
  run ./dsh <<EOF
cd /tmp
pwd
EOF
  # The output should include "/tmp" somewhere.
  [[ "$output" == *"/tmp"* ]]
  [ "$status" -eq 0 ]
}

# Test 4: Output redirection using >
@test "Output redirection" {
  run ./dsh <<EOF
echo "hello, class" > out.txt
cat out.txt
EOF
  # Check that 'cat out.txt' prints the expected text.
  [[ "$output" == *"hello, class"* ]]
  [ "$status" -eq 0 ]
  rm -f out.txt
}

# Test 5: Append redirection using >>
@test "Append redirection" {
  run ./dsh <<EOF
echo "first line" > out.txt
echo "second line" >> out.txt
cat out.txt
EOF
  [[ "$output" == *"first line"* ]]
  [[ "$output" == *"second line"* ]]
  [ "$status" -eq 0 ]
  rm -f out.txt
}

# Test 6: Input redirection using <
@test "Input redirection" {
  # Create a file with known content
  echo "input test" > input.txt
  run ./dsh <<EOF
cat < input.txt
EOF
  [[ "$output" == *"input test"* ]]
  [ "$status" -eq 0 ]
  rm -f input.txt
}

# Test 7: Handling an empty command line should show a warning.
@test "Empty command" {
  run ./dsh <<EOF

EOF
  [[ "$output" == *"warning: no commands provided"* ]]
  [ "$status" -eq 0 ]
}

# Test 8: Pipeline with echo and tr to convert text to uppercase
@test "Pipeline echo and tr" {
  run ./dsh <<EOF
echo hello pipe | tr a-z A-Z
EOF
  [[ "$output" == *"HELLO PIPE"* ]]
  [ "$status" -eq 0 ]
}
