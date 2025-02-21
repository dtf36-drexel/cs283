#!/usr/bin/env bats

# Test external command: echo
@test "External command: echo" {
    run ./dsh <<EOF
echo hello world
EOF
    # Check that the output contains the echoed string
    [[ "$output" == *"hello world"* ]]
    [ "$status" -eq 0 ]
}

# Test built-in cd with a valid directory
@test "Built-in cd: valid directory" {
    # Create a temporary directory for testing
    TMP_DIR=$(mktemp -d)
    run ./dsh <<EOF
cd $TMP_DIR
pwd
EOF
    # Check that the output contains the temporary directory path
    [[ "$output" == *"$TMP_DIR"* ]]
    [ "$status" -eq 0 ]
    rm -rf "$TMP_DIR"
}

# Test built-in cd with an invalid directory (should print an error)
@test "Built-in cd: invalid directory" {
    run ./dsh <<EOF
cd /nonexistentdirectory
EOF
    # Expect an error message starting with "cd error:"
    [[ "$output" == *"cd error:"* ]]
    [ "$status" -eq 0 ]
}

# Test an empty command (should print a warning)
@test "Empty command" {
    run ./dsh <<EOF

EOF
    # The shell should warn when no commands are provided
    [[ "$output" == *"warning: no commands provided"* ]]
    [ "$status" -eq 0 ]
}

# Test external command: which ls
@test "External command: which ls" {
    run ./dsh <<EOF
which ls
EOF
    # Check that the output contains a valid path to ls (e.g., /bin/ls or /usr/bin/ls)
    [[ "$output" == *"/bin/ls"* || "$output" == *"/usr/bin/ls"* ]]
    [ "$status" -eq 0 ]
}

